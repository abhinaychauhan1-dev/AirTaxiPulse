/**
 * @file    : src/viewmodel/PrimaryFlightViewModel.cpp
 * @brief   : Implements presentation data for primary flight telemetry.
 * @author  : Abhinay Chauhan (email: abhinay.chauhan1@gmail.com)
 * @version : 1.0.0
 *
 * Copyright (c) 2024
 * Abhinay Chauhan. All rights reserved.
 */

#include "PrimaryFlightViewModel.h"

#include <cmath>

/// @brief Constructs the primary flight adapter for a telemetry source.
/// @param title Instrument page title.
/// @param telemetrySource Source that supplies live and historical telemetry.
/// @param parent Optional QObject parent for Qt ownership.
PrimaryFlightViewModel::PrimaryFlightViewModel(const QString &title,
                                               IFlightTelemetrySource &telemetrySource,
                                               QObject *parent)
    : QObject(parent)
    , m_title(title)
    , m_telemetrySource(telemetrySource)
{
}

/// @brief Returns the primary flight page title.
/// @return Title text displayed by the UI.
QString PrimaryFlightViewModel::title() const
{
    return m_title;
}

/// @brief Returns calibrated airspeed.
/// @return Calibrated airspeed in knots.
double PrimaryFlightViewModel::cas() const { return m_telemetrySource.cas(); }
/// @brief Returns true airspeed.
/// @return True airspeed in knots.
double PrimaryFlightViewModel::tas() const { return m_telemetrySource.tas(); }
/// @brief Returns barometric altitude.
/// @return Barometric altitude in feet.
double PrimaryFlightViewModel::altBaro() const { return m_telemetrySource.altBaro(); }
/// @brief Returns radar altitude.
/// @return Radar altitude in feet.
double PrimaryFlightViewModel::altRadar() const { return m_telemetrySource.altRadar(); }
/// @brief Returns vertical speed.
/// @return Vertical speed in feet per minute.
double PrimaryFlightViewModel::vs() const { return m_telemetrySource.vs(); }
/// @brief Returns aircraft pitch.
/// @return Pitch angle in degrees.
double PrimaryFlightViewModel::pitch() const { return m_telemetrySource.pitch(); }
/// @brief Returns aircraft roll.
/// @return Roll angle in degrees.
double PrimaryFlightViewModel::roll() const { return m_telemetrySource.roll(); }
/// @brief Returns aircraft yaw.
/// @return Yaw angle in degrees.
double PrimaryFlightViewModel::yaw() const { return m_telemetrySource.yaw(); }
/// @brief Returns magnetic heading.
/// @return Heading in degrees.
double PrimaryFlightViewModel::heading() const { return m_telemetrySource.heading(); }
/// @brief Returns ground track.
/// @return Track in degrees.
double PrimaryFlightViewModel::track() const { return m_telemetrySource.track(); }
/// @brief Normalizes track minus heading to the shortest signed angular delta.
/// @return Angular difference in the range [-180, 180).
double PrimaryFlightViewModel::headingTrackDelta() const
{
    return std::fmod((track() - heading()) + 540.0, 360.0) - 180.0;
}
/// @brief Scales heading-to-track drift for the slip indicator.
/// @return Slip offset clamped to [-1, 1].
double PrimaryFlightViewModel::slipOffset() const
{
    return qBound(-1.0, headingTrackDelta() / 12.0, 1.0);
}
/// @brief Returns calibrated-airspeed progress.
/// @return Normalized progress value.
double PrimaryFlightViewModel::casProgress() const { return m_telemetrySource.casProgress(); }
/// @brief Returns altitude progress.
/// @return Normalized progress value.
double PrimaryFlightViewModel::altProgress() const { return m_telemetrySource.altProgress(); }
/// @brief Returns vertical-speed progress.
/// @return Normalized progress value.
double PrimaryFlightViewModel::vsProgress() const { return m_telemetrySource.vsProgress(); }
/// @brief Returns attitude progress.
/// @return Normalized progress value.
double PrimaryFlightViewModel::attitudeProgress() const { return m_telemetrySource.attitudeProgress(); }
/// @brief Returns heading progress.
/// @return Normalized progress value.
double PrimaryFlightViewModel::headingProgress() const { return m_telemetrySource.headingProgress(); }
/// @brief Returns flight-path-vector progress.
/// @return Normalized progress value.
double PrimaryFlightViewModel::fpvProgress() const { return m_telemetrySource.fpvProgress(); }
/// @brief Returns the active flight-mode label.
/// @return Human-readable flight mode.
QString PrimaryFlightViewModel::flightModeLabel() const { return m_telemetrySource.flightModeLabel(); }
/// @brief Returns the vertical-speed accent color.
/// @return Color associated with the current trend.
QColor PrimaryFlightViewModel::vsAccentColor() const { return m_telemetrySource.vsAccentColor(); }
/// @brief Returns the vertical-speed trend label.
/// @return Human-readable trend text.
QString PrimaryFlightViewModel::vsTrendLabel() const { return m_telemetrySource.vsTrendLabel(); }
/// @brief Returns formatted vertical speed.
/// @return Vertical-speed display text.
QString PrimaryFlightViewModel::vsValueText() const { return m_telemetrySource.vsValueText(); }
/// @brief Returns battery state of charge.
/// @return State of charge in percent.
double PrimaryFlightViewModel::batterySoc() const { return m_telemetrySource.batterySoc(); }
/// @brief Returns battery state of health.
/// @return State of health in percent.
double PrimaryFlightViewModel::batterySoh() const { return m_telemetrySource.batterySoh(); }
/// @brief Returns electrical power consumption.
/// @return Power consumption in kilowatts.
double PrimaryFlightViewModel::powerConsumptionKw() const { return m_telemetrySource.powerConsumptionKw(); }
/// @brief Finds the lowest reported battery-cell temperature.
/// @return Minimum temperature, or 0 when no samples exist.
double PrimaryFlightViewModel::cellTemperatureMin() const
{
    const QVariantList temperatures = m_telemetrySource.batteryCellTemperatures();
    if (temperatures.isEmpty()) {
        return 0.0;
    }
    // Reduce the variable-length telemetry list to its coldest cell.
    double minimum = temperatures.first().toDouble();
    for (const QVariant &value : temperatures) {
        minimum = qMin(minimum, value.toDouble());
    }
    return minimum;
}
/// @brief Finds the highest reported battery-cell temperature.
/// @return Maximum temperature, or 0 when no samples exist.
double PrimaryFlightViewModel::cellTemperatureMax() const
{
    const QVariantList temperatures = m_telemetrySource.batteryCellTemperatures();
    if (temperatures.isEmpty()) {
        return 0.0;
    }
    // Reduce the variable-length telemetry list to its hottest cell.
    double maximum = temperatures.first().toDouble();
    for (const QVariant &value : temperatures) {
        maximum = qMax(maximum, value.toDouble());
    }
    return maximum;
}
/// @brief Returns per-cell battery temperatures.
/// @return Battery-cell temperature samples.
QVariantList PrimaryFlightViewModel::batteryCellTemperatures() const
{
    return m_telemetrySource.batteryCellTemperatures();
}
/// @brief Reports whether battery temperature exceeds the warning threshold.
/// @return true when the hottest cell is at least 60 degrees.
bool PrimaryFlightViewModel::thermalRunawayWarning() const { return cellTemperatureMax() >= 60.0; }
/// @brief Returns high-voltage bus voltage.
/// @return Bus voltage.
double PrimaryFlightViewModel::busVoltage() const { return m_telemetrySource.busVoltage(); }
/// @brief Returns high-voltage bus current.
/// @return Bus current.
double PrimaryFlightViewModel::busCurrent() const { return m_telemetrySource.busCurrent(); }
/// @brief Returns per-motor temperatures.
/// @return Motor temperature samples.
QVariantList PrimaryFlightViewModel::motorTemperatures() const { return m_telemetrySource.motorTemperatures(); }
/// @brief Returns per-motor rotational speeds.
/// @return Motor RPM samples.
QVariantList PrimaryFlightViewModel::motorRpmValues() const { return m_telemetrySource.motorRpmValues(); }
/// @brief Returns propulsion-pod tilt angle.
/// @return Tilt angle in degrees.
double PrimaryFlightViewModel::tiltAngleDeg() const { return m_telemetrySource.tiltAngleDeg(); }
/// @brief Returns propulsion thrust outputs.
/// @return Front and rear thrust samples.
QVariantList PrimaryFlightViewModel::thrustOutputs() const { return m_telemetrySource.thrustOutputs(); }
/// @brief Returns per-inverter voltages.
/// @return Inverter voltage samples.
QVariantList PrimaryFlightViewModel::inverterVoltages() const { return m_telemetrySource.inverterVoltages(); }
/// @brief Returns per-inverter currents.
/// @return Inverter current samples.
QVariantList PrimaryFlightViewModel::inverterCurrents() const { return m_telemetrySource.inverterCurrents(); }
/// @brief Returns per-inverter health values.
/// @return Inverter health percentages.
QVariantList PrimaryFlightViewModel::inverterHealth() const { return m_telemetrySource.inverterHealth(); }
/// @brief Returns current GPS latitude.
/// @return Latitude in decimal degrees.
double PrimaryFlightViewModel::gpsLatitude() const { return m_telemetrySource.gpsLatitude(); }
/// @brief Returns current GPS longitude.
/// @return Longitude in decimal degrees.
double PrimaryFlightViewModel::gpsLongitude() const { return m_telemetrySource.gpsLongitude(); }
/// @brief Formats the current GPS coordinates for display.
/// @return Coordinate text or N/A for non-finite values.
QString PrimaryFlightViewModel::gpsText() const
{
    if (!std::isfinite(gpsLatitude()) || !std::isfinite(gpsLongitude())) {
        return QStringLiteral("N/A");
    }
    return QStringLiteral("%1, %2").arg(gpsLatitude(), 0, 'f', 5).arg(gpsLongitude(), 0, 'f', 5);
}
/// @brief Calculates the mean reported motor temperature.
/// @return Average temperature, or 0 when no samples exist.
double PrimaryFlightViewModel::averageMotorTemperature() const
{
    const QVariantList temperatures = motorTemperatures();
    if (temperatures.isEmpty()) {
        return 0.0;
    }
    // Average all available motor channels rather than assuming a fixed count.
    double total = 0.0;
    for (const QVariant &temperature : temperatures) {
        total += temperature.toDouble();
    }
    return total / temperatures.size();
}
/// @brief Returns recent calibrated-airspeed history.
/// @return Historical calibrated-airspeed samples.
QVariantList PrimaryFlightViewModel::casHistory() const { return m_telemetrySource.casHistory(); }
/// @brief Returns recent altitude history.
/// @return Historical altitude samples.
QVariantList PrimaryFlightViewModel::altHistory() const { return m_telemetrySource.altHistory(); }
/// @brief Returns recent vertical-speed history.
/// @return Historical vertical-speed samples.
QVariantList PrimaryFlightViewModel::vsHistory() const { return m_telemetrySource.vsHistory(); }
/// @brief Returns recent attitude history.
/// @return Historical attitude samples.
QVariantList PrimaryFlightViewModel::attitudeHistory() const { return m_telemetrySource.attitudeHistory(); }
/// @brief Returns recent heading history.
/// @return Historical heading samples.
QVariantList PrimaryFlightViewModel::headingHistory() const { return m_telemetrySource.headingHistory(); }
/// @brief Returns recent flight-path-vector history.
/// @return Historical flight-path-vector samples.
QVariantList PrimaryFlightViewModel::fpvHistory() const { return m_telemetrySource.fpvHistory(); }
/// @brief Returns recent propulsion RPM history.
/// @return Historical RPM samples.
QVariantList PrimaryFlightViewModel::propulsionRpmHistory() const { return m_telemetrySource.propulsionRpmHistory(); }
/// @brief Returns recent propulsion tilt history.
/// @return Historical tilt samples.
QVariantList PrimaryFlightViewModel::propulsionTiltHistory() const { return m_telemetrySource.propulsionTiltHistory(); }
/// @brief Returns recent propulsion temperature history.
/// @return Historical temperature samples.
QVariantList PrimaryFlightViewModel::propulsionTempHistory() const { return m_telemetrySource.propulsionTempHistory(); }
/// @brief Returns recent propulsion thrust history.
/// @return Historical thrust samples.
QVariantList PrimaryFlightViewModel::propulsionThrustHistory() const { return m_telemetrySource.propulsionThrustHistory(); }
/// @brief Returns recent inverter-health history.
/// @return Historical inverter-health samples.
QVariantList PrimaryFlightViewModel::propulsionInverterHealthHistory() const { return m_telemetrySource.propulsionInverterHealthHistory(); }

