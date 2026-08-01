#include "PrimaryFlightViewModel.h"

PrimaryFlightViewModel::PrimaryFlightViewModel(const QString &title,
                                               IFlightTelemetrySource &telemetrySource,
                                               QObject *parent)
    : QObject(parent)
    , m_title(title)
    , m_telemetrySource(telemetrySource)
{
}

QString PrimaryFlightViewModel::title() const
{
    return m_title;
}

double PrimaryFlightViewModel::cas() const { return m_telemetrySource.cas(); }
double PrimaryFlightViewModel::tas() const { return m_telemetrySource.tas(); }
double PrimaryFlightViewModel::altBaro() const { return m_telemetrySource.altBaro(); }
double PrimaryFlightViewModel::altRadar() const { return m_telemetrySource.altRadar(); }
double PrimaryFlightViewModel::vs() const { return m_telemetrySource.vs(); }
double PrimaryFlightViewModel::pitch() const { return m_telemetrySource.pitch(); }
double PrimaryFlightViewModel::roll() const { return m_telemetrySource.roll(); }
double PrimaryFlightViewModel::yaw() const { return m_telemetrySource.yaw(); }
double PrimaryFlightViewModel::heading() const { return m_telemetrySource.heading(); }
double PrimaryFlightViewModel::track() const { return m_telemetrySource.track(); }
double PrimaryFlightViewModel::casProgress() const { return m_telemetrySource.casProgress(); }
double PrimaryFlightViewModel::altProgress() const { return m_telemetrySource.altProgress(); }
double PrimaryFlightViewModel::vsProgress() const { return m_telemetrySource.vsProgress(); }
double PrimaryFlightViewModel::attitudeProgress() const { return m_telemetrySource.attitudeProgress(); }
double PrimaryFlightViewModel::headingProgress() const { return m_telemetrySource.headingProgress(); }
double PrimaryFlightViewModel::fpvProgress() const { return m_telemetrySource.fpvProgress(); }
QString PrimaryFlightViewModel::flightModeLabel() const { return m_telemetrySource.flightModeLabel(); }
QColor PrimaryFlightViewModel::vsAccentColor() const { return m_telemetrySource.vsAccentColor(); }
QString PrimaryFlightViewModel::vsTrendLabel() const { return m_telemetrySource.vsTrendLabel(); }
QString PrimaryFlightViewModel::vsValueText() const { return m_telemetrySource.vsValueText(); }
double PrimaryFlightViewModel::batterySoc() const { return m_telemetrySource.batterySoc(); }
QVariantList PrimaryFlightViewModel::motorTemperatures() const { return m_telemetrySource.motorTemperatures(); }
double PrimaryFlightViewModel::gpsLatitude() const { return m_telemetrySource.gpsLatitude(); }
double PrimaryFlightViewModel::gpsLongitude() const { return m_telemetrySource.gpsLongitude(); }
QVariantList PrimaryFlightViewModel::casHistory() const { return m_telemetrySource.casHistory(); }
QVariantList PrimaryFlightViewModel::altHistory() const { return m_telemetrySource.altHistory(); }
QVariantList PrimaryFlightViewModel::vsHistory() const { return m_telemetrySource.vsHistory(); }
QVariantList PrimaryFlightViewModel::attitudeHistory() const { return m_telemetrySource.attitudeHistory(); }
QVariantList PrimaryFlightViewModel::headingHistory() const { return m_telemetrySource.headingHistory(); }
QVariantList PrimaryFlightViewModel::fpvHistory() const { return m_telemetrySource.fpvHistory(); }