#ifndef OPERATIONALVIEWMODELS_H
#define OPERATIONALVIEWMODELS_H

#include <QDateTime>
#include <QObject>
#include <QTimer>
#include <QVariantList>

#include "../interfaces/IFlightTelemetrySource.h"

class FlightSessionViewModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentDateText READ currentDateText NOTIFY sessionChanged)
    Q_PROPERTY(QString currentTimeText READ currentTimeText NOTIFY sessionChanged)
    Q_PROPERTY(QString elapsedText READ elapsedText NOTIFY sessionChanged)

public:
    explicit FlightSessionViewModel(QObject *parent = nullptr);

    QString currentDateText() const;
    QString currentTimeText() const;
    QString elapsedText() const;

signals:
    void sessionChanged();

private:
    QDateTime m_currentDateTime;
    int m_elapsedSeconds;
    QTimer m_timer;
};

class PropulsionSystemViewModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double frontGroupRpm READ frontGroupRpm NOTIFY telemetryChanged)
    Q_PROPERTY(double rearGroupRpm READ rearGroupRpm NOTIFY telemetryChanged)
    Q_PROPERTY(double averageRpm READ averageRpm NOTIFY telemetryChanged)
    Q_PROPERTY(double averageMotorTemperature READ averageMotorTemperature NOTIFY telemetryChanged)
    Q_PROPERTY(double inverterHealthAverage READ inverterHealthAverage NOTIFY telemetryChanged)
    Q_PROPERTY(double frontThrust READ frontThrust NOTIFY telemetryChanged)
    Q_PROPERTY(double rearThrust READ rearThrust NOTIFY telemetryChanged)
    Q_PROPERTY(double combinedThrust READ combinedThrust NOTIFY telemetryChanged)
    Q_PROPERTY(double thrustBalance READ thrustBalance NOTIFY telemetryChanged)
    Q_PROPERTY(double electricalPowerKw READ electricalPowerKw NOTIFY telemetryChanged)
    Q_PROPERTY(QString stateLabel READ stateLabel NOTIFY telemetryChanged)
    Q_PROPERTY(double rpmProgress READ rpmProgress NOTIFY telemetryChanged)
    Q_PROPERTY(double tiltProgress READ tiltProgress NOTIFY telemetryChanged)
    Q_PROPERTY(double temperatureProgress READ temperatureProgress NOTIFY telemetryChanged)
    Q_PROPERTY(double thrustProgress READ thrustProgress NOTIFY telemetryChanged)
    Q_PROPERTY(double inverterProgress READ inverterProgress NOTIFY telemetryChanged)
    Q_PROPERTY(QString tiltProfileLabel READ tiltProfileLabel NOTIFY telemetryChanged)
    Q_PROPERTY(QString thrustBalanceLabel READ thrustBalanceLabel NOTIFY telemetryChanged)
    Q_PROPERTY(bool thrustBalanced READ thrustBalanced NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList motorThermalWarnings READ motorThermalWarnings NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList inverterHealthWarnings READ inverterHealthWarnings NOTIFY telemetryChanged)

public:
    explicit PropulsionSystemViewModel(IFlightTelemetrySource &telemetrySource,
                                       QObject *parent = nullptr);

    double frontGroupRpm() const;
    double rearGroupRpm() const;
    double averageRpm() const;
    double averageMotorTemperature() const;
    double inverterHealthAverage() const;
    double frontThrust() const;
    double rearThrust() const;
    double combinedThrust() const;
    double thrustBalance() const;
    double electricalPowerKw() const;
    QString stateLabel() const;
    double rpmProgress() const;
    double tiltProgress() const;
    double temperatureProgress() const;
    double thrustProgress() const;
    double inverterProgress() const;
    QString tiltProfileLabel() const;
    QString thrustBalanceLabel() const;
    bool thrustBalanced() const;
    QVariantList motorThermalWarnings() const;
    QVariantList inverterHealthWarnings() const;

signals:
    void telemetryChanged();

private:
    static double average(const QVariantList &values);
    static double valueAt(const QVariantList &values, int index);

    IFlightTelemetrySource &m_telemetrySource;
};

class EnergySystemViewModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool hoverMode READ hoverMode NOTIFY telemetryChanged)
    Q_PROPERTY(double estimatedMinutes READ estimatedMinutes NOTIFY telemetryChanged)
    Q_PROPERTY(int status READ status NOTIFY telemetryChanged)
    Q_PROPERTY(QString statusLabel READ statusLabel NOTIFY telemetryChanged)
    Q_PROPERTY(QString powerProfileLabel READ powerProfileLabel NOTIFY telemetryChanged)
    Q_PROPERTY(double efficiencyPercent READ efficiencyPercent NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList zoneTemperatures READ zoneTemperatures NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList busChannelCurrents READ busChannelCurrents NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList busChannelPowers READ busChannelPowers NOTIFY telemetryChanged)

