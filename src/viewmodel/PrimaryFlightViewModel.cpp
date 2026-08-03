#include "PrimaryFlightViewModel.h"

#include <cmath>

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
double PrimaryFlightViewModel::headingTrackDelta() const
{
    return std::fmod((track() - heading()) + 540.0, 360.0) - 180.0;
}
double PrimaryFlightViewModel::slipOffset() const
{
    return qBound(-1.0, headingTrackDelta() / 12.0, 1.0);
}
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
double PrimaryFlightViewModel::batterySoh() const { return m_telemetrySource.batterySoh(); }
double PrimaryFlightViewModel::powerConsumptionKw() const { return m_telemetrySource.powerConsumptionKw(); }
double PrimaryFlightViewModel::cellTemperatureMin() const
{
    const QVariantList temperatures = m_telemetrySource.batteryCellTemperatures();
    if (temperatures.isEmpty()) {
        return 0.0;
    }
    double minimum = temperatures.first().toDouble();
    for (const QVariant &value : temperatures) {
        minimum = qMin(minimum, value.toDouble());
    }
    return minimum;
}
double PrimaryFlightViewModel::cellTemperatureMax() const
{
    const QVariantList temperatures = m_telemetrySource.batteryCellTemperatures();
    if (temperatures.isEmpty()) {
        return 0.0;
    }
    double maximum = temperatures.first().toDouble();
    for (const QVariant &value : temperatures) {
        maximum = qMax(maximum, value.toDouble());
    }
    return maximum;
}
QVariantList PrimaryFlightViewModel::batteryCellTemperatures() const
{
    return m_telemetrySource.batteryCellTemperatures();
}
bool PrimaryFlightViewModel::thermalRunawayWarning() const { return cellTemperatureMax() >= 60.0; }
double PrimaryFlightViewModel::busVoltage() const { return m_telemetrySource.busVoltage(); }
double PrimaryFlightViewModel::busCurrent() const { return m_telemetrySource.busCurrent(); }
QVariantList PrimaryFlightViewModel::motorTemperatures() const { return m_telemetrySource.motorTemperatures(); }
QVariantList PrimaryFlightViewModel::motorRpmValues() const { return m_telemetrySource.motorRpmValues(); }
double PrimaryFlightViewModel::tiltAngleDeg() const { return m_telemetrySource.tiltAngleDeg(); }
QVariantList PrimaryFlightViewModel::thrustOutputs() const { return m_telemetrySource.thrustOutputs(); }
QVariantList PrimaryFlightViewModel::inverterVoltages() const { return m_telemetrySource.inverterVoltages(); }
QVariantList PrimaryFlightViewModel::inverterCurrents() const { return m_telemetrySource.inverterCurrents(); }
QVariantList PrimaryFlightViewModel::inverterHealth() const { return m_telemetrySource.inverterHealth(); }
double PrimaryFlightViewModel::gpsLatitude() const { return m_telemetrySource.gpsLatitude(); }
double PrimaryFlightViewModel::gpsLongitude() const { return m_telemetrySource.gpsLongitude(); }
QString PrimaryFlightViewModel::gpsText() const
{
    if (!std::isfinite(gpsLatitude()) || !std::isfinite(gpsLongitude())) {
        return QStringLiteral("N/A");
    }
    return QStringLiteral("%1, %2").arg(gpsLatitude(), 0, 'f', 5).arg(gpsLongitude(), 0, 'f', 5);
}
double PrimaryFlightViewModel::averageMotorTemperature() const
{
    const QVariantList temperatures = motorTemperatures();
    if (temperatures.isEmpty()) {
        return 0.0;
    }
    double total = 0.0;
    for (const QVariant &temperature : temperatures) {
        total += temperature.toDouble();
    }
    return total / temperatures.size();
}
QVariantList PrimaryFlightViewModel::casHistory() const { return m_telemetrySource.casHistory(); }
QVariantList PrimaryFlightViewModel::altHistory() const { return m_telemetrySource.altHistory(); }
QVariantList PrimaryFlightViewModel::vsHistory() const { return m_telemetrySource.vsHistory(); }
QVariantList PrimaryFlightViewModel::attitudeHistory() const { return m_telemetrySource.attitudeHistory(); }
QVariantList PrimaryFlightViewModel::headingHistory() const { return m_telemetrySource.headingHistory(); }
QVariantList PrimaryFlightViewModel::fpvHistory() const { return m_telemetrySource.fpvHistory(); }
QVariantList PrimaryFlightViewModel::propulsionRpmHistory() const { return m_telemetrySource.propulsionRpmHistory(); }
QVariantList PrimaryFlightViewModel::propulsionTiltHistory() const { return m_telemetrySource.propulsionTiltHistory(); }
QVariantList PrimaryFlightViewModel::propulsionTempHistory() const { return m_telemetrySource.propulsionTempHistory(); }
QVariantList PrimaryFlightViewModel::propulsionThrustHistory() const { return m_telemetrySource.propulsionThrustHistory(); }
QVariantList PrimaryFlightViewModel::propulsionInverterHealthHistory() const { return m_telemetrySource.propulsionInverterHealthHistory(); }

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

double PrimaryFlightViewModel::speedTapeValue(int index) const
{
    return qRound(cas() / 10.0) * 10.0 + (4 - qBound(0, index, 8)) * 10.0;
}

double PrimaryFlightViewModel::altitudeTapeValue(int index) const
{
    return qRound(altBaro() / 500.0) * 500.0 + (4 - qBound(0, index, 8)) * 500.0;
}

int PrimaryFlightViewModel::verticalSpeedMark(int index) const
{
    return (3 - qBound(0, index, 6)) * 1000;
}

QString PrimaryFlightViewModel::progressConfidence(double progress) const
{
    return progress >= 0.72 ? QStringLiteral("High")
           : progress >= 0.45 ? QStringLiteral("Moderate") : QStringLiteral("Low");
}

QString PrimaryFlightViewModel::historyTrend(const QVariantList &values) const
{
    if (values.size() < 2) {
        return QStringLiteral("Stable");
    }
    const double delta = values.last().toDouble() - values.at(values.size() - 2).toDouble();
    return delta > 0.001 ? QStringLiteral("Rising")
           : delta < -0.001 ? QStringLiteral("Falling") : QStringLiteral("Stable");
}