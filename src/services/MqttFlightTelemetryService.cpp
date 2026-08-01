#include "MqttFlightTelemetryService.h"

#include <QtMath>
#include <cmath>

namespace {
double unitClamp(double value)
{
    return qMax(0.0, qMin(1.0, value));
}
}

MqttFlightTelemetryService::MqttFlightTelemetryService(QObject *parent)
    : QObject(parent)
    , m_client(this)
    , m_telemetry(this)
    , m_flightModeLabel(QStringLiteral("MQTT Standby"))
    , m_batterySoc(100.0)
    , m_motorTemperatures({0.0, 0.0, 0.0, 0.0})
    , m_gpsLatitude(0.0)
    , m_gpsLongitude(0.0)
{
    connect(&m_client, &MqttTelemetryClient::telemetryDecoded,
            this, &MqttFlightTelemetryService::onTelemetryDecoded,
            Qt::QueuedConnection);
    connect(&m_client, &MqttTelemetryClient::connectedChanged,
            this, &MqttFlightTelemetryService::connectedChanged);
    connect(&m_client, &MqttTelemetryClient::connectionConfigChanged,
            this, &MqttFlightTelemetryService::connectionConfigChanged);
        connect(&m_client, &MqttTelemetryClient::processingConfigChanged,
            this, &MqttFlightTelemetryService::processingConfigChanged);
        connect(&m_client, &MqttTelemetryClient::backlogStatsChanged,
            this, &MqttFlightTelemetryService::backlogStatsChanged);
    connect(&m_client, &MqttTelemetryClient::errorOccurred,
            this, &MqttFlightTelemetryService::errorOccurred);

    appendHistory(m_casHistory, m_telemetry.cas());
    appendHistory(m_altHistory, m_telemetry.altBaro());
    appendHistory(m_vsHistory, m_telemetry.vs());
    appendHistory(m_attitudeHistory, qAbs(m_telemetry.pitch()) + qAbs(m_telemetry.roll()));
    appendHistory(m_headingHistory, m_telemetry.heading());
    appendHistory(m_fpvHistory, wrappedAngleDeltaDegrees(m_telemetry.track(), m_telemetry.heading()));
}

FlightTelemetryData *MqttFlightTelemetryService::telemetry() const
{
    return const_cast<FlightTelemetryData *>(&m_telemetry);
}

QString MqttFlightTelemetryService::brokerHost() const
{
    return m_client.host();
}

void MqttFlightTelemetryService::setBrokerHost(const QString &host)
{
    m_client.setHost(host);
}

quint16 MqttFlightTelemetryService::brokerPort() const
{
    return m_client.port();
}

void MqttFlightTelemetryService::setBrokerPort(quint16 port)
{
    m_client.setPort(port);
}

QString MqttFlightTelemetryService::topicFilter() const
{
    return m_client.topicFilter();
}

void MqttFlightTelemetryService::setTopicFilter(const QString &topicFilter)
{
    m_client.setTopicFilter(topicFilter);
}

bool MqttFlightTelemetryService::asyncPayloadLoggingEnabled() const
{
    return m_client.asyncPayloadLoggingEnabled();
}

void MqttFlightTelemetryService::setAsyncPayloadLoggingEnabled(bool enabled)
{
    m_client.setAsyncPayloadLoggingEnabled(enabled);
}

int MqttFlightTelemetryService::maxConcurrentParsers() const
{
    return m_client.maxConcurrentParsers();
}

void MqttFlightTelemetryService::setMaxConcurrentParsers(int maxConcurrentParsers)
{
    m_client.setMaxConcurrentParsers(maxConcurrentParsers);
}

int MqttFlightTelemetryService::maxPendingMessages() const
{
    return m_client.maxPendingMessages();
}

void MqttFlightTelemetryService::setMaxPendingMessages(int maxPendingMessages)
{
    m_client.setMaxPendingMessages(maxPendingMessages);
}

quint64 MqttFlightTelemetryService::droppedMessageCount() const
{
    return m_client.droppedMessageCount();
}

bool MqttFlightTelemetryService::connected() const
{
    return m_client.connected();
}

void MqttFlightTelemetryService::connectToBroker()
{
    m_client.connectToBroker();
}

void MqttFlightTelemetryService::disconnectFromBroker()
{
    m_client.disconnectFromBroker();
}

double MqttFlightTelemetryService::cas() const
{
    QReadLocker lock(&m_stateLock);
    return m_telemetry.cas();
}

double MqttFlightTelemetryService::tas() const
{
    QReadLocker lock(&m_stateLock);
    return m_telemetry.tas();
}

double MqttFlightTelemetryService::altBaro() const
{
    QReadLocker lock(&m_stateLock);
    return m_telemetry.altBaro();
}

