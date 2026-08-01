#ifndef MQTTTELEMETRYCLIENT_H
#define MQTTTELEMETRYCLIENT_H

#include <QObject>
#include <QQueue>
#include <QVariantMap>

#include <QtMqtt/QMqttClient>

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
    explicit MqttTelemetryClient(QObject *parent = nullptr);

    QString host() const;
    void setHost(const QString &host);

    quint16 port() const;
    void setPort(quint16 port);

    QString topicFilter() const;
    void setTopicFilter(const QString &topicFilter);

    bool connected() const;
    bool asyncPayloadLoggingEnabled() const;
    void setAsyncPayloadLoggingEnabled(bool enabled);
    int maxConcurrentParsers() const;
    void setMaxConcurrentParsers(int maxConcurrentParsers);
    int maxPendingMessages() const;
    void setMaxPendingMessages(int maxPendingMessages);
    quint64 droppedMessageCount() const;

    void setCredentials(const QString &username, const QString &password);

public slots:
    void connectToBroker();
    void disconnectFromBroker();

signals:
    void connectionConfigChanged();
    void processingConfigChanged();
    void connectedChanged();
    void backlogStatsChanged();
    void telemetryDecoded(const QVariantMap &payload);
    void errorOccurred(const QString &message);

private slots:
    void handleStateChanged(QMqttClient::ClientState state);
    void handleMessageReceived(const QByteArray &message, const QMqttTopicName &topic);

private:
    struct PendingPayload
    {
        QString topicName;
        QByteArray message;
    };

    void enqueueForProcessing(const QByteArray &message, const QString &topicName);
    void scheduleNextParsers();

    static QVariantMap decodePayload(const QByteArray &message);
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
    QQueue<PendingPayload> m_pendingPayloads;
};

#endif // MQTTTELEMETRYCLIENT_H
