#include "FlightSimulationService.h"

#include <QtMath>
#include <cmath>

namespace {
double wrappedAngleDeltaDegrees(double angleA, double angleB)
{
    const double wrapped = std::fmod((angleA - angleB) + 540.0, 360.0) - 180.0;
    return qAbs(wrapped);
}

double unitClamp(double value)
{
    return qMax(0.0, qMin(1.0, value));
}

double mapToBand(double normalizedValue, double bandMinimum, double bandMaximum)
{
    const double clamped = unitClamp(normalizedValue);
    return bandMinimum + (bandMaximum - bandMinimum) * clamped;
}
}

FlightSimulationService::FlightSimulationService(QObject *parent)
    : QObject(parent)
    , m_phase(0.0)
    , m_flightModeLabel(QStringLiteral("Pad Hover Hold"))
    , m_batterySoc(92.0)
    , m_motorTemperatures({62.0, 63.5, 61.8, 64.2})
    , m_gpsLatitude(37.7749)
    , m_gpsLongitude(-122.4194)
    , m_telemetry()
{
    appendHistory(m_casHistory, m_telemetry.cas() * 0.96);
    appendHistory(m_casHistory, m_telemetry.cas() * 0.98);
    appendHistory(m_casHistory, m_telemetry.cas());

    appendHistory(m_altHistory, m_telemetry.altBaro() * 0.98);
    appendHistory(m_altHistory, m_telemetry.altBaro() * 0.99);
    appendHistory(m_altHistory, m_telemetry.altBaro());

    appendHistory(m_vsHistory, m_telemetry.vs() * 0.85);
    appendHistory(m_vsHistory, m_telemetry.vs() * 0.95);
    appendHistory(m_vsHistory, m_telemetry.vs());

    appendHistory(m_attitudeHistory, qAbs(m_telemetry.pitch()) + qAbs(m_telemetry.roll()) * 0.9);
    appendHistory(m_attitudeHistory, qAbs(m_telemetry.pitch()) + qAbs(m_telemetry.roll()));
    appendHistory(m_attitudeHistory, qAbs(m_telemetry.pitch()) + qAbs(m_telemetry.roll()) * 1.05);

    appendHistory(m_headingHistory, m_telemetry.heading());
    appendHistory(m_headingHistory, m_telemetry.heading() + 1.2);
    appendHistory(m_headingHistory, m_telemetry.heading() - 0.7);

    const double initialPathError = wrappedAngleDeltaDegrees(m_telemetry.track(), m_telemetry.heading());
    appendHistory(m_fpvHistory, initialPathError * 1.3 + qAbs(m_telemetry.pitch()) * 0.5 + 1.0);
    appendHistory(m_fpvHistory, initialPathError * 1.1 + qAbs(m_telemetry.pitch()) * 0.4 + 0.6);
    appendHistory(m_fpvHistory, initialPathError + qAbs(m_telemetry.pitch()) * 0.35);

    connect(&m_timer, &QTimer::timeout, this, &FlightSimulationService::updateSimulation);
    m_timer.start(500);
}

FlightTelemetryData *FlightSimulationService::telemetry() const
{
    return const_cast<FlightTelemetryData *>(&m_telemetry);
}

double FlightSimulationService::cas() const { return m_telemetry.cas(); }
double FlightSimulationService::tas() const { return m_telemetry.tas(); }
double FlightSimulationService::altBaro() const { return m_telemetry.altBaro(); }
double FlightSimulationService::altRadar() const { return m_telemetry.altRadar(); }
double FlightSimulationService::vs() const { return m_telemetry.vs(); }
double FlightSimulationService::pitch() const { return m_telemetry.pitch(); }
double FlightSimulationService::roll() const { return m_telemetry.roll(); }
double FlightSimulationService::yaw() const { return m_telemetry.yaw(); }
double FlightSimulationService::heading() const { return m_telemetry.heading(); }
double FlightSimulationService::track() const { return m_telemetry.track(); }

