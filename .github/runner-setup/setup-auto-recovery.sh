#!/usr/bin/env bash
# setup-auto-recovery.sh — one-time setup for vmatldcsdoc-1 runner auto-recovery
#
# Installs:
#   1. systemd override for the runner service (Restart=always)
#   2. Watchdog script that detects zombie/dead runner states
#   3. systemd timer running the watchdog every 5 minutes
#
# Usage:
#   sudo bash setup-auto-recovery.sh            # install
#   sudo bash setup-auto-recovery.sh --uninstall # remove everything
#
# Idempotent: safe to run multiple times.
# NOTE: This script targets GNU/Linux (RHEL/AlmaLinux). It uses GNU coreutils
# extensions (stat -c, find -printf) that are not available on macOS/BSD.

set -euo pipefail

RUNNER_SERVICE="actions.runner.quasar-team-quasar.vmatldcsdoc-1.service"
OVERRIDE_DIR="/etc/systemd/system/${RUNNER_SERVICE}.d"
WATCHDOG_SCRIPT="/usr/local/bin/quasar-runner-watchdog.sh"
WATCHDOG_SERVICE="quasar-runner-watchdog.service"
WATCHDOG_TIMER="quasar-runner-watchdog.timer"
RUNNER_HOME="/home/quasar/actions-runner"
V1_FLOW_DROPIN="${OVERRIDE_DIR}/v1flow.conf"

# ---------- uninstall ----------
if [[ "${1:-}" == "--uninstall" ]]; then
    echo "Removing auto-recovery components..."
    systemctl stop "${WATCHDOG_TIMER}" 2>/dev/null || true
    systemctl disable "${WATCHDOG_TIMER}" 2>/dev/null || true
    rm -f "/etc/systemd/system/${WATCHDOG_TIMER}"
    rm -f "/etc/systemd/system/${WATCHDOG_SERVICE}"
    rm -f "${OVERRIDE_DIR}/override.conf"
    rm -f "${V1_FLOW_DROPIN}"
    rmdir "${OVERRIDE_DIR}" 2>/dev/null || true
    rm -f "${WATCHDOG_SCRIPT}"
    systemctl daemon-reload
    echo "Done. Runner service override, v1 flow drop-in, and watchdog removed."
    exit 0
fi

# ---------- preflight checks ----------
if [[ $EUID -ne 0 ]]; then
    echo "Error: run as root (sudo)." >&2
    exit 1
fi

if ! systemctl cat "${RUNNER_SERVICE}" &>/dev/null; then
    echo "Error: ${RUNNER_SERVICE} not found. Is the runner installed?" >&2
    exit 1
fi

# ---------- 1. systemd override: auto-restart on failure ----------
echo "[1/3] Installing systemd override for ${RUNNER_SERVICE}..."
mkdir -p "${OVERRIDE_DIR}"
cat > "${OVERRIDE_DIR}/override.conf" <<'UNIT'
[Unit]
StartLimitIntervalSec=300
StartLimitBurst=5

[Service]
Restart=always
RestartSec=15
UNIT

# Preserve existing v1 flow drop-in if present, otherwise create it.
# This forces the legacy HTTP polling flow, which is compatible with
# CERN's proxy/firewall (the v2 websocket broker flow is not).
if [[ ! -f "${V1_FLOW_DROPIN}" ]]; then
    echo "    Installing v1 flow drop-in (CERN proxy compatibility)..."
    cat > "${V1_FLOW_DROPIN}" <<'UNIT'
[Service]
Environment=ACTIONS_RUNNER_USE_V2_FLOW=false
UNIT
fi

systemctl daemon-reload
echo "    Override installed. Runner will auto-restart on crash/exit."

# ---------- 2. watchdog script ----------
echo "[2/3] Installing watchdog script at ${WATCHDOG_SCRIPT}..."
cat > "${WATCHDOG_SCRIPT}" <<'WATCHDOG'
#!/usr/bin/env bash
# quasar-runner-watchdog.sh — detect and recover dead/zombie runner states
# Runs via systemd timer every 5 minutes. Logs to journald (tag: quasar-runner-watchdog).
# NOTE: Uses GNU coreutils (stat -c, find -printf). Linux only.

set -euo pipefail

RUNNER_SERVICE="actions.runner.quasar-team-quasar.vmatldcsdoc-1.service"
RUNNER_HOME="/home/quasar/actions-runner"
LOG_TAG="quasar-runner-watchdog"

log() { logger -t "${LOG_TAG}" "$*"; }

