/**
 * @file    : src/services/MqttFlightTelemetryService.cpp
 * @brief   : Implements the MQTT-backed flight telemetry service.
 * @author  : Abhinay Chauhan (email: abhinay.chauhan1@gmail.com)
 * @version : 1.0.0
 *
 * Copyright (c) 2024
 * Abhinay Chauhan. All rights reserved.
 */

#include "MqttFlightTelemetryService.h"

#include <QStringList>
#include <QtMath>
#include <cmath>

namespace {
/// @brief Constrains a normalized value to the range 0.0 to 1.0.
double unitClamp(double value)
{
    return qMax(0.0, qMin(1.0, value));
}
}

/// @brief Constructs the service and connects MQTT client notifications.
/// @param parent Optional QObject that owns the service.
MqttFlightTelemetryService::MqttFlightTelemetryService(QObject *parent)
    : QObject(parent)
    , m_client(this)
    , m_telemetry(this)
    , m_flightModeLabel(QStringLiteral("MQTT Standby"))
    , m_batterySoc(100.0)
    , m_batterySoh(100.0)
    , m_powerConsumptionKw(0.0)
    , m_batteryCellTemperatures({0.0, 0.0, 0.0, 0.0, 0.0, 0.0})
    , m_busVoltage(0.0)
    , m_busCurrent(0.0)
    , m_motorTemperatures({0.0, 0.0, 0.0, 0.0})
    , m_motorRpmValues({0.0, 0.0, 0.0, 0.0})
    , m_tiltAngleDeg(0.0)
    , m_thrustOutputs({0.0, 0.0})
    , m_inverterVoltages({0.0, 0.0, 0.0, 0.0})
    , m_inverterCurrents({0.0, 0.0, 0.0, 0.0})
    , m_inverterHealth({0.0, 0.0, 0.0, 0.0})
    , m_gpsLatitude(0.0)
    , m_gpsLongitude(0.0)
{
    // Forward client state and deliver decoded payloads on the service thread.
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

        // Seed each bounded history so QML receives a valid initial series.
    appendHistory(m_casHistory, m_telemetry.cas());
    appendHistory(m_altHistory, m_telemetry.altBaro());
    appendHistory(m_vsHistory, m_telemetry.vs());
    appendHistory(m_attitudeHistory, qAbs(m_telemetry.pitch()) + qAbs(m_telemetry.roll()));
    appendHistory(m_headingHistory, m_telemetry.heading());
    appendHistory(m_fpvHistory, wrappedAngleDeltaDegrees(m_telemetry.track(), m_telemetry.heading()));
    appendHistory(m_propulsionRpmHistory, 0.0);
    appendHistory(m_propulsionTiltHistory, 0.0);
    appendHistory(m_propulsionTempHistory, 0.0);
    appendHistory(m_propulsionThrustHistory, 0.0);
    appendHistory(m_propulsionInverterHealthHistory, 0.0);
}

/// @brief Returns the service-owned telemetry object exposed to QML.
/// @return Mutable pointer to the current telemetry data.
FlightTelemetryData *MqttFlightTelemetryService::telemetry() const
{
    return const_cast<FlightTelemetryData *>(&m_telemetry);
}

/// @brief Returns the configured MQTT broker host.
QString MqttFlightTelemetryService::brokerHost() const
{
    return m_client.host();
}

/// @brief Forwards a broker host update to the MQTT client.
/// @param host Broker hostname or IP address.
void MqttFlightTelemetryService::setBrokerHost(const QString &host)
{
    m_client.setHost(host);
}

/// @brief Returns the configured MQTT broker port.
quint16 MqttFlightTelemetryService::brokerPort() const
{
    return m_client.port();
}

/// @brief Forwards a broker port update to the MQTT client.
/// @param port MQTT broker port.
void MqttFlightTelemetryService::setBrokerPort(quint16 port)
{
    m_client.setPort(port);
}

/// @brief Returns the configured telemetry topic filter.
QString MqttFlightTelemetryService::topicFilter() const
{
    return m_client.topicFilter();
}

/// @brief Forwards a topic-filter update to the MQTT client.
/// @param topicFilter MQTT subscription filter.
void MqttFlightTelemetryService::setTopicFilter(const QString &topicFilter)
{
    m_client.setTopicFilter(topicFilter);
}

