#include "MqttTelemetryClient.h"

#include <QDateTime>
#include <QFile>
#include <QFutureWatcher>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMutex>
#include <QMutexLocker>
#include <QtConcurrent/QtConcurrentRun>

MqttTelemetryClient::MqttTelemetryClient(QObject *parent)
    : QObject(parent)
    , m_client(this)
    , m_host(QStringLiteral("127.0.0.1"))
    , m_port(1883)
    , m_topicFilter(QStringLiteral("airtaxi/telemetry/#"))
    , m_asyncPayloadLoggingEnabled(false)
    , m_maxConcurrentParsers(4)
    , m_maxPendingMessages(128)
    , m_activeParsers(0)
    , m_droppedMessageCount(0)
    , m_nextSequence(0)
    , m_nextSequenceToPublish(0)
{
    m_client.setHostname(m_host);
    m_client.setPort(m_port);
    m_client.setClientId(QStringLiteral("AirTaxiPulseClient"));

    connect(&m_client, &QMqttClient::stateChanged,
            this, &MqttTelemetryClient::handleStateChanged);
    connect(&m_client, &QMqttClient::messageReceived,
            this, &MqttTelemetryClient::handleMessageReceived);
    connect(&m_client,
            qOverload<QMqttClient::ClientError>(&QMqttClient::errorChanged),
            this,
            [this](QMqttClient::ClientError error) {
                if (error == QMqttClient::NoError) {
                    return;
                }
                emit errorOccurred(m_client.errorString());
            });
}

QString MqttTelemetryClient::host() const
{
    return m_host;
}

void MqttTelemetryClient::setHost(const QString &host)
{
    if (m_host == host) {
        return;
    }
    m_host = host;
    m_client.setHostname(m_host);
    emit connectionConfigChanged();
}

quint16 MqttTelemetryClient::port() const
{
    return m_port;
}

void MqttTelemetryClient::setPort(quint16 port)
{
    if (m_port == port) {
        return;
    }
    m_port = port;
    m_client.setPort(m_port);
    emit connectionConfigChanged();
}

QString MqttTelemetryClient::topicFilter() const
{
    return m_topicFilter;
}

void MqttTelemetryClient::setTopicFilter(const QString &topicFilter)
{
    if (m_topicFilter == topicFilter) {
        return;
    }
    m_topicFilter = topicFilter;
    emit connectionConfigChanged();
}

bool MqttTelemetryClient::connected() const
{
    return m_client.state() == QMqttClient::Connected;
}

bool MqttTelemetryClient::asyncPayloadLoggingEnabled() const
{
    return m_asyncPayloadLoggingEnabled;
}

void MqttTelemetryClient::setAsyncPayloadLoggingEnabled(bool enabled)
{
    if (m_asyncPayloadLoggingEnabled == enabled) {
        return;
    }
    m_asyncPayloadLoggingEnabled = enabled;
    emit connectionConfigChanged();
}

int MqttTelemetryClient::maxConcurrentParsers() const
{
    return m_maxConcurrentParsers;
}

void MqttTelemetryClient::setMaxConcurrentParsers(int maxConcurrentParsers)
{
    const int normalized = qMax(1, maxConcurrentParsers);
    if (m_maxConcurrentParsers == normalized) {
        return;
    }
    m_maxConcurrentParsers = normalized;
    emit processingConfigChanged();
    scheduleNextParsers();
}

int MqttTelemetryClient::maxPendingMessages() const
{
    return m_maxPendingMessages;
}

void MqttTelemetryClient::setMaxPendingMessages(int maxPendingMessages)
{
    const int normalized = qMax(1, maxPendingMessages);
    if (m_maxPendingMessages == normalized) {
        return;
    }
    m_maxPendingMessages = normalized;
    while (m_pendingPayloads.size() > m_maxPendingMessages) {
        const PendingPayload dropped = m_pendingPayloads.dequeue();
        completePayload(dropped.sequence, {});
        ++m_droppedMessageCount;
    }
    emit processingConfigChanged();
    emit backlogStatsChanged();
}

quint64 MqttTelemetryClient::droppedMessageCount() const
{
    return m_droppedMessageCount;
}

void MqttTelemetryClient::setCredentials(const QString &username, const QString &password)
{
    m_client.setUsername(username);
    m_client.setPassword(password);
}

void MqttTelemetryClient::connectToBroker()
{
    if (m_client.state() == QMqttClient::Connected ||
        m_client.state() == QMqttClient::Connecting) {
        return;
    }
    m_client.connectToHost();
}

