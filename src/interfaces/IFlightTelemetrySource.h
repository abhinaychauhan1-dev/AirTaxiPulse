#ifndef IFLIGHTTELEMETRYSOURCE_H
#define IFLIGHTTELEMETRYSOURCE_H

#include <QColor>
#include <QVariantList>
#include <QString>

class IFlightTelemetrySource
{
public:
    virtual ~IFlightTelemetrySource() = default;

    virtual double cas() const = 0;
    virtual double tas() const = 0;
    virtual double altBaro() const = 0;
    virtual double altRadar() const = 0;
    virtual double vs() const = 0;
    virtual double pitch() const = 0;
    virtual double roll() const = 0;
    virtual double yaw() const = 0;
    virtual double heading() const = 0;
    virtual double track() const = 0;
    virtual double casProgress() const = 0;
    virtual double altProgress() const = 0;
    virtual double vsProgress() const = 0;
    virtual double attitudeProgress() const = 0;
    virtual double headingProgress() const = 0;
    virtual double fpvProgress() const = 0;
    virtual QString flightModeLabel() const = 0;
    virtual QColor vsAccentColor() const = 0;
    virtual QString vsTrendLabel() const = 0;
    virtual QString vsValueText() const = 0;
    virtual double batterySoc() const = 0;
    virtual QVariantList motorTemperatures() const = 0;
    virtual QVariantList motorRpmValues() const = 0;
    virtual double tiltAngleDeg() const = 0;
    virtual QVariantList thrustOutputs() const = 0;
    virtual QVariantList inverterVoltages() const = 0;
    virtual QVariantList inverterCurrents() const = 0;
    virtual QVariantList inverterHealth() const = 0;
    virtual double gpsLatitude() const = 0;
    virtual double gpsLongitude() const = 0;
    virtual QVariantList casHistory() const = 0;
    virtual QVariantList altHistory() const = 0;
    virtual QVariantList vsHistory() const = 0;
    virtual QVariantList attitudeHistory() const = 0;
    virtual QVariantList headingHistory() const = 0;
    virtual QVariantList fpvHistory() const = 0;
    virtual QVariantList propulsionRpmHistory() const = 0;
    virtual QVariantList propulsionTiltHistory() const = 0;
    virtual QVariantList propulsionTempHistory() const = 0;
    virtual QVariantList propulsionThrustHistory() const = 0;
    virtual QVariantList propulsionInverterHealthHistory() const = 0;
};

#endif // IFLIGHTTELEMETRYSOURCE_H