# --- Check 1: Is the service running? ---
if ! systemctl is-active --quiet "${RUNNER_SERVICE}"; then
    log "WARN: Runner service is not active. Restarting..."
    systemctl restart "${RUNNER_SERVICE}"
    sleep 5
    if systemctl is-active --quiet "${RUNNER_SERVICE}"; then
        log "OK: Runner service restarted successfully."
    else
        log "ERROR: Runner service failed to restart."
    fi
    exit 0
fi

# --- Check 2: Can we reach GitHub? ---
if ! curl -sf --max-time 10 --retry 1 -o /dev/null https://api.github.com; then
    log "WARN: Cannot reach api.github.com. Network may be down. Skipping further checks."
    exit 0
fi

# --- Check 3: Zombie detection via runner listener log ---
# The runner writes to Runner_*.log during session refreshes and job polls.
# If the log file hasn't been modified in 15+ minutes AND the runner process
# has no active network connections to GitHub, it is likely in a zombie state
# (process alive but not polling for jobs).
# Threshold is 900s (15 min) to avoid false-positives on legitimately idle runners.
DIAG_DIR="${RUNNER_HOME}/_diag"
if [[ -d "${DIAG_DIR}" ]]; then
    LATEST_RUNNER_LOG=$(find "${DIAG_DIR}" -name 'Runner_*.log' -printf '%T@ %p\n' 2>/dev/null \
        | sort -rn | head -1 | cut -d' ' -f2-)

    if [[ -n "${LATEST_RUNNER_LOG:-}" ]]; then
        LOG_MTIME=$(stat -c %Y "${LATEST_RUNNER_LOG}" 2>/dev/null || echo 0)
        NOW=$(date +%s)
        AGE=$(( NOW - LOG_MTIME ))
        if [[ ${AGE} -gt 900 ]]; then
            # Double-check: if the runner process has active network connections,
            # it may just be idle (no jobs). Only restart if truly disconnected.
            RUNNER_PID=$(systemctl show -p MainPID --value "${RUNNER_SERVICE}" 2>/dev/null || echo 0)
            if [[ -n "${RUNNER_PID}" && "${RUNNER_PID}" != "0" ]] && \
               ss -tnp 2>/dev/null | grep -q "pid=${RUNNER_PID}"; then
                log "OK: Runner log stale (${AGE}s) but process ${RUNNER_PID} has active connections. Skipping restart."
                exit 0
            fi

            log "WARN: Runner log stale (${AGE}s old) and no active connections. Possible zombie state. Restarting..."
            systemctl restart "${RUNNER_SERVICE}"
            sleep 5
            if systemctl is-active --quiet "${RUNNER_SERVICE}"; then
                log "OK: Runner restarted after zombie detection."
            else
                log "ERROR: Runner failed to restart after zombie detection."
            fi
            exit 0
        fi
    fi
else
    log "WARN: Diag directory ${DIAG_DIR} not found. Cannot check for zombie state."
fi

log "OK: Runner healthy."
WATCHDOG
chmod +x "${WATCHDOG_SCRIPT}"
echo "    Watchdog script installed."

# ---------- 3. systemd timer ----------
echo "[3/3] Installing watchdog timer..."

cat > "/etc/systemd/system/${WATCHDOG_SERVICE}" <<UNIT
[Unit]
Description=quasar runner watchdog — detect and recover dead/zombie runner
After=network-online.target

[Service]
Type=oneshot
ExecStart=${WATCHDOG_SCRIPT}
# Runs as root because it needs systemctl restart privileges
UNIT

cat > "/etc/systemd/system/${WATCHDOG_TIMER}" <<'UNIT'
[Unit]
Description=Run quasar runner watchdog every 5 minutes

[Timer]
OnBootSec=2min
OnUnitActiveSec=5min
Persistent=true

[Install]
WantedBy=timers.target
UNIT

systemctl daemon-reload
systemctl enable --now "${WATCHDOG_TIMER}"
echo "    Watchdog timer enabled and started."

# ---------- summary ----------
echo ""
echo "Auto-recovery setup complete."
echo ""
echo "Verify with:"
echo "  systemctl cat ${RUNNER_SERVICE}          # shows override"
echo "  systemctl list-timers | grep watchdog     # shows timer active"
echo "  journalctl -t quasar-runner-watchdog -f   # watch watchdog logs"
echo ""
echo "Test auto-restart:"
echo "  sudo systemctl stop ${RUNNER_SERVICE}"
echo "  sleep 20"
echo "  systemctl is-active ${RUNNER_SERVICE}     # should be 'active'"
