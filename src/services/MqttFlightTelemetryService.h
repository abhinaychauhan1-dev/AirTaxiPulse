/**
 * @file    : src/services/MqttFlightTelemetryService.h
 * @brief   : Declares the MQTT-backed flight telemetry service.
 * @author  : Abhinay Chauhan (email: abhinay.chauhan1@gmail.com)
 * @version : 1.0.0
 *
 * Copyright (c) 2024
 * Abhinay Chauhan. All rights reserved.
 */

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

/// @brief Adapts decoded MQTT payloads into thread-safe flight telemetry for QML.
///
/// The service forwards broker and parser configuration to MqttTelemetryClient,
/// validates incoming component arrays, and maintains presentation metrics and
/// bounded histories under a read-write lock.
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
    Q_PROPERTY(double batterySoh READ batterySoh NOTIFY telemetryChanged)
    Q_PROPERTY(double powerConsumptionKw READ powerConsumptionKw NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList batteryCellTemperatures READ batteryCellTemperatures NOTIFY telemetryChanged)
    Q_PROPERTY(double busVoltage READ busVoltage NOTIFY telemetryChanged)
    Q_PROPERTY(double busCurrent READ busCurrent NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList motorTemperatures READ motorTemperatures NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList motorRpmValues READ motorRpmValues NOTIFY telemetryChanged)
    Q_PROPERTY(double tiltAngleDeg READ tiltAngleDeg NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList thrustOutputs READ thrustOutputs NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList inverterVoltages READ inverterVoltages NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList inverterCurrents READ inverterCurrents NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList inverterHealth READ inverterHealth NOTIFY telemetryChanged)
    Q_PROPERTY(double gpsLatitude READ gpsLatitude NOTIFY telemetryChanged)
    Q_PROPERTY(double gpsLongitude READ gpsLongitude NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList casHistory READ casHistory NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList altHistory READ altHistory NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList vsHistory READ vsHistory NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList attitudeHistory READ attitudeHistory NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList headingHistory READ headingHistory NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList fpvHistory READ fpvHistory NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList propulsionRpmHistory READ propulsionRpmHistory NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList propulsionTiltHistory READ propulsionTiltHistory NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList propulsionTempHistory READ propulsionTempHistory NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList propulsionThrustHistory READ propulsionThrustHistory NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList propulsionInverterHealthHistory READ propulsionInverterHealthHistory NOTIFY telemetryChanged)

