# Air Taxi eVTOL Pulse

Air Taxi eVTOL Pulse is a Qt Quick operations dashboard that simulates live eVTOL mission telemetry. It is built for both demo storytelling and engineering UI iteration.

## Executive Summary (Demo-Friendly)

This app visualizes an air taxi mission lifecycle in real time:

- Lift-off and climb behavior
- Cruise speed and altitude window
- Route direction and path guidance
- Ride stability and confidence indicators
- Safety/automation-related module navigation

Use this for flight-ops demonstrations, design reviews, and simulation walkthroughs.

## Quick Demo Flow (2-3 Minutes)

1. Open **Primary Flight & Navigation Data**.
2. Highlight mission phase transitions (hover, climb, cruise, descent).
3. Show how confidence/trend strips react to simulated telemetry.
4. Call out altitude color/pattern mapping and path guidance changes.
5. Switch top modules to explain DEP, Energy, FCS, and Safety coverage.

## Developer Quickstart

### Prerequisites

- Qt 6.7.2 with Qt Quick and Qt Quick Controls 2
- MinGW 64-bit toolchain (matching Qt kit)
- Windows environment (current project setup)

### Build and Run (Qt Creator)

1. Open `AirTaxiPulse.pro`.
2. Select `Desktop Qt 6.7.2 MinGW 64-bit` kit.
3. Build in Debug or Release.
4. Run.

### Build from Terminal (example)

```powershell
Set-Location d:\Qt_Programs\My_Projects\AirTaxiPulse\build\Desktop_Qt_6_7_2_MinGW_64_bit_Debug
& "C:\Qt\Tools\mingw1120_64\bin\mingw32-make.exe" -j4
```

## Project Layout

- `src/app`: application bootstrap and runtime setup
- `src/view`: main QML shell (`main.qml`)
- `src/view/features`: module pages and `FlightSimulationCard`
- `src/viewmodel`: module registry and page-level viewmodels
- `src/services`: simulation and telemetry service logic
- `src/model`: telemetry data model
- `src/interfaces`: telemetry source contracts

## UI Modules

- Primary Flight & Navigation Data
- Propulsion & Distributed Electric Propulsion (DEP) Data
- Energy, Power & Battery Subsystems
- Flight Control System (FCS) & Automation
- Safety, Health & Environmental Monitoring

## Notes for Maintainers

- Keep card visuals readable at different window sizes.
- Validate layout behavior across common desktop resolutions.
- Preserve user-friendly wording in mission-facing labels.
- For visual changes, smoke-test both animation and text overlap.

## Known Environment Notes

- Multi-monitor startup placement can vary by window manager behavior.
- MinGW tools may require explicit PATH setup in terminal builds.

## License

Add your license information here.
