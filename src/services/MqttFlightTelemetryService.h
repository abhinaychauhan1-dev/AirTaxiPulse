#ifndef MQTTFLIGHTTELEMETRYSERVICE_H
#define MQTTFLIGHTTELEMETRYSERVICE_H

#include <QObject>
#include <QColor>
#include <QReadWriteLock>
#include <QVariantList>
#include <QVector>

#include "MqttTelemetryClient.h"
#include "../interfaces/IFlightTelemetrySource.h"
#include "../model/FlightTelemetryData.h"

class MqttFlightTelemetryService : public QObject, public IFlightTelemetrySource
{
    Q_OBJECT

    Q_PROPERTY(FlightTelemetryData* telemetry READ telemetry CONSTANT)
    Q_PROPERTY(QString brokerHost READ brokerHost WRITE setBrokerHost NOTIFY connectionConfigChanged)
    Q_PROPERTY(quint16 brokerPort READ brokerPort WRITE setBrokerPort NOTIFY connectionConfigChanged)
    Q_PROPERTY(QString topicFilter READ topicFilter WRITE setTopicFilter NOTIFY connectionConfigChanged)
    Q_PROPERTY(bool asyncPayloadLoggingEnabled READ asyncPayloadLoggingEnabled WRITE setAsyncPayloadLoggingEnabled NOTIFY processingConfigChanged)
    Q_PROPERTY(int maxConcurrentParsers READ maxConcurrentParsers WRITE setMaxConcurrentParsers NOTIFY processingConfigChanged)
    Q_PROPERTY(int maxPendingMessages READ maxPendingMessages WRITE setMaxPendingMessages NOTIFY processingConfigChanged)
    Q_PROPERTY(quint64 droppedMessageCount READ droppedMessageCount NOTIFY backlogStatsChanged)
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
    Q_PROPERTY(double casProgress READ casProgress NOTIFY telemetryChanged)
    Q_PROPERTY(double altProgress READ altProgress NOTIFY telemetryChanged)
    Q_PROPERTY(double vsProgress READ vsProgress NOTIFY telemetryChanged)
    Q_PROPERTY(double attitudeProgress READ attitudeProgress NOTIFY telemetryChanged)
    Q_PROPERTY(double headingProgress READ headingProgress NOTIFY telemetryChanged)
    Q_PROPERTY(double fpvProgress READ fpvProgress NOTIFY telemetryChanged)
    Q_PROPERTY(QString flightModeLabel READ flightModeLabel NOTIFY telemetryChanged)
    Q_PROPERTY(QColor vsAccentColor READ vsAccentColor NOTIFY telemetryChanged)
    Q_PROPERTY(QString vsTrendLabel READ vsTrendLabel NOTIFY telemetryChanged)
    Q_PROPERTY(QString vsValueText READ vsValueText NOTIFY telemetryChanged)
    Q_PROPERTY(double batterySoc READ batterySoc NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList motorTemperatures READ motorTemperatures NOTIFY telemetryChanged)
    Q_PROPERTY(double gpsLatitude READ gpsLatitude NOTIFY telemetryChanged)
    Q_PROPERTY(double gpsLongitude READ gpsLongitude NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList casHistory READ casHistory NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList altHistory READ altHistory NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList vsHistory READ vsHistory NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList attitudeHistory READ attitudeHistory NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList headingHistory READ headingHistory NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList fpvHistory READ fpvHistory NOTIFY telemetryChanged)

public:
    explicit MqttFlightTelemetryService(QObject *parent = nullptr);

    FlightTelemetryData *telemetry() const;

    QString brokerHost() const;
    void setBrokerHost(const QString &host);

    quint16 brokerPort() const;
    void setBrokerPort(quint16 port);

    QString topicFilter() const;
    void setTopicFilter(const QString &topicFilter);

    bool asyncPayloadLoggingEnabled() const;
    void setAsyncPayloadLoggingEnabled(bool enabled);

    int maxConcurrentParsers() const;
    void setMaxConcurrentParsers(int maxConcurrentParsers);

    int maxPendingMessages() const;
    void setMaxPendingMessages(int maxPendingMessages);

    quint64 droppedMessageCount() const;

    bool connected() const;

    Q_INVOKABLE void connectToBroker();
    Q_INVOKABLE void disconnectFromBroker();

    double cas() const override;
    double tas() const override;
    double altBaro() const override;
    double altRadar() const override;
    double vs() const override;
    double pitch() const override;
    double roll() const override;
    double yaw() const override;
    double heading() const override;
    double track() const override;

    double casProgress() const override;
    double altProgress() const override;
    double vsProgress() const override;
    double attitudeProgress() const override;
    double headingProgress() const override;
    double fpvProgress() const override;
    QString flightModeLabel() const override;
    QColor vsAccentColor() const override;
    QString vsTrendLabel() const override;
    QString vsValueText() const override;
    double batterySoc() const override;
    QVariantList motorTemperatures() const override;
    double gpsLatitude() const override;
    double gpsLongitude() const override;

    QVariantList casHistory() const override;
    QVariantList altHistory() const override;
    QVariantList vsHistory() const override;
    QVariantList attitudeHistory() const override;
    QVariantList headingHistory() const override;
    QVariantList fpvHistory() const override;

signals:
    void telemetryChanged();
    void connectedChanged();
    void connectionConfigChanged();
    void processingConfigChanged();
    void backlogStatsChanged();
    void errorOccurred(const QString &message);

private slots:
    void onTelemetryDecoded(const QVariantMap &payload);

private:
    static double clamp(double value, double minimumValue, double maximumValue);
    static double wrappedAngleDeltaDegrees(double angleA, double angleB);
    static double mapToBand(double normalizedValue, double bandMinimum, double bandMaximum);
    void appendHistory(QVector<double> &history, double value);
    QVariantList historyToVariantList(const QVector<double> &history) const;

    mutable QReadWriteLock m_stateLock;
    MqttTelemetryClient m_client;
    FlightTelemetryData m_telemetry;
    QString m_flightModeLabel;
    double m_batterySoc;
    QVector<double> m_motorTemperatures;
    double m_gpsLatitude;
    double m_gpsLongitude;
    QVector<double> m_casHistory;
    QVector<double> m_altHistory;
    QVector<double> m_vsHistory;
    QVector<double> m_attitudeHistory;
    QVector<double> m_headingHistory;
    QVector<double> m_fpvHistory;
};

#endif // MQTTFLIGHTTELEMETRYSERVICE_H
