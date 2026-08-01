#ifndef FLIGHTTELEMETRYDATA_H
#define FLIGHTTELEMETRYDATA_H

#include <QObject>

class FlightTelemetryData : public QObject
{
    Q_OBJECT

    Q_PROPERTY(double cas READ cas WRITE setCas NOTIFY casChanged)
    Q_PROPERTY(double tas READ tas WRITE setTas NOTIFY tasChanged)
    Q_PROPERTY(double altBaro READ altBaro WRITE setAltBaro NOTIFY altBaroChanged)
    Q_PROPERTY(double altRadar READ altRadar WRITE setAltRadar NOTIFY altRadarChanged)
    Q_PROPERTY(double vs READ vs WRITE setVs NOTIFY vsChanged)
    Q_PROPERTY(double pitch READ pitch WRITE setPitch NOTIFY pitchChanged)
    Q_PROPERTY(double roll READ roll WRITE setRoll NOTIFY rollChanged)
    Q_PROPERTY(double yaw READ yaw WRITE setYaw NOTIFY yawChanged)
    Q_PROPERTY(double heading READ heading WRITE setHeading NOTIFY headingChanged)
    Q_PROPERTY(double track READ track WRITE setTrack NOTIFY trackChanged)

public:
    explicit FlightTelemetryData(QObject *parent = nullptr);

    double cas() const;
    void setCas(double value);

    double tas() const;
    void setTas(double value);

    double altBaro() const;
    void setAltBaro(double value);

    double altRadar() const;
    void setAltRadar(double value);

    double vs() const;
    void setVs(double value);

    double pitch() const;
    void setPitch(double value);

    double roll() const;
    void setRoll(double value);

    double yaw() const;
    void setYaw(double value);

    double heading() const;
    void setHeading(double value);

    double track() const;
    void setTrack(double value);

signals:
    void casChanged();
    void tasChanged();
    void altBaroChanged();
    void altRadarChanged();
    void vsChanged();
    void pitchChanged();
    void rollChanged();
    void yawChanged();
    void headingChanged();
    void trackChanged();

private:
    double m_cas;
    double m_tas;
    double m_altBaro;
    double m_altRadar;
    double m_vs;
    double m_pitch;
    double m_roll;
    double m_yaw;
    double m_heading;
    double m_track;
};

#endif // FLIGHTTELEMETRYDATA_H