public:
    /// @brief Constructs the service and connects client notifications.
    /// @param parent Optional QObject that owns the service.
    explicit MqttFlightTelemetryService(QObject *parent = nullptr);

    /// @brief Returns the mutable telemetry object exposed to QML.
    /// @return Pointer to the service-owned telemetry data.
    FlightTelemetryData *telemetry() const;

    /// @brief Returns the configured MQTT broker host.
    /// @return Broker hostname or IP address.
    QString brokerHost() const;
    /// @brief Sets the MQTT broker host.
    /// @param host Broker hostname or IP address.
    void setBrokerHost(const QString &host);

    /// @brief Returns the configured MQTT broker port.
    /// @return MQTT broker port.
    quint16 brokerPort() const;
    /// @brief Sets the MQTT broker port.
    /// @param port MQTT broker port.
    void setBrokerPort(quint16 port);

    /// @brief Returns the telemetry subscription filter.
    /// @return MQTT topic filter.
    QString topicFilter() const;
    /// @brief Sets the telemetry subscription filter.
    /// @param topicFilter MQTT topic filter.
    void setTopicFilter(const QString &topicFilter);

    /// @brief Returns whether raw payload logging is enabled.
    /// @return true when payloads are logged asynchronously.
    bool asyncPayloadLoggingEnabled() const;
    /// @brief Enables or disables asynchronous raw payload logging.
    /// @param enabled true to log received MQTT payloads.
    void setAsyncPayloadLoggingEnabled(bool enabled);

    /// @brief Returns the parser concurrency limit.
    /// @return Maximum number of simultaneous parser tasks.
    int maxConcurrentParsers() const;
    /// @brief Sets the parser concurrency limit.
    /// @param maxConcurrentParsers Requested parser count.
    void setMaxConcurrentParsers(int maxConcurrentParsers);

    /// @brief Returns the pending-message capacity.
    /// @return Maximum number of queued payloads.
    int maxPendingMessages() const;
    /// @brief Sets the pending-message capacity.
    /// @param maxPendingMessages Requested backlog capacity.
    void setMaxPendingMessages(int maxPendingMessages);

    /// @brief Decreases parser concurrency by one down to the supported minimum.
    Q_INVOKABLE void decreaseParserConcurrency();
    /// @brief Increases parser concurrency by one up to the UI-supported maximum.
    Q_INVOKABLE void increaseParserConcurrency();
    /// @brief Increases pending capacity by the configured UI increment.
    Q_INVOKABLE void increasePendingCapacity();

    /// @brief Returns the number of payloads dropped due to backlog pressure.
    /// @return Cumulative dropped-message count.
    quint64 droppedMessageCount() const;

    /// @brief Returns whether the MQTT broker connection is established.
    /// @return true when connected to the broker.
    bool connected() const;

    /// @brief Starts a connection to the configured MQTT broker.
    Q_INVOKABLE void connectToBroker();
    /// @brief Disconnects from the MQTT broker.
    Q_INVOKABLE void disconnectFromBroker();

    /// @brief Returns calibrated airspeed in knots.
    double cas() const override;
    /// @brief Returns true airspeed in knots.
    double tas() const override;
    /// @brief Returns barometric altitude in feet.
    double altBaro() const override;
    /// @brief Returns radar altitude in feet.
    double altRadar() const override;
    /// @brief Returns vertical speed in feet per minute.
    double vs() const override;
    /// @brief Returns pitch in degrees.
    double pitch() const override;
    /// @brief Returns roll in degrees.
    double roll() const override;
    /// @brief Returns yaw in degrees.
    double yaw() const override;
    /// @brief Returns heading in degrees.
    double heading() const override;
    /// @brief Returns ground track in degrees.
    double track() const override;

    /// @brief Returns calibrated-airspeed progress mapped to the display band.
    double casProgress() const override;
    /// @brief Returns blended-altitude progress mapped to the display band.
    double altProgress() const override;
    /// @brief Returns vertical-speed progress mapped to the display band.
    double vsProgress() const override;
    /// @brief Returns attitude-stability progress mapped to the display band.
    double attitudeProgress() const override;
    /// @brief Returns heading-derived progress mapped to the display band.
    double headingProgress() const override;
    /// @brief Returns flight-path-vector alignment progress.
    double fpvProgress() const override;
    /// @brief Returns the current flight mode label.
    QString flightModeLabel() const override;
    /// @brief Returns the vertical-speed status color.
    QColor vsAccentColor() const override;
    /// @brief Returns the vertical-speed trend label.
    QString vsTrendLabel() const override;
    /// @brief Returns signed vertical-speed text with units.
    QString vsValueText() const override;
    /// @brief Returns battery state of charge as a percentage.
    double batterySoc() const override;
    /// @brief Returns battery state of health as a percentage.
    double batterySoh() const override;
    /// @brief Returns propulsion power consumption in kilowatts.
    double powerConsumptionKw() const override;
    /// @brief Returns battery-cell temperatures in degrees Celsius.
    QVariantList batteryCellTemperatures() const override;
    /// @brief Returns high-voltage bus voltage in volts.
    double busVoltage() const override;
    /// @brief Returns high-voltage bus current in amperes.
    double busCurrent() const override;
    /// @brief Returns motor temperatures in degrees Celsius.
    QVariantList motorTemperatures() const override;
    /// @brief Returns motor speeds in revolutions per minute.
    QVariantList motorRpmValues() const override;
    /// @brief Returns propulsion tilt angle in degrees.
    double tiltAngleDeg() const override;
    /// @brief Returns front and rear thrust outputs.
    QVariantList thrustOutputs() const override;
    /// @brief Returns per-inverter voltages.
    QVariantList inverterVoltages() const override;
    /// @brief Returns per-inverter currents.
    QVariantList inverterCurrents() const override;
    /// @brief Returns per-inverter health percentages.
    QVariantList inverterHealth() const override;
    /// @brief Returns GPS latitude in decimal degrees.
    double gpsLatitude() const override;
    /// @brief Returns GPS longitude in decimal degrees.
    double gpsLongitude() const override;

    /// @brief Returns recent calibrated-airspeed samples.
    QVariantList casHistory() const override;
    /// @brief Returns recent barometric-altitude samples.
    QVariantList altHistory() const override;
    /// @brief Returns recent vertical-speed samples.
    QVariantList vsHistory() const override;
    /// @brief Returns recent combined attitude-load samples.
    QVariantList attitudeHistory() const override;
    /// @brief Returns recent heading samples.
    QVariantList headingHistory() const override;
    /// @brief Returns recent flight-path-vector error samples.
    QVariantList fpvHistory() const override;
    /// @brief Returns recent average motor-speed samples.
    QVariantList propulsionRpmHistory() const override;
    /// @brief Returns recent propulsion tilt samples.
    QVariantList propulsionTiltHistory() const override;
    /// @brief Returns recent average motor-temperature samples.
    QVariantList propulsionTempHistory() const override;
    /// @brief Returns recent total-thrust samples.
    QVariantList propulsionThrustHistory() const override;
    /// @brief Returns recent average inverter-health samples.
    QVariantList propulsionInverterHealthHistory() const override;

