/**
 * @file    : src/viewmodel/OperationalViewModels.h
 * @brief   : Declares view models for operational aircraft subsystems.
 * @author  : Abhinay Chauhan (email: abhinay.chauhan1@gmail.com)
 * @version : 1.0.0
 *
 * Copyright (c) 2024
 * Abhinay Chauhan. All rights reserved.
 */

#ifndef OPERATIONALVIEWMODELS_H
#define OPERATIONALVIEWMODELS_H

#include <QDateTime>
#include <QObject>
#include <QTimer>
#include <QVariantList>

#include "../interfaces/IFlightTelemetrySource.h"

/// @brief Exposes current date, time, and elapsed session time to QML.
class FlightSessionViewModel : public QObject
{
    Q_OBJECT
    /// @brief Current local date formatted for display.
    Q_PROPERTY(QString currentDateText READ currentDateText NOTIFY sessionChanged)
    /// @brief Current local time formatted for display.
    Q_PROPERTY(QString currentTimeText READ currentTimeText NOTIFY sessionChanged)
    /// @brief Elapsed session duration formatted as hours, minutes, and seconds.
    Q_PROPERTY(QString elapsedText READ elapsedText NOTIFY sessionChanged)

public:
    /// @brief Constructs and starts the one-second session clock.
    /// @param parent Optional QObject parent for Qt ownership.
    explicit FlightSessionViewModel(QObject *parent = nullptr);

    /// @brief Returns the formatted current date.
    /// @return Date text for the session display.
    QString currentDateText() const;
    /// @brief Returns the formatted current time.
    /// @return Time text for the session display.
    QString currentTimeText() const;
    /// @brief Returns the formatted elapsed session duration.
    /// @return Duration in hh:mm:ss form.
    QString elapsedText() const;

signals:
    /// @brief Emitted once per second when session time values change.
    void sessionChanged();

private:
    QDateTime m_currentDateTime;
    int m_elapsedSeconds;
    QTimer m_timer;
};

/// @brief Derives propulsion status, progress, and warnings from aircraft telemetry.
class PropulsionSystemViewModel : public QObject
{
    Q_OBJECT
    /// @brief Average RPM of the front motor group.
    Q_PROPERTY(double frontGroupRpm READ frontGroupRpm NOTIFY telemetryChanged)
    /// @brief Average RPM of the rear motor group.
    Q_PROPERTY(double rearGroupRpm READ rearGroupRpm NOTIFY telemetryChanged)
    /// @brief Average RPM across all motors.
    Q_PROPERTY(double averageRpm READ averageRpm NOTIFY telemetryChanged)
    /// @brief Average temperature across all motors.
    Q_PROPERTY(double averageMotorTemperature READ averageMotorTemperature NOTIFY telemetryChanged)
    /// @brief Average health percentage across all inverters.
    Q_PROPERTY(double inverterHealthAverage READ inverterHealthAverage NOTIFY telemetryChanged)
    /// @brief Front propulsion-group thrust.
    Q_PROPERTY(double frontThrust READ frontThrust NOTIFY telemetryChanged)
    /// @brief Rear propulsion-group thrust.
    Q_PROPERTY(double rearThrust READ rearThrust NOTIFY telemetryChanged)
    /// @brief Sum of front and rear thrust.
    Q_PROPERTY(double combinedThrust READ combinedThrust NOTIFY telemetryChanged)
    /// @brief Normalized front-to-rear thrust imbalance.
    Q_PROPERTY(double thrustBalance READ thrustBalance NOTIFY telemetryChanged)
    /// @brief Total inverter electrical power in kilowatts.
    Q_PROPERTY(double electricalPowerKw READ electricalPowerKw NOTIFY telemetryChanged)
    /// @brief Overall propulsion health label.
    Q_PROPERTY(QString stateLabel READ stateLabel NOTIFY telemetryChanged)
    /// @brief Normalized motor RPM progress.
    Q_PROPERTY(double rpmProgress READ rpmProgress NOTIFY telemetryChanged)
    /// @brief Normalized tilt progress from hover toward cruise.
    Q_PROPERTY(double tiltProgress READ tiltProgress NOTIFY telemetryChanged)
    /// @brief Normalized motor temperature progress.
    Q_PROPERTY(double temperatureProgress READ temperatureProgress NOTIFY telemetryChanged)
    /// @brief Normalized combined-thrust progress.
    Q_PROPERTY(double thrustProgress READ thrustProgress NOTIFY telemetryChanged)
    /// @brief Normalized inverter-health progress.
    Q_PROPERTY(double inverterProgress READ inverterProgress NOTIFY telemetryChanged)
    /// @brief Hover- or cruise-biased tilt profile label.
    Q_PROPERTY(QString tiltProfileLabel READ tiltProfileLabel NOTIFY telemetryChanged)
    /// @brief Human-readable thrust-balance label.
    Q_PROPERTY(QString thrustBalanceLabel READ thrustBalanceLabel NOTIFY telemetryChanged)
    /// @brief Whether front and rear thrust are within the balance tolerance.
    Q_PROPERTY(bool thrustBalanced READ thrustBalanced NOTIFY telemetryChanged)
    /// @brief Per-motor thermal warning flags.
    Q_PROPERTY(QVariantList motorThermalWarnings READ motorThermalWarnings NOTIFY telemetryChanged)
    /// @brief Per-inverter health warning flags.
    Q_PROPERTY(QVariantList inverterHealthWarnings READ inverterHealthWarnings NOTIFY telemetryChanged)

public:
    /// @brief Constructs the propulsion adapter for a telemetry source.
    /// @param telemetrySource Source that supplies propulsion telemetry.
    /// @param parent Optional QObject parent for Qt ownership.
    explicit PropulsionSystemViewModel(IFlightTelemetrySource &telemetrySource,
                                       QObject *parent = nullptr);

