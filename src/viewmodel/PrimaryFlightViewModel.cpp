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
double PrimaryFlightViewModel::batterySoh() const
{
    const QVariantList healthValues = m_telemetrySource.inverterHealth();
    if (healthValues.isEmpty()) {
        return 0.0;
    }
    double total = 0.0;
    for (const QVariant &value : healthValues) {
        total += value.toDouble();
    }
    return total / healthValues.size();
}
double PrimaryFlightViewModel::powerConsumptionKw() const
{
    const QVariantList voltages = m_telemetrySource.inverterVoltages();
    const QVariantList currents = m_telemetrySource.inverterCurrents();
    const int count = qMin(voltages.size(), currents.size());
    double totalPowerKw = 0.0;
    for (int i = 0; i < count; ++i) {
        totalPowerKw += voltages[i].toDouble() * currents[i].toDouble() / 1000.0;
    }
    return totalPowerKw;
}
double PrimaryFlightViewModel::cellTemperatureMin() const
{
    const QVariantList temperatures = m_telemetrySource.motorTemperatures();
    if (temperatures.isEmpty()) {
        return 0.0;
    }
    double minimum = temperatures.first().toDouble() - 24.0;
    for (const QVariant &value : temperatures) {
        minimum = qMin(minimum, value.toDouble() - 24.0);
    }
    return minimum;
}
double PrimaryFlightViewModel::cellTemperatureMax() const
{
    const QVariantList temperatures = m_telemetrySource.motorTemperatures();
    if (temperatures.isEmpty()) {
        return 0.0;
    }
    double maximum = temperatures.first().toDouble() - 18.0;
    for (const QVariant &value : temperatures) {
        maximum = qMax(maximum, value.toDouble() - 18.0);
    }
    return maximum;
}
bool PrimaryFlightViewModel::thermalRunawayWarning() const { return cellTemperatureMax() >= 60.0; }
double PrimaryFlightViewModel::busVoltage() const
{
    const QVariantList voltages = m_telemetrySource.inverterVoltages();
    if (voltages.isEmpty()) {
        return 0.0;
    }
    double total = 0.0;
    for (const QVariant &value : voltages) {
        total += value.toDouble();
    }
    return total / voltages.size();
}
double PrimaryFlightViewModel::busCurrent() const
{
    const QVariantList currents = m_telemetrySource.inverterCurrents();
    double total = 0.0;
    for (const QVariant &value : currents) {
        total += value.toDouble();
    }
    return total;
}
QVariantList PrimaryFlightViewModel::motorTemperatures() const { return m_telemetrySource.motorTemperatures(); }
QVariantList PrimaryFlightViewModel::motorRpmValues() const { return m_telemetrySource.motorRpmValues(); }
double PrimaryFlightViewModel::tiltAngleDeg() const { return m_telemetrySource.tiltAngleDeg(); }
QVariantList PrimaryFlightViewModel::thrustOutputs() const { return m_telemetrySource.thrustOutputs(); }
QVariantList PrimaryFlightViewModel::inverterVoltages() const { return m_telemetrySource.inverterVoltages(); }
QVariantList PrimaryFlightViewModel::inverterCurrents() const { return m_telemetrySource.inverterCurrents(); }
QVariantList PrimaryFlightViewModel::inverterHealth() const { return m_telemetrySource.inverterHealth(); }
double PrimaryFlightViewModel::gpsLatitude() const { return m_telemetrySource.gpsLatitude(); }
double PrimaryFlightViewModel::gpsLongitude() const { return m_telemetrySource.gpsLongitude(); }
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