signals:
    /// @brief Notifies observers that telemetry values and histories changed.
    void telemetryChanged();
    /// @brief Notifies observers that the broker connection state changed.
    void connectedChanged();
    /// @brief Notifies observers that broker connection settings changed.
    void connectionConfigChanged();
    /// @brief Notifies observers that parser or backlog settings changed.
    void processingConfigChanged();
    /// @brief Notifies observers that backlog drop statistics changed.
    void backlogStatsChanged();
    /// @brief Reports broker, subscription, or payload validation failures.
    /// @param message Human-readable error description.
    void errorOccurred(const QString &message);

private slots:
    /// @brief Applies a normalized MQTT payload to service state and histories.
    /// @param payload Decoded telemetry fields.
    void onTelemetryDecoded(const QVariantMap &payload);

private:
    /// @brief Restricts a value to an inclusive numeric range.
    static double clamp(double value, double minimumValue, double maximumValue);
    /// @brief Calculates the shortest absolute separation between two headings.
    static double wrappedAngleDeltaDegrees(double angleA, double angleB);
    /// @brief Maps a normalized value into a presentation band.
    static double mapToBand(double normalizedValue, double bandMinimum, double bandMaximum);
    /// @brief Appends a sample while preserving the bounded history length.
    void appendHistory(QVector<double> &history, double value);
    /// @brief Converts an internal numeric history for QML consumption.
    QVariantList historyToVariantList(const QVector<double> &history) const;

    mutable QReadWriteLock m_stateLock;
    MqttTelemetryClient m_client;
    FlightTelemetryData m_telemetry;
    QString m_flightModeLabel;
    double m_batterySoc;
    double m_batterySoh;
    double m_powerConsumptionKw;
    QVector<double> m_batteryCellTemperatures;
    double m_busVoltage;
    double m_busCurrent;
    QVector<double> m_motorTemperatures;
    QVector<double> m_motorRpmValues;
    double m_tiltAngleDeg;
    QVector<double> m_thrustOutputs;
    QVector<double> m_inverterVoltages;
    QVector<double> m_inverterCurrents;
    QVector<double> m_inverterHealth;
    double m_gpsLatitude;
    double m_gpsLongitude;
    QVector<double> m_casHistory;
    QVector<double> m_altHistory;
    QVector<double> m_vsHistory;
    QVector<double> m_attitudeHistory;
    QVector<double> m_headingHistory;
    QVector<double> m_fpvHistory;
    QVector<double> m_propulsionRpmHistory;
    QVector<double> m_propulsionTiltHistory;
    QVector<double> m_propulsionTempHistory;
    QVector<double> m_propulsionThrustHistory;
    QVector<double> m_propulsionInverterHealthHistory;
};

#endif // MQTTFLIGHTTELEMETRYSERVICE_H