    /// @brief Returns average front-group RPM.
    /// @return Front-group RPM.
    double frontGroupRpm() const;
    /// @brief Returns average rear-group RPM.
    /// @return Rear-group RPM.
    double rearGroupRpm() const;
    /// @brief Returns average RPM across all motors.
    /// @return Average motor RPM.
    double averageRpm() const;
    /// @brief Returns average motor temperature.
    /// @return Mean motor temperature.
    double averageMotorTemperature() const;
    /// @brief Returns average inverter health.
    /// @return Mean inverter-health percentage.
    double inverterHealthAverage() const;
    /// @brief Returns front-group thrust.
    /// @return Front thrust value.
    double frontThrust() const;
    /// @brief Returns rear-group thrust.
    /// @return Rear thrust value.
    double rearThrust() const;
    /// @brief Returns combined propulsion thrust.
    /// @return Sum of front and rear thrust.
    double combinedThrust() const;
    /// @brief Calculates normalized thrust imbalance.
    /// @return Signed front-to-rear balance ratio.
    double thrustBalance() const;
    /// @brief Calculates total inverter electrical power.
    /// @return Electrical power in kilowatts.
    double electricalPowerKw() const;
    /// @brief Returns the aggregate propulsion-state label.
    /// @return Thermal, degraded, or nominal state text.
    QString stateLabel() const;
    /// @brief Returns normalized RPM progress.
    /// @return Value clamped to [0, 1].
    double rpmProgress() const;
    /// @brief Returns normalized tilt progress.
    /// @return Value clamped to [0, 1].
    double tiltProgress() const;
    /// @brief Returns normalized temperature progress.
    /// @return Value clamped to [0, 1].
    double temperatureProgress() const;
    /// @brief Returns normalized thrust progress.
    /// @return Value clamped to [0, 1].
    double thrustProgress() const;
    /// @brief Returns normalized inverter-health progress.
    /// @return Value clamped to [0, 1].
    double inverterProgress() const;
    /// @brief Returns the tilt-profile label.
    /// @return Hover- or cruise-biased text.
    QString tiltProfileLabel() const;
    /// @brief Returns the thrust-balance label.
    /// @return Centered, front-bias, or rear-bias text.
    QString thrustBalanceLabel() const;
    /// @brief Reports whether thrust is within tolerance.
    /// @return true when absolute imbalance is below three percent.
    bool thrustBalanced() const;
    /// @brief Builds per-motor thermal warning flags.
    /// @return Warning flags aligned with motor temperature samples.
    QVariantList motorThermalWarnings() const;
    /// @brief Builds per-inverter health warning flags.
    /// @return Warning flags aligned with inverter health samples.
    QVariantList inverterHealthWarnings() const;

signals:
    /// @brief Emitted when propulsion telemetry changes.
    void telemetryChanged();

private:
    /// @brief Calculates the numeric average of a variant list.
    /// @param values Numeric values to average.
    /// @return Mean value, or 0 for an empty list.
    static double average(const QVariantList &values);
    /// @brief Safely reads a numeric value from a variant list.
    /// @param values Source values.
    /// @param index Requested index.
    /// @return Value at the index, or 0 when out of range.
    static double valueAt(const QVariantList &values, int index);