double MqttFlightTelemetryService::altRadar() const
{
    QReadLocker lock(&m_stateLock);
    return m_telemetry.altRadar();
}

double MqttFlightTelemetryService::vs() const
{
    QReadLocker lock(&m_stateLock);
    return m_telemetry.vs();
}

double MqttFlightTelemetryService::pitch() const
{
    QReadLocker lock(&m_stateLock);
    return m_telemetry.pitch();
}

double MqttFlightTelemetryService::roll() const
{
    QReadLocker lock(&m_stateLock);
    return m_telemetry.roll();
}

double MqttFlightTelemetryService::yaw() const
{
    QReadLocker lock(&m_stateLock);
    return m_telemetry.yaw();
}

double MqttFlightTelemetryService::heading() const
{
    QReadLocker lock(&m_stateLock);
    return m_telemetry.heading();
}

double MqttFlightTelemetryService::track() const
{
    QReadLocker lock(&m_stateLock);
    return m_telemetry.track();
}

double MqttFlightTelemetryService::casProgress() const
{
    const double normalizedSpeed = clamp((cas() - 20.0) / 130.0, 0.0, 1.0);
    return mapToBand(normalizedSpeed, 0.38, 0.78);
}

double MqttFlightTelemetryService::altProgress() const
{
    const double blendedAltitude = altBaro() * 0.82 + altRadar() * 0.18;
    const double normalizedAltitude = clamp(blendedAltitude / 2200.0, 0.0, 1.0);
    return mapToBand(normalizedAltitude, 0.55, 0.92);
}

double MqttFlightTelemetryService::vsProgress() const
{
    const double normalizedVerticalSpeed = clamp(qAbs(vs()) / 1600.0, 0.0, 1.0);
    return mapToBand(normalizedVerticalSpeed, 0.18, 0.88);
}

double MqttFlightTelemetryService::attitudeProgress() const
{
    const double attitudeLoad = qAbs(pitch()) * 0.7 + qAbs(roll()) * 0.45;
    const double normalizedStability = clamp(1.0 - attitudeLoad / 18.0, 0.0, 1.0);
    return mapToBand(normalizedStability, 0.32, 0.82);
}

double MqttFlightTelemetryService::headingProgress() const
{
    const double headingWave = 0.5 + 0.5 * qSin(qDegreesToRadians(heading() * 1.8));
    return mapToBand(headingWave, 0.30, 0.70);
}

double MqttFlightTelemetryService::fpvProgress() const
{
    const double pathError = wrappedAngleDeltaDegrees(track(), heading());
    const double verticalPenalty = qAbs(vs()) / 2200.0;
    const double normalizedPathQuality = clamp(1.0 - pathError / 18.0 - verticalPenalty * 0.25, 0.0, 1.0);
    return mapToBand(normalizedPathQuality, 0.24, 0.66);
}

QString MqttFlightTelemetryService::flightModeLabel() const
{
    QReadLocker lock(&m_stateLock);
    return m_flightModeLabel;
}

QColor MqttFlightTelemetryService::vsAccentColor() const
{
    return vs() >= 0.0 ? QColor("#79d57a") : QColor("#e07c7c");
}

QString MqttFlightTelemetryService::vsTrendLabel() const
{
    return vs() >= 0.0 ? QStringLiteral("Ascending") : QStringLiteral("Descending");
}

QString MqttFlightTelemetryService::vsValueText() const
{
    const QString sign = vs() >= 0.0 ? QStringLiteral("+") : QStringLiteral("-");
    return sign + QString::number(qAbs(vs()), 'f', 0) + QStringLiteral(" ft/min");
}

double MqttFlightTelemetryService::batterySoc() const
{
    QReadLocker lock(&m_stateLock);
    return m_batterySoc;
}

QVariantList MqttFlightTelemetryService::motorTemperatures() const
{
    QReadLocker lock(&m_stateLock);
    return historyToVariantList(m_motorTemperatures);
}

double MqttFlightTelemetryService::gpsLatitude() const
{
    QReadLocker lock(&m_stateLock);
    return m_gpsLatitude;
}

double MqttFlightTelemetryService::gpsLongitude() const
{
    QReadLocker lock(&m_stateLock);
    return m_gpsLongitude;
}

QVariantList MqttFlightTelemetryService::casHistory() const
{
    QReadLocker lock(&m_stateLock);
    return historyToVariantList(m_casHistory);
}

QVariantList MqttFlightTelemetryService::altHistory() const
{
    QReadLocker lock(&m_stateLock);
    return historyToVariantList(m_altHistory);
}

QVariantList MqttFlightTelemetryService::vsHistory() const
{
    QReadLocker lock(&m_stateLock);
    return historyToVariantList(m_vsHistory);
}

