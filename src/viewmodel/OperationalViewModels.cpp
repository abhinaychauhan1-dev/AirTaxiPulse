/**
 * @file    : src/viewmodel/OperationalViewModels.cpp
 * @brief   : Implements operational view models for aircraft subsystems.
 * @author  : Abhinay Chauhan (email: abhinay.chauhan1@gmail.com)
 * @version : 1.0.0
 *
 * Copyright (c) 2024
 * Abhinay Chauhan. All rights reserved.
 */

#include "OperationalViewModels.h"

#include <QTime>
#include <QVariantMap>
#include <QtMath>

/// @brief Constructs and starts the one-second session clock.
/// @param parent Optional QObject parent for Qt ownership.
FlightSessionViewModel::FlightSessionViewModel(QObject *parent)
    : QObject(parent)
    , m_currentDateTime(QDateTime::currentDateTime())
    , m_elapsedSeconds(0)
{
    // Refresh wall-clock and elapsed values from one shared timer tick.
    m_timer.setInterval(1000);
    connect(&m_timer, &QTimer::timeout, this, [this]() {
        m_currentDateTime = QDateTime::currentDateTime();
        ++m_elapsedSeconds;
        emit sessionChanged();
    });
    m_timer.start();
}

/// @brief Returns the formatted current date.
/// @return Date text for the session display.
QString FlightSessionViewModel::currentDateText() const
{
    return m_currentDateTime.toString(QStringLiteral("ddd, dd MMM yyyy"));
}

/// @brief Returns the formatted current time.
/// @return Time text for the session display.
QString FlightSessionViewModel::currentTimeText() const
{
    return m_currentDateTime.toString(QStringLiteral("hh:mm:ss"));
}

/// @brief Formats the elapsed session duration.
/// @return Duration in hh:mm:ss form.
QString FlightSessionViewModel::elapsedText() const
{
    const int hours = m_elapsedSeconds / 3600;
    const int minutes = (m_elapsedSeconds % 3600) / 60;
    const int seconds = m_elapsedSeconds % 60;
    return QStringLiteral("%1:%2:%3")
        .arg(hours, 2, 10, QLatin1Char('0'))
        .arg(minutes, 2, 10, QLatin1Char('0'))
        .arg(seconds, 2, 10, QLatin1Char('0'));
}

/// @brief Constructs the propulsion adapter for a telemetry source.
/// @param telemetrySource Source that supplies propulsion telemetry.
/// @param parent Optional QObject parent for Qt ownership.
PropulsionSystemViewModel::PropulsionSystemViewModel(IFlightTelemetrySource &telemetrySource,
                                                     QObject *parent)
    : QObject(parent)
    , m_telemetrySource(telemetrySource)
{
}

// Shared list helpers keep missing telemetry values safe for UI consumers.
/// @brief Calculates the numeric average of a variant list.
/// @return Mean value, or 0 for an empty list.
double PropulsionSystemViewModel::average(const QVariantList &values)
{
    if (values.isEmpty()) {
        return 0.0;
    }
    double total = 0.0;
    for (const QVariant &value : values) {
        total += value.toDouble();
    }
    return total / values.size();
}

/// @brief Safely reads a numeric value from a variant list.
/// @return Value at the index, or 0 when out of range.
double PropulsionSystemViewModel::valueAt(const QVariantList &values, int index)
{
    return index >= 0 && index < values.size() ? values.at(index).toDouble() : 0.0;
}

/// @brief Returns average front-group RPM.
/// @return Front-group RPM.
double PropulsionSystemViewModel::frontGroupRpm() const
{
    const QVariantList values = m_telemetrySource.motorRpmValues();
    return (valueAt(values, 0) + valueAt(values, 1)) / 2.0;
}

/// @brief Returns average rear-group RPM.
/// @return Rear-group RPM.
double PropulsionSystemViewModel::rearGroupRpm() const
{
    const QVariantList values = m_telemetrySource.motorRpmValues();
    return (valueAt(values, 2) + valueAt(values, 3)) / 2.0;
}

/// @brief Returns average RPM across all motors.
/// @return Average motor RPM.
double PropulsionSystemViewModel::averageRpm() const
{
    return average(m_telemetrySource.motorRpmValues());
}

/// @brief Returns average motor temperature.
/// @return Mean motor temperature.
double PropulsionSystemViewModel::averageMotorTemperature() const
{
    return average(m_telemetrySource.motorTemperatures());
}

/// @brief Returns average inverter health.
/// @return Mean inverter-health percentage.
double PropulsionSystemViewModel::inverterHealthAverage() const
{
    return average(m_telemetrySource.inverterHealth());
}

/// @brief Returns front-group thrust.
/// @return Front thrust value.
double PropulsionSystemViewModel::frontThrust() const
{
    return valueAt(m_telemetrySource.thrustOutputs(), 0);
}

/// @brief Returns rear-group thrust.
/// @return Rear thrust value.
double PropulsionSystemViewModel::rearThrust() const
{
    return valueAt(m_telemetrySource.thrustOutputs(), 1);
}

