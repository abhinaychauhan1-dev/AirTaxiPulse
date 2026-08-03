#include "OperationalViewModels.h"

#include <QTime>
#include <QVariantMap>
#include <QtMath>

FlightSessionViewModel::FlightSessionViewModel(QObject *parent)
    : QObject(parent)
    , m_currentDateTime(QDateTime::currentDateTime())
    , m_elapsedSeconds(0)
{
    m_timer.setInterval(1000);
    connect(&m_timer, &QTimer::timeout, this, [this]() {
        m_currentDateTime = QDateTime::currentDateTime();
        ++m_elapsedSeconds;
        emit sessionChanged();
    });
    m_timer.start();
}

QString FlightSessionViewModel::currentDateText() const
{
    return m_currentDateTime.toString(QStringLiteral("ddd, dd MMM yyyy"));
}

QString FlightSessionViewModel::currentTimeText() const
{
    return m_currentDateTime.toString(QStringLiteral("hh:mm:ss"));
}

QString FlightSessionViewModel::elapsedText() const
{
    const int hours = m_elapsedSeconds / 3600;
    const int minutes = (m_elapsedSeconds % 3600) / 60;
    const int seconds = m_elapsedSeconds % 60;
    return QStringLiteral("%1:%2:%3")
        .arg(hours, 2, 10, QLatin1Char('0'))
        .arg(minutes, 2, 10, QLatin1Char('0'))
        .arg(seconds, 2, 10, QLatin1Char('0'));
}

PropulsionSystemViewModel::PropulsionSystemViewModel(IFlightTelemetrySource &telemetrySource,
                                                     QObject *parent)
    : QObject(parent)
    , m_telemetrySource(telemetrySource)
{
}

double PropulsionSystemViewModel::average(const QVariantList &values)
{
    if (values.isEmpty()) {
        return 0.0;
    }
    double total = 0.0;
    for (const QVariant &value : values) {
        total += value.toDouble();
    }
    return total / values.size();
}

double PropulsionSystemViewModel::valueAt(const QVariantList &values, int index)
{
    return index >= 0 && index < values.size() ? values.at(index).toDouble() : 0.0;
}

double PropulsionSystemViewModel::frontGroupRpm() const
{
    const QVariantList values = m_telemetrySource.motorRpmValues();
    return (valueAt(values, 0) + valueAt(values, 1)) / 2.0;
}

double PropulsionSystemViewModel::rearGroupRpm() const
{
    const QVariantList values = m_telemetrySource.motorRpmValues();
    return (valueAt(values, 2) + valueAt(values, 3)) / 2.0;
}

double PropulsionSystemViewModel::averageRpm() const
{
    return average(m_telemetrySource.motorRpmValues());
}

double PropulsionSystemViewModel::averageMotorTemperature() const
{
    return average(m_telemetrySource.motorTemperatures());
}

double PropulsionSystemViewModel::inverterHealthAverage() const
{
    return average(m_telemetrySource.inverterHealth());
}

double PropulsionSystemViewModel::frontThrust() const
{
    return valueAt(m_telemetrySource.thrustOutputs(), 0);
}

double PropulsionSystemViewModel::rearThrust() const
{
    return valueAt(m_telemetrySource.thrustOutputs(), 1);
}

double PropulsionSystemViewModel::combinedThrust() const
{
    return frontThrust() + rearThrust();
}

double PropulsionSystemViewModel::thrustBalance() const
{
    return combinedThrust() > 0.0 ? (frontThrust() - rearThrust()) / combinedThrust() : 0.0;
}

double PropulsionSystemViewModel::electricalPowerKw() const
{
    const QVariantList voltages = m_telemetrySource.inverterVoltages();
    const QVariantList currents = m_telemetrySource.inverterCurrents();
    const int count = qMin(voltages.size(), currents.size());
    double total = 0.0;
    for (int index = 0; index < count; ++index) {
        total += voltages.at(index).toDouble() * currents.at(index).toDouble() / 1000.0;
    }
    return total;
}

QString PropulsionSystemViewModel::stateLabel() const
{
    if (averageMotorTemperature() >= 85.0) {
        return QStringLiteral("THERMAL LIMIT");
    }
    if (inverterHealthAverage() < 90.0) {
        return QStringLiteral("DEGRADED");
    }
    return QStringLiteral("ALL CHANNELS NOMINAL");
}