/// @brief Returns whether asynchronous raw payload logging is enabled.
bool MqttFlightTelemetryService::asyncPayloadLoggingEnabled() const
{
    return m_client.asyncPayloadLoggingEnabled();
}

/// @brief Enables or disables asynchronous raw payload logging.
/// @param enabled true to log received MQTT payloads.
void MqttFlightTelemetryService::setAsyncPayloadLoggingEnabled(bool enabled)
{
    m_client.setAsyncPayloadLoggingEnabled(enabled);
}

/// @brief Returns the maximum number of simultaneous parser tasks.
int MqttFlightTelemetryService::maxConcurrentParsers() const
{
    return m_client.maxConcurrentParsers();
}

/// @brief Forwards the parser concurrency limit to the MQTT client.
/// @param maxConcurrentParsers Requested parser count.
void MqttFlightTelemetryService::setMaxConcurrentParsers(int maxConcurrentParsers)
{
    m_client.setMaxConcurrentParsers(maxConcurrentParsers);
}

/// @brief Returns the maximum number of queued payloads.
int MqttFlightTelemetryService::maxPendingMessages() const
{
    return m_client.maxPendingMessages();
}

/// @brief Forwards the pending-message capacity to the MQTT client.
/// @param maxPendingMessages Requested backlog capacity.
void MqttFlightTelemetryService::setMaxPendingMessages(int maxPendingMessages)
{
    m_client.setMaxPendingMessages(maxPendingMessages);
}

/// @brief Decreases parser concurrency by one down to the supported minimum.
void MqttFlightTelemetryService::decreaseParserConcurrency()
{
    setMaxConcurrentParsers(qMax(1, maxConcurrentParsers() - 1));
}

/// @brief Increases parser concurrency by one up to the UI-supported maximum.
void MqttFlightTelemetryService::increaseParserConcurrency()
{
    setMaxConcurrentParsers(qMin(16, maxConcurrentParsers() + 1));
}

/// @brief Increases pending capacity by 32 up to the UI-supported maximum.
void MqttFlightTelemetryService::increasePendingCapacity()
{
    setMaxPendingMessages(qMin(4096, maxPendingMessages() + 32));
}

/// @brief Returns the cumulative number of payloads dropped from the backlog.
quint64 MqttFlightTelemetryService::droppedMessageCount() const
{
    return m_client.droppedMessageCount();
}

/// @brief Returns whether the MQTT broker connection is established.
bool MqttFlightTelemetryService::connected() const
{
    return m_client.connected();
}

/// @brief Starts a connection to the configured MQTT broker.
void MqttFlightTelemetryService::connectToBroker()
{
    m_client.connectToBroker();
}

/// @brief Disconnects from the MQTT broker.
void MqttFlightTelemetryService::disconnectFromBroker()
{
    m_client.disconnectFromBroker();
}

/// @brief Returns calibrated airspeed under the state read lock.
double MqttFlightTelemetryService::cas() const
{
    QReadLocker lock(&m_stateLock);
    return m_telemetry.cas();
}

/// @brief Returns true airspeed under the state read lock.
double MqttFlightTelemetryService::tas() const
{
    QReadLocker lock(&m_stateLock);
    return m_telemetry.tas();
}

/// @brief Returns barometric altitude under the state read lock.
double MqttFlightTelemetryService::altBaro() const
{
    QReadLocker lock(&m_stateLock);
    return m_telemetry.altBaro();
}

/// @brief Returns radar altitude under the state read lock.
double MqttFlightTelemetryService::altRadar() const
{
    QReadLocker lock(&m_stateLock);
    return m_telemetry.altRadar();
}

/// @brief Returns vertical speed under the state read lock.
double MqttFlightTelemetryService::vs() const
{
    QReadLocker lock(&m_stateLock);
    return m_telemetry.vs();
}

/// @brief Returns pitch under the state read lock.
double MqttFlightTelemetryService::pitch() const
{
    QReadLocker lock(&m_stateLock);
    return m_telemetry.pitch();
}

/// @brief Returns roll under the state read lock.
double MqttFlightTelemetryService::roll() const
{
    QReadLocker lock(&m_stateLock);
    return m_telemetry.roll();
}

/// @brief Returns yaw under the state read lock.
double MqttFlightTelemetryService::yaw() const
{
    QReadLocker lock(&m_stateLock);
    return m_telemetry.yaw();
}

