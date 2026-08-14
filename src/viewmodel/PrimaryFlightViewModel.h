/**
 * @file    : src/viewmodel/PrimaryFlightViewModel.h
 * @brief   : Declares the view model for primary flight telemetry.
 * @author  : Abhinay Chauhan (email: abhinay.chauhan1@gmail.com)
 * @version : 1.0.0
 *
 * Copyright (c) 2024
 * Abhinay Chauhan. All rights reserved.
 */

#ifndef PRIMARYFLIGHTVIEWMODEL_H
#define PRIMARYFLIGHTVIEWMODEL_H

#include <QObject>
#include <QColor>
#include <QVariantList>

#include "../interfaces/IFlightTelemetrySource.h"

/// @brief Adapts primary flight, energy, propulsion, and navigation telemetry for QML.
class PrimaryFlightViewModel : public QObject
{
    Q_OBJECT

    /// @brief Title displayed above the primary flight instruments.
    Q_PROPERTY(QString title READ title CONSTANT)
    /// @brief Calibrated airspeed in knots.
    Q_PROPERTY(double cas READ cas NOTIFY telemetryChanged)
    /// @brief True airspeed in knots.
    Q_PROPERTY(double tas READ tas NOTIFY telemetryChanged)
    /// @brief Barometric altitude in feet.
    Q_PROPERTY(double altBaro READ altBaro NOTIFY telemetryChanged)
    /// @brief Radar altitude in feet.
    Q_PROPERTY(double altRadar READ altRadar NOTIFY telemetryChanged)
    /// @brief Vertical speed in feet per minute.
    Q_PROPERTY(double vs READ vs NOTIFY telemetryChanged)
    /// @brief Aircraft pitch angle in degrees.
    Q_PROPERTY(double pitch READ pitch NOTIFY telemetryChanged)
    /// @brief Aircraft roll angle in degrees.
    Q_PROPERTY(double roll READ roll NOTIFY telemetryChanged)
    /// @brief Aircraft yaw angle in degrees.
    Q_PROPERTY(double yaw READ yaw NOTIFY telemetryChanged)
    /// @brief Magnetic heading in degrees.
    Q_PROPERTY(double heading READ heading NOTIFY telemetryChanged)
    /// @brief Ground track in degrees.
    Q_PROPERTY(double track READ track NOTIFY telemetryChanged)
    /// @brief Signed shortest angular difference between track and heading.
    Q_PROPERTY(double headingTrackDelta READ headingTrackDelta NOTIFY telemetryChanged)
    /// @brief Normalized slip indicator offset.
    Q_PROPERTY(double slipOffset READ slipOffset NOTIFY telemetryChanged)
    /// @brief Normalized calibrated-airspeed confidence or range progress.
    Q_PROPERTY(double casProgress READ casProgress NOTIFY telemetryChanged)
    /// @brief Normalized altitude confidence or range progress.
    Q_PROPERTY(double altProgress READ altProgress NOTIFY telemetryChanged)
    /// @brief Normalized vertical-speed confidence or range progress.
    Q_PROPERTY(double vsProgress READ vsProgress NOTIFY telemetryChanged)
    /// @brief Normalized attitude confidence or range progress.
    Q_PROPERTY(double attitudeProgress READ attitudeProgress NOTIFY telemetryChanged)
    /// @brief Normalized heading confidence or range progress.
    Q_PROPERTY(double headingProgress READ headingProgress NOTIFY telemetryChanged)
    /// @brief Normalized flight-path-vector confidence or range progress.
    Q_PROPERTY(double fpvProgress READ fpvProgress NOTIFY telemetryChanged)
    /// @brief Human-readable active flight mode.
    Q_PROPERTY(QString flightModeLabel READ flightModeLabel NOTIFY telemetryChanged)
    /// @brief Accent color selected for the current vertical-speed trend.
    Q_PROPERTY(QColor vsAccentColor READ vsAccentColor NOTIFY telemetryChanged)
    /// @brief Human-readable vertical-speed trend.
    Q_PROPERTY(QString vsTrendLabel READ vsTrendLabel NOTIFY telemetryChanged)
    /// @brief Formatted vertical-speed value.
    Q_PROPERTY(QString vsValueText READ vsValueText NOTIFY telemetryChanged)
    /// @brief Battery state of charge in percent.
    Q_PROPERTY(double batterySoc READ batterySoc NOTIFY telemetryChanged)
    /// @brief Battery state of health in percent.
    Q_PROPERTY(double batterySoh READ batterySoh NOTIFY telemetryChanged)
    /// @brief Aircraft electrical power consumption in kilowatts.
    Q_PROPERTY(double powerConsumptionKw READ powerConsumptionKw NOTIFY telemetryChanged)
    /// @brief Minimum reported battery-cell temperature.
    Q_PROPERTY(double cellTemperatureMin READ cellTemperatureMin NOTIFY telemetryChanged)
    /// @brief Maximum reported battery-cell temperature.
    Q_PROPERTY(double cellTemperatureMax READ cellTemperatureMax NOTIFY telemetryChanged)
    /// @brief Per-cell battery temperatures.
    Q_PROPERTY(QVariantList batteryCellTemperatures READ batteryCellTemperatures NOTIFY telemetryChanged)
    /// @brief Whether the maximum cell temperature has reached the warning threshold.
    Q_PROPERTY(bool thermalRunawayWarning READ thermalRunawayWarning NOTIFY telemetryChanged)
    /// @brief High-voltage bus voltage.
    Q_PROPERTY(double busVoltage READ busVoltage NOTIFY telemetryChanged)
    /// @brief High-voltage bus current.
    Q_PROPERTY(double busCurrent READ busCurrent NOTIFY telemetryChanged)
    /// @brief Per-motor temperatures.
    Q_PROPERTY(QVariantList motorTemperatures READ motorTemperatures NOTIFY telemetryChanged)
    /// @brief Per-motor rotational speeds.
    Q_PROPERTY(QVariantList motorRpmValues READ motorRpmValues NOTIFY telemetryChanged)
    /// @brief Propulsion-pod tilt angle in degrees.
    Q_PROPERTY(double tiltAngleDeg READ tiltAngleDeg NOTIFY telemetryChanged)
    /// @brief Front and rear propulsion thrust outputs.
    Q_PROPERTY(QVariantList thrustOutputs READ thrustOutputs NOTIFY telemetryChanged)
    /// @brief Per-inverter voltages.
    Q_PROPERTY(QVariantList inverterVoltages READ inverterVoltages NOTIFY telemetryChanged)
    /// @brief Per-inverter currents.
    Q_PROPERTY(QVariantList inverterCurrents READ inverterCurrents NOTIFY telemetryChanged)
    /// @brief Per-inverter health percentages.
    Q_PROPERTY(QVariantList inverterHealth READ inverterHealth NOTIFY telemetryChanged)
    /// @brief Current GPS latitude in decimal degrees.
    Q_PROPERTY(double gpsLatitude READ gpsLatitude NOTIFY telemetryChanged)
    /// @brief Current GPS longitude in decimal degrees.
    Q_PROPERTY(double gpsLongitude READ gpsLongitude NOTIFY telemetryChanged)
    /// @brief Formatted GPS coordinates or an unavailable marker.
    Q_PROPERTY(QString gpsText READ gpsText NOTIFY telemetryChanged)
    /// @brief Mean temperature across all reported motors.
    Q_PROPERTY(double averageMotorTemperature READ averageMotorTemperature NOTIFY telemetryChanged)
    /// @brief Recent calibrated-airspeed samples.
    Q_PROPERTY(QVariantList casHistory READ casHistory NOTIFY telemetryChanged)
    /// @brief Recent barometric-altitude samples.
    Q_PROPERTY(QVariantList altHistory READ altHistory NOTIFY telemetryChanged)
    /// @brief Recent vertical-speed samples.
    Q_PROPERTY(QVariantList vsHistory READ vsHistory NOTIFY telemetryChanged)
    /// @brief Recent attitude samples.
    Q_PROPERTY(QVariantList attitudeHistory READ attitudeHistory NOTIFY telemetryChanged)
    /// @brief Recent heading samples.
    Q_PROPERTY(QVariantList headingHistory READ headingHistory NOTIFY telemetryChanged)
    /// @brief Recent flight-path-vector samples.
    Q_PROPERTY(QVariantList fpvHistory READ fpvHistory NOTIFY telemetryChanged)
    /// @brief Recent average propulsion RPM samples.
    Q_PROPERTY(QVariantList propulsionRpmHistory READ propulsionRpmHistory NOTIFY telemetryChanged)
    /// @brief Recent propulsion tilt-angle samples.
    Q_PROPERTY(QVariantList propulsionTiltHistory READ propulsionTiltHistory NOTIFY telemetryChanged)
    /// @brief Recent propulsion temperature samples.
    Q_PROPERTY(QVariantList propulsionTempHistory READ propulsionTempHistory NOTIFY telemetryChanged)
    /// @brief Recent combined-thrust samples.
    Q_PROPERTY(QVariantList propulsionThrustHistory READ propulsionThrustHistory NOTIFY telemetryChanged)
    /// @brief Recent inverter-health samples.
    Q_PROPERTY(QVariantList propulsionInverterHealthHistory READ propulsionInverterHealthHistory NOTIFY telemetryChanged)

public:
    /// @brief Constructs the primary flight adapter for a telemetry source.
    /// @param title Instrument page title.
    /// @param telemetrySource Source that supplies live and historical telemetry.
    /// @param parent Optional QObject parent for Qt ownership.
    explicit PrimaryFlightViewModel(const QString &title,
                                    IFlightTelemetrySource &telemetrySource,
                                    QObject *parent = nullptr);