QVariantList MqttFlightTelemetryService::attitudeHistory() const
{
    QReadLocker lock(&m_stateLock);
    return historyToVariantList(m_attitudeHistory);
}

QVariantList MqttFlightTelemetryService::headingHistory() const
{
    QReadLocker lock(&m_stateLock);
    return historyToVariantList(m_headingHistory);
}

QVariantList MqttFlightTelemetryService::fpvHistory() const
{
    QReadLocker lock(&m_stateLock);
    return historyToVariantList(m_fpvHistory);
}

void MqttFlightTelemetryService::onTelemetryDecoded(const QVariantMap &payload)
{
    QWriteLocker lock(&m_stateLock);

    m_telemetry.setCas(payload.value(QStringLiteral("cas"), m_telemetry.cas()).toDouble());
    m_telemetry.setTas(payload.value(QStringLiteral("tas"), m_telemetry.tas()).toDouble());
    m_telemetry.setAltBaro(payload.value(QStringLiteral("altBaro"), m_telemetry.altBaro()).toDouble());
    m_telemetry.setAltRadar(payload.value(QStringLiteral("altRadar"), m_telemetry.altRadar()).toDouble());
    m_telemetry.setVs(payload.value(QStringLiteral("vs"), m_telemetry.vs()).toDouble());
    m_telemetry.setPitch(payload.value(QStringLiteral("pitch"), m_telemetry.pitch()).toDouble());
    m_telemetry.setRoll(payload.value(QStringLiteral("roll"), m_telemetry.roll()).toDouble());
    m_telemetry.setYaw(payload.value(QStringLiteral("yaw"), m_telemetry.yaw()).toDouble());
    m_telemetry.setHeading(payload.value(QStringLiteral("heading"), m_telemetry.heading()).toDouble());
    m_telemetry.setTrack(payload.value(QStringLiteral("track"), m_telemetry.track()).toDouble());

    if (payload.contains(QStringLiteral("flightMode"))) {
        m_flightModeLabel = payload.value(QStringLiteral("flightMode")).toString();
    }
    if (payload.contains(QStringLiteral("batterySoc"))) {
        m_batterySoc = clamp(payload.value(QStringLiteral("batterySoc")).toDouble(), 0.0, 100.0);
    }
    if (payload.contains(QStringLiteral("gpsLatitude"))) {
        m_gpsLatitude = payload.value(QStringLiteral("gpsLatitude")).toDouble();
    }
    if (payload.contains(QStringLiteral("gpsLongitude"))) {
        m_gpsLongitude = payload.value(QStringLiteral("gpsLongitude")).toDouble();
    }
    if (payload.contains(QStringLiteral("motorTemps"))) {
        const QVariantList motorTemps = payload.value(QStringLiteral("motorTemps")).toList();
        m_motorTemperatures.clear();
        m_motorTemperatures.reserve(motorTemps.size());
        for (const QVariant &value : motorTemps) {
            m_motorTemperatures.append(value.toDouble());
        }
    }

    appendHistory(m_casHistory, m_telemetry.cas());
    appendHistory(m_altHistory, m_telemetry.altBaro());
    appendHistory(m_vsHistory, m_telemetry.vs());
    appendHistory(m_attitudeHistory, qAbs(m_telemetry.pitch()) + qAbs(m_telemetry.roll()));
    appendHistory(m_headingHistory, m_telemetry.heading());
    appendHistory(m_fpvHistory, wrappedAngleDeltaDegrees(m_telemetry.track(), m_telemetry.heading()));

    lock.unlock();
    emit telemetryChanged();
}

double MqttFlightTelemetryService::clamp(double value, double minimumValue, double maximumValue)
{
    return qMax(minimumValue, qMin(maximumValue, value));
}

double MqttFlightTelemetryService::wrappedAngleDeltaDegrees(double angleA, double angleB)
{
    const double wrapped = std::fmod((angleA - angleB) + 540.0, 360.0) - 180.0;
    return qAbs(wrapped);
}

double MqttFlightTelemetryService::mapToBand(double normalizedValue, double bandMinimum, double bandMaximum)
{
    const double clamped = unitClamp(normalizedValue);
    return bandMinimum + (bandMaximum - bandMinimum) * clamped;
}

void MqttFlightTelemetryService::appendHistory(QVector<double> &history, double value)
{
    history.append(value);
    while (history.size() > 24) {
        history.removeFirst();
    }
}

QVariantList MqttFlightTelemetryService::historyToVariantList(const QVector<double> &history) const
{
    QVariantList values;
    values.reserve(history.size());
    for (double value : history) {
        values.append(value);
    }
    return values;
}