/// @brief Returns combined propulsion thrust.
/// @return Sum of front and rear thrust.
double PropulsionSystemViewModel::combinedThrust() const
{
    return frontThrust() + rearThrust();
}

/// @brief Calculates normalized front-to-rear thrust imbalance.
/// @return Signed balance ratio, or 0 when no thrust is produced.
double PropulsionSystemViewModel::thrustBalance() const
{
    return combinedThrust() > 0.0 ? (frontThrust() - rearThrust()) / combinedThrust() : 0.0;
}

/// @brief Calculates total electrical power across paired inverter channels.
/// @return Electrical power in kilowatts.
double PropulsionSystemViewModel::electricalPowerKw() const
{
    const QVariantList voltages = m_telemetrySource.inverterVoltages();
    const QVariantList currents = m_telemetrySource.inverterCurrents();
    const int count = qMin(voltages.size(), currents.size());
    // Sum only voltage/current pairs available from both telemetry lists.
    double total = 0.0;
    for (int index = 0; index < count; ++index) {
        total += voltages.at(index).toDouble() * currents.at(index).toDouble() / 1000.0;
    }
    return total;
}

/// @brief Returns the aggregate propulsion-state label.
/// @return Thermal, degraded, or nominal state text.
QString PropulsionSystemViewModel::stateLabel() const
{
    if (averageMotorTemperature() >= 85.0) {
        return QStringLiteral("THERMAL LIMIT");
    }
    if (inverterHealthAverage() < 90.0) {
        return QStringLiteral("DEGRADED");
    }
    return QStringLiteral("ALL CHANNELS NOMINAL");
}

/// @brief Returns normalized RPM progress.
/// @return Value clamped to [0, 1].
double PropulsionSystemViewModel::rpmProgress() const
{
    return qBound(0.0, averageRpm() / 2800.0, 1.0);
}

/// @brief Returns normalized tilt progress.
/// @return Value clamped to [0, 1].
double PropulsionSystemViewModel::tiltProgress() const
{
    return 1.0 - qBound(0.0, m_telemetrySource.tiltAngleDeg() / 90.0, 1.0);
}

/// @brief Returns normalized temperature progress.
/// @return Value clamped to [0, 1].
double PropulsionSystemViewModel::temperatureProgress() const
{
    return qBound(0.0, averageMotorTemperature() / 95.0, 1.0);
}

/// @brief Returns normalized thrust progress.
/// @return Value clamped to [0, 1].
double PropulsionSystemViewModel::thrustProgress() const
{
    return qBound(0.0, combinedThrust() / 28.0, 1.0);
}

/// @brief Returns normalized inverter-health progress.
/// @return Value clamped to [0, 1].
double PropulsionSystemViewModel::inverterProgress() const
{
    return qBound(0.0, inverterHealthAverage() / 100.0, 1.0);
}

/// @brief Returns the tilt-profile label.
/// @return Hover- or cruise-biased text.
QString PropulsionSystemViewModel::tiltProfileLabel() const
{
    return m_telemetrySource.tiltAngleDeg() > 45.0
        ? QStringLiteral("Hover-biased") : QStringLiteral("Cruise-biased");
}

/// @brief Reports whether thrust is within the balance tolerance.
/// @return true when absolute imbalance is below three percent.
bool PropulsionSystemViewModel::thrustBalanced() const
{
    return qAbs(thrustBalance()) < 0.03;
}

/// @brief Returns the thrust-balance label.
/// @return Centered, front-bias, or rear-bias text.
QString PropulsionSystemViewModel::thrustBalanceLabel() const
{
    if (thrustBalanced()) {
        return QStringLiteral("CENTERED");
    }
    return thrustBalance() > 0.0 ? QStringLiteral("FRONT BIAS") : QStringLiteral("REAR BIAS");
}

/// @brief Builds per-motor thermal warning flags.
/// @return Flags aligned with motor temperature samples.
QVariantList PropulsionSystemViewModel::motorThermalWarnings() const
{
    QVariantList warnings;
    const QVariantList temperatures = m_telemetrySource.motorTemperatures();
    for (const QVariant &temperature : temperatures) warnings.append(temperature.toDouble() >= 85.0);
    return warnings;
}

/// @brief Builds per-inverter health warning flags.
/// @return Flags aligned with inverter health samples.
QVariantList PropulsionSystemViewModel::inverterHealthWarnings() const
{
    QVariantList warnings;
    const QVariantList healthValues = m_telemetrySource.inverterHealth();
    for (const QVariant &health : healthValues) warnings.append(health.toDouble() < 90.0);
    return warnings;
}

/// @brief Constructs the energy adapter for a telemetry source.
/// @param telemetrySource Source that supplies energy telemetry.
/// @param parent Optional QObject parent for Qt ownership.
EnergySystemViewModel::EnergySystemViewModel(IFlightTelemetrySource &telemetrySource,
                                             QObject *parent)
    : QObject(parent)
    , m_telemetrySource(telemetrySource)
{
}

/// @brief Reports whether the active flight mode is hover-biased.
/// @return true for hover or lift-off modes.
bool EnergySystemViewModel::hoverMode() const
{
    const QString mode = m_telemetrySource.flightModeLabel();
    return mode.contains(QStringLiteral("Hover")) || mode.contains(QStringLiteral("Lift-Off"));
}