    IFlightTelemetrySource &m_telemetrySource;
};

/// @brief Derives battery, power-bus, and thermal presentation data from telemetry.
class EnergySystemViewModel : public QObject
{
    Q_OBJECT
    /// @brief Whether the current flight mode uses the hover power profile.
    Q_PROPERTY(bool hoverMode READ hoverMode NOTIFY telemetryChanged)
    /// @brief Estimated battery endurance in minutes.
    Q_PROPERTY(double estimatedMinutes READ estimatedMinutes NOTIFY telemetryChanged)
    /// @brief Current energy-system status enum value.
    Q_PROPERTY(int status READ status NOTIFY telemetryChanged)
    /// @brief Human-readable energy-system status.
    Q_PROPERTY(QString statusLabel READ statusLabel NOTIFY telemetryChanged)
    /// @brief Active hover or cruise power-profile label.
    Q_PROPERTY(QString powerProfileLabel READ powerProfileLabel NOTIFY telemetryChanged)
    /// @brief Estimated energy-system efficiency in percent.
    Q_PROPERTY(double efficiencyPercent READ efficiencyPercent NOTIFY telemetryChanged)
    /// @brief Battery-zone temperatures exposed to QML.
    Q_PROPERTY(QVariantList zoneTemperatures READ zoneTemperatures NOTIFY telemetryChanged)
    /// @brief Derived current for each power-bus channel.
    Q_PROPERTY(QVariantList busChannelCurrents READ busChannelCurrents NOTIFY telemetryChanged)
    /// @brief Derived power for each power-bus channel.
    Q_PROPERTY(QVariantList busChannelPowers READ busChannelPowers NOTIFY telemetryChanged)

public:
    /// @brief Classifies the aggregate energy-system condition.
    enum Status {
        Nominal,
        LowReserve,
        ThermalWarning
    };
    Q_ENUM(Status)

    /// @brief Constructs the energy adapter for a telemetry source.
    /// @param telemetrySource Source that supplies energy telemetry.
    /// @param parent Optional QObject parent for Qt ownership.
    explicit EnergySystemViewModel(IFlightTelemetrySource &telemetrySource,
                                   QObject *parent = nullptr);

    /// @brief Reports whether the active flight mode is hover-biased.
    /// @return true for hover or lift-off modes.
    bool hoverMode() const;
    /// @brief Estimates remaining endurance from charge and power demand.
    /// @return Estimated minutes, or 0 for negligible power demand.
    double estimatedMinutes() const;
    /// @brief Returns the aggregate energy-system status.
    /// @return A Status enum value exposed as an integer.
    int status() const;
    /// @brief Returns the energy-system status label.
    /// @return Human-readable status text.
    QString statusLabel() const;
    /// @brief Returns the active power-profile label.
    /// @return Hover or cruise profile text.
    QString powerProfileLabel() const;
    /// @brief Estimates energy-system efficiency.
    /// @return Efficiency percentage with a 72 percent floor.
    double efficiencyPercent() const;
    /// @brief Returns six battery-zone temperatures.
    /// @return QML-compatible temperature list.
    QVariantList zoneTemperatures() const;
    /// @brief Returns current allocated to five bus channels.
    /// @return QML-compatible current list.
    QVariantList busChannelCurrents() const;
    /// @brief Returns power allocated to five bus channels.
    /// @return QML-compatible power list.
    QVariantList busChannelPowers() const;

