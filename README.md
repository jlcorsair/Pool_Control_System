# Pool Control Logging Research

Last updated: 2026-06-25

## Goal

Build a logging system for the pool controller that does not require the Windows
ScreenLogic PC application to run continuously. The target logger is an ESP32
that collects controller data and publishes log files to GitHub.

## Local Materials Reviewed

- `Pentair ScreenLogic/` contains Pentair ScreenLogic Connect installer assets,
  protocol adapter update files, and an IntelliCenter user guide.
- `Pentair ScreenLogic/screenlogic-connect.nuspec` identifies the package as
  ScreenLogic Connect `5.2.738.0020220206`.
- `Pentair ScreenLogic/tools/ChocolateyInstall.ps1` points to Pentair's
  ScreenLogic installer and protocol adapter firmware updater. It labels the
  adapter firmware update as build `738.0`, released `2019-03-26`.
- The ScreenLogic package metadata says ScreenLogic historical data monitors
  energy use and records pool, spa, and air temperatures.
- The IntelliCenter user guide references usage history, pH/ORP graph items,
  and USB import/export of status/configuration data. That is local controller
  functionality, not a documented network API.
- Extracted Windows payload from `setup.exe` contains:
  - `CRYSTALPAD.exe`
  - `HLCONFIG.exe`
  - `pentair_start.exe`
  - `zip.csv`
- Strings inside the Windows binaries include:
  - `screenlogicserver.pentair.com`
  - `screenlogicmonitor.pentair.com`
  - `homelogic.com`
  - `RS485 Half Duplex`
  - `RS485 Full Duplex`
  - `Initializing History Files`
  - `HISTORY`
  - `ScreenLogic`

## Main Finding

I did not find an official Pentair logging API in the local files. The usable
paths appear to be community-documented or reverse engineered:

1. Use the ScreenLogic protocol adapter over the LAN.
2. Connect an ESP32 directly to the Pentair RS-485 bus.
3. For IntelliCenter v3 systems, use the local IntelliCenter network WebSocket
   API if available.

## Option 1: ScreenLogic Protocol Adapter Over LAN

This is likely the closest match to the current Windows app workflow. The
ScreenLogic protocol adapter is the "brick" that the PC application talks to.

Observed from local files:

- Adapter update instructions mention the default adapter address
  `192.168.2.2`.
- The bundled updater tools are firmware/update utilities, not a logging API.
- The application binaries reference Pentair/ScreenLogic relay services and
  local history initialization.

Community references:

- `node-screenlogic`: https://github.com/parnic/node-screenlogic
- `screenlogicpy`: https://github.com/dieselrabbit/screenlogicpy
- Home Assistant ScreenLogic integration uses this general approach.

Known protocol shape from community implementations:

- Local adapter discovery uses UDP broadcast, commonly on port `1444`.
- Local control/data access then uses TCP to the protocol adapter, commonly
  port `80`.
- Remote lookup can involve Pentair relay/address services such as
  `screenlogicserver.pentair.com`.
- Community libraries expose current controller state and history-oriented
  ScreenLogic messages, including temperature and chemistry-related data.

Likely ESP32 approach:

- Implement just the small subset of ScreenLogic binary messages needed for
  logging.
- Poll current state on a schedule.
- If needed, also query history/chemistry history messages.
- Write logs locally to flash/SD first, then upload batched records.

Pros:

- No wiring into the automation panel if the ScreenLogic adapter is already on
  the LAN.
- Closest to replacing the Windows app.

Cons:

- Protocol is reverse engineered, not officially supported.
- ESP32 implementation work is non-trivial because the protocol is binary.

## Option 2: ESP32 on RS-485 Bus

Pentair automation systems use RS-485 between the controller and equipment.
This is a strong ESP32-native approach if wiring access is acceptable.

Community references:

- OpenHAB Pentair binding:
  https://www.openhab.org/addons/bindings/pentair/
- OPNpool ESPHome project:
  https://github.com/cvonk/OPNpool_meets_ESPHome
- nodejs-poolController:
  https://github.com/tagyoureit/nodejs-poolController

Important protocol notes from public/community documentation:

- Physical layer: RS-485.
- Typical serial settings: `9600,8N1`.
- Pentair messages are proprietary binary frames.
- OpenHAB documents Pentair frames as starting with `FF 00 FF A5`.

Likely ESP32 hardware:

- ESP32 development board.
- 3.3 V RS-485 transceiver, for example MAX3485-compatible hardware.
- Proper A/B wiring to the pool controller RS-485 bus.
- Optional SD card for local buffering.

Pros:

- Does not depend on the ScreenLogic protocol adapter.
- Direct access to the bus used by pumps, chlorinators, heaters, and chemistry
  devices.
- Existing ESPHome and nodejs-poolController work can guide the implementation.

Cons:

- Requires wiring into the controller bus.
- Must avoid bus contention and unsafe write behavior.
- Full protocol support is broad; for logging, implement read-only/passive
  capture first.

## Option 3: IntelliCenter Local Network API

If the installed controller is IntelliCenter rather than only
EasyTouch/IntelliTouch with ScreenLogic, the local network API may be the best
path.

The `nodejs-poolController` README in this workspace states:

- IntelliCenter v3.004+ exposes a JSON WebSocket API on port `6680`.
- It can use "IntelliCenter Network" as an alternative to RS-485.
- The local WebSocket is trusted-LAN-only and should not be exposed to the
  internet.

This option should be verified against the actual controller model and firmware.

## GitHub Logging

GitHub can store logs using the REST API endpoint for creating or updating file
contents:

https://docs.github.com/en/rest/repos/contents#create-or-update-file-contents

Practical design:

- Use daily newline-delimited JSON files:
  - `logs/2026-06-25.ndjson`
  - `logs/2026-06-26.ndjson`
- Batch writes instead of committing every sample.
- Prefer one upload every 15 minutes, hourly, or daily.
- Keep each record small and self-describing.

Example record:

```json
{"ts":"2026-06-25T16:00:00Z","air_f":78,"pool_f":82,"spa_f":80,"pump_rpm":1800,"orp_mv":720,"ph":7.5}
```

Security guidance:

- Do not put a broad GitHub token on the ESP32.
- If the ESP32 writes directly to GitHub, use a fine-grained token restricted
  to one repository with Contents write permission.
- Safer architecture: ESP32 posts to a small relay service, and the relay writes
  to GitHub.

## Recommended Build Path

1. Identify the installed controller model:
   - ScreenLogic with EasyTouch/IntelliTouch
   - IntelliCenter
   - Standalone IntelliChem or other equipment
2. Confirm whether the ScreenLogic protocol adapter is reachable on the LAN.
3. If it is reachable, prototype data collection with an existing library first:
   - `node-screenlogic`
   - `screenlogicpy`
   - `nodejs-poolController`
4. Decide hardware path:
   - Use ScreenLogic-over-Wi-Fi if no panel wiring is desired.
   - Use ESP32 RS-485 if direct bus logging is acceptable.
   - Use IntelliCenter port `6680` if the controller supports it.
5. Store logs locally first, then batch-upload to GitHub.
6. Keep the first version read-only. Add control/write features only after the
   logging path is reliable.

## Current Repository Notes

- `Pentair ScreenLogic/` is retained as the local reference material.
- `nodejs-poolController/` is present as a local community implementation
  reference. It supports RS-485, ScreenLogic network connections, REST APIs,
  WebSocket APIs, MQTT, InfluxDB, and Home Assistant integrations.
- This repository is intended to track research, hardware notes, and future
  ESP32 logger code.