/// @brief Estimates remaining endurance from charge and power demand.
/// @return Estimated minutes, or 0 for negligible power demand.
double EnergySystemViewModel::estimatedMinutes() const
{
    const double powerKw = m_telemetrySource.powerConsumptionKw();
    return powerKw > 1.0 ? m_telemetrySource.batterySoc() * 4.2 / powerKw * 60.0 : 0.0;
}

/// @brief Finds the maximum reported cell temperature.
/// @return Maximum cell temperature, or 0 for no samples.
double EnergySystemViewModel::cellTemperatureMax() const
{
    const QVariantList temperatures = m_telemetrySource.batteryCellTemperatures();
    if (temperatures.isEmpty()) {
        return 0.0;
    }
    // Reduce all available cells to the thermal status input.
    double maximum = temperatures.first().toDouble();
    for (const QVariant &temperature : temperatures) {
        maximum = qMax(maximum, temperature.toDouble());
    }
    return maximum;
}

/// @brief Classifies the aggregate energy-system condition.
/// @return A Status enum value exposed as an integer.
int EnergySystemViewModel::status() const
{
    if (cellTemperatureMax() >= 60.0) {
        return ThermalWarning;
    }
    if (m_telemetrySource.batterySoc() < 25.0) {
        return LowReserve;
    }
    return Nominal;
}

/// @brief Returns the energy-system status label.
/// @return Human-readable status text.
QString EnergySystemViewModel::statusLabel() const
{
    switch (status()) {
    case ThermalWarning:
        return QStringLiteral("THERMAL WARNING");
    case LowReserve:
        return QStringLiteral("LOW RESERVE");
    default:
        return QStringLiteral("BATTERY NOMINAL");
    }
}

/// @brief Returns the active power-profile label.
/// @return Hover or cruise profile text.
QString EnergySystemViewModel::powerProfileLabel() const
{
    return hoverMode() ? QStringLiteral("HOVER POWER PROFILE") : QStringLiteral("CRUISE POWER PROFILE");
}

/// @brief Estimates energy-system efficiency from current demand.
/// @return Efficiency percentage with a 72 percent floor.
double EnergySystemViewModel::efficiencyPercent() const
{
    return qMax(72.0, 96.0 - m_telemetrySource.powerConsumptionKw() / 40.0);
}

/// @brief Returns a battery-zone temperature by index.
/// @return Temperature, or 0 when out of range.
double EnergySystemViewModel::zoneTemperature(int index) const
{
    const QVariantList temperatures = m_telemetrySource.batteryCellTemperatures();
    return index >= 0 && index < temperatures.size() ? temperatures.at(index).toDouble() : 0.0;
}

/// @brief Returns a bus-channel display name.
/// @return Channel name, or an empty string when out of range.
QString EnergySystemViewModel::busChannelName(int index) const
{
    static const QStringList names = {
        QStringLiteral("LIFT A"), QStringLiteral("LIFT B"), QStringLiteral("AVIONICS"),
        QStringLiteral("THERMAL"), QStringLiteral("RESERVE")
    };
    return index >= 0 && index < names.size() ? names.at(index) : QString();
}

/// @brief Returns a compact bus-channel code.
/// @return Channel code, or an empty string when out of range.
QString EnergySystemViewModel::busChannelCode(int index) const
{
    static const QStringList codes = {
        QStringLiteral("LA"), QStringLiteral("LB"), QStringLiteral("AV"),
        QStringLiteral("TH"), QStringLiteral("RS")
    };
    return index >= 0 && index < codes.size() ? codes.at(index) : QString();
}

/// @brief Returns the configured load share for a bus channel.
/// @return Fractional bus share, or 0 when out of range.
double EnergySystemViewModel::busChannelShare(int index) const
{
    static const double shares[] = {0.28, 0.28, 0.12, 0.20, 0.12};
    return index >= 0 && index < 5 ? shares[index] : 0.0;
}

/// @brief Calculates current allocated to a bus channel.
/// @return Channel current, or 0 when out of range.
double EnergySystemViewModel::busChannelCurrent(int index) const
{
    return m_telemetrySource.busCurrent() * busChannelShare(index);
}

/// @brief Calculates power allocated to a bus channel.
/// @return Channel power in kilowatts, or 0 when out of range.
double EnergySystemViewModel::busChannelPower(int index) const
{
    return m_telemetrySource.busVoltage() * busChannelCurrent(index) / 1000.0;
}

/// @brief Returns six battery-zone temperatures.
/// @return QML-compatible temperature list.
QVariantList EnergySystemViewModel::zoneTemperatures() const
{
    QVariantList values;
    for (int index = 0; index < 6; ++index) values.append(zoneTemperature(index));
    return values;
}

/// @brief Returns current allocated to five bus channels.
/// @return QML-compatible current list.
QVariantList EnergySystemViewModel::busChannelCurrents() const
{
    QVariantList values;
    for (int index = 0; index < 5; ++index) values.append(busChannelCurrent(index));
    return values;
}

