/**
 * @file    : src/model/FlightTelemetryData.cpp
 * @brief   : Implements storage and change notification for flight telemetry.
 * @author  : Abhinay Chauhan (email: abhinay.chauhan1@gmail.com)
 * @version : 1.0.0
 *
 * Copyright (c) 2024
 * Abhinay Chauhan. All rights reserved.
 */

#include "FlightTelemetryData.h"

/// @brief Initializes telemetry with representative flight values.
/// @param parent Optional QObject parent for Qt ownership.
FlightTelemetryData::FlightTelemetryData(QObject *parent)
    : QObject(parent)
    , m_cas(145.0)
    , m_tas(159.0)
    , m_altBaro(8200.0)
    , m_altRadar(430.0)
    , m_vs(1200.0)
    , m_pitch(2.4)
    , m_roll(-3.2)
    , m_yaw(1.1)
    , m_heading(132.0)
    , m_track(138.0)
{
}

/// @brief Returns calibrated airspeed in knots.
double FlightTelemetryData::cas() const { return m_cas; }

/// @brief Updates calibrated airspeed and notifies observers when it changes.
/// @param value Calibrated airspeed in knots.
void FlightTelemetryData::setCas(double value)
{
    if (m_cas == value) {
        return;
    }
    m_cas = value;
    emit casChanged();
}

/// @brief Returns true airspeed in knots.
double FlightTelemetryData::tas() const { return m_tas; }

/// @brief Updates true airspeed and notifies observers when it changes.
/// @param value True airspeed in knots.
void FlightTelemetryData::setTas(double value)
{
    if (m_tas == value) {
        return;
    }
    m_tas = value;
    emit tasChanged();
}

/// @brief Returns barometric altitude in feet.
double FlightTelemetryData::altBaro() const { return m_altBaro; }

/// @brief Updates barometric altitude and notifies observers when it changes.
/// @param value Barometric altitude in feet.
void FlightTelemetryData::setAltBaro(double value)
{
    if (m_altBaro == value) {
        return;
    }
    m_altBaro = value;
    emit altBaroChanged();
}

/// @brief Returns radar altitude in feet.
double FlightTelemetryData::altRadar() const { return m_altRadar; }

/// @brief Updates radar altitude and notifies observers when it changes.
/// @param value Radar altitude in feet.
void FlightTelemetryData::setAltRadar(double value)
{
    if (m_altRadar == value) {
        return;
    }
    m_altRadar = value;
    emit altRadarChanged();
}

/// @brief Returns vertical speed in feet per minute.
double FlightTelemetryData::vs() const { return m_vs; }

/// @brief Updates vertical speed and notifies observers when it changes.
/// @param value Vertical speed in feet per minute.
void FlightTelemetryData::setVs(double value)
{
    if (m_vs == value) {
        return;
    }
    m_vs = value;
    emit vsChanged();
}

/// @brief Returns pitch attitude in degrees.
double FlightTelemetryData::pitch() const { return m_pitch; }

/// @brief Updates pitch attitude and notifies observers when it changes.
/// @param value Pitch angle in degrees.
void FlightTelemetryData::setPitch(double value)
{
    if (m_pitch == value) {
        return;
    }
    m_pitch = value;
    emit pitchChanged();
}

/// @brief Returns roll attitude in degrees.
double FlightTelemetryData::roll() const { return m_roll; }

/// @brief Updates roll attitude and notifies observers when it changes.
/// @param value Roll angle in degrees.
void FlightTelemetryData::setRoll(double value)
{
    if (m_roll == value) {
        return;
    }
    m_roll = value;
    emit rollChanged();
}

/// @brief Returns yaw attitude in degrees.
double FlightTelemetryData::yaw() const { return m_yaw; }

/// @brief Updates yaw attitude and notifies observers when it changes.
/// @param value Yaw angle in degrees.
void FlightTelemetryData::setYaw(double value)
{
    if (m_yaw == value) {
        return;
    }
    m_yaw = value;
    emit yawChanged();
}

/// @brief Returns aircraft heading in degrees.
double FlightTelemetryData::heading() const { return m_heading; }

/// @brief Updates aircraft heading and notifies observers when it changes.
/// @param value Heading in degrees.
void FlightTelemetryData::setHeading(double value)
{
    if (m_heading == value) {
        return;
    }
    m_heading = value;
    emit headingChanged();
}

/// @brief Returns aircraft ground track in degrees.
double FlightTelemetryData::track() const { return m_track; }

/// @brief Updates aircraft ground track and notifies observers when it changes.
/// @param value Ground track in degrees.
void FlightTelemetryData::setTrack(double value)
{
    if (m_track == value) {
        return;
    }
    m_track = value;
    emit trackChanged();
}