    /// @brief Returns a battery-zone temperature by index.
    /// @param index Zero-based zone index.
    /// @return Temperature, or 0 when out of range.
    Q_INVOKABLE double zoneTemperature(int index) const;
    /// @brief Returns a bus-channel display name.
    /// @param index Zero-based channel index.
    /// @return Channel name, or an empty string when out of range.
    Q_INVOKABLE QString busChannelName(int index) const;
    /// @brief Returns a compact bus-channel code.
    /// @param index Zero-based channel index.
    /// @return Channel code, or an empty string when out of range.
    Q_INVOKABLE QString busChannelCode(int index) const;
    /// @brief Calculates current allocated to a bus channel.
    /// @param index Zero-based channel index.
    /// @return Channel current, or 0 when out of range.
    Q_INVOKABLE double busChannelCurrent(int index) const;
    /// @brief Calculates power allocated to a bus channel.
    /// @param index Zero-based channel index.
    /// @return Channel power in kilowatts, or 0 when out of range.
    Q_INVOKABLE double busChannelPower(int index) const;

signals:
    /// @brief Emitted when energy telemetry changes.
    void telemetryChanged();

private:
    /// @brief Finds the maximum reported cell temperature.
    /// @return Maximum cell temperature, or 0 for no samples.
    double cellTemperatureMax() const;
    /// @brief Returns the configured load share for a bus channel.
    /// @param index Zero-based channel index.
    /// @return Fractional bus share, or 0 when out of range.
    double busChannelShare(int index) const;

    IFlightTelemetrySource &m_telemetrySource;
};

/// @brief Exposes flight-control modes, actuators, and route guidance to QML.
class FlightControlSystemViewModel : public QObject
{
    Q_OBJECT
    /// @brief Whether automatic flight control is engaged.
    Q_PROPERTY(bool autopilotEngaged READ autopilotEngaged NOTIFY stateChanged)
    /// @brief Selected guidance mode index.
    Q_PROPERTY(int guidanceMode READ guidanceMode NOTIFY stateChanged)
    /// @brief Operator-commanded flight mode, or -1 when following telemetry.
    Q_PROPERTY(int commandedFlightMode READ commandedFlightMode NOTIFY stateChanged)
    /// @brief Normalized telemetry flight-mode label.
    Q_PROPERTY(QString flightMode READ flightMode NOTIFY telemetryChanged)
    /// @brief Effective flight-mode index.
    Q_PROPERTY(int flightModeIndex READ flightModeIndex NOTIFY stateChanged)
    /// @brief Effective left elevon command in degrees.
    Q_PROPERTY(double leftElevon READ leftElevon NOTIFY stateChanged)
    /// @brief Effective right elevon command in degrees.
    Q_PROPERTY(double rightElevon READ rightElevon NOTIFY stateChanged)
    /// @brief Effective ruddervator command in degrees.
    Q_PROPERTY(double ruddervator READ ruddervator NOTIFY stateChanged)
    /// @brief Effective propulsion tilt-actuator command in degrees.
    Q_PROPERTY(double tiltActuator READ tiltActuator NOTIFY stateChanged)
    /// @brief Effective actuator values in display order.
    Q_PROPERTY(QVariantList actuatorValues READ actuatorValues NOTIFY stateChanged)
    /// @brief Immutable route waypoint definitions.
    Q_PROPERTY(QVariantList waypoints READ waypoints CONSTANT)
    /// @brief Whether navigation follows the automatic waypoint sequence.
    Q_PROPERTY(bool automaticRouteSequence READ automaticRouteSequence NOTIFY stateChanged)
    /// @brief Effective navigation waypoint index.
    Q_PROPERTY(int navigationWaypoint READ navigationWaypoint NOTIFY stateChanged)
    /// @brief Great-circle distance to the active waypoint in nautical miles.
    Q_PROPERTY(double nextWaypointDistance READ nextWaypointDistance NOTIFY stateChanged)
    /// @brief Automatic-sequence or direct-to route status.
    Q_PROPERTY(QString routeLegStatus READ routeLegStatus NOTIFY stateChanged)

public:
    /// @brief Constructs flight-control state and the fixed demonstration route.
    /// @param telemetrySource Source that supplies flight and navigation telemetry.
    /// @param parent Optional QObject parent for Qt ownership.
    explicit FlightControlSystemViewModel(IFlightTelemetrySource &telemetrySource,
                                          QObject *parent = nullptr);