    /// @brief Returns the primary flight page title.
    /// @return Title text displayed by the UI.
    QString title() const;

    /// @brief Returns calibrated airspeed.
    /// @return Calibrated airspeed in knots.
    double cas() const;
    /// @brief Returns true airspeed.
    /// @return True airspeed in knots.
    double tas() const;
    /// @brief Returns barometric altitude.
    /// @return Barometric altitude in feet.
    double altBaro() const;
    /// @brief Returns radar altitude.
    /// @return Radar altitude in feet.
    double altRadar() const;
    /// @brief Returns vertical speed.
    /// @return Vertical speed in feet per minute.
    double vs() const;
    /// @brief Returns aircraft pitch.
    /// @return Pitch angle in degrees.
    double pitch() const;
    /// @brief Returns aircraft roll.
    /// @return Roll angle in degrees.
    double roll() const;
    /// @brief Returns aircraft yaw.
    /// @return Yaw angle in degrees.
    double yaw() const;
    /// @brief Returns magnetic heading.
    /// @return Heading in degrees.
    double heading() const;
    /// @brief Returns ground track.
    /// @return Track in degrees.
    double track() const;
    /// @brief Returns the normalized signed heading-to-track difference.
    /// @return Shortest angular delta in the range [-180, 180).
    double headingTrackDelta() const;
    /// @brief Returns the normalized slip indicator offset.
    /// @return Slip offset clamped to [-1, 1].
    double slipOffset() const;
    /// @brief Returns calibrated-airspeed progress.
    /// @return Normalized progress value.
    double casProgress() const;
    /// @brief Returns altitude progress.
    /// @return Normalized progress value.
    double altProgress() const;
    /// @brief Returns vertical-speed progress.
    /// @return Normalized progress value.
    double vsProgress() const;
    /// @brief Returns attitude progress.
    /// @return Normalized progress value.
    double attitudeProgress() const;
    /// @brief Returns heading progress.
    /// @return Normalized progress value.
    double headingProgress() const;
    /// @brief Returns flight-path-vector progress.
    /// @return Normalized progress value.
    double fpvProgress() const;
    /// @brief Returns the active flight-mode label.
    /// @return Human-readable flight mode.
    QString flightModeLabel() const;
    /// @brief Returns the vertical-speed accent color.
    /// @return Color associated with the current trend.
    QColor vsAccentColor() const;
    /// @brief Returns the vertical-speed trend label.
    /// @return Human-readable trend text.
    QString vsTrendLabel() const;
    /// @brief Returns formatted vertical speed.
    /// @return Vertical-speed display text.
    QString vsValueText() const;
    /// @brief Returns battery state of charge.
    /// @return State of charge in percent.
    double batterySoc() const;
    /// @brief Returns battery state of health.
    /// @return State of health in percent.
    double batterySoh() const;
    /// @brief Returns electrical power consumption.
    /// @return Power consumption in kilowatts.
    double powerConsumptionKw() const;
    /// @brief Finds the lowest reported battery-cell temperature.
    /// @return Minimum temperature, or 0 when no samples exist.
    double cellTemperatureMin() const;
    /// @brief Finds the highest reported battery-cell temperature.
    /// @return Maximum temperature, or 0 when no samples exist.
    double cellTemperatureMax() const;
    /// @brief Returns per-cell battery temperatures.
    /// @return Battery-cell temperature samples.
    QVariantList batteryCellTemperatures() const;
    /// @brief Reports whether battery temperature exceeds the warning threshold.
    /// @return true when the hottest cell is at least 60 degrees.
    bool thermalRunawayWarning() const;
    /// @brief Returns high-voltage bus voltage.
    /// @return Bus voltage.
    double busVoltage() const;
    /// @brief Returns high-voltage bus current.
    /// @return Bus current.
    double busCurrent() const;
    /// @brief Returns per-motor temperatures.
    /// @return Motor temperature samples.
    QVariantList motorTemperatures() const;
    /// @brief Returns per-motor rotational speeds.
    /// @return Motor RPM samples.
    QVariantList motorRpmValues() const;
    /// @brief Returns propulsion-pod tilt angle.
    /// @return Tilt angle in degrees.
    double tiltAngleDeg() const;
    /// @brief Returns propulsion thrust outputs.
    /// @return Front and rear thrust samples.
    QVariantList thrustOutputs() const;
    /// @brief Returns per-inverter voltages.
    /// @return Inverter voltage samples.
    QVariantList inverterVoltages() const;
    /// @brief Returns per-inverter currents.
    /// @return Inverter current samples.
    QVariantList inverterCurrents() const;
    /// @brief Returns per-inverter health values.
    /// @return Inverter health percentages.
    QVariantList inverterHealth() const;
    /// @brief Returns current GPS latitude.
    /// @return Latitude in decimal degrees.
    double gpsLatitude() const;
    /// @brief Returns current GPS longitude.
    /// @return Longitude in decimal degrees.
    double gpsLongitude() const;
    /// @brief Formats the current GPS coordinates for display.
    /// @return Coordinate text or N/A for non-finite values.
    QString gpsText() const;
    /// @brief Calculates the mean reported motor temperature.
    /// @return Average temperature, or 0 when no samples exist.
    double averageMotorTemperature() const;
    /// @brief Returns recent calibrated-airspeed history.
    /// @return Historical calibrated-airspeed samples.
    QVariantList casHistory() const;
    /// @brief Returns recent altitude history.
    /// @return Historical altitude samples.
    QVariantList altHistory() const;
    /// @brief Returns recent vertical-speed history.
    /// @return Historical vertical-speed samples.
    QVariantList vsHistory() const;
    /// @brief Returns recent attitude history.
    /// @return Historical attitude samples.
    QVariantList attitudeHistory() const;
    /// @brief Returns recent heading history.
    /// @return Historical heading samples.
    QVariantList headingHistory() const;
    /// @brief Returns recent flight-path-vector history.
    /// @return Historical flight-path-vector samples.
    QVariantList fpvHistory() const;
    /// @brief Returns recent propulsion RPM history.
    /// @return Historical RPM samples.
    QVariantList propulsionRpmHistory() const;
    /// @brief Returns recent propulsion tilt history.
    /// @return Historical tilt samples.
    QVariantList propulsionTiltHistory() const;
    /// @brief Returns recent propulsion temperature history.
    /// @return Historical temperature samples.
    QVariantList propulsionTempHistory() const;
    /// @brief Returns recent propulsion thrust history.
    /// @return Historical thrust samples.
    QVariantList propulsionThrustHistory() const;
    /// @brief Returns recent inverter-health history.
    /// @return Historical inverter-health samples.
    QVariantList propulsionInverterHealthHistory() const;

