/**
 * @file    : src/interfaces/IFlightTelemetrySource.h
 * @brief   : Declares the interface for flight telemetry data sources.
 * @author  : Abhinay Chauhan (email: abhinay.chauhan1@gmail.com)
 * @version : 1.0.0
 *
 * Copyright (c) 2024
 * Abhinay Chauhan. All rights reserved.
 */

#ifndef IFLIGHTTELEMETRYSOURCE_H
#define IFLIGHTTELEMETRYSOURCE_H

#include <QColor>
#include <QVariantList>
#include <QString>

/// @brief Defines the telemetry contract consumed by Air Taxi view models.
class IFlightTelemetrySource
{
public:
    /// @brief Destroys the telemetry source interface.
    virtual ~IFlightTelemetrySource() = default;

    /// @brief Returns calibrated airspeed.
    /// @return Calibrated airspeed in knots.
    virtual double cas() const = 0;

    /// @brief Returns true airspeed.
    /// @return True airspeed in knots.
    virtual double tas() const = 0;

    /// @brief Returns barometric altitude.
    /// @return Barometric altitude in feet.
    virtual double altBaro() const = 0;

    /// @brief Returns radar altitude.
    /// @return Radar altitude in feet.
    virtual double altRadar() const = 0;

    /// @brief Returns vertical speed.
    /// @return Vertical speed in feet per minute.
    virtual double vs() const = 0;

    /// @brief Returns aircraft pitch attitude.
    /// @return Pitch angle in degrees.
    virtual double pitch() const = 0;

    /// @brief Returns aircraft roll attitude.
    /// @return Roll angle in degrees.
    virtual double roll() const = 0;

    /// @brief Returns aircraft yaw attitude.
    /// @return Yaw angle in degrees.
    virtual double yaw() const = 0;

    /// @brief Returns aircraft heading.
    /// @return Heading in degrees.
    virtual double heading() const = 0;

    /// @brief Returns aircraft ground track.
    /// @return Ground track in degrees.
    virtual double track() const = 0;

    /// @brief Returns normalized calibrated-airspeed progress.
    /// @return Progress value used by the primary flight display.
    virtual double casProgress() const = 0;

    /// @brief Returns normalized altitude progress.
    /// @return Progress value used by the primary flight display.
    virtual double altProgress() const = 0;

    /// @brief Returns normalized vertical-speed progress.
    /// @return Progress value used by the primary flight display.
    virtual double vsProgress() const = 0;

    /// @brief Returns normalized attitude progress.
    /// @return Progress value used by the primary flight display.
    virtual double attitudeProgress() const = 0;

    /// @brief Returns normalized heading progress.
    /// @return Progress value used by the primary flight display.
    virtual double headingProgress() const = 0;

    /// @brief Returns normalized flight-path-vector progress.
    /// @return Progress value used by the primary flight display.
    virtual double fpvProgress() const = 0;

    /// @brief Returns the active flight mode label.
    /// @return Human-readable flight mode text.
    virtual QString flightModeLabel() const = 0;

    /// @brief Returns the vertical-speed accent color.
    /// @return Color representing the current vertical-speed state.
    virtual QColor vsAccentColor() const = 0;

    /// @brief Returns the vertical-speed trend label.
    /// @return Human-readable climb or descent trend text.
    virtual QString vsTrendLabel() const = 0;

    /// @brief Returns formatted vertical-speed text.
    /// @return Vertical-speed value formatted for display.
    virtual QString vsValueText() const = 0;

    /// @brief Returns battery state of charge.
    /// @return Battery state of charge as a percentage.
    virtual double batterySoc() const = 0;

    /// @brief Returns battery state of health.
    /// @return Battery state of health as a percentage.
    virtual double batterySoh() const = 0;

    /// @brief Returns current electrical power consumption.
    /// @return Power consumption in kilowatts.
    virtual double powerConsumptionKw() const = 0;

    /// @brief Returns battery cell temperatures.
    /// @return QML-compatible list of cell temperatures in Celsius.
    virtual QVariantList batteryCellTemperatures() const = 0;

    /// @brief Returns high-voltage bus voltage.
    /// @return Bus voltage in volts.
    virtual double busVoltage() const = 0;

    /// @brief Returns high-voltage bus current.
    /// @return Bus current in amperes.
    virtual double busCurrent() const = 0;

    /// @brief Returns propulsion motor temperatures.
    /// @return QML-compatible list of motor temperatures in Celsius.
    virtual QVariantList motorTemperatures() const = 0;

    /// @brief Returns propulsion motor speeds.
    /// @return QML-compatible list of motor speeds in revolutions per minute.
    virtual QVariantList motorRpmValues() const = 0;

    /// @brief Returns the propulsion tilt angle.
    /// @return Tilt angle in degrees.
    virtual double tiltAngleDeg() const = 0;

    /// @brief Returns propulsion thrust outputs.
    /// @return QML-compatible list of thrust output values.
    virtual QVariantList thrustOutputs() const = 0;

    /// @brief Returns propulsion inverter voltages.
    /// @return QML-compatible list of inverter voltages.
    virtual QVariantList inverterVoltages() const = 0;

    /// @brief Returns propulsion inverter currents.
    /// @return QML-compatible list of inverter currents.
    virtual QVariantList inverterCurrents() const = 0;

    /// @brief Returns propulsion inverter health values.
    /// @return QML-compatible list of inverter health indicators.
    virtual QVariantList inverterHealth() const = 0;

    /// @brief Returns the current GPS latitude.
    /// @return Latitude in decimal degrees.
    virtual double gpsLatitude() const = 0;

    /// @brief Returns the current GPS longitude.
    /// @return Longitude in decimal degrees.
    virtual double gpsLongitude() const = 0;

    /// @brief Returns calibrated-airspeed history.
    /// @return QML-compatible sequence of recent airspeed samples.
    virtual QVariantList casHistory() const = 0;

    /// @brief Returns altitude history.
    /// @return QML-compatible sequence of recent altitude samples.
    virtual QVariantList altHistory() const = 0;

    /// @brief Returns vertical-speed history.
    /// @return QML-compatible sequence of recent vertical-speed samples.
    virtual QVariantList vsHistory() const = 0;

    /// @brief Returns attitude history.
    /// @return QML-compatible sequence of recent attitude samples.
    virtual QVariantList attitudeHistory() const = 0;

    /// @brief Returns heading history.
    /// @return QML-compatible sequence of recent heading samples.
    virtual QVariantList headingHistory() const = 0;

    /// @brief Returns flight-path-vector history.
    /// @return QML-compatible sequence of recent flight-path-vector samples.
    virtual QVariantList fpvHistory() const = 0;

    /// @brief Returns propulsion speed history.
    /// @return QML-compatible sequence of recent motor-speed samples.
    virtual QVariantList propulsionRpmHistory() const = 0;

    /// @brief Returns propulsion tilt history.
    /// @return QML-compatible sequence of recent tilt-angle samples.
    virtual QVariantList propulsionTiltHistory() const = 0;

    /// @brief Returns propulsion temperature history.
    /// @return QML-compatible sequence of recent motor-temperature samples.
    virtual QVariantList propulsionTempHistory() const = 0;

    /// @brief Returns propulsion thrust history.
    /// @return QML-compatible sequence of recent thrust samples.
    virtual QVariantList propulsionThrustHistory() const = 0;

    /// @brief Returns propulsion inverter health history.
    /// @return QML-compatible sequence of recent inverter health samples.
    virtual QVariantList propulsionInverterHealthHistory() const = 0;
};

#endif // IFLIGHTTELEMETRYSOURCE_H