    /// @brief Returns whether autopilot is engaged.
    /// @return true when automatic actuator commands are active.
    bool autopilotEngaged() const;
    /// @brief Returns the selected guidance mode.
    /// @return Guidance mode index.
    int guidanceMode() const;
    /// @brief Returns the commanded flight mode.
    /// @return Mode index, or -1 when following telemetry.
    int commandedFlightMode() const;
    /// @brief Normalizes source flight mode for the controls UI.
    /// @return HOVER, TRANSITION, CRUISE, or VERTICAL LANDING.
    QString flightMode() const;
    /// @brief Returns the effective flight-mode index.
    /// @return Commanded mode when set; otherwise the telemetry mode.
    int flightModeIndex() const;
    /// @brief Returns the effective left elevon command.
    /// @return Left elevon angle in degrees.
    double leftElevon() const;
    /// @brief Returns the effective right elevon command.
    /// @return Right elevon angle in degrees.
    double rightElevon() const;
    /// @brief Returns the effective ruddervator command.
    /// @return Ruddervator angle in degrees.
    double ruddervator() const;
    /// @brief Returns the effective tilt-actuator command.
    /// @return Tilt angle in degrees.
    double tiltActuator() const;
    /// @brief Returns all effective actuator commands.
    /// @return Values ordered by actuator index.
    QVariantList actuatorValues() const;
    /// @brief Returns immutable route waypoints.
    /// @return Waypoint maps for QML.
    QVariantList waypoints() const;
    /// @brief Reports whether route sequencing is automatic.
    /// @return true for automatic sequencing.
    bool automaticRouteSequence() const;
    /// @brief Returns the active navigation waypoint.
    /// @return Effective waypoint index.
    int navigationWaypoint() const;
    /// @brief Calculates distance to the active waypoint.
    /// @return Great-circle distance in nautical miles.
    double nextWaypointDistance() const;
    /// @brief Returns the route-leg status.
    /// @return AUTO SEQUENCE or DIRECT TO.
    QString routeLegStatus() const;

    /// @brief Returns a flight-mode name by index.
    /// @param index Mode index.
    /// @return Mode name, or an empty string when out of range.
    Q_INVOKABLE QString modeName(int index) const;
    /// @brief Returns a guidance-mode name by index.
    /// @param index Guidance index.
    /// @return Guidance name, or an empty string when out of range.
    Q_INVOKABLE QString guidanceName(int index) const;
    /// @brief Returns an actuator name by index.
    /// @param index Actuator index.
    /// @return Actuator name, or an empty string when out of range.
    Q_INVOKABLE QString actuatorName(int index) const;
    /// @brief Returns an effective actuator value by index.
    /// @param index Actuator index.
    /// @return Effective actuator command.
    Q_INVOKABLE double actuatorValue(int index) const;
    /// @brief Returns the minimum command for an actuator.
    /// @param index Actuator index.
    /// @return Minimum command value.
    Q_INVOKABLE double actuatorMinimum(int index) const;
    /// @brief Returns the maximum command for an actuator.
    /// @param index Actuator index.
    /// @return Maximum command value.
    Q_INVOKABLE double actuatorMaximum(int index) const;
    /// @brief Clears the operator flight-mode override.
    Q_INVOKABLE void clearCommandedFlightMode();
    /// @brief Selects an operator-commanded flight mode.
    /// @param index Mode index clamped to [0, 3].
    Q_INVOKABLE void setCommandedFlightMode(int index);
    /// @brief Selects a guidance mode.
    /// @param index Guidance index clamped to [0, 2].
    Q_INVOKABLE void setGuidanceMode(int index);
    /// @brief Toggles automatic flight control while preserving current commands.
    Q_INVOKABLE void toggleAutopilot();
    /// @brief Sets a bounded manual actuator command.
    /// @param index Actuator index.
    /// @param value Requested command value.
    Q_INVOKABLE void setManualActuator(int index, double value);
    /// @brief Selects a direct-to navigation waypoint.
    /// @param index Valid waypoint index.
    Q_INVOKABLE void selectDirectTo(int index);
    /// @brief Resumes automatic route sequencing.
    Q_INVOKABLE void resumeAutomaticRoute();

signals:
    /// @brief Emitted when source flight telemetry changes.
    void telemetryChanged();
    /// @brief Emitted when operator-controlled flight-control state changes.
    void stateChanged();

private:
    /// @brief Clamps a value to inclusive bounds.
    /// @return Bounded value.
    static double clamp(double value, double minimum, double maximum);
    /// @brief Maps normalized telemetry mode text to an index.
    /// @return Telemetry flight-mode index.
    int telemetryFlightModeIndex() const;
    /// @brief Selects the automatic waypoint from the effective flight mode.
    /// @return Automatic waypoint index.
    int automaticWaypoint() const;
    /// @brief Calculates great-circle distance to a waypoint.
    /// @param index Waypoint index.
    /// @return Distance in nautical miles, or 0 when out of range.
    double distanceToWaypoint(int index) const;