/// @brief Formats the summary for a primary flight instrument.
/// @param index Instrument index selected by QML.
/// @return Human-readable instrument summary.
QString PrimaryFlightViewModel::instrumentSummary(int index) const
{
    switch (index) {
    case 0:
        return QStringLiteral("CAS %1 kt  |  TAS %2 kt").arg(cas(), 0, 'f', 0).arg(tas(), 0, 'f', 0);
    case 1:
        return QStringLiteral("BARO %1 ft  |  RADIO %2 ft").arg(altBaro(), 0, 'f', 0).arg(altRadar(), 0, 'f', 0);
    case 2:
        return QStringLiteral("VERTICAL RATE %1%2 ft/min").arg(vs() >= 0.0 ? QStringLiteral("+") : QString()).arg(vs(), 0, 'f', 0);
    case 3:
        return QStringLiteral("PITCH %1°  |  ROLL %2°  |  YAW %3°")
            .arg(pitch(), 0, 'f', 1).arg(roll(), 0, 'f', 1).arg(yaw(), 0, 'f', 1);
    case 4:
        return QStringLiteral("MAG %1°  |  TRACK %2°  |  DRIFT %3°")
            .arg(heading(), 0, 'f', 0).arg(track(), 0, 'f', 0).arg(headingTrackDelta(), 0, 'f', 1);
    default:
        return QStringLiteral("PATH QUALITY %1%  |  CROSS-TRACK %2°")
            .arg(fpvProgress() * 100.0, 0, 'f', 0).arg(headingTrackDelta(), 0, 'f', 1);
    }
}