double PropulsionSystemViewModel::rpmProgress() const
{
    return qBound(0.0, averageRpm() / 2800.0, 1.0);
}

double PropulsionSystemViewModel::tiltProgress() const
{
    return 1.0 - qBound(0.0, m_telemetrySource.tiltAngleDeg() / 90.0, 1.0);
}

double PropulsionSystemViewModel::temperatureProgress() const
{
    return qBound(0.0, averageMotorTemperature() / 95.0, 1.0);
}

double PropulsionSystemViewModel::thrustProgress() const
{
    return qBound(0.0, combinedThrust() / 28.0, 1.0);
}

double PropulsionSystemViewModel::inverterProgress() const
{
    return qBound(0.0, inverterHealthAverage() / 100.0, 1.0);
}

QString PropulsionSystemViewModel::tiltProfileLabel() const
{
    return m_telemetrySource.tiltAngleDeg() > 45.0
        ? QStringLiteral("Hover-biased") : QStringLiteral("Cruise-biased");
}

bool PropulsionSystemViewModel::thrustBalanced() const
{
    return qAbs(thrustBalance()) < 0.03;
}

QString PropulsionSystemViewModel::thrustBalanceLabel() const
{
    if (thrustBalanced()) {
        return QStringLiteral("CENTERED");
    }
    return thrustBalance() > 0.0 ? QStringLiteral("FRONT BIAS") : QStringLiteral("REAR BIAS");
}

QVariantList PropulsionSystemViewModel::motorThermalWarnings() const
{
    QVariantList warnings;
    const QVariantList temperatures = m_telemetrySource.motorTemperatures();
    for (const QVariant &temperature : temperatures) warnings.append(temperature.toDouble() >= 85.0);
    return warnings;
}

QVariantList PropulsionSystemViewModel::inverterHealthWarnings() const
{
    QVariantList warnings;
    const QVariantList healthValues = m_telemetrySource.inverterHealth();
    for (const QVariant &health : healthValues) warnings.append(health.toDouble() < 90.0);
    return warnings;
}

EnergySystemViewModel::EnergySystemViewModel(IFlightTelemetrySource &telemetrySource,
                                             QObject *parent)
    : QObject(parent)
    , m_telemetrySource(telemetrySource)
{
}

bool EnergySystemViewModel::hoverMode() const
{
    const QString mode = m_telemetrySource.flightModeLabel();
    return mode.contains(QStringLiteral("Hover")) || mode.contains(QStringLiteral("Lift-Off"));
}

double EnergySystemViewModel::estimatedMinutes() const
{
    const double powerKw = m_telemetrySource.powerConsumptionKw();
    return powerKw > 1.0 ? m_telemetrySource.batterySoc() * 4.2 / powerKw * 60.0 : 0.0;
}

double EnergySystemViewModel::cellTemperatureMax() const
{
    const QVariantList temperatures = m_telemetrySource.batteryCellTemperatures();
    if (temperatures.isEmpty()) {
        return 0.0;
    }
    double maximum = temperatures.first().toDouble();
    for (const QVariant &temperature : temperatures) {
        maximum = qMax(maximum, temperature.toDouble());
    }
    return maximum;
}

int EnergySystemViewModel::status() const
{
    if (cellTemperatureMax() >= 60.0) {
        return ThermalWarning;
    }
    if (m_telemetrySource.batterySoc() < 25.0) {
        return LowReserve;
    }
    return Nominal;
}

QString EnergySystemViewModel::statusLabel() const
{
    switch (status()) {
    case ThermalWarning:
        return QStringLiteral("THERMAL WARNING");
    case LowReserve:
        return QStringLiteral("LOW RESERVE");
    default:
        return QStringLiteral("BATTERY NOMINAL");
    }
}

QString EnergySystemViewModel::powerProfileLabel() const
{
    return hoverMode() ? QStringLiteral("HOVER POWER PROFILE") : QStringLiteral("CRUISE POWER PROFILE");
}

double EnergySystemViewModel::efficiencyPercent() const
{
    return qMax(72.0, 96.0 - m_telemetrySource.powerConsumptionKw() / 40.0);
}

double EnergySystemViewModel::zoneTemperature(int index) const
{
    const QVariantList temperatures = m_telemetrySource.batteryCellTemperatures();
    return index >= 0 && index < temperatures.size() ? temperatures.at(index).toDouble() : 0.0;
}

