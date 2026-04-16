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
    rmdir "${OVERRIDE_DIR}" 2>/dev/null || true
    rm -f "${WATCHDOG_SCRIPT}"
    systemctl daemon-reload
    echo "Done. Runner service override and watchdog removed."
    exit 0
fi

# ---------- preflight checks ----------
if [[ $EUID -ne 0 ]]; then
    echo "Error: run as root (sudo)." >&2
    exit 1
fi

if ! systemctl list-unit-files "${RUNNER_SERVICE}" &>/dev/null; then
    echo "Error: ${RUNNER_SERVICE} not found. Is the runner installed?" >&2
    exit 1
fi

# ---------- 1. systemd override: auto-restart on failure ----------
echo "[1/3] Installing systemd override for ${RUNNER_SERVICE}..."
mkdir -p "${OVERRIDE_DIR}"
cat > "${OVERRIDE_DIR}/override.conf" <<'UNIT'
[Service]
Restart=always
RestartSec=15
StartLimitIntervalSec=300
StartLimitBurst=5
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
if ! curl -sf --max-time 10 -o /dev/null https://github.com; then
    log "WARN: Cannot reach github.com. Network may be down. Skipping further checks."
    exit 0
fi

# --- Check 3: Zombie detection via runner listener log ---
# The runner writes a Worker log file when it's actively listening.
# If the most recent log entry is older than 10 minutes, the runner
# may be in a zombie state (process alive but not polling for jobs).
DIAG_DIR="${RUNNER_HOME}/_diag"
if [[ -d "${DIAG_DIR}" ]]; then
    LATEST_WORKER_LOG=$(find "${DIAG_DIR}" -name 'Worker_*.log' -printf '%T@ %p\n' 2>/dev/null \
        | sort -rn | head -1 | cut -d' ' -f2-)
    LATEST_RUNNER_LOG=$(find "${DIAG_DIR}" -name 'Runner_*.log' -printf '%T@ %p\n' 2>/dev/null \
        | sort -rn | head -1 | cut -d' ' -f2-)

    # Check the Runner log for signs of life (session refreshes, job polls).
    # If the log file hasn't been modified in 10+ minutes, the runner is likely zombie'd.
    if [[ -n "${LATEST_RUNNER_LOG:-}" ]]; then
        LOG_MTIME=$(stat -c %Y "${LATEST_RUNNER_LOG}" 2>/dev/null || echo 0)
        NOW=$(date +%s)
        AGE=$(( NOW - LOG_MTIME ))
        if [[ ${AGE} -gt 600 ]]; then
            log "WARN: Runner log stale (${AGE}s old). Possible zombie state. Restarting..."
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
