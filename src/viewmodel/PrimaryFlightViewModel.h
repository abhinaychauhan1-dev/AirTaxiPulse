#ifndef PRIMARYFLIGHTVIEWMODEL_H
#define PRIMARYFLIGHTVIEWMODEL_H

#include <QObject>
#include <QColor>
#include <QVariantList>

#include "../interfaces/IFlightTelemetrySource.h"

class PrimaryFlightViewModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString title READ title CONSTANT)
    Q_PROPERTY(double cas READ cas NOTIFY telemetryChanged)
    Q_PROPERTY(double tas READ tas NOTIFY telemetryChanged)
    Q_PROPERTY(double altBaro READ altBaro NOTIFY telemetryChanged)
    Q_PROPERTY(double altRadar READ altRadar NOTIFY telemetryChanged)
    Q_PROPERTY(double vs READ vs NOTIFY telemetryChanged)
    Q_PROPERTY(double pitch READ pitch NOTIFY telemetryChanged)
    Q_PROPERTY(double roll READ roll NOTIFY telemetryChanged)
    Q_PROPERTY(double yaw READ yaw NOTIFY telemetryChanged)
    Q_PROPERTY(double heading READ heading NOTIFY telemetryChanged)
    Q_PROPERTY(double track READ track NOTIFY telemetryChanged)
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
    Q_PROPERTY(QVariantList casHistory READ casHistory NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList altHistory READ altHistory NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList vsHistory READ vsHistory NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList attitudeHistory READ attitudeHistory NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList headingHistory READ headingHistory NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList fpvHistory READ fpvHistory NOTIFY telemetryChanged)

public:
    explicit PrimaryFlightViewModel(const QString &title,
                                    IFlightTelemetrySource &telemetrySource,
                                    QObject *parent = nullptr);

    QString title() const;

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
    QVariantList casHistory() const;
    QVariantList altHistory() const;
    QVariantList vsHistory() const;
    QVariantList attitudeHistory() const;
    QVariantList headingHistory() const;
    QVariantList fpvHistory() const;

signals:
    void telemetryChanged();

private:
    QString m_title;
    IFlightTelemetrySource &m_telemetrySource;
};

#endif // PRIMARYFLIGHTVIEWMODEL_H