/// @brief Returns power allocated to five bus channels.
/// @return QML-compatible power list.
QVariantList EnergySystemViewModel::busChannelPowers() const
{
    QVariantList values;
    for (int index = 0; index < 5; ++index) values.append(busChannelPower(index));
    return values;
}

/// @brief Constructs flight-control state and the fixed demonstration route.
/// @param telemetrySource Source that supplies flight and navigation telemetry.
/// @param parent Optional QObject parent for Qt ownership.
FlightControlSystemViewModel::FlightControlSystemViewModel(IFlightTelemetrySource &telemetrySource,
                                                           QObject *parent)
    : QObject(parent)
    , m_telemetrySource(telemetrySource)
    , m_autopilotEngaged(true)
    , m_guidanceMode(0)
    , m_commandedFlightMode(-1)
    , m_automaticRouteSequence(true)
    , m_selectedWaypoint(1)
    , m_manualLeftElevon(0.0)
    , m_manualRightElevon(0.0)
    , m_manualRuddervator(0.0)
    , m_manualTiltActuator(45.0)
{
    // Build immutable waypoint maps in the shape consumed by QML.
    const auto waypoint = [](const QString &code, const QString &name, double latitude, double longitude) {
        return QVariantMap{{QStringLiteral("code"), code}, {QStringLiteral("name"), name},
                           {QStringLiteral("latitude"), latitude}, {QStringLiteral("longitude"), longitude}};
    };
    m_waypoints = {
        waypoint(QStringLiteral("PAD A"), QStringLiteral("Downtown Vertiport"), 37.7749, -122.4194),
        waypoint(QStringLiteral("TRN-1"), QStringLiteral("Transition Gate"), 37.7792, -122.4124),
        waypoint(QStringLiteral("CRZ-2"), QStringLiteral("Bay Corridor"), 37.7868, -122.4012),
        waypoint(QStringLiteral("APP-1"), QStringLiteral("Approach Fix"), 37.7935, -122.3928),
        waypoint(QStringLiteral("PAD B"), QStringLiteral("Harbor Vertiport"), 37.7978, -122.3860)
    };
}

/// @brief Clamps a value to inclusive bounds.
/// @return Bounded value.
double FlightControlSystemViewModel::clamp(double value, double minimum, double maximum)
{
    return qBound(minimum, value, maximum);
}

/// @brief Returns whether autopilot is engaged.
/// @return true when automatic actuator commands are active.
bool FlightControlSystemViewModel::autopilotEngaged() const { return m_autopilotEngaged; }
/// @brief Returns the selected guidance mode.
/// @return Guidance mode index.
int FlightControlSystemViewModel::guidanceMode() const { return m_guidanceMode; }
/// @brief Returns the commanded flight mode.
/// @return Mode index, or -1 when following telemetry.
int FlightControlSystemViewModel::commandedFlightMode() const { return m_commandedFlightMode; }

/// @brief Normalizes source flight mode for the controls UI.
/// @return HOVER, TRANSITION, CRUISE, or VERTICAL LANDING.
QString FlightControlSystemViewModel::flightMode() const
{
    const QString sourceMode = m_telemetrySource.flightModeLabel();
    if (sourceMode.contains(QStringLiteral("Hover")) || sourceMode.contains(QStringLiteral("Lift-Off")))
        return QStringLiteral("HOVER");
    if (sourceMode.contains(QStringLiteral("Transition")))
        return QStringLiteral("TRANSITION");
    if (sourceMode.contains(QStringLiteral("Cruise")))
        return QStringLiteral("CRUISE");
    return QStringLiteral("VERTICAL LANDING");
}

/// @brief Maps normalized telemetry mode text to an index.
/// @return Telemetry flight-mode index.
int FlightControlSystemViewModel::telemetryFlightModeIndex() const
{
    const QString mode = flightMode();
    return mode == QStringLiteral("HOVER") ? 0 : mode == QStringLiteral("TRANSITION") ? 1
           : mode == QStringLiteral("CRUISE") ? 2 : 3;
}

/// @brief Returns the effective flight-mode index.
/// @return Commanded mode when set; otherwise the telemetry mode.
int FlightControlSystemViewModel::flightModeIndex() const
{
    return m_commandedFlightMode >= 0 ? m_commandedFlightMode : telemetryFlightModeIndex();
}

/// @brief Returns the effective left elevon command.
/// @return Left elevon angle in degrees.
double FlightControlSystemViewModel::leftElevon() const
{
    return m_autopilotEngaged
        ? clamp(m_telemetrySource.pitch() * 0.55 - m_telemetrySource.roll() * 0.75, -18.0, 18.0)
        : m_manualLeftElevon;
}

/// @brief Returns the effective right elevon command.
/// @return Right elevon angle in degrees.
double FlightControlSystemViewModel::rightElevon() const
{
    return m_autopilotEngaged
        ? clamp(m_telemetrySource.pitch() * 0.55 + m_telemetrySource.roll() * 0.75, -18.0, 18.0)
        : m_manualRightElevon;
}

/// @brief Returns the effective ruddervator command.
/// @return Ruddervator angle in degrees.
double FlightControlSystemViewModel::ruddervator() const
{
    return m_autopilotEngaged ? clamp(m_telemetrySource.yaw() * 0.8, -20.0, 20.0) : m_manualRuddervator;
}