void MqttTelemetryClient::disconnectFromBroker()
{
    if (m_client.state() == QMqttClient::Disconnected) {
        return;
    }
    m_client.disconnectFromHost();
}

void MqttTelemetryClient::handleStateChanged(QMqttClient::ClientState state)
{
    emit connectedChanged();

    if (state == QMqttClient::Connected) {
        const auto subscription = m_client.subscribe(m_topicFilter, 0);
        if (!subscription) {
            emit errorOccurred(QStringLiteral("Failed to subscribe to topic filter: %1")
                               .arg(m_topicFilter));
        }
    }
}

void MqttTelemetryClient::handleMessageReceived(const QByteArray &message, const QMqttTopicName &topic)
{
    const QByteArray messageCopy = message;
    const QString topicName = topic.name();

    enqueueForProcessing(messageCopy, topicName);

    if (m_asyncPayloadLoggingEnabled) {
        QtConcurrent::run([topicName, messageCopy]() {
            appendPayloadLogLine(topicName, messageCopy);
        });
    }
}

void MqttTelemetryClient::enqueueForProcessing(const QByteArray &message, const QString &topicName)
{
    if (m_pendingPayloads.size() >= m_maxPendingMessages) {
        const PendingPayload dropped = m_pendingPayloads.dequeue();
        completePayload(dropped.sequence, {});
        ++m_droppedMessageCount;
        emit backlogStatsChanged();
    }

    m_pendingPayloads.enqueue(PendingPayload{m_nextSequence++, topicName, message});
    scheduleNextParsers();
}

void MqttTelemetryClient::scheduleNextParsers()
{
    while (m_activeParsers < m_maxConcurrentParsers && !m_pendingPayloads.isEmpty()) {
        const PendingPayload pending = m_pendingPayloads.dequeue();
        ++m_activeParsers;

        auto *watcher = new QFutureWatcher<QVariantMap>(this);
        connect(watcher, &QFutureWatcher<QVariantMap>::finished,
                this,
                [this, watcher, sequence = pending.sequence]() {
                    const QVariantMap decoded = watcher->result();
                    watcher->deleteLater();
                    --m_activeParsers;

                    completePayload(sequence, decoded);

                    scheduleNextParsers();
                });
        watcher->setFuture(QtConcurrent::run([message = pending.message]() {
            return decodePayload(message);
        }));
    }
}

void MqttTelemetryClient::completePayload(quint64 sequence, const QVariantMap &payload)
{
    m_completedPayloads.insert(sequence, payload);
    while (m_completedPayloads.contains(m_nextSequenceToPublish)) {
        const QVariantMap nextPayload = m_completedPayloads.take(m_nextSequenceToPublish++);
        if (!nextPayload.isEmpty()) {
            emit telemetryDecoded(nextPayload);
        }
    }
}