    IFlightTelemetrySource &m_telemetrySource;
    bool m_autopilotEngaged;
    int m_guidanceMode;
    int m_commandedFlightMode;
    bool m_automaticRouteSequence;
    int m_selectedWaypoint;
    double m_manualLeftElevon;
    double m_manualRightElevon;
    double m_manualRuddervator;
    double m_manualTiltActuator;
    QVariantList m_waypoints;
};

/// @brief Simulates and exposes acoustic, navigation, traffic, and parachute safety state.
class SafetySystemViewModel : public QObject
{
    Q_OBJECT
    /// @brief Configured acoustic warning threshold in decibels.
    Q_PROPERTY(int noiseLimit READ noiseLimit NOTIFY stateChanged)
    /// @brief Recent acoustic-level history.
    Q_PROPERTY(QVariantList noiseHistory READ noiseHistory NOTIFY stateChanged)
    /// @brief Current simulated acoustic level in dBA.
    Q_PROPERTY(double acousticDb READ acousticDb NOTIFY stateChanged)
    /// @brief Whether acoustic level meets or exceeds the configured limit.
    Q_PROPERTY(bool noiseWarning READ noiseWarning NOTIFY stateChanged)
    /// @brief Simulated number of visible navigation satellites.
    Q_PROPERTY(int satelliteCount READ satelliteCount NOTIFY stateChanged)
    /// @brief Simulated GNSS signal strength percentage.
    Q_PROPERTY(double signalStrength READ signalStrength NOTIFY stateChanged)
    /// @brief Simulated navigation accuracy in meters.
    Q_PROPERTY(double navigationAccuracy READ navigationAccuracy NOTIFY stateChanged)
    /// @brief Whether receiver autonomous integrity monitoring is available.
    Q_PROPERTY(bool raimAvailable READ raimAvailable NOTIFY stateChanged)
    /// @brief Simulated lateral path deviation in meters.
    Q_PROPERTY(double lateralDeviation READ lateralDeviation NOTIFY stateChanged)
    /// @brief Simulated vertical path deviation in meters.
    Q_PROPERTY(double verticalDeviation READ verticalDeviation NOTIFY stateChanged)
    /// @brief Combined path-alignment quality percentage.
    Q_PROPERTY(double alignmentQuality READ alignmentQuality NOTIFY stateChanged)
    /// @brief Whether telemetry indicates approach or low-altitude landing flight.
    Q_PROPERTY(bool landingPhase READ landingPhase NOTIFY stateChanged)
    /// @brief Ballistic recovery system state index.
    Q_PROPERTY(int parachuteState READ parachuteState NOTIFY stateChanged)
    /// @brief Normalized parachute deployment progress.
    Q_PROPERTY(double parachuteProgress READ parachuteProgress NOTIFY stateChanged)
    /// @brief Whether test override bypasses the deployment envelope.
    Q_PROPERTY(bool parachuteTestOverride READ parachuteTestOverride NOTIFY stateChanged)
    /// @brief Time of the latest parachute state event.
    Q_PROPERTY(QString parachuteEventTime READ parachuteEventTime NOTIFY stateChanged)
    /// @brief Whether altitude and airspeed satisfy the deployment envelope.
    Q_PROPERTY(bool parachuteEnvelopeValid READ parachuteEnvelopeValid NOTIFY stateChanged)
    /// @brief Whether the armed parachute may deploy.
    Q_PROPERTY(bool parachuteDeployAllowed READ parachuteDeployAllowed NOTIFY stateChanged)
    /// @brief Deployment-envelope status or inhibit reason.
    Q_PROPERTY(QString parachuteInhibitReason READ parachuteInhibitReason NOTIFY stateChanged)
    /// @brief Whether degraded GNSS test values are active.
    Q_PROPERTY(bool gnssTestMode READ gnssTestMode NOTIFY stateChanged)
    /// @brief Selected traffic target index.
    Q_PROPERTY(int selectedTraffic READ selectedTraffic NOTIFY stateChanged)
    /// @brief Simulated ranges for traffic targets in nautical miles.
    Q_PROPERTY(QVariantList trafficRanges READ trafficRanges NOTIFY stateChanged)
    /// @brief Simulated bearings for traffic targets in degrees.
    Q_PROPERTY(QVariantList trafficBearings READ trafficBearings NOTIFY stateChanged)
    /// @brief Summary text for each safety monitor.
    Q_PROPERTY(QVariantList monitorSummaries READ monitorSummaries NOTIFY stateChanged)
    /// @brief Number of active acoustic, GNSS, and traffic alerts.
    Q_PROPERTY(int alertCount READ alertCount NOTIFY stateChanged)

public:
    /// @brief Constructs safety simulation state and starts its update timers.
    /// @param telemetrySource Source that supplies aircraft telemetry.
    /// @param parent Optional QObject parent for Qt ownership.
    explicit SafetySystemViewModel(IFlightTelemetrySource &telemetrySource,
                                   QObject *parent = nullptr);