/// @brief Returns the effective tilt-actuator command.
/// @return Tilt angle in degrees.
double FlightControlSystemViewModel::tiltActuator() const
{
    return m_autopilotEngaged ? m_telemetrySource.tiltAngleDeg() : m_manualTiltActuator;
}

/// @brief Returns all effective actuator commands.
/// @return Values ordered by actuator index.
QVariantList FlightControlSystemViewModel::actuatorValues() const
{
    return {leftElevon(), rightElevon(), ruddervator(), tiltActuator()};
}

/// @brief Returns immutable route waypoints.
/// @return Waypoint maps for QML.
QVariantList FlightControlSystemViewModel::waypoints() const { return m_waypoints; }
/// @brief Reports whether route sequencing is automatic.
/// @return true for automatic sequencing.
bool FlightControlSystemViewModel::automaticRouteSequence() const { return m_automaticRouteSequence; }
/// @brief Selects the automatic waypoint from the effective flight mode.
/// @return Automatic waypoint index.
int FlightControlSystemViewModel::automaticWaypoint() const { return qMin(m_waypoints.size() - 1, flightModeIndex() + 1); }
/// @brief Returns the active navigation waypoint.
/// @return Effective waypoint index.
int FlightControlSystemViewModel::navigationWaypoint() const { return m_automaticRouteSequence ? automaticWaypoint() : m_selectedWaypoint; }

/// @brief Calculates great-circle distance to a waypoint.
/// @param index Waypoint index.
/// @return Distance in nautical miles, or 0 when out of range.
double FlightControlSystemViewModel::distanceToWaypoint(int index) const
{
    if (index < 0 || index >= m_waypoints.size())
        return 0.0;
    const QVariantMap point = m_waypoints.at(index).toMap();
    const double radians = M_PI / 180.0;
    const double latitude = m_telemetrySource.gpsLatitude();
    const double longitude = m_telemetrySource.gpsLongitude();
    const double targetLatitude = point.value(QStringLiteral("latitude")).toDouble();
    const double targetLongitude = point.value(QStringLiteral("longitude")).toDouble();
    const double latitudeDelta = (targetLatitude - latitude) * radians;
    const double longitudeDelta = (targetLongitude - longitude) * radians;
    // Apply the haversine formula and clamp rounding error before the square roots.
    double haversine = qPow(qSin(latitudeDelta / 2.0), 2)
        + qCos(latitude * radians) * qCos(targetLatitude * radians) * qPow(qSin(longitudeDelta / 2.0), 2);
    haversine = clamp(haversine, 0.0, 1.0);
    return 3440.065 * 2.0 * qAtan2(qSqrt(haversine), qSqrt(1.0 - haversine));
}

/// @brief Calculates distance to the active waypoint.
/// @return Great-circle distance in nautical miles.
double FlightControlSystemViewModel::nextWaypointDistance() const { return distanceToWaypoint(navigationWaypoint()); }
/// @brief Returns the route-leg status.
/// @return AUTO SEQUENCE or DIRECT TO.
QString FlightControlSystemViewModel::routeLegStatus() const { return m_automaticRouteSequence ? QStringLiteral("AUTO SEQUENCE") : QStringLiteral("DIRECT TO"); }

/// @brief Returns a flight-mode name by index.
/// @return Mode name, or an empty string when out of range.
QString FlightControlSystemViewModel::modeName(int index) const
{
    static const QStringList values{QStringLiteral("HOVER"), QStringLiteral("TRANSITION"), QStringLiteral("CRUISE"), QStringLiteral("VERTICAL LANDING")};
    return index >= 0 && index < values.size() ? values.at(index) : QString();
}

/// @brief Returns a guidance-mode name by index.
/// @return Guidance name, or an empty string when out of range.
QString FlightControlSystemViewModel::guidanceName(int index) const
{
    static const QStringList values{QStringLiteral("ROUTE"), QStringLiteral("HEADING"), QStringLiteral("ATTITUDE")};
    return index >= 0 && index < values.size() ? values.at(index) : QString();
}

/// @brief Returns an actuator name by index.
/// @return Actuator name, or an empty string when out of range.
QString FlightControlSystemViewModel::actuatorName(int index) const
{
    static const QStringList values{QStringLiteral("LEFT ELEVON"), QStringLiteral("RIGHT ELEVON"), QStringLiteral("RUDDERVATOR"), QStringLiteral("TILT ACTUATOR")};
    return index >= 0 && index < values.size() ? values.at(index) : QString();
}

/// @brief Returns an effective actuator value by index.
/// @return Effective actuator command.
double FlightControlSystemViewModel::actuatorValue(int index) const
{
    switch (index) { case 0: return leftElevon(); case 1: return rightElevon(); case 2: return ruddervator(); default: return tiltActuator(); }
}

/// @brief Returns the minimum command for an actuator.
/// @return Minimum command value.
double FlightControlSystemViewModel::actuatorMinimum(int index) const { return index == 3 ? 0.0 : index == 2 ? -20.0 : -18.0; }
/// @brief Returns the maximum command for an actuator.
/// @return Maximum command value.
double FlightControlSystemViewModel::actuatorMaximum(int index) const { return index == 3 ? 90.0 : index == 2 ? 20.0 : 18.0; }