    /// @brief Formats the summary for a primary flight instrument.
    /// @param index Instrument index selected by QML.
    /// @return Human-readable instrument summary.
    Q_INVOKABLE QString instrumentSummary(int index) const;
    /// @brief Calculates a calibrated-airspeed tape mark.
    /// @param index Tape mark index.
    /// @return Airspeed value for the requested mark.
    Q_INVOKABLE double speedTapeValue(int index) const;
    /// @brief Calculates a barometric-altitude tape mark.
    /// @param index Tape mark index.
    /// @return Altitude value for the requested mark.
    Q_INVOKABLE double altitudeTapeValue(int index) const;
    /// @brief Calculates a vertical-speed scale mark.
    /// @param index Scale mark index.
    /// @return Vertical-speed value for the requested mark.
    Q_INVOKABLE int verticalSpeedMark(int index) const;
    /// @brief Maps normalized progress to a confidence label.
    /// @param progress Normalized progress value.
    /// @return High, Moderate, or Low confidence text.
    Q_INVOKABLE QString progressConfidence(double progress) const;
    /// @brief Determines the direction of the latest history change.
    /// @param values Ordered historical samples.
    /// @return Rising, Falling, or Stable.
    Q_INVOKABLE QString historyTrend(const QVariantList &values) const;

signals:
    /// @brief Emitted when source telemetry changes and QML bindings must refresh.
    void telemetryChanged();

private:
    QString m_title;
    IFlightTelemetrySource &m_telemetrySource;
};

#endif // PRIMARYFLIGHTVIEWMODEL_H