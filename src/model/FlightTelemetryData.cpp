#include "FlightTelemetryData.h"

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

double FlightTelemetryData::cas() const { return m_cas; }
void FlightTelemetryData::setCas(double value)
{
    if (m_cas == value) {
        return;
    }
    m_cas = value;
    emit casChanged();
}

double FlightTelemetryData::tas() const { return m_tas; }
void FlightTelemetryData::setTas(double value)
{
    if (m_tas == value) {
        return;
    }
    m_tas = value;
    emit tasChanged();
}

double FlightTelemetryData::altBaro() const { return m_altBaro; }
void FlightTelemetryData::setAltBaro(double value)
{
    if (m_altBaro == value) {
        return;
    }
    m_altBaro = value;
    emit altBaroChanged();
}

double FlightTelemetryData::altRadar() const { return m_altRadar; }
void FlightTelemetryData::setAltRadar(double value)
{
    if (m_altRadar == value) {
        return;
    }
    m_altRadar = value;
    emit altRadarChanged();
}

double FlightTelemetryData::vs() const { return m_vs; }
void FlightTelemetryData::setVs(double value)
{
    if (m_vs == value) {
        return;
    }
    m_vs = value;
    emit vsChanged();
}

double FlightTelemetryData::pitch() const { return m_pitch; }
void FlightTelemetryData::setPitch(double value)
{
    if (m_pitch == value) {
        return;
    }
    m_pitch = value;
    emit pitchChanged();
}

double FlightTelemetryData::roll() const { return m_roll; }
void FlightTelemetryData::setRoll(double value)
{
    if (m_roll == value) {
        return;
    }
    m_roll = value;
    emit rollChanged();
}

double FlightTelemetryData::yaw() const { return m_yaw; }
void FlightTelemetryData::setYaw(double value)
{
    if (m_yaw == value) {
        return;
    }
    m_yaw = value;
    emit yawChanged();
}

double FlightTelemetryData::heading() const { return m_heading; }
void FlightTelemetryData::setHeading(double value)
{
    if (m_heading == value) {
        return;
    }
    m_heading = value;
    emit headingChanged();
}

double FlightTelemetryData::track() const { return m_track; }
void FlightTelemetryData::setTrack(double value)
{
    if (m_track == value) {
        return;
    }
    m_track = value;
    emit trackChanged();
}