public:
    enum Status {
        Nominal,
        LowReserve,
        ThermalWarning
    };
    Q_ENUM(Status)

    explicit EnergySystemViewModel(IFlightTelemetrySource &telemetrySource,
                                   QObject *parent = nullptr);

    bool hoverMode() const;
    double estimatedMinutes() const;
    int status() const;
    QString statusLabel() const;
    QString powerProfileLabel() const;
    double efficiencyPercent() const;
    QVariantList zoneTemperatures() const;
    QVariantList busChannelCurrents() const;
    QVariantList busChannelPowers() const;

    Q_INVOKABLE double zoneTemperature(int index) const;
    Q_INVOKABLE QString busChannelName(int index) const;
    Q_INVOKABLE QString busChannelCode(int index) const;
    Q_INVOKABLE double busChannelCurrent(int index) const;
    Q_INVOKABLE double busChannelPower(int index) const;

signals:
    void telemetryChanged();

private:
    double cellTemperatureMax() const;
    double busChannelShare(int index) const;

    IFlightTelemetrySource &m_telemetrySource;
};

class FlightControlSystemViewModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool autopilotEngaged READ autopilotEngaged NOTIFY stateChanged)
    Q_PROPERTY(int guidanceMode READ guidanceMode NOTIFY stateChanged)
    Q_PROPERTY(int commandedFlightMode READ commandedFlightMode NOTIFY stateChanged)
    Q_PROPERTY(QString flightMode READ flightMode NOTIFY telemetryChanged)
    Q_PROPERTY(int flightModeIndex READ flightModeIndex NOTIFY stateChanged)
    Q_PROPERTY(double leftElevon READ leftElevon NOTIFY stateChanged)
    Q_PROPERTY(double rightElevon READ rightElevon NOTIFY stateChanged)
    Q_PROPERTY(double ruddervator READ ruddervator NOTIFY stateChanged)
    Q_PROPERTY(double tiltActuator READ tiltActuator NOTIFY stateChanged)
    Q_PROPERTY(QVariantList actuatorValues READ actuatorValues NOTIFY stateChanged)
    Q_PROPERTY(QVariantList waypoints READ waypoints CONSTANT)
    Q_PROPERTY(bool automaticRouteSequence READ automaticRouteSequence NOTIFY stateChanged)
    Q_PROPERTY(int navigationWaypoint READ navigationWaypoint NOTIFY stateChanged)
    Q_PROPERTY(double nextWaypointDistance READ nextWaypointDistance NOTIFY stateChanged)
    Q_PROPERTY(QString routeLegStatus READ routeLegStatus NOTIFY stateChanged)

public:
    explicit FlightControlSystemViewModel(IFlightTelemetrySource &telemetrySource,
                                          QObject *parent = nullptr);

    bool autopilotEngaged() const;
    int guidanceMode() const;
    int commandedFlightMode() const;
    QString flightMode() const;
    int flightModeIndex() const;
    double leftElevon() const;
    double rightElevon() const;
    double ruddervator() const;
    double tiltActuator() const;
    QVariantList actuatorValues() const;
    QVariantList waypoints() const;
    bool automaticRouteSequence() const;
    int navigationWaypoint() const;
    double nextWaypointDistance() const;
    QString routeLegStatus() const;

    Q_INVOKABLE QString modeName(int index) const;
    Q_INVOKABLE QString guidanceName(int index) const;
    Q_INVOKABLE QString actuatorName(int index) const;
    Q_INVOKABLE double actuatorValue(int index) const;
    Q_INVOKABLE double actuatorMinimum(int index) const;
    Q_INVOKABLE double actuatorMaximum(int index) const;
    Q_INVOKABLE void clearCommandedFlightMode();
    Q_INVOKABLE void setCommandedFlightMode(int index);
    Q_INVOKABLE void setGuidanceMode(int index);
    Q_INVOKABLE void toggleAutopilot();
    Q_INVOKABLE void setManualActuator(int index, double value);
    Q_INVOKABLE void selectDirectTo(int index);
    Q_INVOKABLE void resumeAutomaticRoute();

signals:
    void telemetryChanged();
    void stateChanged();

private:
    static double clamp(double value, double minimum, double maximum);
    int telemetryFlightModeIndex() const;
    int automaticWaypoint() const;
    double distanceToWaypoint(int index) const;

    IFlightTelemetrySource &m_telemetrySource;
    bool m_autopilotEngaged;
    int m_guidanceMode;
    int m_commandedFlightMode;
    bool m_automaticRouteSequence;
    int m_selectedWaypoint;
    double m_manualLeftElevon;
    double m_manualRightElevon;
    double m_manualRuddervator;
    double m_manualTiltActuator;
    QVariantList m_waypoints;
};

class SafetySystemViewModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int noiseLimit READ noiseLimit NOTIFY stateChanged)
    Q_PROPERTY(QVariantList noiseHistory READ noiseHistory NOTIFY stateChanged)
    Q_PROPERTY(double acousticDb READ acousticDb NOTIFY stateChanged)
    Q_PROPERTY(bool noiseWarning READ noiseWarning NOTIFY stateChanged)
    Q_PROPERTY(int satelliteCount READ satelliteCount NOTIFY stateChanged)
    Q_PROPERTY(double signalStrength READ signalStrength NOTIFY stateChanged)
    Q_PROPERTY(double navigationAccuracy READ navigationAccuracy NOTIFY stateChanged)
    Q_PROPERTY(bool raimAvailable READ raimAvailable NOTIFY stateChanged)
    Q_PROPERTY(double lateralDeviation READ lateralDeviation NOTIFY stateChanged)
    Q_PROPERTY(double verticalDeviation READ verticalDeviation NOTIFY stateChanged)
    Q_PROPERTY(double alignmentQuality READ alignmentQuality NOTIFY stateChanged)
    Q_PROPERTY(bool landingPhase READ landingPhase NOTIFY stateChanged)
    Q_PROPERTY(int parachuteState READ parachuteState NOTIFY stateChanged)
    Q_PROPERTY(double parachuteProgress READ parachuteProgress NOTIFY stateChanged)
    Q_PROPERTY(bool parachuteTestOverride READ parachuteTestOverride NOTIFY stateChanged)
    Q_PROPERTY(QString parachuteEventTime READ parachuteEventTime NOTIFY stateChanged)
    Q_PROPERTY(bool parachuteEnvelopeValid READ parachuteEnvelopeValid NOTIFY stateChanged)
    Q_PROPERTY(bool parachuteDeployAllowed READ parachuteDeployAllowed NOTIFY stateChanged)
    Q_PROPERTY(QString parachuteInhibitReason READ parachuteInhibitReason NOTIFY stateChanged)
    Q_PROPERTY(bool gnssTestMode READ gnssTestMode NOTIFY stateChanged)
    Q_PROPERTY(int selectedTraffic READ selectedTraffic NOTIFY stateChanged)
    Q_PROPERTY(QVariantList trafficRanges READ trafficRanges NOTIFY stateChanged)
    Q_PROPERTY(QVariantList trafficBearings READ trafficBearings NOTIFY stateChanged)
    Q_PROPERTY(QVariantList monitorSummaries READ monitorSummaries NOTIFY stateChanged)
    Q_PROPERTY(int alertCount READ alertCount NOTIFY stateChanged)

public:
    explicit SafetySystemViewModel(IFlightTelemetrySource &telemetrySource,
                                   QObject *parent = nullptr);

    int noiseLimit() const;
    QVariantList noiseHistory() const;
    double acousticDb() const;
    bool noiseWarning() const;
    int satelliteCount() const;
    double signalStrength() const;
    double navigationAccuracy() const;
    bool raimAvailable() const;
    double lateralDeviation() const;
    double verticalDeviation() const;
    double alignmentQuality() const;
    bool landingPhase() const;
    int parachuteState() const;
    double parachuteProgress() const;
    bool parachuteTestOverride() const;
    QString parachuteEventTime() const;
    bool parachuteEnvelopeValid() const;
    bool parachuteDeployAllowed() const;
    QString parachuteInhibitReason() const;
    bool gnssTestMode() const;
    int selectedTraffic() const;
    QVariantList trafficRanges() const;
    QVariantList trafficBearings() const;
    QVariantList monitorSummaries() const;
    int alertCount() const;

    Q_INVOKABLE void setNoiseLimit(int value);
    Q_INVOKABLE void toggleGnssTestMode();
    Q_INVOKABLE void toggleParachuteTestOverride();
    Q_INVOKABLE void toggleParachuteArm();
    Q_INVOKABLE void deployOrResetParachute();
    Q_INVOKABLE void setSelectedTraffic(int index);
    Q_INVOKABLE QString monitorCode(int index) const;
    Q_INVOKABLE QString monitorSummary(int index, int selectedTraffic) const;
    Q_INVOKABLE QString signedValue(double value) const;
    Q_INVOKABLE QString trafficCode(int index) const;
    Q_INVOKABLE double trafficRange(int index) const;
    Q_INVOKABLE double trafficBearing(int index) const;

signals:
    void telemetryChanged();
    void stateChanged();

private:
    static double clamp(double value, double minimum, double maximum);
    static double average(const QVariantList &values);
    void updateSimulation();
    void updateParachuteDeployment();

    IFlightTelemetrySource &m_telemetrySource;
    double m_simulationPhase;
    int m_noiseLimit;
    QVariantList m_noiseHistory;
    int m_parachuteState;
    double m_parachuteProgress;
    bool m_parachuteTestOverride;
    QString m_parachuteEventTime;
    bool m_gnssTestMode;
    int m_selectedTraffic;
    QTimer m_simulationTimer;
    QTimer m_parachuteTimer;
};

#endif // OPERATIONALVIEWMODELS_H