    /// @brief Returns the acoustic warning limit.
    /// @return Limit in decibels.
    int noiseLimit() const;
    /// @brief Returns recent acoustic history.
    /// @return Acoustic-level samples.
    QVariantList noiseHistory() const;
    /// @brief Calculates the current simulated acoustic level.
    /// @return Acoustic level in dBA.
    double acousticDb() const;
    /// @brief Reports whether the acoustic limit is exceeded.
    /// @return true when acousticDb is at least noiseLimit.
    bool noiseWarning() const;
    /// @brief Returns simulated visible satellite count.
    /// @return Satellite count.
    int satelliteCount() const;
    /// @brief Returns simulated GNSS signal strength.
    /// @return Signal strength percentage.
    double signalStrength() const;
    /// @brief Returns simulated navigation accuracy.
    /// @return Horizontal accuracy in meters.
    double navigationAccuracy() const;
    /// @brief Reports GNSS integrity availability.
    /// @return true when satellite count and accuracy pass thresholds.
    bool raimAvailable() const;
    /// @brief Calculates lateral path deviation.
    /// @return Bounded lateral deviation in meters.
    double lateralDeviation() const;
    /// @brief Calculates vertical path deviation.
    /// @return Bounded vertical deviation in meters.
    double verticalDeviation() const;
    /// @brief Calculates combined alignment quality.
    /// @return Quality percentage clamped to [0, 100].
    double alignmentQuality() const;
    /// @brief Reports whether the aircraft is in a landing phase.
    /// @return true for arrival mode or radar altitude below 150 feet.
    bool landingPhase() const;
    /// @brief Returns the parachute state.
    /// @return Safe, armed, deploying, or deployed state index.
    int parachuteState() const;
    /// @brief Returns parachute deployment progress.
    /// @return Normalized progress value.
    double parachuteProgress() const;
    /// @brief Returns test-override state.
    /// @return true when deployment-envelope override is enabled.
    bool parachuteTestOverride() const;
    /// @brief Returns the latest parachute event time.
    /// @return Time text in hh:mm:ss form.
    QString parachuteEventTime() const;
    /// @brief Validates the telemetry deployment envelope.
    /// @return true when altitude and airspeed are within limits.
    bool parachuteEnvelopeValid() const;
    /// @brief Reports whether parachute deployment is permitted.
    /// @return true when armed and the envelope or override is valid.
    bool parachuteDeployAllowed() const;
    /// @brief Returns deployment-envelope status text.
    /// @return Inhibit reason or valid-envelope text.
    QString parachuteInhibitReason() const;
    /// @brief Returns degraded GNSS test-mode state.
    /// @return true when test values are active.
    bool gnssTestMode() const;
    /// @brief Returns the selected traffic target.
    /// @return Traffic index.
    int selectedTraffic() const;
    /// @brief Returns all simulated traffic ranges.
    /// @return Three range values in nautical miles.
    QVariantList trafficRanges() const;
    /// @brief Returns all simulated traffic bearings.
    /// @return Three bearing values in degrees.
    QVariantList trafficBearings() const;
    /// @brief Returns summaries for all safety monitors.
    /// @return Five monitor summary strings.
    QVariantList monitorSummaries() const;
    /// @brief Counts active acoustic, GNSS, and proximity alerts.
    /// @return Number of active alerts.
    int alertCount() const;

