#ifndef FLIGHTSIMULATIONSERVICE_H
#define FLIGHTSIMULATIONSERVICE_H

#include <QObject>
#include <QTimer>
#include <QVector>
#include <QVariantList>
#include <QColor>

#include "../model/FlightTelemetryData.h"
#include "../interfaces/IFlightTelemetrySource.h"

class FlightSimulationService : public QObject, public IFlightTelemetrySource
{
    Q_OBJECT

    Q_PROPERTY(FlightTelemetryData* telemetry READ telemetry CONSTANT)
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
    explicit FlightSimulationService(QObject *parent = nullptr);

    FlightTelemetryData *telemetry() const;

    double cas() const;
    double tas() const;
    double altBaro() const;
    double altRadar() const;
    double vs() const;
    double pitch() const;
    double roll() const;
    double yaw() const;
    double heading() const;
    double track() const;

    double casProgress() const;
    double altProgress() const;
    double vsProgress() const;
    double attitudeProgress() const;
    double headingProgress() const;
    double fpvProgress() const;
    QString flightModeLabel() const;
    QColor vsAccentColor() const;
    QString vsTrendLabel() const;
    QString vsValueText() const;
    double batterySoc() const;
    QVariantList motorTemperatures() const;
    double gpsLatitude() const;
    double gpsLongitude() const;
    QVariantList casHistory() const;
    QVariantList altHistory() const;
    QVariantList vsHistory() const;
    QVariantList attitudeHistory() const;
    QVariantList headingHistory() const;
    QVariantList fpvHistory() const;

signals:
    void telemetryChanged();

private slots:
    void updateSimulation();

private:
    static double clamp(double value, double minimumValue, double maximumValue);
    void appendHistory(QVector<double> &history, double value);
    QVariantList historyToVariantList(const QVector<double> &history) const;

    double m_phase;
    QString m_flightModeLabel;
    double m_batterySoc;
    QVector<double> m_motorTemperatures;
    double m_gpsLatitude;
    double m_gpsLongitude;
    FlightTelemetryData m_telemetry;
    QVector<double> m_casHistory;
    QVector<double> m_altHistory;
    QVector<double> m_vsHistory;
    QVector<double> m_attitudeHistory;
    QVector<double> m_headingHistory;
    QVector<double> m_fpvHistory;
    QTimer m_timer;
};

#endif // FLIGHTSIMULATIONSERVICE_H