/// @brief Clears the operator flight-mode override.
void FlightControlSystemViewModel::clearCommandedFlightMode() { m_commandedFlightMode = -1; emit stateChanged(); }
/// @brief Selects a bounded operator-commanded flight mode.
void FlightControlSystemViewModel::setCommandedFlightMode(int index) { m_commandedFlightMode = qBound(0, index, 3); emit stateChanged(); }
/// @brief Selects a bounded guidance mode.
void FlightControlSystemViewModel::setGuidanceMode(int index) { m_guidanceMode = qBound(0, index, 2); emit stateChanged(); }

/// @brief Toggles automatic flight control while preserving current commands.
void FlightControlSystemViewModel::toggleAutopilot()
{
    if (m_autopilotEngaged) {
        // Capture the automatic commands to avoid jumps when manual control takes over.
        m_manualLeftElevon = leftElevon();
        m_manualRightElevon = rightElevon();
        m_manualRuddervator = ruddervator();
        m_manualTiltActuator = tiltActuator();
    }
    m_autopilotEngaged = !m_autopilotEngaged;
    emit stateChanged();
}

/// @brief Sets a bounded manual actuator command.
/// @param index Actuator index.
/// @param value Requested command value.
void FlightControlSystemViewModel::setManualActuator(int index, double value)
{
    const double bounded = clamp(value, actuatorMinimum(index), actuatorMaximum(index));
    switch (index) { case 0: m_manualLeftElevon = bounded; break; case 1: m_manualRightElevon = bounded; break;
    case 2: m_manualRuddervator = bounded; break; default: m_manualTiltActuator = bounded; break; }
    emit stateChanged();
}

/// @brief Selects a direct-to navigation waypoint.
/// @param index Valid waypoint index.
void FlightControlSystemViewModel::selectDirectTo(int index)
{
    if (index < 0 || index >= m_waypoints.size()) return;
    m_selectedWaypoint = index;
    m_automaticRouteSequence = false;
    m_guidanceMode = 0;
    emit stateChanged();
}

/// @brief Resumes automatic route sequencing.
void FlightControlSystemViewModel::resumeAutomaticRoute()
{
    m_automaticRouteSequence = true;
    m_selectedWaypoint = automaticWaypoint();
    m_guidanceMode = 0;
    emit stateChanged();
}

/// @brief Constructs safety simulation state and starts its update timers.
/// @param telemetrySource Source that supplies aircraft telemetry.
/// @param parent Optional QObject parent for Qt ownership.
SafetySystemViewModel::SafetySystemViewModel(IFlightTelemetrySource &telemetrySource, QObject *parent)
    : QObject(parent), m_telemetrySource(telemetrySource), m_simulationPhase(0.0), m_noiseLimit(75),
      m_noiseHistory({63, 65, 64, 67, 69, 68, 70, 72, 71, 69, 68, 70}), m_parachuteState(0),
      m_parachuteProgress(0.0), m_parachuteTestOverride(false), m_parachuteEventTime(QStringLiteral("--:--:--")),
    m_gnssTestMode(false), m_selectedTraffic(0)
{
    // Drive general safety simulation and parachute deployment at independent rates.
    m_simulationTimer.setInterval(500);
    connect(&m_simulationTimer, &QTimer::timeout, this, &SafetySystemViewModel::updateSimulation);
    m_simulationTimer.start();
    m_parachuteTimer.setInterval(80);
    connect(&m_parachuteTimer, &QTimer::timeout, this, &SafetySystemViewModel::updateParachuteDeployment);
}

