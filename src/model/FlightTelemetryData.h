/**
 * @file    : src/model/FlightTelemetryData.h
 * @brief   : Declares the aircraft flight telemetry data model.
 * @author  : Abhinay Chauhan (email: abhinay.chauhan1@gmail.com)
 * @version : 1.0.0
 *
 * Copyright (c) 2024
 * Abhinay Chauhan. All rights reserved.
 */

#ifndef FLIGHTTELEMETRYDATA_H
#define FLIGHTTELEMETRYDATA_H

#include <QObject>

/// @brief Stores mutable aircraft telemetry and emits change notifications for QML.
class FlightTelemetryData : public QObject
{
    Q_OBJECT

    /// @brief Calibrated airspeed in knots.
    Q_PROPERTY(double cas READ cas WRITE setCas NOTIFY casChanged)

    /// @brief True airspeed in knots.
    Q_PROPERTY(double tas READ tas WRITE setTas NOTIFY tasChanged)

    /// @brief Barometric altitude in feet.
    Q_PROPERTY(double altBaro READ altBaro WRITE setAltBaro NOTIFY altBaroChanged)

    /// @brief Radar altitude in feet.
    Q_PROPERTY(double altRadar READ altRadar WRITE setAltRadar NOTIFY altRadarChanged)

    /// @brief Vertical speed in feet per minute.
    Q_PROPERTY(double vs READ vs WRITE setVs NOTIFY vsChanged)

    /// @brief Pitch attitude in degrees.
    Q_PROPERTY(double pitch READ pitch WRITE setPitch NOTIFY pitchChanged)

    /// @brief Roll attitude in degrees.
    Q_PROPERTY(double roll READ roll WRITE setRoll NOTIFY rollChanged)

    /// @brief Yaw attitude in degrees.
    Q_PROPERTY(double yaw READ yaw WRITE setYaw NOTIFY yawChanged)

    /// @brief Aircraft heading in degrees.
    Q_PROPERTY(double heading READ heading WRITE setHeading NOTIFY headingChanged)

    /// @brief Aircraft ground track in degrees.
    Q_PROPERTY(double track READ track WRITE setTrack NOTIFY trackChanged)

public:
    /// @brief Constructs telemetry data with representative initial flight values.
    /// @param parent Optional QObject parent for Qt ownership.
    explicit FlightTelemetryData(QObject *parent = nullptr);

    /// @brief Returns calibrated airspeed.
    /// @return Calibrated airspeed in knots.
    double cas() const;

    /// @brief Sets calibrated airspeed.
    /// @param value Calibrated airspeed in knots.
    void setCas(double value);

    /// @brief Returns true airspeed.
    /// @return True airspeed in knots.
    double tas() const;

    /// @brief Sets true airspeed.
    /// @param value True airspeed in knots.
    void setTas(double value);

    /// @brief Returns barometric altitude.
    /// @return Barometric altitude in feet.
    double altBaro() const;

    /// @brief Sets barometric altitude.
    /// @param value Barometric altitude in feet.
    void setAltBaro(double value);

    /// @brief Returns radar altitude.
    /// @return Radar altitude in feet.
    double altRadar() const;

    /// @brief Sets radar altitude.
    /// @param value Radar altitude in feet.
    void setAltRadar(double value);

    /// @brief Returns vertical speed.
    /// @return Vertical speed in feet per minute.
    double vs() const;

    /// @brief Sets vertical speed.
    /// @param value Vertical speed in feet per minute.
    void setVs(double value);

    /// @brief Returns pitch attitude.
    /// @return Pitch angle in degrees.
    double pitch() const;

    /// @brief Sets pitch attitude.
    /// @param value Pitch angle in degrees.
    void setPitch(double value);

    /// @brief Returns roll attitude.
    /// @return Roll angle in degrees.
    double roll() const;

    /// @brief Sets roll attitude.
    /// @param value Roll angle in degrees.
    void setRoll(double value);

    /// @brief Returns yaw attitude.
    /// @return Yaw angle in degrees.
    double yaw() const;

    /// @brief Sets yaw attitude.
    /// @param value Yaw angle in degrees.
    void setYaw(double value);

    /// @brief Returns aircraft heading.
    /// @return Heading in degrees.
    double heading() const;

    /// @brief Sets aircraft heading.
    /// @param value Heading in degrees.
    void setHeading(double value);

    /// @brief Returns aircraft ground track.
    /// @return Ground track in degrees.
    double track() const;

    /// @brief Sets aircraft ground track.
    /// @param value Ground track in degrees.
    void setTrack(double value);

signals:
    /// @brief Emitted when calibrated airspeed changes.
    void casChanged();

    /// @brief Emitted when true airspeed changes.
    void tasChanged();

    /// @brief Emitted when barometric altitude changes.
    void altBaroChanged();

    /// @brief Emitted when radar altitude changes.
    void altRadarChanged();

    /// @brief Emitted when vertical speed changes.
    void vsChanged();

    /// @brief Emitted when pitch attitude changes.
    void pitchChanged();

    /// @brief Emitted when roll attitude changes.
    void rollChanged();

    /// @brief Emitted when yaw attitude changes.
    void yawChanged();

    /// @brief Emitted when aircraft heading changes.
    void headingChanged();

    /// @brief Emitted when aircraft ground track changes.
    void trackChanged();

private:
    double m_cas;
    double m_tas;
    double m_altBaro;
    double m_altRadar;
    double m_vs;
    double m_pitch;
    double m_roll;
    double m_yaw;
    double m_heading;
    double m_track;
};

#endif // FLIGHTTELEMETRYDATA_H