QString EnergySystemViewModel::busChannelName(int index) const
{
    static const QStringList names = {
        QStringLiteral("LIFT A"), QStringLiteral("LIFT B"), QStringLiteral("AVIONICS"),
        QStringLiteral("THERMAL"), QStringLiteral("RESERVE")
    };
    return index >= 0 && index < names.size() ? names.at(index) : QString();
}

QString EnergySystemViewModel::busChannelCode(int index) const
{
    static const QStringList codes = {
        QStringLiteral("LA"), QStringLiteral("LB"), QStringLiteral("AV"),
        QStringLiteral("TH"), QStringLiteral("RS")
    };
    return index >= 0 && index < codes.size() ? codes.at(index) : QString();
}

double EnergySystemViewModel::busChannelShare(int index) const
{
    static const double shares[] = {0.28, 0.28, 0.12, 0.20, 0.12};
    return index >= 0 && index < 5 ? shares[index] : 0.0;
}

double EnergySystemViewModel::busChannelCurrent(int index) const
{
    return m_telemetrySource.busCurrent() * busChannelShare(index);
}

double EnergySystemViewModel::busChannelPower(int index) const
{
    return m_telemetrySource.busVoltage() * busChannelCurrent(index) / 1000.0;
}

QVariantList EnergySystemViewModel::zoneTemperatures() const
{
    QVariantList values;
    for (int index = 0; index < 6; ++index) values.append(zoneTemperature(index));
    return values;
}

QVariantList EnergySystemViewModel::busChannelCurrents() const
{
    QVariantList values;
    for (int index = 0; index < 5; ++index) values.append(busChannelCurrent(index));
    return values;
}

QVariantList EnergySystemViewModel::busChannelPowers() const
{
    QVariantList values;
    for (int index = 0; index < 5; ++index) values.append(busChannelPower(index));
    return values;
}

FlightControlSystemViewModel::FlightControlSystemViewModel(IFlightTelemetrySource &telemetrySource,
                                                           QObject *parent)
    : QObject(parent)
    , m_telemetrySource(telemetrySource)
    , m_autopilotEngaged(true)
    , m_guidanceMode(0)
    , m_commandedFlightMode(-1)
    , m_automaticRouteSequence(true)
    , m_selectedWaypoint(1)
    , m_manualLeftElevon(0.0)
    , m_manualRightElevon(0.0)
    , m_manualRuddervator(0.0)
    , m_manualTiltActuator(45.0)
{
    const auto waypoint = [](const QString &code, const QString &name, double latitude, double longitude) {
        return QVariantMap{{QStringLiteral("code"), code}, {QStringLiteral("name"), name},
                           {QStringLiteral("latitude"), latitude}, {QStringLiteral("longitude"), longitude}};
    };
    m_waypoints = {
        waypoint(QStringLiteral("PAD A"), QStringLiteral("Downtown Vertiport"), 37.7749, -122.4194),
        waypoint(QStringLiteral("TRN-1"), QStringLiteral("Transition Gate"), 37.7792, -122.4124),
        waypoint(QStringLiteral("CRZ-2"), QStringLiteral("Bay Corridor"), 37.7868, -122.4012),
        waypoint(QStringLiteral("APP-1"), QStringLiteral("Approach Fix"), 37.7935, -122.3928),
        waypoint(QStringLiteral("PAD B"), QStringLiteral("Harbor Vertiport"), 37.7978, -122.3860)
    };
}

double FlightControlSystemViewModel::clamp(double value, double minimum, double maximum)
{
    return qBound(minimum, value, maximum);
}

bool FlightControlSystemViewModel::autopilotEngaged() const { return m_autopilotEngaged; }
int FlightControlSystemViewModel::guidanceMode() const { return m_guidanceMode; }
int FlightControlSystemViewModel::commandedFlightMode() const { return m_commandedFlightMode; }

QString FlightControlSystemViewModel::flightMode() const
{
    const QString sourceMode = m_telemetrySource.flightModeLabel();
    if (sourceMode.contains(QStringLiteral("Hover")) || sourceMode.contains(QStringLiteral("Lift-Off")))
        return QStringLiteral("HOVER");
    if (sourceMode.contains(QStringLiteral("Transition")))
        return QStringLiteral("TRANSITION");
    if (sourceMode.contains(QStringLiteral("Cruise")))
        return QStringLiteral("CRUISE");
    return QStringLiteral("VERTICAL LANDING");
}