/// @brief Calculates a calibrated-airspeed tape mark around the current value.
/// @param index Tape mark index.
/// @return Airspeed value for the requested mark.
double PrimaryFlightViewModel::speedTapeValue(int index) const
{
    return qRound(cas() / 10.0) * 10.0 + (4 - qBound(0, index, 8)) * 10.0;
}

/// @brief Calculates a barometric-altitude tape mark around the current value.
/// @param index Tape mark index.
/// @return Altitude value for the requested mark.
double PrimaryFlightViewModel::altitudeTapeValue(int index) const
{
    return qRound(altBaro() / 500.0) * 500.0 + (4 - qBound(0, index, 8)) * 500.0;
}

/// @brief Calculates a vertical-speed scale mark.
/// @param index Scale mark index.
/// @return Vertical-speed value for the requested mark.
int PrimaryFlightViewModel::verticalSpeedMark(int index) const
{
    return (3 - qBound(0, index, 6)) * 1000;
}

/// @brief Maps normalized progress to a confidence label.
/// @param progress Normalized progress value.
/// @return High, Moderate, or Low confidence text.
QString PrimaryFlightViewModel::progressConfidence(double progress) const
{
    return progress >= 0.72 ? QStringLiteral("High")
           : progress >= 0.45 ? QStringLiteral("Moderate") : QStringLiteral("Low");
}

/// @brief Determines the direction of the latest history change.
/// @param values Ordered historical samples.
/// @return Rising, Falling, or Stable.
QString PrimaryFlightViewModel::historyTrend(const QVariantList &values) const
{
    if (values.size() < 2) {
        return QStringLiteral("Stable");
    }
    // Compare only the latest samples to describe the immediate trend.
    const double delta = values.last().toDouble() - values.at(values.size() - 2).toDouble();
    return delta > 0.001 ? QStringLiteral("Rising")
           : delta < -0.001 ? QStringLiteral("Falling") : QStringLiteral("Stable");
}