/// @brief Returns heading under the state read lock.
double MqttFlightTelemetryService::heading() const
{
    QReadLocker lock(&m_stateLock);
    return m_telemetry.heading();
}

/// @brief Returns ground track under the state read lock.
double MqttFlightTelemetryService::track() const
{
    QReadLocker lock(&m_stateLock);
    return m_telemetry.track();
}

/// @brief Returns calibrated-airspeed progress mapped to the display band.
double MqttFlightTelemetryService::casProgress() const
{
    const double normalizedSpeed = clamp((cas() - 20.0) / 130.0, 0.0, 1.0);
    return mapToBand(normalizedSpeed, 0.38, 0.78);
}

/// @brief Returns blended-altitude progress mapped to the display band.
double MqttFlightTelemetryService::altProgress() const
{
    const double blendedAltitude = altBaro() * 0.82 + altRadar() * 0.18;
    const double normalizedAltitude = clamp(blendedAltitude / 2200.0, 0.0, 1.0);
    return mapToBand(normalizedAltitude, 0.55, 0.92);
}

/// @brief Returns vertical-speed progress mapped to the display band.
double MqttFlightTelemetryService::vsProgress() const
{
    const double normalizedVerticalSpeed = clamp(qAbs(vs()) / 1600.0, 0.0, 1.0);
    return mapToBand(normalizedVerticalSpeed, 0.18, 0.88);
}

/// @brief Returns attitude-stability progress mapped to the display band.
double MqttFlightTelemetryService::attitudeProgress() const
{
    const double attitudeLoad = qAbs(pitch()) * 0.7 + qAbs(roll()) * 0.45;
    const double normalizedStability = clamp(1.0 - attitudeLoad / 18.0, 0.0, 1.0);
    return mapToBand(normalizedStability, 0.32, 0.82);
}

/// @brief Returns heading-derived progress mapped to the display band.
double MqttFlightTelemetryService::headingProgress() const
{
    const double headingWave = 0.5 + 0.5 * qSin(qDegreesToRadians(heading() * 1.8));
    return mapToBand(headingWave, 0.30, 0.70);
}

/// @brief Returns flight-path-vector alignment progress.
double MqttFlightTelemetryService::fpvProgress() const
{
    const double pathError = wrappedAngleDeltaDegrees(track(), heading());
    const double verticalPenalty = qAbs(vs()) / 2200.0;
    return clamp(1.0 - pathError / 18.0 - verticalPenalty * 0.25, 0.0, 1.0);
}

/// @brief Returns the current flight mode label.
QString MqttFlightTelemetryService::flightModeLabel() const
{
    QReadLocker lock(&m_stateLock);
    return m_flightModeLabel;
}

/// @brief Returns the vertical-speed status color.
QColor MqttFlightTelemetryService::vsAccentColor() const
{
    return vs() >= 0.0 ? QColor("#79d57a") : QColor("#e07c7c");
}

/// @brief Returns the vertical-speed trend label.
QString MqttFlightTelemetryService::vsTrendLabel() const
{
    return vs() >= 0.0 ? QStringLiteral("Ascending") : QStringLiteral("Descending");
}

/// @brief Returns signed vertical-speed text with units.
QString MqttFlightTelemetryService::vsValueText() const
{
    const QString sign = vs() >= 0.0 ? QStringLiteral("+") : QStringLiteral("-");
    return sign + QString::number(qAbs(vs()), 'f', 0) + QStringLiteral(" ft/min");
}

/// @brief Returns battery state of charge under the state read lock.
double MqttFlightTelemetryService::batterySoc() const
{
    QReadLocker lock(&m_stateLock);
    return m_batterySoc;
}

/// @brief Returns battery state of health under the state read lock.
double MqttFlightTelemetryService::batterySoh() const
{
    QReadLocker lock(&m_stateLock);
    return m_batterySoh;
}

/// @brief Returns power consumption under the state read lock.
double MqttFlightTelemetryService::powerConsumptionKw() const
{
    QReadLocker lock(&m_stateLock);
    return m_powerConsumptionKw;
}

/// @brief Returns battery-cell temperatures under the state read lock.
QVariantList MqttFlightTelemetryService::batteryCellTemperatures() const
{
    QReadLocker lock(&m_stateLock);
    return historyToVariantList(m_batteryCellTemperatures);
}

