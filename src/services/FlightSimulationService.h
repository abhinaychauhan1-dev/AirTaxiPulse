/**
 * @file    : src/services/FlightSimulationService.h
 * @brief   : Declares the simulated flight telemetry service.
 * @author  : Abhinay Chauhan (email: abhinay.chauhan1@gmail.com)
 * @version : 1.0.0
 *
 * Copyright (c) 2024
 * Abhinay Chauhan. All rights reserved.
 */

#ifndef FLIGHTSIMULATIONSERVICE_H
#define FLIGHTSIMULATIONSERVICE_H

#include <QObject>
#include <QTimer>
#include <QVector>
#include <QVariantList>
#include <QColor>

#include "../model/FlightTelemetryData.h"
#include "../interfaces/IFlightTelemetrySource.h"

/// @brief Generates deterministic flight, energy, and propulsion telemetry for the UI.
///
/// The service advances a repeating flight profile, maintains bounded trend histories,
/// and exposes both raw telemetry and presentation-ready values to QML.
class FlightSimulationService : public QObject, public IFlightTelemetrySource
{
    Q_OBJECT

    Q_PROPERTY(FlightTelemetryData* telemetry READ telemetry CONSTANT)
    Q_PROPERTY(double casProgress READ casProgress NOTIFY telemetryChanged)
    Q_PROPERTY(double altProgress READ altProgress NOTIFY telemetryChanged)
    Q_PROPERTY(double vsProgress READ vsProgress NOTIFY telemetryChanged)
    Q_PROPERTY(double attitudeProgress READ attitudeProgress NOTIFY telemetryChanged)
    Q_PROPERTY(double headingProgress READ headingProgress NOTIFY telemetryChanged)
    Q_PROPERTY(double fpvProgress READ fpvProgress NOTIFY telemetryChanged)
    Q_PROPERTY(QString flightModeLabel READ flightModeLabel NOTIFY telemetryChanged)
    Q_PROPERTY(QColor vsAccentColor READ vsAccentColor NOTIFY telemetryChanged)
    Q_PROPERTY(QString vsTrendLabel READ vsTrendLabel NOTIFY telemetryChanged)
    Q_PROPERTY(QString vsValueText READ vsValueText NOTIFY telemetryChanged)
    Q_PROPERTY(double batterySoc READ batterySoc NOTIFY telemetryChanged)
    Q_PROPERTY(double batterySoh READ batterySoh NOTIFY telemetryChanged)
    Q_PROPERTY(double powerConsumptionKw READ powerConsumptionKw NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList batteryCellTemperatures READ batteryCellTemperatures NOTIFY telemetryChanged)
    Q_PROPERTY(double busVoltage READ busVoltage NOTIFY telemetryChanged)
    Q_PROPERTY(double busCurrent READ busCurrent NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList motorTemperatures READ motorTemperatures NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList motorRpmValues READ motorRpmValues NOTIFY telemetryChanged)
    Q_PROPERTY(double tiltAngleDeg READ tiltAngleDeg NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList thrustOutputs READ thrustOutputs NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList inverterVoltages READ inverterVoltages NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList inverterCurrents READ inverterCurrents NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList inverterHealth READ inverterHealth NOTIFY telemetryChanged)
    Q_PROPERTY(double gpsLatitude READ gpsLatitude NOTIFY telemetryChanged)
    Q_PROPERTY(double gpsLongitude READ gpsLongitude NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList casHistory READ casHistory NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList altHistory READ altHistory NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList vsHistory READ vsHistory NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList attitudeHistory READ attitudeHistory NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList headingHistory READ headingHistory NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList fpvHistory READ fpvHistory NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList propulsionRpmHistory READ propulsionRpmHistory NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList propulsionTiltHistory READ propulsionTiltHistory NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList propulsionTempHistory READ propulsionTempHistory NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList propulsionThrustHistory READ propulsionThrustHistory NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList propulsionInverterHealthHistory READ propulsionInverterHealthHistory NOTIFY telemetryChanged)

public:
    /// @brief Constructs the service and starts periodic telemetry generation.
    /// @param parent Optional QObject that owns the service.
    explicit FlightSimulationService(QObject *parent = nullptr);

    /// @brief Returns the mutable telemetry object exposed to QML.
    /// @return Pointer to the service-owned telemetry data.
    FlightTelemetryData *telemetry() const;

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