QVariantMap MqttTelemetryClient::decodePayload(const QByteArray &message)
{
    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(message, &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        return {};
    }

    const QJsonObject object = document.object();
    QVariantMap payload;

    const auto insertOptionalNumber = [&object, &payload](const QString &targetKey, const QStringList &keys) {
        for (const QString &key : keys) {
            const QJsonValue value = object.value(key);
            if (value.isDouble()) {
                payload.insert(targetKey, value.toDouble());
                return;
            }
        }
    };
    insertOptionalNumber(QStringLiteral("cas"), {QStringLiteral("cas")});
    insertOptionalNumber(QStringLiteral("tas"), {QStringLiteral("tas")});
    insertOptionalNumber(QStringLiteral("altBaro"), {QStringLiteral("altBaro"), QStringLiteral("alt_baro")});
    insertOptionalNumber(QStringLiteral("altRadar"), {QStringLiteral("altRadar"), QStringLiteral("alt_radar")});
    insertOptionalNumber(QStringLiteral("vs"), {QStringLiteral("vs")});
    insertOptionalNumber(QStringLiteral("pitch"), {QStringLiteral("pitch")});
    insertOptionalNumber(QStringLiteral("roll"), {QStringLiteral("roll")});
    insertOptionalNumber(QStringLiteral("yaw"), {QStringLiteral("yaw")});
    insertOptionalNumber(QStringLiteral("heading"), {QStringLiteral("heading")});
    insertOptionalNumber(QStringLiteral("track"), {QStringLiteral("track")});
    insertOptionalNumber(QStringLiteral("batterySoc"), {QStringLiteral("batterySoc"), QStringLiteral("battery_soc")});
    insertOptionalNumber(QStringLiteral("batterySoh"), {QStringLiteral("batterySoh"), QStringLiteral("battery_soh")});
    insertOptionalNumber(QStringLiteral("powerConsumptionKw"), {QStringLiteral("powerConsumptionKw"), QStringLiteral("power_consumption_kw")});
    insertOptionalNumber(QStringLiteral("busVoltage"), {QStringLiteral("busVoltage"), QStringLiteral("bus_voltage")});
    insertOptionalNumber(QStringLiteral("busCurrent"), {QStringLiteral("busCurrent"), QStringLiteral("bus_current")});

    const QJsonValue flightModeValue = object.value(QStringLiteral("flightMode"));
    if (flightModeValue.isString()) {
        payload.insert(QStringLiteral("flightMode"), flightModeValue.toString());
    } else {
        const QJsonValue alternative = object.value(QStringLiteral("flight_mode"));
        if (alternative.isString()) {
            payload.insert(QStringLiteral("flightMode"), alternative.toString());
        }
    }

    QVariantList motorTemps;
    const QJsonValue motorTempsValue = object.value(QStringLiteral("motorTemps"));
    const QJsonValue altMotorTempsValue = object.value(QStringLiteral("motor_temps"));
    const QJsonArray motorTempArray = motorTempsValue.isArray() ? motorTempsValue.toArray() : altMotorTempsValue.toArray();
    for (const QJsonValue &temp : motorTempArray) {
        if (temp.isDouble()) {
            motorTemps.append(temp.toDouble());
        }
    }
    if (!motorTemps.isEmpty()) {
        payload.insert(QStringLiteral("motorTemps"), motorTemps);
    }

    const QVariantList batteryCellTemperatures = [&object]() {
        QVariantList values;
        const QJsonValue primary = object.value(QStringLiteral("batteryCellTemperatures"));
        const QJsonValue fallback = object.value(QStringLiteral("battery_cell_temperatures"));
        const QJsonArray array = primary.isArray() ? primary.toArray() : fallback.toArray();
        for (const QJsonValue &temperature : array) {
            if (temperature.isDouble()) {
                values.append(temperature.toDouble());
            }
        }
        return values;
    }();
    if (!batteryCellTemperatures.isEmpty()) {
        payload.insert(QStringLiteral("batteryCellTemperatures"), batteryCellTemperatures);
    }

    QVariantList motorRpms;
    const QJsonValue motorRpmsValue = object.value(QStringLiteral("motorRpms"));
    const QJsonValue altMotorRpmsValue = object.value(QStringLiteral("motor_rpms"));
    const QJsonArray motorRpmArray = motorRpmsValue.isArray() ? motorRpmsValue.toArray() : altMotorRpmsValue.toArray();
    for (const QJsonValue &rpm : motorRpmArray) {
        if (rpm.isDouble()) {
            motorRpms.append(rpm.toDouble());
        }
    }
    if (!motorRpms.isEmpty()) {
        payload.insert(QStringLiteral("motorRpms"), motorRpms);
    }

    const QJsonValue tiltAngleValue = object.value(QStringLiteral("tiltAngle"));
    if (tiltAngleValue.isDouble()) {
        payload.insert(QStringLiteral("tiltAngle"), tiltAngleValue.toDouble());
    } else {
        const QJsonValue altTiltAngleValue = object.value(QStringLiteral("tilt_angle"));
        if (altTiltAngleValue.isDouble()) {
            payload.insert(QStringLiteral("tiltAngle"), altTiltAngleValue.toDouble());
        }
    }

    QVariantList thrustOutputs;
    const QJsonValue thrustOutputsValue = object.value(QStringLiteral("thrustOutputs"));
    const QJsonValue altThrustOutputsValue = object.value(QStringLiteral("thrust_outputs"));
    const QJsonArray thrustArray = thrustOutputsValue.isArray() ? thrustOutputsValue.toArray() : altThrustOutputsValue.toArray();
    for (const QJsonValue &thrust : thrustArray) {
        if (thrust.isDouble()) {
            thrustOutputs.append(thrust.toDouble());
        }
    }
    if (!thrustOutputs.isEmpty()) {
        payload.insert(QStringLiteral("thrustOutputs"), thrustOutputs);
    }

    const QJsonValue inverterValue = object.value(QStringLiteral("inverter"));
    if (inverterValue.isObject()) {
        const QJsonObject inverterObject = inverterValue.toObject();

        QVariantList inverterVoltages;
        const QJsonValue voltagesValue = inverterObject.value(QStringLiteral("voltages"));
        if (voltagesValue.isArray()) {
            for (const QJsonValue &voltage : voltagesValue.toArray()) {
                if (voltage.isDouble()) {
                    inverterVoltages.append(voltage.toDouble());
                }
            }
        }
        if (!inverterVoltages.isEmpty()) {
            payload.insert(QStringLiteral("inverterVoltages"), inverterVoltages);
        }

        QVariantList inverterCurrents;
        const QJsonValue currentsValue = inverterObject.value(QStringLiteral("currents"));
        if (currentsValue.isArray()) {
            for (const QJsonValue &current : currentsValue.toArray()) {
                if (current.isDouble()) {
                    inverterCurrents.append(current.toDouble());
                }
            }
        }
        if (!inverterCurrents.isEmpty()) {
            payload.insert(QStringLiteral("inverterCurrents"), inverterCurrents);
        }

        QVariantList inverterHealth;
        const QJsonValue healthValue = inverterObject.value(QStringLiteral("health"));
        if (healthValue.isArray()) {
            for (const QJsonValue &health : healthValue.toArray()) {
                if (health.isDouble()) {
                    inverterHealth.append(health.toDouble());
                }
            }
        }
        if (!inverterHealth.isEmpty()) {
            payload.insert(QStringLiteral("inverterHealth"), inverterHealth);
        }
    }

    const auto readNumericArray = [&object](const QString &primaryKey, const QString &fallbackKey) {
        QVariantList values;
        const QJsonValue primaryValue = object.value(primaryKey);
        const QJsonValue fallbackValue = object.value(fallbackKey);
        const QJsonArray array = primaryValue.isArray() ? primaryValue.toArray() : fallbackValue.toArray();
        for (const QJsonValue &entry : array) {
            if (entry.isDouble()) {
                values.append(entry.toDouble());
            }
        }
        return values;
    };

    if (!payload.contains(QStringLiteral("inverterVoltages"))) {
        const QVariantList inverterVoltages = readNumericArray(QStringLiteral("inverterVoltages"), QStringLiteral("inverter_voltages"));
        if (!inverterVoltages.isEmpty()) {
            payload.insert(QStringLiteral("inverterVoltages"), inverterVoltages);
        }
    }
    if (!payload.contains(QStringLiteral("inverterCurrents"))) {
        const QVariantList inverterCurrents = readNumericArray(QStringLiteral("inverterCurrents"), QStringLiteral("inverter_currents"));
        if (!inverterCurrents.isEmpty()) {
            payload.insert(QStringLiteral("inverterCurrents"), inverterCurrents);
        }
    }
    if (!payload.contains(QStringLiteral("inverterHealth"))) {
        const QVariantList inverterHealth = readNumericArray(QStringLiteral("inverterHealth"), QStringLiteral("inverter_health"));
        if (!inverterHealth.isEmpty()) {
            payload.insert(QStringLiteral("inverterHealth"), inverterHealth);
        }
    }

    const QJsonValue gpsValue = object.value(QStringLiteral("gps"));
    if (gpsValue.isObject()) {
        const QJsonObject gpsObject = gpsValue.toObject();
        if (gpsObject.value(QStringLiteral("lat")).isDouble()) {
            payload.insert(QStringLiteral("gpsLatitude"), gpsObject.value(QStringLiteral("lat")).toDouble());
        }
        if (gpsObject.value(QStringLiteral("lon")).isDouble()) {
            payload.insert(QStringLiteral("gpsLongitude"), gpsObject.value(QStringLiteral("lon")).toDouble());
        }
    }

    if (!payload.contains(QStringLiteral("gpsLatitude"))) {
        insertOptionalNumber(QStringLiteral("gpsLatitude"), {QStringLiteral("gpsLatitude"), QStringLiteral("gps_latitude")});
    }
    if (!payload.contains(QStringLiteral("gpsLongitude"))) {
        insertOptionalNumber(QStringLiteral("gpsLongitude"), {QStringLiteral("gpsLongitude"), QStringLiteral("gps_longitude")});
    }

    return payload;
}

void MqttTelemetryClient::appendPayloadLogLine(const QString &topicName, const QByteArray &message)
{
    static QMutex logMutex;
    QMutexLocker locker(&logMutex);

    QFile logFile(QStringLiteral("AirTaxiPulse_mqtt_payloads.log"));
    if (!logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        return;
    }

    const QByteArray timestamp = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs).toUtf8();
    logFile.write(timestamp);
    logFile.write(" ");
    logFile.write(topicName.toUtf8());
    logFile.write(" ");
    logFile.write(message);
    logFile.write("\n");
}