double FlightSimulationService::casProgress() const
{
    const double normalizedSpeed = clamp((m_telemetry.cas() - 20.0) / 130.0, 0.0, 1.0);
    return mapToBand(normalizedSpeed, 0.38, 0.78);
}
double FlightSimulationService::altProgress() const
{
    const double blendedAltitude = m_telemetry.altBaro() * 0.82 + m_telemetry.altRadar() * 0.18;
    const double normalizedAltitude = clamp(blendedAltitude / 2200.0, 0.0, 1.0);
    return mapToBand(normalizedAltitude, 0.55, 0.92);
}
double FlightSimulationService::vsProgress() const
{
    const double normalizedVerticalSpeed = clamp(qAbs(m_telemetry.vs()) / 1600.0, 0.0, 1.0);
    return mapToBand(normalizedVerticalSpeed, 0.18, 0.88);
}
double FlightSimulationService::attitudeProgress() const
{
    const double attitudeLoad = qAbs(m_telemetry.pitch()) * 0.7 + qAbs(m_telemetry.roll()) * 0.45;
    const double normalizedStability = clamp(1.0 - attitudeLoad / 18.0, 0.0, 1.0);
    return mapToBand(normalizedStability, 0.32, 0.82);
}
double FlightSimulationService::headingProgress() const
{
    const double headingWave = 0.5 + 0.5 * qSin(qDegreesToRadians(m_telemetry.heading() * 1.8));
    return mapToBand(headingWave, 0.30, 0.70);
}
double FlightSimulationService::fpvProgress() const
{
    const double pathError = wrappedAngleDeltaDegrees(m_telemetry.track(), m_telemetry.heading());
    const double verticalPenalty = qAbs(m_telemetry.vs()) / 2200.0;
    const double normalizedPathQuality = clamp(1.0 - pathError / 18.0 - verticalPenalty * 0.25, 0.0, 1.0);
    return mapToBand(normalizedPathQuality, 0.24, 0.66);
}
QString FlightSimulationService::flightModeLabel() const { return m_flightModeLabel; }
QColor FlightSimulationService::vsAccentColor() const { return m_telemetry.vs() >= 0.0 ? QColor("#79d57a") : QColor("#e07c7c"); }
QString FlightSimulationService::vsTrendLabel() const { return m_telemetry.vs() >= 0.0 ? QStringLiteral("Ascending") : QStringLiteral("Descending"); }
QString FlightSimulationService::vsValueText() const
{
    const QString sign = m_telemetry.vs() >= 0.0 ? QStringLiteral("+") : QStringLiteral("-");
    return sign + QString::number(qAbs(m_telemetry.vs()), 'f', 0) + QStringLiteral(" ft/min");
}
double FlightSimulationService::batterySoc() const { return m_batterySoc; }
QVariantList FlightSimulationService::motorTemperatures() const { return historyToVariantList(m_motorTemperatures); }
double FlightSimulationService::gpsLatitude() const { return m_gpsLatitude; }
double FlightSimulationService::gpsLongitude() const { return m_gpsLongitude; }

QVariantList FlightSimulationService::casHistory() const { return historyToVariantList(m_casHistory); }
QVariantList FlightSimulationService::altHistory() const { return historyToVariantList(m_altHistory); }
QVariantList FlightSimulationService::vsHistory() const { return historyToVariantList(m_vsHistory); }
QVariantList FlightSimulationService::attitudeHistory() const { return historyToVariantList(m_attitudeHistory); }
QVariantList FlightSimulationService::headingHistory() const { return historyToVariantList(m_headingHistory); }
QVariantList FlightSimulationService::fpvHistory() const { return historyToVariantList(m_fpvHistory); }

double FlightSimulationService::clamp(double value, double minimumValue, double maximumValue)
{
    return qMax(minimumValue, qMin(maximumValue, value));
}

void FlightSimulationService::appendHistory(QVector<double> &history, double value)
{
    history.append(value);
    while (history.size() > 24) {
        history.removeFirst();
    }
}

QVariantList FlightSimulationService::historyToVariantList(const QVector<double> &history) const
{
    QVariantList values;
    values.reserve(history.size());
    for (double value : history) {
        values.append(value);
    }
    return values;
}