int FlightControlSystemViewModel::telemetryFlightModeIndex() const
{
    const QString mode = flightMode();
    return mode == QStringLiteral("HOVER") ? 0 : mode == QStringLiteral("TRANSITION") ? 1
           : mode == QStringLiteral("CRUISE") ? 2 : 3;
}

int FlightControlSystemViewModel::flightModeIndex() const
{
    return m_commandedFlightMode >= 0 ? m_commandedFlightMode : telemetryFlightModeIndex();
}

double FlightControlSystemViewModel::leftElevon() const
{
    return m_autopilotEngaged
        ? clamp(m_telemetrySource.pitch() * 0.55 - m_telemetrySource.roll() * 0.75, -18.0, 18.0)
        : m_manualLeftElevon;
}

double FlightControlSystemViewModel::rightElevon() const
{
    return m_autopilotEngaged
        ? clamp(m_telemetrySource.pitch() * 0.55 + m_telemetrySource.roll() * 0.75, -18.0, 18.0)
        : m_manualRightElevon;
}

double FlightControlSystemViewModel::ruddervator() const
{
    return m_autopilotEngaged ? clamp(m_telemetrySource.yaw() * 0.8, -20.0, 20.0) : m_manualRuddervator;
}

double FlightControlSystemViewModel::tiltActuator() const
{
    return m_autopilotEngaged ? m_telemetrySource.tiltAngleDeg() : m_manualTiltActuator;
}

QVariantList FlightControlSystemViewModel::actuatorValues() const
{
    return {leftElevon(), rightElevon(), ruddervator(), tiltActuator()};
}

QVariantList FlightControlSystemViewModel::waypoints() const { return m_waypoints; }
bool FlightControlSystemViewModel::automaticRouteSequence() const { return m_automaticRouteSequence; }
int FlightControlSystemViewModel::automaticWaypoint() const { return qMin(m_waypoints.size() - 1, flightModeIndex() + 1); }
int FlightControlSystemViewModel::navigationWaypoint() const { return m_automaticRouteSequence ? automaticWaypoint() : m_selectedWaypoint; }

double FlightControlSystemViewModel::distanceToWaypoint(int index) const
{
    if (index < 0 || index >= m_waypoints.size())
        return 0.0;
    const QVariantMap point = m_waypoints.at(index).toMap();
    const double radians = M_PI / 180.0;
    const double latitude = m_telemetrySource.gpsLatitude();
    const double longitude = m_telemetrySource.gpsLongitude();
    const double targetLatitude = point.value(QStringLiteral("latitude")).toDouble();
    const double targetLongitude = point.value(QStringLiteral("longitude")).toDouble();
    const double latitudeDelta = (targetLatitude - latitude) * radians;
    const double longitudeDelta = (targetLongitude - longitude) * radians;
    double haversine = qPow(qSin(latitudeDelta / 2.0), 2)
        + qCos(latitude * radians) * qCos(targetLatitude * radians) * qPow(qSin(longitudeDelta / 2.0), 2);
    haversine = clamp(haversine, 0.0, 1.0);
    return 3440.065 * 2.0 * qAtan2(qSqrt(haversine), qSqrt(1.0 - haversine));
}

double FlightControlSystemViewModel::nextWaypointDistance() const { return distanceToWaypoint(navigationWaypoint()); }
QString FlightControlSystemViewModel::routeLegStatus() const { return m_automaticRouteSequence ? QStringLiteral("AUTO SEQUENCE") : QStringLiteral("DIRECT TO"); }

QString FlightControlSystemViewModel::modeName(int index) const
{
    static const QStringList values{QStringLiteral("HOVER"), QStringLiteral("TRANSITION"), QStringLiteral("CRUISE"), QStringLiteral("VERTICAL LANDING")};
    return index >= 0 && index < values.size() ? values.at(index) : QString();
}

QString FlightControlSystemViewModel::guidanceName(int index) const
{
    static const QStringList values{QStringLiteral("ROUTE"), QStringLiteral("HEADING"), QStringLiteral("ATTITUDE")};
    return index >= 0 && index < values.size() ? values.at(index) : QString();
}