/// @brief Returns bus voltage under the state read lock.
double MqttFlightTelemetryService::busVoltage() const
{
    QReadLocker lock(&m_stateLock);
    return m_busVoltage;
}

/// @brief Returns bus current under the state read lock.
double MqttFlightTelemetryService::busCurrent() const
{
    QReadLocker lock(&m_stateLock);
    return m_busCurrent;
}

/// @brief Returns motor temperatures under the state read lock.
QVariantList MqttFlightTelemetryService::motorTemperatures() const
{
    QReadLocker lock(&m_stateLock);
    return historyToVariantList(m_motorTemperatures);
}

/// @brief Returns motor speeds under the state read lock.
QVariantList MqttFlightTelemetryService::motorRpmValues() const
{
    QReadLocker lock(&m_stateLock);
    return historyToVariantList(m_motorRpmValues);
}

/// @brief Returns propulsion tilt angle under the state read lock.
double MqttFlightTelemetryService::tiltAngleDeg() const
{
    QReadLocker lock(&m_stateLock);
    return m_tiltAngleDeg;
}

/// @brief Returns thrust outputs under the state read lock.
QVariantList MqttFlightTelemetryService::thrustOutputs() const
{
    QReadLocker lock(&m_stateLock);
    return historyToVariantList(m_thrustOutputs);
}

/// @brief Returns inverter voltages under the state read lock.
QVariantList MqttFlightTelemetryService::inverterVoltages() const
{
    QReadLocker lock(&m_stateLock);
    return historyToVariantList(m_inverterVoltages);
}

/// @brief Returns inverter currents under the state read lock.
QVariantList MqttFlightTelemetryService::inverterCurrents() const
{
    QReadLocker lock(&m_stateLock);
    return historyToVariantList(m_inverterCurrents);
}

/// @brief Returns inverter health values under the state read lock.
QVariantList MqttFlightTelemetryService::inverterHealth() const
{
    QReadLocker lock(&m_stateLock);
    return historyToVariantList(m_inverterHealth);
}

/// @brief Returns GPS latitude under the state read lock.
double MqttFlightTelemetryService::gpsLatitude() const
{
    QReadLocker lock(&m_stateLock);
    return m_gpsLatitude;
}

/// @brief Returns GPS longitude under the state read lock.
double MqttFlightTelemetryService::gpsLongitude() const
{
    QReadLocker lock(&m_stateLock);
    return m_gpsLongitude;
}

/// @brief Returns recent calibrated-airspeed samples.
QVariantList MqttFlightTelemetryService::casHistory() const
{
    QReadLocker lock(&m_stateLock);
    return historyToVariantList(m_casHistory);
}

/// @brief Returns recent barometric-altitude samples.
QVariantList MqttFlightTelemetryService::altHistory() const
{
    QReadLocker lock(&m_stateLock);
    return historyToVariantList(m_altHistory);
}

/// @brief Returns recent vertical-speed samples.
QVariantList MqttFlightTelemetryService::vsHistory() const
{
    QReadLocker lock(&m_stateLock);
    return historyToVariantList(m_vsHistory);
}

/// @brief Returns recent combined attitude-load samples.
QVariantList MqttFlightTelemetryService::attitudeHistory() const
{
    QReadLocker lock(&m_stateLock);
    return historyToVariantList(m_attitudeHistory);
}

/// @brief Returns recent heading samples.
QVariantList MqttFlightTelemetryService::headingHistory() const
{
    QReadLocker lock(&m_stateLock);
    return historyToVariantList(m_headingHistory);
}

/// @brief Returns recent flight-path-vector error samples.
QVariantList MqttFlightTelemetryService::fpvHistory() const
{
    QReadLocker lock(&m_stateLock);
    return historyToVariantList(m_fpvHistory);
}

/// @brief Returns recent average motor-speed samples.
QVariantList MqttFlightTelemetryService::propulsionRpmHistory() const
{
    QReadLocker lock(&m_stateLock);
    return historyToVariantList(m_propulsionRpmHistory);
}

/// @brief Returns recent propulsion tilt samples.
QVariantList MqttFlightTelemetryService::propulsionTiltHistory() const
{
    QReadLocker lock(&m_stateLock);
    return historyToVariantList(m_propulsionTiltHistory);
}

/// @brief Returns recent average motor-temperature samples.
QVariantList MqttFlightTelemetryService::propulsionTempHistory() const
{
    QReadLocker lock(&m_stateLock);
    return historyToVariantList(m_propulsionTempHistory);
}