/// @brief Clamps a value to inclusive bounds.
double SafetySystemViewModel::clamp(double value, double minimum, double maximum) { return qBound(minimum, value, maximum); }
/// @brief Calculates the numeric average of a variant list.
double SafetySystemViewModel::average(const QVariantList &values) { if (values.isEmpty()) return 0.0; double total = 0.0; for (const QVariant &value : values) total += value.toDouble(); return total / values.size(); }
/// @brief Returns the acoustic warning limit.
int SafetySystemViewModel::noiseLimit() const { return m_noiseLimit; }
/// @brief Returns recent acoustic history.
QVariantList SafetySystemViewModel::noiseHistory() const { return m_noiseHistory; }
/// @brief Calculates the current simulated acoustic level.
double SafetySystemViewModel::acousticDb() const { return clamp(54.0 + average(m_telemetrySource.motorRpmValues()) / 145.0 + m_telemetrySource.cas() / 22.0 + 2.5 * qSin(m_simulationPhase * 1.4), 48.0, 96.0); }
/// @brief Reports whether the acoustic limit is exceeded.
bool SafetySystemViewModel::noiseWarning() const { return acousticDb() >= m_noiseLimit; }
/// @brief Returns simulated visible satellite count.
int SafetySystemViewModel::satelliteCount() const { return m_gnssTestMode ? 7 : qRound(13.0 + 2.0 * qSin(m_simulationPhase * 0.35)); }
/// @brief Returns simulated GNSS signal strength.
double SafetySystemViewModel::signalStrength() const { return m_gnssTestMode ? 46.0 : 88.0 + 5.0 * qSin(m_simulationPhase * 0.55); }
/// @brief Returns simulated navigation accuracy.
double SafetySystemViewModel::navigationAccuracy() const { return m_gnssTestMode ? 4.8 : 0.7 + 0.25 * qAbs(qSin(m_simulationPhase * 0.72)); }
/// @brief Reports GNSS integrity availability.
bool SafetySystemViewModel::raimAvailable() const { return satelliteCount() >= 8 && navigationAccuracy() < 2.0; }
/// @brief Calculates lateral path deviation.
double SafetySystemViewModel::lateralDeviation() const { return clamp((m_telemetrySource.track() - m_telemetrySource.heading()) * 0.45 + 0.25 * qSin(m_simulationPhase), -2.5, 2.5); }
/// @brief Calculates vertical path deviation.
double SafetySystemViewModel::verticalDeviation() const { return clamp(m_telemetrySource.vs() / 900.0 + 0.2 * qCos(m_simulationPhase * 0.8), -2.5, 2.5); }
/// @brief Calculates combined alignment quality.
double SafetySystemViewModel::alignmentQuality() const { return clamp(100.0 - qAbs(lateralDeviation()) * 17.0 - qAbs(verticalDeviation()) * 12.0, 0.0, 100.0); }
/// @brief Reports whether the aircraft is in a landing phase.
bool SafetySystemViewModel::landingPhase() const { return m_telemetrySource.flightModeLabel().contains(QStringLiteral("Arrival")) || m_telemetrySource.altRadar() < 150.0; }
/// @brief Returns the parachute state.
int SafetySystemViewModel::parachuteState() const { return m_parachuteState; }
/// @brief Returns parachute deployment progress.
double SafetySystemViewModel::parachuteProgress() const { return m_parachuteProgress; }
/// @brief Returns test-override state.
bool SafetySystemViewModel::parachuteTestOverride() const { return m_parachuteTestOverride; }
/// @brief Returns the latest parachute event time.
QString SafetySystemViewModel::parachuteEventTime() const { return m_parachuteEventTime; }
/// @brief Validates the telemetry deployment envelope.
bool SafetySystemViewModel::parachuteEnvelopeValid() const { return m_telemetrySource.altRadar() >= 100.0 && m_telemetrySource.cas() >= 15.0 && m_telemetrySource.cas() <= 150.0; }
/// @brief Reports whether parachute deployment is permitted.
bool SafetySystemViewModel::parachuteDeployAllowed() const { return m_parachuteState == 1 && (parachuteEnvelopeValid() || m_parachuteTestOverride); }
/// @brief Returns deployment-envelope status text.
QString SafetySystemViewModel::parachuteInhibitReason() const { if (m_telemetrySource.altRadar() < 100.0) return QStringLiteral("INHIBIT: RADAR ALT < 100 FT"); if (m_telemetrySource.cas() < 15.0) return QStringLiteral("INHIBIT: AIRSPEED < 15 KT"); if (m_telemetrySource.cas() > 150.0) return QStringLiteral("INHIBIT: AIRSPEED > 150 KT"); return QStringLiteral("DEPLOYMENT ENVELOPE VALID"); }
/// @brief Returns degraded GNSS test-mode state.
bool SafetySystemViewModel::gnssTestMode() const { return m_gnssTestMode; }
/// @brief Returns the selected traffic target.
int SafetySystemViewModel::selectedTraffic() const { return m_selectedTraffic; }
/// @brief Returns all simulated traffic ranges.
QVariantList SafetySystemViewModel::trafficRanges() const { return {trafficRange(0), trafficRange(1), trafficRange(2)}; }
/// @brief Returns all simulated traffic bearings.
QVariantList SafetySystemViewModel::trafficBearings() const { return {trafficBearing(0), trafficBearing(1), trafficBearing(2)}; }
/// @brief Returns summaries for all safety monitors.
QVariantList SafetySystemViewModel::monitorSummaries() const { QVariantList values; for (int index = 0; index < 5; ++index) values.append(monitorSummary(index, m_selectedTraffic)); return values; }
/// @brief Counts active acoustic, GNSS, and proximity alerts.
int SafetySystemViewModel::alertCount() const { return (noiseWarning() ? 1 : 0) + (!raimAvailable() ? 1 : 0) + (trafficRange(m_selectedTraffic) < 1.0 ? 1 : 0); }
/// @brief Sets the bounded acoustic warning limit.
void SafetySystemViewModel::setNoiseLimit(int value) { m_noiseLimit = qBound(40, value, 100); emit stateChanged(); }
/// @brief Toggles degraded GNSS test values.
void SafetySystemViewModel::toggleGnssTestMode() { m_gnssTestMode = !m_gnssTestMode; emit stateChanged(); }
/// @brief Toggles parachute deployment-envelope override when permitted.
void SafetySystemViewModel::toggleParachuteTestOverride() { if (m_parachuteState < 2) { m_parachuteTestOverride = !m_parachuteTestOverride; emit stateChanged(); } }
/// @brief Selects a bounded traffic target.
void SafetySystemViewModel::setSelectedTraffic(int index) { m_selectedTraffic = qBound(0, index, 2); emit stateChanged(); }

