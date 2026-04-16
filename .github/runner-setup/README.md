# Runner Auto-Recovery Setup

One-time setup for the `vmatldcsdoc-1` self-hosted runner to automatically
recover from crashes, zombie states, and connection failures.

## What it installs

1. **systemd override** — `Restart=always` so the runner service auto-restarts
   on crash or exit (15s delay, up to 5 restarts per 5 minutes)
2. **v1 flow forcing** — `ACTIONS_RUNNER_USE_V2_FLOW=false` to avoid the v2
   websocket broker protocol that is incompatible with CERN's proxy/firewall
3. **Watchdog script + timer** — runs every 5 minutes, detects zombie states
   (process alive but not polling) by checking runner log staleness, and
   restarts the service if needed

## Install

```bash
ssh vmatldcsdoc-1
sudo bash /path/to/setup-auto-recovery.sh
```

Or copy it to the machine first:

```bash
scp .github/runner-setup/setup-auto-recovery.sh vmatldcsdoc-1:/tmp/
ssh vmatldcsdoc-1 'sudo bash /tmp/setup-auto-recovery.sh'
```

## Verify

```bash
# Check systemd override is applied
systemctl cat actions.runner.quasar-team-quasar.vmatldcsdoc-1.service

# Check watchdog timer is running
systemctl list-timers | grep watchdog

# Watch watchdog logs
journalctl -t quasar-runner-watchdog -f

# Test auto-restart (stops service, waits, checks it came back)
sudo systemctl stop actions.runner.quasar-team-quasar.vmatldcsdoc-1.service
sleep 20
systemctl is-active actions.runner.quasar-team-quasar.vmatldcsdoc-1.service
# Expected output: active
```

## Uninstall

```bash
sudo bash setup-auto-recovery.sh --uninstall
```