QString FlightControlSystemViewModel::actuatorName(int index) const
{
    static const QStringList values{QStringLiteral("LEFT ELEVON"), QStringLiteral("RIGHT ELEVON"), QStringLiteral("RUDDERVATOR"), QStringLiteral("TILT ACTUATOR")};
    return index >= 0 && index < values.size() ? values.at(index) : QString();
}

double FlightControlSystemViewModel::actuatorValue(int index) const
{
    switch (index) { case 0: return leftElevon(); case 1: return rightElevon(); case 2: return ruddervator(); default: return tiltActuator(); }
}

double FlightControlSystemViewModel::actuatorMinimum(int index) const { return index == 3 ? 0.0 : index == 2 ? -20.0 : -18.0; }
double FlightControlSystemViewModel::actuatorMaximum(int index) const { return index == 3 ? 90.0 : index == 2 ? 20.0 : 18.0; }

void FlightControlSystemViewModel::clearCommandedFlightMode() { m_commandedFlightMode = -1; emit stateChanged(); }
void FlightControlSystemViewModel::setCommandedFlightMode(int index) { m_commandedFlightMode = qBound(0, index, 3); emit stateChanged(); }
void FlightControlSystemViewModel::setGuidanceMode(int index) { m_guidanceMode = qBound(0, index, 2); emit stateChanged(); }

void FlightControlSystemViewModel::toggleAutopilot()
{
    if (m_autopilotEngaged) {
        m_manualLeftElevon = leftElevon();
        m_manualRightElevon = rightElevon();
        m_manualRuddervator = ruddervator();
        m_manualTiltActuator = tiltActuator();
    }
    m_autopilotEngaged = !m_autopilotEngaged;
    emit stateChanged();
}

void FlightControlSystemViewModel::setManualActuator(int index, double value)
{
    const double bounded = clamp(value, actuatorMinimum(index), actuatorMaximum(index));
    switch (index) { case 0: m_manualLeftElevon = bounded; break; case 1: m_manualRightElevon = bounded; break;
    case 2: m_manualRuddervator = bounded; break; default: m_manualTiltActuator = bounded; break; }
    emit stateChanged();
}

void FlightControlSystemViewModel::selectDirectTo(int index)
{
    if (index < 0 || index >= m_waypoints.size()) return;
    m_selectedWaypoint = index;
    m_automaticRouteSequence = false;
    m_guidanceMode = 0;
    emit stateChanged();
}

void FlightControlSystemViewModel::resumeAutomaticRoute()
{
    m_automaticRouteSequence = true;
    m_selectedWaypoint = automaticWaypoint();
    m_guidanceMode = 0;
    emit stateChanged();
}

SafetySystemViewModel::SafetySystemViewModel(IFlightTelemetrySource &telemetrySource, QObject *parent)
    : QObject(parent), m_telemetrySource(telemetrySource), m_simulationPhase(0.0), m_noiseLimit(75),
      m_noiseHistory({63, 65, 64, 67, 69, 68, 70, 72, 71, 69, 68, 70}), m_parachuteState(0),
      m_parachuteProgress(0.0), m_parachuteTestOverride(false), m_parachuteEventTime(QStringLiteral("--:--:--")),
    m_gnssTestMode(false), m_selectedTraffic(0)
{
    m_simulationTimer.setInterval(500);
    connect(&m_simulationTimer, &QTimer::timeout, this, &SafetySystemViewModel::updateSimulation);
    m_simulationTimer.start();
    m_parachuteTimer.setInterval(80);
    connect(&m_parachuteTimer, &QTimer::timeout, this, &SafetySystemViewModel::updateParachuteDeployment);
}