/// @brief Returns recent total-thrust samples.
QVariantList MqttFlightTelemetryService::propulsionThrustHistory() const
{
    QReadLocker lock(&m_stateLock);
    return historyToVariantList(m_propulsionThrustHistory);
}

/// @brief Returns recent average inverter-health samples.
QVariantList MqttFlightTelemetryService::propulsionInverterHealthHistory() const
{
    QReadLocker lock(&m_stateLock);
    return historyToVariantList(m_propulsionInverterHealthHistory);
}

/// @brief Applies a normalized payload to service state and bounded histories.
/// @param payload Decoded telemetry fields.
void MqttFlightTelemetryService::onTelemetryDecoded(const QVariantMap &payload)
{
    QWriteLocker lock(&m_stateLock);
    QStringList validationErrors;

    // Preserve fixed component counts and collect validation failures for later reporting.
    const auto replaceFixedValues = [&validationErrors](QVector<double> &target,
                                                         const QVariantList &values,
                                                         int expectedSize,
                                                         const QString &fieldName) {
        if (values.size() != expectedSize) {
            validationErrors.append(QStringLiteral("%1 requires %2 values; received %3")
                                    .arg(fieldName).arg(expectedSize).arg(values.size()));
            return;
        }
        target.clear();
        target.reserve(values.size());
        for (const QVariant &value : values) {
            target.append(value.toDouble());
        }
    };

    // Apply scalar flight values, retaining previous values for omitted fields.
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
    if (payload.contains(QStringLiteral("batterySoh"))) {
        m_batterySoh = clamp(payload.value(QStringLiteral("batterySoh")).toDouble(), 0.0, 100.0);
    }
    if (payload.contains(QStringLiteral("powerConsumptionKw"))) {
        m_powerConsumptionKw = qMax(0.0, payload.value(QStringLiteral("powerConsumptionKw")).toDouble());
    }
    if (payload.contains(QStringLiteral("busVoltage"))) {
        m_busVoltage = qMax(0.0, payload.value(QStringLiteral("busVoltage")).toDouble());
    }
    if (payload.contains(QStringLiteral("busCurrent"))) {
        m_busCurrent = qMax(0.0, payload.value(QStringLiteral("busCurrent")).toDouble());
    }
    if (payload.contains(QStringLiteral("batteryCellTemperatures"))) {
        const QVariantList temperatures = payload.value(QStringLiteral("batteryCellTemperatures")).toList();
        replaceFixedValues(m_batteryCellTemperatures, temperatures, 6,
                           QStringLiteral("batteryCellTemperatures"));
    }
    if (payload.contains(QStringLiteral("gpsLatitude"))) {
        m_gpsLatitude = payload.value(QStringLiteral("gpsLatitude")).toDouble();
    }
    if (payload.contains(QStringLiteral("gpsLongitude"))) {
        m_gpsLongitude = payload.value(QStringLiteral("gpsLongitude")).toDouble();
    }
    if (payload.contains(QStringLiteral("motorTemps"))) {
        const QVariantList motorTemps = payload.value(QStringLiteral("motorTemps")).toList();
        replaceFixedValues(m_motorTemperatures, motorTemps, 4, QStringLiteral("motorTemps"));
    }
    if (payload.contains(QStringLiteral("motorRpms"))) {
        const QVariantList motorRpms = payload.value(QStringLiteral("motorRpms")).toList();
        replaceFixedValues(m_motorRpmValues, motorRpms, 4, QStringLiteral("motorRpms"));
    }
    if (payload.contains(QStringLiteral("tiltAngle"))) {
        m_tiltAngleDeg = clamp(payload.value(QStringLiteral("tiltAngle")).toDouble(), 0.0, 90.0);
    }
    if (payload.contains(QStringLiteral("thrustOutputs"))) {
        const QVariantList thrustOutputs = payload.value(QStringLiteral("thrustOutputs")).toList();
        replaceFixedValues(m_thrustOutputs, thrustOutputs, 2, QStringLiteral("thrustOutputs"));
    }
    if (payload.contains(QStringLiteral("inverterVoltages"))) {
        const QVariantList inverterVoltages = payload.value(QStringLiteral("inverterVoltages")).toList();
        replaceFixedValues(m_inverterVoltages, inverterVoltages, 4,
                           QStringLiteral("inverterVoltages"));
    }
    if (payload.contains(QStringLiteral("inverterCurrents"))) {
        const QVariantList inverterCurrents = payload.value(QStringLiteral("inverterCurrents")).toList();
        replaceFixedValues(m_inverterCurrents, inverterCurrents, 4,
                           QStringLiteral("inverterCurrents"));
    }
    if (payload.contains(QStringLiteral("inverterHealth"))) {
        const QVariantList inverterHealth = payload.value(QStringLiteral("inverterHealth")).toList();
        if (inverterHealth.size() == 4) {
            m_inverterHealth.clear();
            m_inverterHealth.reserve(inverterHealth.size());
            for (const QVariant &value : inverterHealth) {
                m_inverterHealth.append(clamp(value.toDouble(), 0.0, 100.0));
            }
        } else {
            validationErrors.append(QStringLiteral("inverterHealth requires 4 values; received %1")
                                    .arg(inverterHealth.size()));
        }
    }

    // Update raw and aggregate trend series from the accepted state snapshot.
    appendHistory(m_casHistory, m_telemetry.cas());
    appendHistory(m_altHistory, m_telemetry.altBaro());
    appendHistory(m_vsHistory, m_telemetry.vs());
    appendHistory(m_attitudeHistory, qAbs(m_telemetry.pitch()) + qAbs(m_telemetry.roll()));
    appendHistory(m_headingHistory, m_telemetry.heading());
    appendHistory(m_fpvHistory, wrappedAngleDeltaDegrees(m_telemetry.track(), m_telemetry.heading()));

    if (!m_motorRpmValues.isEmpty()) {
        double rpmTotal = 0.0;
        for (double rpm : m_motorRpmValues) {
            rpmTotal += rpm;
        }
        appendHistory(m_propulsionRpmHistory, rpmTotal / m_motorRpmValues.size());
    }
    appendHistory(m_propulsionTiltHistory, m_tiltAngleDeg);
    if (!m_motorTemperatures.isEmpty()) {
        double tempTotal = 0.0;
        for (double temperature : m_motorTemperatures) {
            tempTotal += temperature;
        }
        appendHistory(m_propulsionTempHistory, tempTotal / m_motorTemperatures.size());
    }
    if (!m_thrustOutputs.isEmpty()) {
        double thrustTotal = 0.0;
        for (double thrust : m_thrustOutputs) {
            thrustTotal += thrust;
        }
        appendHistory(m_propulsionThrustHistory, thrustTotal);
    }
    if (!m_inverterHealth.isEmpty()) {
        double healthTotal = 0.0;
        for (double health : m_inverterHealth) {
            healthTotal += health;
        }
        appendHistory(m_propulsionInverterHealthHistory, healthTotal / m_inverterHealth.size());
    }

    // Release state protection before notifying observers or reporting validation errors.
    lock.unlock();
    for (const QString &validationError : validationErrors) {
        emit errorOccurred(validationError);
    }
    emit telemetryChanged();
}