void FlightSimulationService::updateSimulation()
{
    m_phase += 0.24;

    const double cyclePosition = std::fmod(m_phase, 40.0);
    double targetCas = 0.0;
    double targetTas = 0.0;
    double targetAltBaro = 0.0;
    double targetAltRadar = 0.0;
    double targetVs = 0.0;
    double targetPitch = 0.0;
    double targetRoll = 0.0;
    double targetYaw = 0.0;
    double targetHeading = 0.0;
    double targetTrack = 0.0;

    if (cyclePosition < 8.0) {
        m_flightModeLabel = QStringLiteral("Pad Hover Hold");
        targetCas = 18.0 + 2.0 * qSin(m_phase * 1.5);
        targetTas = targetCas + 2.0 + 0.5 * qCos(m_phase * 0.9);
        targetAltBaro = 24.0 + 4.0 * qSin(m_phase * 0.7);
        targetAltRadar = 18.0 + 3.0 * qCos(m_phase * 1.1);
        targetVs = 30.0 * qSin(m_phase * 1.3);
        targetPitch = 7.0 + 1.5 * qSin(m_phase * 1.4);
        targetRoll = 4.5 * qSin(m_phase * 1.1);
        targetYaw = 2.0 * qSin(m_phase * 0.8);
        targetHeading = 132.0 + 0.8 * qSin(m_phase * 0.5);
        targetTrack = targetHeading + 0.6 * qSin(m_phase * 0.9);
    } else if (cyclePosition < 16.0) {
        m_flightModeLabel = QStringLiteral("Lift-Off Climb");
        const double climbPhase = cyclePosition - 8.0;
        targetCas = 22.0 + climbPhase * 4.5 + 1.5 * qSin(m_phase * 1.1);
        targetTas = targetCas + 5.0 + 1.5 * qCos(m_phase * 0.7);
        targetAltBaro = 24.0 + climbPhase * 140.0 + 8.0 * qSin(m_phase * 0.6);
        targetAltRadar = 18.0 + climbPhase * 18.0 + 6.0 * qCos(m_phase * 1.2);
        targetVs = 780.0 + 120.0 * qSin(m_phase * 0.8);
        targetPitch = 10.0 - 0.4 * climbPhase + 1.2 * qSin(m_phase * 1.0);
        targetRoll = 5.0 * qSin(m_phase * 0.9);
        targetYaw = 3.5 * qSin(m_phase * 0.7);
        targetHeading = 132.0 + 1.8 * qSin(m_phase * 0.4);
        targetTrack = targetHeading + 1.0 * qSin(m_phase * 0.8);
    } else if (cyclePosition < 24.0) {
        m_flightModeLabel = QStringLiteral("Transition to Cruise");
        const double transitionPhase = cyclePosition - 16.0;
        targetCas = 55.0 + transitionPhase * 10.5 + 4.0 * qSin(m_phase * 0.8);
        targetTas = targetCas + 10.0 + 2.0 * qCos(m_phase * 0.6);
        targetAltBaro = 1120.0 + transitionPhase * 70.0 + 10.0 * qSin(m_phase * 0.5);
        targetAltRadar = 120.0 + transitionPhase * 15.0 + 4.0 * qCos(m_phase * 1.0);
        targetVs = 420.0 + 80.0 * qSin(m_phase * 0.9);
        targetPitch = 7.5 - 0.45 * transitionPhase + 1.1 * qSin(m_phase * 1.15);
        targetRoll = 7.0 * qSin(m_phase * 0.95);
        targetYaw = 5.0 * qSin(m_phase * 0.8);
        targetHeading = std::fmod(132.0 + transitionPhase * 1.9 + 360.0, 360.0);
        targetTrack = targetHeading + 2.0 * qSin(m_phase * 0.75) + 0.8 * qSin(m_phase * 1.4);
    } else if (cyclePosition < 33.0) {
        m_flightModeLabel = QStringLiteral("Cruise Corridor");
        targetCas = 128.0 + 8.0 * qSin(m_phase * 0.55) + 3.0 * qCos(m_phase * 1.3);
        targetTas = targetCas + 12.0 + 2.5 * qCos(m_phase * 0.9);
        targetAltBaro = 1850.0 + 35.0 * qSin(m_phase * 0.35);
        targetAltRadar = 360.0 + 20.0 * qCos(m_phase * 0.9);
        targetVs = 45.0 * qSin(m_phase * 1.2);
        targetPitch = 2.5 + 1.0 * qSin(m_phase * 0.7);
        targetRoll = 2.2 * qSin(m_phase * 0.6);
        targetYaw = 1.2 * qSin(m_phase * 0.45);
        targetHeading = std::fmod(148.0 + 4.0 * qSin(m_phase * 0.25) + 360.0, 360.0);
        targetTrack = targetHeading + 1.6 * qSin(m_phase * 0.85);
    } else {
        m_flightModeLabel = QStringLiteral("Arrival Descent");
        const double approachPhase = cyclePosition - 33.0;
        targetCas = 118.0 - approachPhase * 2.8 + 5.0 * qSin(m_phase * 0.9);
        targetTas = targetCas + 10.0 + 2.0 * qCos(m_phase * 0.8);
        targetAltBaro = 1650.0 - approachPhase * 135.0 + 10.0 * qSin(m_phase * 0.5);
        targetAltRadar = 320.0 - approachPhase * 20.0 + 5.0 * qCos(m_phase * 1.0);
        targetVs = -520.0 - 120.0 * qSin(m_phase * 0.85);
        targetPitch = 5.0 + 1.5 * qSin(m_phase * 0.8);
        targetRoll = 6.0 * qSin(m_phase * 0.95);
        targetYaw = 4.5 * qSin(m_phase * 0.7);
        targetHeading = std::fmod(152.0 - approachPhase * 1.5 + 360.0, 360.0);
        targetTrack = targetHeading - 2.0 * qSin(m_phase * 0.7) - 0.6 * qCos(m_phase * 1.4);
    }

    auto approach = [](double current, double target, double maxStep) {
        const double delta = target - current;
        if (delta > maxStep) {
            return current + maxStep;
        }
        if (delta < -maxStep) {
            return current - maxStep;
        }
        return target;
    };

    const double newCas = approach(m_telemetry.cas(), targetCas, 8.0);
    const double newTas = approach(m_telemetry.tas(), targetTas, 8.0);
    const double newAltBaro = approach(m_telemetry.altBaro(), targetAltBaro, 95.0);
    const double newAltRadar = approach(m_telemetry.altRadar(), targetAltRadar, 18.0);
    const double newVs = approach(m_telemetry.vs(), targetVs, 120.0);
    const double newPitch = approach(m_telemetry.pitch(), targetPitch, 0.9);
    const double newRoll = approach(m_telemetry.roll(), targetRoll, 0.9);
    const double newYaw = approach(m_telemetry.yaw(), targetYaw, 0.7);
    const double newHeading = std::fmod(approach(m_telemetry.heading(), targetHeading, 2.5) + 360.0, 360.0);
    const double newTrack = approach(m_telemetry.track(), targetTrack, 3.0);

    m_telemetry.setCas(newCas);
    m_telemetry.setTas(newTas);
    m_telemetry.setAltBaro(newAltBaro);
    m_telemetry.setAltRadar(newAltRadar);
    m_telemetry.setVs(newVs);
    m_telemetry.setPitch(newPitch);
    m_telemetry.setRoll(newRoll);
    m_telemetry.setYaw(newYaw);
    m_telemetry.setHeading(newHeading);
    m_telemetry.setTrack(newTrack);

    m_batterySoc = clamp(m_batterySoc - 0.015, 18.0, 100.0);
    const double thermalWave = 2.6 * qSin(m_phase * 0.52);
    for (int i = 0; i < m_motorTemperatures.size(); ++i) {
        const double phaseOffset = i * 0.4;
        const double targetMotorTemp = 64.0 + thermalWave + 1.8 * qSin(m_phase * 0.73 + phaseOffset);
        m_motorTemperatures[i] = approach(m_motorTemperatures[i], targetMotorTemp, 0.8);
    }
    m_gpsLatitude += 0.000045 * qCos(qDegreesToRadians(newTrack));
    m_gpsLongitude += 0.000045 * qSin(qDegreesToRadians(newTrack));

    appendHistory(m_casHistory, m_telemetry.cas());
    appendHistory(m_altHistory, m_telemetry.altBaro());
    appendHistory(m_vsHistory, m_telemetry.vs());
    appendHistory(m_attitudeHistory, qAbs(m_telemetry.pitch()) + qAbs(m_telemetry.roll()));
    appendHistory(m_headingHistory, m_telemetry.heading());
    const double pathError = wrappedAngleDeltaDegrees(m_telemetry.track(), m_telemetry.heading());
    appendHistory(m_fpvHistory, pathError * 1.2 + qAbs(m_telemetry.pitch()) * 0.35);

    emit telemetryChanged();
}