double SafetySystemViewModel::clamp(double value, double minimum, double maximum) { return qBound(minimum, value, maximum); }
double SafetySystemViewModel::average(const QVariantList &values) { if (values.isEmpty()) return 0.0; double total = 0.0; for (const QVariant &value : values) total += value.toDouble(); return total / values.size(); }
int SafetySystemViewModel::noiseLimit() const { return m_noiseLimit; }
QVariantList SafetySystemViewModel::noiseHistory() const { return m_noiseHistory; }
double SafetySystemViewModel::acousticDb() const { return clamp(54.0 + average(m_telemetrySource.motorRpmValues()) / 145.0 + m_telemetrySource.cas() / 22.0 + 2.5 * qSin(m_simulationPhase * 1.4), 48.0, 96.0); }
bool SafetySystemViewModel::noiseWarning() const { return acousticDb() >= m_noiseLimit; }
int SafetySystemViewModel::satelliteCount() const { return m_gnssTestMode ? 7 : qRound(13.0 + 2.0 * qSin(m_simulationPhase * 0.35)); }
double SafetySystemViewModel::signalStrength() const { return m_gnssTestMode ? 46.0 : 88.0 + 5.0 * qSin(m_simulationPhase * 0.55); }
double SafetySystemViewModel::navigationAccuracy() const { return m_gnssTestMode ? 4.8 : 0.7 + 0.25 * qAbs(qSin(m_simulationPhase * 0.72)); }
bool SafetySystemViewModel::raimAvailable() const { return satelliteCount() >= 8 && navigationAccuracy() < 2.0; }
double SafetySystemViewModel::lateralDeviation() const { return clamp((m_telemetrySource.track() - m_telemetrySource.heading()) * 0.45 + 0.25 * qSin(m_simulationPhase), -2.5, 2.5); }
double SafetySystemViewModel::verticalDeviation() const { return clamp(m_telemetrySource.vs() / 900.0 + 0.2 * qCos(m_simulationPhase * 0.8), -2.5, 2.5); }
double SafetySystemViewModel::alignmentQuality() const { return clamp(100.0 - qAbs(lateralDeviation()) * 17.0 - qAbs(verticalDeviation()) * 12.0, 0.0, 100.0); }
bool SafetySystemViewModel::landingPhase() const { return m_telemetrySource.flightModeLabel().contains(QStringLiteral("Arrival")) || m_telemetrySource.altRadar() < 150.0; }
int SafetySystemViewModel::parachuteState() const { return m_parachuteState; }
double SafetySystemViewModel::parachuteProgress() const { return m_parachuteProgress; }
bool SafetySystemViewModel::parachuteTestOverride() const { return m_parachuteTestOverride; }
QString SafetySystemViewModel::parachuteEventTime() const { return m_parachuteEventTime; }
bool SafetySystemViewModel::parachuteEnvelopeValid() const { return m_telemetrySource.altRadar() >= 100.0 && m_telemetrySource.cas() >= 15.0 && m_telemetrySource.cas() <= 150.0; }
bool SafetySystemViewModel::parachuteDeployAllowed() const { return m_parachuteState == 1 && (parachuteEnvelopeValid() || m_parachuteTestOverride); }
QString SafetySystemViewModel::parachuteInhibitReason() const { if (m_telemetrySource.altRadar() < 100.0) return QStringLiteral("INHIBIT: RADAR ALT < 100 FT"); if (m_telemetrySource.cas() < 15.0) return QStringLiteral("INHIBIT: AIRSPEED < 15 KT"); if (m_telemetrySource.cas() > 150.0) return QStringLiteral("INHIBIT: AIRSPEED > 150 KT"); return QStringLiteral("DEPLOYMENT ENVELOPE VALID"); }
bool SafetySystemViewModel::gnssTestMode() const { return m_gnssTestMode; }
int SafetySystemViewModel::selectedTraffic() const { return m_selectedTraffic; }
QVariantList SafetySystemViewModel::trafficRanges() const { return {trafficRange(0), trafficRange(1), trafficRange(2)}; }
QVariantList SafetySystemViewModel::trafficBearings() const { return {trafficBearing(0), trafficBearing(1), trafficBearing(2)}; }
QVariantList SafetySystemViewModel::monitorSummaries() const { QVariantList values; for (int index = 0; index < 5; ++index) values.append(monitorSummary(index, m_selectedTraffic)); return values; }
int SafetySystemViewModel::alertCount() const { return (noiseWarning() ? 1 : 0) + (!raimAvailable() ? 1 : 0) + (trafficRange(m_selectedTraffic) < 1.0 ? 1 : 0); }
void SafetySystemViewModel::setNoiseLimit(int value) { m_noiseLimit = qBound(40, value, 100); emit stateChanged(); }
void SafetySystemViewModel::toggleGnssTestMode() { m_gnssTestMode = !m_gnssTestMode; emit stateChanged(); }
void SafetySystemViewModel::toggleParachuteTestOverride() { if (m_parachuteState < 2) { m_parachuteTestOverride = !m_parachuteTestOverride; emit stateChanged(); } }
void SafetySystemViewModel::setSelectedTraffic(int index) { m_selectedTraffic = qBound(0, index, 2); emit stateChanged(); }