/// @brief Restricts a value to the supplied inclusive range.
double MqttFlightTelemetryService::clamp(double value, double minimumValue, double maximumValue)
{
    return qMax(minimumValue, qMin(maximumValue, value));
}

/// @brief Calculates the shortest absolute separation between two headings.
double MqttFlightTelemetryService::wrappedAngleDeltaDegrees(double angleA, double angleB)
{
    const double wrapped = std::fmod((angleA - angleB) + 540.0, 360.0) - 180.0;
    return qAbs(wrapped);
}

/// @brief Maps a normalized value into a presentation band.
double MqttFlightTelemetryService::mapToBand(double normalizedValue, double bandMinimum, double bandMaximum)
{
    const double clamped = unitClamp(normalizedValue);
    return bandMinimum + (bandMaximum - bandMinimum) * clamped;
}

/// @brief Appends a sample while retaining the latest 24 values.
void MqttFlightTelemetryService::appendHistory(QVector<double> &history, double value)
{
    history.append(value);
    while (history.size() > 24) {
        history.removeFirst();
    }
}

/// @brief Converts an internal numeric history for QML consumption.
QVariantList MqttFlightTelemetryService::historyToVariantList(const QVector<double> &history) const
{
    QVariantList values;
    values.reserve(history.size());
    for (double value : history) {
        values.append(value);
    }
    return values;
}
