/**
 * @file    : src/services/MqttTelemetryClient.h
 * @brief   : Declares the client used to receive and parse MQTT telemetry.
 * @author  : Abhinay Chauhan (email: abhinay.chauhan1@gmail.com)
 * @version : 1.0.0
 *
 * Copyright (c) 2024
 * Abhinay Chauhan. All rights reserved.
 */

#ifndef MQTTTELEMETRYCLIENT_H
#define MQTTTELEMETRYCLIENT_H

#include <QObject>
#include <QMap>
#include <QQueue>
#include <QVariantMap>

#include <QtMqtt/QMqttClient>

/// @brief Receives MQTT telemetry and decodes payloads through a bounded async pipeline.
///
/// The client owns the broker connection, limits parser concurrency and backlog size,
/// and restores source-message order before publishing decoded telemetry.
class MqttTelemetryClient : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString host READ host WRITE setHost NOTIFY connectionConfigChanged)
    Q_PROPERTY(quint16 port READ port WRITE setPort NOTIFY connectionConfigChanged)
    Q_PROPERTY(QString topicFilter READ topicFilter WRITE setTopicFilter NOTIFY connectionConfigChanged)
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
    Q_PROPERTY(bool asyncPayloadLoggingEnabled READ asyncPayloadLoggingEnabled WRITE setAsyncPayloadLoggingEnabled NOTIFY connectionConfigChanged)
    Q_PROPERTY(int maxConcurrentParsers READ maxConcurrentParsers WRITE setMaxConcurrentParsers NOTIFY processingConfigChanged)
    Q_PROPERTY(int maxPendingMessages READ maxPendingMessages WRITE setMaxPendingMessages NOTIFY processingConfigChanged)
    Q_PROPERTY(quint64 droppedMessageCount READ droppedMessageCount NOTIFY backlogStatsChanged)

public:
    /// @brief Constructs an MQTT telemetry client with local broker defaults.
    /// @param parent Optional QObject that owns the client.
    explicit MqttTelemetryClient(QObject *parent = nullptr);

    /// @brief Returns the configured broker host.
    /// @return Broker hostname or IP address.
    QString host() const;
    /// @brief Sets the broker host.
    /// @param host Broker hostname or IP address.
    void setHost(const QString &host);

    /// @brief Returns the configured broker port.
    /// @return MQTT broker port.
    quint16 port() const;
    /// @brief Sets the broker port.
    /// @param port MQTT broker port.
    void setPort(quint16 port);

    /// @brief Returns the telemetry subscription filter.
    /// @return MQTT topic filter.
    QString topicFilter() const;
    /// @brief Sets the telemetry subscription filter.
    /// @param topicFilter MQTT topic filter to subscribe to after connection.
    void setTopicFilter(const QString &topicFilter);

    /// @brief Returns whether the broker connection is established.
    /// @return true when the MQTT client is connected.
    bool connected() const;
    /// @brief Returns whether raw payload logging is enabled.
    /// @return true when payloads are logged asynchronously.
    bool asyncPayloadLoggingEnabled() const;
    /// @brief Enables or disables asynchronous raw payload logging.
    /// @param enabled true to append received payloads to the log file.
    void setAsyncPayloadLoggingEnabled(bool enabled);
    /// @brief Returns the parser concurrency limit.
    /// @return Maximum number of simultaneous decode tasks.
    int maxConcurrentParsers() const;
    /// @brief Sets the parser concurrency limit.
    /// @param maxConcurrentParsers Requested limit, normalized to at least one.
    void setMaxConcurrentParsers(int maxConcurrentParsers);
    /// @brief Returns the pending-message capacity.
    /// @return Maximum number of queued payloads.
    int maxPendingMessages() const;
    /// @brief Sets the pending-message capacity and trims excess queued payloads.
    /// @param maxPendingMessages Requested capacity, normalized to at least one.
    void setMaxPendingMessages(int maxPendingMessages);
    /// @brief Returns the number of payloads discarded due to backlog pressure.
    /// @return Cumulative dropped-message count.
    quint64 droppedMessageCount() const;

    /// @brief Configures credentials used for subsequent broker connections.
    /// @param username MQTT username.
    /// @param password MQTT password.
    void setCredentials(const QString &username, const QString &password);

public slots:
    /// @brief Starts a broker connection unless one is active or pending.
    void connectToBroker();
    /// @brief Disconnects from the broker when currently connected.
    void disconnectFromBroker();

signals:
    /// @brief Notifies observers that broker connection settings changed.
    void connectionConfigChanged();
    /// @brief Notifies observers that parser or backlog settings changed.
    void processingConfigChanged();
    /// @brief Notifies observers that the broker connection state changed.
    void connectedChanged();
    /// @brief Notifies observers that backlog drop statistics changed.
    void backlogStatsChanged();
    /// @brief Publishes a successfully decoded payload in receive order.
    /// @param payload Normalized telemetry fields.
    void telemetryDecoded(const QVariantMap &payload);
    /// @brief Reports MQTT connection or subscription failures.
    /// @param message Human-readable error description.
    void errorOccurred(const QString &message);

private slots:
    /// @brief Handles broker state transitions and subscribes after connection.
    /// @param state New MQTT client state.
    void handleStateChanged(QMqttClient::ClientState state);
    /// @brief Queues a received MQTT message for decoding and optional logging.
    /// @param message Raw MQTT payload.
    /// @param topic Source MQTT topic.
    void handleMessageReceived(const QByteArray &message, const QMqttTopicName &topic);

private:
    /// @brief Payload awaiting an available asynchronous parser.
    struct PendingPayload
    {
        quint64 sequence;
        QString topicName;
        QByteArray message;
    };

    /// @brief Adds a payload to the bounded parser backlog.
    /// @param message Raw payload to decode.
    /// @param topicName Source topic name.
    void enqueueForProcessing(const QByteArray &message, const QString &topicName);
    /// @brief Starts queued decode tasks until the concurrency limit is reached.
    void scheduleNextParsers();
    /// @brief Stores a completed decode and publishes contiguous results in order.
    /// @param sequence Receive-order sequence number.
    /// @param payload Decoded payload, or an empty map for an invalid or dropped message.
    void completePayload(quint64 sequence, const QVariantMap &payload);

    /// @brief Decodes and normalizes a JSON telemetry payload.
    /// @param message Raw JSON payload.
    /// @return Normalized telemetry map, or an empty map when decoding fails.
    static QVariantMap decodePayload(const QByteArray &message);
    /// @brief Appends a timestamped raw payload to the diagnostic log.
    /// @param topicName Source MQTT topic.
    /// @param message Raw MQTT payload.
    static void appendPayloadLogLine(const QString &topicName, const QByteArray &message);

    QMqttClient m_client;
    QString m_host;
    quint16 m_port;
    QString m_topicFilter;
    bool m_asyncPayloadLoggingEnabled;
    int m_maxConcurrentParsers;
    int m_maxPendingMessages;
    int m_activeParsers;
    quint64 m_droppedMessageCount;
    quint64 m_nextSequence;
    quint64 m_nextSequenceToPublish;
    QQueue<PendingPayload> m_pendingPayloads;
    QMap<quint64, QVariantMap> m_completedPayloads;
};

#endif // MQTTTELEMETRYCLIENT_H