    /// @brief Sets the bounded acoustic warning limit.
    /// @param value Requested limit clamped to [40, 100].
    Q_INVOKABLE void setNoiseLimit(int value);
    /// @brief Toggles degraded GNSS test values.
    Q_INVOKABLE void toggleGnssTestMode();
    /// @brief Toggles parachute deployment-envelope override when permitted.
    Q_INVOKABLE void toggleParachuteTestOverride();
    /// @brief Toggles the parachute between safe and armed states.
    Q_INVOKABLE void toggleParachuteArm();
    /// @brief Starts an allowed deployment or resets a completed deployment.
    Q_INVOKABLE void deployOrResetParachute();
    /// @brief Selects a traffic target.
    /// @param index Traffic index clamped to [0, 2].
    Q_INVOKABLE void setSelectedTraffic(int index);
    /// @brief Returns a safety-monitor code.
    /// @param index Monitor index.
    /// @return Monitor code, or an empty string when out of range.
    Q_INVOKABLE QString monitorCode(int index) const;
    /// @brief Formats a safety-monitor summary.
    /// @param index Monitor index.
    /// @param selectedTraffic Traffic index used by the TCAS summary.
    /// @return Human-readable monitor status.
    Q_INVOKABLE QString monitorSummary(int index, int selectedTraffic) const;
    /// @brief Formats a signed numeric value.
    /// @param value Number to format.
    /// @return Fixed-point text with an explicit positive sign.
    Q_INVOKABLE QString signedValue(double value) const;
    /// @brief Returns a traffic-target code.
    /// @param index Traffic index.
    /// @return Target code, or an empty string when out of range.
    Q_INVOKABLE QString trafficCode(int index) const;
    /// @brief Calculates simulated range to a traffic target.
    /// @param index Traffic index.
    /// @return Range in nautical miles, or 0 when out of range.
    Q_INVOKABLE double trafficRange(int index) const;
    /// @brief Calculates simulated bearing to a traffic target.
    /// @param index Traffic index.
    /// @return Bearing in degrees, or 0 when out of range.
    Q_INVOKABLE double trafficBearing(int index) const;

signals:
    /// @brief Emitted when source safety telemetry changes.
    void telemetryChanged();
    /// @brief Emitted when simulated or operator-controlled safety state changes.
    void stateChanged();

private:
    /// @brief Clamps a value to inclusive bounds.
    /// @return Bounded value.
    static double clamp(double value, double minimum, double maximum);
    /// @brief Calculates the numeric average of a variant list.
    /// @return Mean value, or 0 for an empty list.
    static double average(const QVariantList &values);
    /// @brief Advances acoustic, GNSS, traffic, and history simulation state.
    void updateSimulation();
    /// @brief Advances an active parachute deployment toward completion.
    void updateParachuteDeployment();

    IFlightTelemetrySource &m_telemetrySource;
    double m_simulationPhase;
    int m_noiseLimit;
    QVariantList m_noiseHistory;
    int m_parachuteState;
    double m_parachuteProgress;
    bool m_parachuteTestOverride;
    QString m_parachuteEventTime;
    bool m_gnssTestMode;
    int m_selectedTraffic;
    QTimer m_simulationTimer;
    QTimer m_parachuteTimer;
};

#endif // OPERATIONALVIEWMODELS_H