    /// @brief Returns normalized calibrated-airspeed display progress.
    /// @return Progress value mapped to the instrument display band.
    double casProgress() const;
    /// @brief Returns normalized blended-altitude display progress.
    /// @return Progress value mapped to the instrument display band.
    double altProgress() const;
    /// @brief Returns normalized vertical-speed display progress.
    /// @return Progress value mapped to the instrument display band.
    double vsProgress() const;
    /// @brief Returns normalized attitude-stability display progress.
    /// @return Progress value mapped to the instrument display band.
    double attitudeProgress() const;
    /// @brief Returns heading display progress derived from the current heading.
    /// @return Progress value mapped to the instrument display band.
    double headingProgress() const;
    /// @brief Returns flight-path-vector alignment progress.
    /// @return Alignment value in the range 0.0 to 1.0.
    double fpvProgress() const;
    /// @brief Returns the label for the active simulated flight phase.
    /// @return Human-readable flight mode label.
    QString flightModeLabel() const;
    /// @brief Returns the vertical-speed status color.
    /// @return Accent color for climbing or descending flight.
    QColor vsAccentColor() const;
    /// @brief Returns the vertical-speed trend description.
    /// @return Ascending or descending label.
    QString vsTrendLabel() const;
    /// @brief Formats vertical speed for presentation.
    /// @return Signed vertical-speed text with units.
    QString vsValueText() const;
    /// @brief Returns battery state of charge.
    /// @return State of charge as a percentage.
    double batterySoc() const;
    /// @brief Returns battery state of health.
    /// @return State of health as a percentage.
    double batterySoh() const override;
    /// @brief Returns current propulsion power consumption.
    /// @return Power consumption in kilowatts.
    double powerConsumptionKw() const override;
    /// @brief Returns simulated battery-cell temperatures.
    /// @return Cell temperatures in degrees Celsius.
    QVariantList batteryCellTemperatures() const override;
    /// @brief Returns high-voltage bus voltage.
    /// @return Bus voltage in volts.
    double busVoltage() const override;
    /// @brief Returns high-voltage bus current.
    /// @return Bus current in amperes.
    double busCurrent() const override;
    /// @brief Returns simulated motor temperatures.
    /// @return Motor temperatures in degrees Celsius.
    QVariantList motorTemperatures() const;
    /// @brief Returns simulated motor speeds.
    /// @return Motor speeds in revolutions per minute.
    QVariantList motorRpmValues() const override;
    /// @brief Returns the propulsion tilt angle.
    /// @return Tilt angle in degrees.
    double tiltAngleDeg() const override;
    /// @brief Returns front and rear thrust outputs.
    /// @return Thrust output values.
    QVariantList thrustOutputs() const override;
    /// @brief Returns inverter voltage measurements.
    /// @return Per-inverter voltages in volts.
    QVariantList inverterVoltages() const override;
    /// @brief Returns inverter current measurements.
    /// @return Per-inverter currents in amperes.
    QVariantList inverterCurrents() const override;
    /// @brief Returns inverter health estimates.
    /// @return Per-inverter health percentages.
    QVariantList inverterHealth() const override;
    /// @brief Returns the simulated GPS latitude.
    /// @return Latitude in decimal degrees.
    double gpsLatitude() const;
    /// @brief Returns the simulated GPS longitude.
    /// @return Longitude in decimal degrees.
    double gpsLongitude() const;
    /// @brief Returns recent calibrated-airspeed samples.
    /// @return Bounded airspeed history.
    QVariantList casHistory() const;
    /// @brief Returns recent barometric-altitude samples.
    /// @return Bounded altitude history.
    QVariantList altHistory() const;
    /// @brief Returns recent vertical-speed samples.
    /// @return Bounded vertical-speed history.
    QVariantList vsHistory() const;
    /// @brief Returns recent combined attitude-load samples.
    /// @return Bounded attitude history.
    QVariantList attitudeHistory() const;
    /// @brief Returns recent heading samples.
    /// @return Bounded heading history.
    QVariantList headingHistory() const;
    /// @brief Returns recent flight-path-vector error samples.
    /// @return Bounded path-error history.
    QVariantList fpvHistory() const;
    /// @brief Returns recent average propulsion-speed samples.
    /// @return Bounded motor-speed history.
    QVariantList propulsionRpmHistory() const override;
    /// @brief Returns recent propulsion tilt samples.
    /// @return Bounded tilt-angle history.
    QVariantList propulsionTiltHistory() const override;
    /// @brief Returns recent average motor-temperature samples.
    /// @return Bounded motor-temperature history.
    QVariantList propulsionTempHistory() const override;
    /// @brief Returns recent total-thrust samples.
    /// @return Bounded thrust history.
    QVariantList propulsionThrustHistory() const override;
    /// @brief Returns recent average inverter-health samples.
    /// @return Bounded inverter-health history.
    QVariantList propulsionInverterHealthHistory() const override;

signals:
    /// @brief Notifies observers that telemetry values and histories have advanced.
    void telemetryChanged();

private slots:
    /// @brief Advances the simulation by one timer interval.
    void updateSimulation();

private:
    /// @brief Restricts a value to an inclusive numeric range.
    /// @param value Value to restrict.
    /// @param minimumValue Inclusive lower bound.
    /// @param maximumValue Inclusive upper bound.
    /// @return The value constrained to the supplied bounds.
    static double clamp(double value, double minimumValue, double maximumValue);
    /// @brief Appends a sample while preserving the bounded history length.
    /// @param history History buffer to update.
    /// @param value Sample to append.
    void appendHistory(QVector<double> &history, double value);
    /// @brief Converts an internal numeric history for QML consumption.
    /// @param history Numeric history to convert.
    /// @return QVariant list containing the history samples.
    QVariantList historyToVariantList(const QVector<double> &history) const;

    double m_phase;
    QString m_flightModeLabel;
    double m_batterySoc;
    double m_batterySoh;
    double m_powerConsumptionKw;
    QVector<double> m_batteryCellTemperatures;
    double m_busVoltage;
    double m_busCurrent;
    QVector<double> m_motorTemperatures;
    QVector<double> m_motorRpmValues;
    double m_tiltAngleDeg;
    QVector<double> m_thrustOutputs;
    QVector<double> m_inverterVoltages;
    QVector<double> m_inverterCurrents;
    QVector<double> m_inverterHealth;
    double m_gpsLatitude;
    double m_gpsLongitude;
    FlightTelemetryData m_telemetry;
    QVector<double> m_casHistory;
    QVector<double> m_altHistory;
    QVector<double> m_vsHistory;
    QVector<double> m_attitudeHistory;
    QVector<double> m_headingHistory;
    QVector<double> m_fpvHistory;
    QVector<double> m_propulsionRpmHistory;
    QVector<double> m_propulsionTiltHistory;
    QVector<double> m_propulsionTempHistory;
    QVector<double> m_propulsionThrustHistory;
    QVector<double> m_propulsionInverterHealthHistory;
    QTimer m_timer;
};

#endif // FLIGHTSIMULATIONSERVICE_H