/// @brief Toggles the parachute between safe and armed states.
void SafetySystemViewModel::toggleParachuteArm()
{
    if (m_parachuteState == 0) m_parachuteState = 1; else if (m_parachuteState == 1) m_parachuteState = 0; else return;
    m_parachuteEventTime = QTime::currentTime().toString(QStringLiteral("hh:mm:ss")); emit stateChanged();
}

/// @brief Starts an allowed deployment or resets a completed deployment.
void SafetySystemViewModel::deployOrResetParachute()
{
    if (m_parachuteState == 3) { m_parachuteState = 0; m_parachuteProgress = 0.0; m_parachuteTestOverride = false; }
    else if (parachuteDeployAllowed()) { m_parachuteProgress = 0.0; m_parachuteState = 2; m_parachuteTimer.start(); }
    else return;
    m_parachuteEventTime = QTime::currentTime().toString(QStringLiteral("hh:mm:ss")); emit stateChanged();
}

/// @brief Returns a safety-monitor code.
QString SafetySystemViewModel::monitorCode(int index) const { static const QStringList values{QStringLiteral("NOISE"), QStringLiteral("BRS"), QStringLiteral("TCAS"), QStringLiteral("GNSS"), QStringLiteral("LANDING")}; return index >= 0 && index < values.size() ? values.at(index) : QString(); }
/// @brief Formats a signed numeric value.
QString SafetySystemViewModel::signedValue(double value) const { return QStringLiteral("%1%2").arg(value >= 0.0 ? QStringLiteral("+") : QString()).arg(value, 0, 'f', 1); }
/// @brief Returns a traffic-target code.
QString SafetySystemViewModel::trafficCode(int index) const { static const QStringList values{QStringLiteral("TFC-21"), QStringLiteral("TFC-07"), QStringLiteral("OBS-14")}; return index >= 0 && index < values.size() ? values.at(index) : QString(); }
/// @brief Calculates simulated range to a traffic target.
double SafetySystemViewModel::trafficRange(int index) const { static const double ranges[]{1.35, 2.8, 0.72}; return index >= 0 && index < 3 ? qMax(0.18, ranges[index] + 0.18 * qSin(m_simulationPhase * (0.7 + index * 0.16) + index)) : 0.0; }
/// @brief Calculates simulated bearing to a traffic target.
double SafetySystemViewModel::trafficBearing(int index) const { static const double bearings[]{38.0, 218.0, 312.0}; return index >= 0 && index < 3 ? bearings[index] + 8.0 * qSin(m_simulationPhase * 0.5 + index) : 0.0; }
/// @brief Formats a safety-monitor summary.
QString SafetySystemViewModel::monitorSummary(int index, int selectedTraffic) const
{
    if (index == 0) return QStringLiteral("%1 dBA  |  LIMIT %2 dBA").arg(acousticDb(), 0, 'f', 1).arg(m_noiseLimit);
    if (index == 1) return m_parachuteState == 0 ? QStringLiteral("SYSTEM SAFE") : m_parachuteState == 1 ? QStringLiteral("ARMED / READY") : m_parachuteState == 2 ? QStringLiteral("DEPLOYING %1%").arg(qRound(m_parachuteProgress * 100.0)) : QStringLiteral("DEPLOYED");
    if (index == 2) return QStringLiteral("%1  |  %2 NM").arg(trafficCode(selectedTraffic)).arg(trafficRange(selectedTraffic), 0, 'f', 1);
    if (index == 3) return QStringLiteral("%1 SAT  |  HPL %2 m  |  RAIM %3").arg(satelliteCount()).arg(navigationAccuracy(), 0, 'f', 1).arg(raimAvailable() ? QStringLiteral("PASS") : QStringLiteral("FAIL"));
    return QStringLiteral("ALIGN %1%  |  LAT %2 m").arg(alignmentQuality(), 0, 'f', 0).arg(signedValue(lateralDeviation()));
}

/// @brief Advances acoustic, GNSS, traffic, and history simulation state.
void SafetySystemViewModel::updateSimulation()
{
    m_simulationPhase += 0.18;
    m_noiseHistory.append(acousticDb());
    // Bound history growth while retaining enough samples for the chart.
    while (m_noiseHistory.size() > 28) m_noiseHistory.removeFirst();
    emit stateChanged();
}

/// @brief Advances an active parachute deployment toward completion.
void SafetySystemViewModel::updateParachuteDeployment()
{
    if (m_parachuteState != 2) { m_parachuteTimer.stop(); return; }
    // Progress the deployment state machine and stop its timer at completion.
    m_parachuteProgress = qMin(1.0, m_parachuteProgress + 0.035);
    if (m_parachuteProgress >= 1.0) { m_parachuteState = 3; m_parachuteTimer.stop(); m_parachuteEventTime = QTime::currentTime().toString(QStringLiteral("hh:mm:ss")); }
    emit stateChanged();
}