void SafetySystemViewModel::toggleParachuteArm()
{
    if (m_parachuteState == 0) m_parachuteState = 1; else if (m_parachuteState == 1) m_parachuteState = 0; else return;
    m_parachuteEventTime = QTime::currentTime().toString(QStringLiteral("hh:mm:ss")); emit stateChanged();
}

void SafetySystemViewModel::deployOrResetParachute()
{
    if (m_parachuteState == 3) { m_parachuteState = 0; m_parachuteProgress = 0.0; m_parachuteTestOverride = false; }
    else if (parachuteDeployAllowed()) { m_parachuteProgress = 0.0; m_parachuteState = 2; m_parachuteTimer.start(); }
    else return;
    m_parachuteEventTime = QTime::currentTime().toString(QStringLiteral("hh:mm:ss")); emit stateChanged();
}

QString SafetySystemViewModel::monitorCode(int index) const { static const QStringList values{QStringLiteral("NOISE"), QStringLiteral("BRS"), QStringLiteral("TCAS"), QStringLiteral("GNSS"), QStringLiteral("LANDING")}; return index >= 0 && index < values.size() ? values.at(index) : QString(); }
QString SafetySystemViewModel::signedValue(double value) const { return QStringLiteral("%1%2").arg(value >= 0.0 ? QStringLiteral("+") : QString()).arg(value, 0, 'f', 1); }
QString SafetySystemViewModel::trafficCode(int index) const { static const QStringList values{QStringLiteral("TFC-21"), QStringLiteral("TFC-07"), QStringLiteral("OBS-14")}; return index >= 0 && index < values.size() ? values.at(index) : QString(); }
double SafetySystemViewModel::trafficRange(int index) const { static const double ranges[]{1.35, 2.8, 0.72}; return index >= 0 && index < 3 ? qMax(0.18, ranges[index] + 0.18 * qSin(m_simulationPhase * (0.7 + index * 0.16) + index)) : 0.0; }
double SafetySystemViewModel::trafficBearing(int index) const { static const double bearings[]{38.0, 218.0, 312.0}; return index >= 0 && index < 3 ? bearings[index] + 8.0 * qSin(m_simulationPhase * 0.5 + index) : 0.0; }
QString SafetySystemViewModel::monitorSummary(int index, int selectedTraffic) const
{
    if (index == 0) return QStringLiteral("%1 dBA  |  LIMIT %2 dBA").arg(acousticDb(), 0, 'f', 1).arg(m_noiseLimit);
    if (index == 1) return m_parachuteState == 0 ? QStringLiteral("SYSTEM SAFE") : m_parachuteState == 1 ? QStringLiteral("ARMED / READY") : m_parachuteState == 2 ? QStringLiteral("DEPLOYING %1%").arg(qRound(m_parachuteProgress * 100.0)) : QStringLiteral("DEPLOYED");
    if (index == 2) return QStringLiteral("%1  |  %2 NM").arg(trafficCode(selectedTraffic)).arg(trafficRange(selectedTraffic), 0, 'f', 1);
    if (index == 3) return QStringLiteral("%1 SAT  |  HPL %2 m  |  RAIM %3").arg(satelliteCount()).arg(navigationAccuracy(), 0, 'f', 1).arg(raimAvailable() ? QStringLiteral("PASS") : QStringLiteral("FAIL"));
    return QStringLiteral("ALIGN %1%  |  LAT %2 m").arg(alignmentQuality(), 0, 'f', 0).arg(signedValue(lateralDeviation()));
}

void SafetySystemViewModel::updateSimulation()
{
    m_simulationPhase += 0.18;
    m_noiseHistory.append(acousticDb());
    while (m_noiseHistory.size() > 28) m_noiseHistory.removeFirst();
    emit stateChanged();
}

void SafetySystemViewModel::updateParachuteDeployment()
{
    if (m_parachuteState != 2) { m_parachuteTimer.stop(); return; }
    m_parachuteProgress = qMin(1.0, m_parachuteProgress + 0.035);
    if (m_parachuteProgress >= 1.0) { m_parachuteState = 3; m_parachuteTimer.stop(); m_parachuteEventTime = QTime::currentTime().toString(QStringLiteral("hh:mm:ss")); }
    emit stateChanged();
}
