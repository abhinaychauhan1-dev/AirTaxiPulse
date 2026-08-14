/**
 * @file    : src/viewmodel/AirTaxiModuleRegistry.cpp
 * @brief   : Implements registration and wiring for application view models.
 * @author  : Abhinay Chauhan (email: abhinay.chauhan1@gmail.com)
 * @version : 1.0.0
 *
 * Copyright (c) 2024
 * Abhinay Chauhan. All rights reserved.
 */

#include "AirTaxiModuleRegistry.h"

#include <QPair>

/// @brief Creates feature cards from title/subtitle pairs and assigns Qt ownership.
/// @param parent QObject that owns the created cards.
/// @param items Card title and subtitle pairs.
/// @return List of newly allocated card objects.
static QList<FeatureOverviewCardData*> makeCards(QObject *parent, const QList<QPair<QString, QString>> &items)
{
    QList<FeatureOverviewCardData*> cards;
    cards.reserve(items.size());
    for (const auto &item : items) {
        cards.append(new FeatureOverviewCardData(item.first, item.second, parent));
    }
    return cards;
}

/// @brief Builds the default module graph around an owned simulation source.
/// @param parent Optional QObject parent for Qt ownership.
AirTaxiModuleRegistry::AirTaxiModuleRegistry(QObject *parent)
    : QObject(parent)
    , m_ownedFlightSimulation(std::make_unique<FlightSimulationService>(this))
    , m_telemetrySource(m_ownedFlightSimulation.get())
    , m_telemetryNotifier(m_ownedFlightSimulation.get())
    , m_primaryFlight(QStringLiteral("Primary Flight & Navigation Data"), *m_telemetrySource, this)
    , m_session(this)
    , m_propulsionSystem(*m_telemetrySource, this)
    , m_energySystem(*m_telemetrySource, this)
    , m_flightControlSystem(*m_telemetrySource, this)
    , m_safetySystem(*m_telemetrySource, this)
    , m_propulsion(
          QStringLiteral("Propulsion & Distributed Electric Propulsion (DEP) Data"),
          QStringLiteral("Monitor propulsion status, motor RPM, distributed electric propulsion health, thrust balance and thermal state for each pod."),
          makeCards(this, {
              {QStringLiteral("Motors"), QStringLiteral("Overview")},
              {QStringLiteral("DEP Pods"), QStringLiteral("Status")},
              {QStringLiteral("Power"), QStringLiteral("Trends")},
              {QStringLiteral("Thermal"), QStringLiteral("Actions")}
          }),
          this)
    , m_energy(
          QStringLiteral("Energy, Power & Battery Subsystems"),
          QStringLiteral("Track battery state-of-charge, energy flow, power demand, charging readiness, and subsystem efficiency metrics."),
          makeCards(this, {
              {QStringLiteral("Batteries"), QStringLiteral("Overview")},
              {QStringLiteral("Power Flow"), QStringLiteral("Status")},
              {QStringLiteral("Efficiency"), QStringLiteral("Trends")},
              {QStringLiteral("Cooling"), QStringLiteral("Actions")}
          }),
          this)
    , m_fcs(
          QStringLiteral("Flight Control System (FCS) & Automation"),
          QStringLiteral("Check flight control system status, automation modes, sensor fusion health and guidance status."),
          makeCards(this, {
              {QStringLiteral("FCS Modes"), QStringLiteral("Overview")},
              {QStringLiteral("Sensors"), QStringLiteral("Status")},
              {QStringLiteral("Automation"), QStringLiteral("Trends")},
              {QStringLiteral("Commands"), QStringLiteral("Actions")}
          }),
          this)
    , m_safety(
          QStringLiteral("Safety, Health & Environmental Monitoring"),
          QStringLiteral("Review safety alerts, environmental sensors, health monitoring, and fault detection readiness."),
          makeCards(this, {
              {QStringLiteral("Alerts"), QStringLiteral("Overview")},
              {QStringLiteral("Health"), QStringLiteral("Status")},
              {QStringLiteral("Enviro"), QStringLiteral("Trends")},
              {QStringLiteral("Compliance"), QStringLiteral("Actions")}
          }),
          this)
{
    // Fan out source updates to each telemetry-backed QML view model.
    QObject::connect(m_telemetryNotifier, SIGNAL(telemetryChanged()),
                     &m_primaryFlight, SIGNAL(telemetryChanged()));
    QObject::connect(m_telemetryNotifier, SIGNAL(telemetryChanged()),
                     &m_propulsionSystem, SIGNAL(telemetryChanged()));
    QObject::connect(m_telemetryNotifier, SIGNAL(telemetryChanged()),
                     &m_energySystem, SIGNAL(telemetryChanged()));
    QObject::connect(m_telemetryNotifier, SIGNAL(telemetryChanged()),
                     &m_flightControlSystem, SIGNAL(telemetryChanged()));
    QObject::connect(m_telemetryNotifier, SIGNAL(telemetryChanged()),
                     &m_flightControlSystem, SIGNAL(stateChanged()));
    QObject::connect(m_telemetryNotifier, SIGNAL(telemetryChanged()),
                     &m_safetySystem, SIGNAL(telemetryChanged()));
    QObject::connect(m_telemetryNotifier, SIGNAL(telemetryChanged()),
                     &m_safetySystem, SIGNAL(stateChanged()));
}

/// @brief Builds the module graph around a caller-provided telemetry source.
/// @param telemetrySource Source that supplies aircraft telemetry values.
/// @param telemetryNotifier QObject that emits the telemetryChanged() signal.
/// @param parent Optional QObject parent for Qt ownership.
AirTaxiModuleRegistry::AirTaxiModuleRegistry(IFlightTelemetrySource &telemetrySource,
                                             QObject &telemetryNotifier,
                                             QObject *parent)
    : QObject(parent)
    , m_ownedFlightSimulation(nullptr)
    , m_telemetrySource(&telemetrySource)
    , m_telemetryNotifier(&telemetryNotifier)
    , m_primaryFlight(QStringLiteral("Primary Flight & Navigation Data"), *m_telemetrySource, this)
    , m_session(this)
    , m_propulsionSystem(*m_telemetrySource, this)
    , m_energySystem(*m_telemetrySource, this)
    , m_flightControlSystem(*m_telemetrySource, this)
    , m_safetySystem(*m_telemetrySource, this)
    , m_propulsion(
          QStringLiteral("Propulsion & Distributed Electric Propulsion (DEP) Data"),
          QStringLiteral("Monitor propulsion status, motor RPM, distributed electric propulsion health, thrust balance and thermal state for each pod."),
          makeCards(this, {
              {QStringLiteral("Motors"), QStringLiteral("Overview")},
              {QStringLiteral("DEP Pods"), QStringLiteral("Status")},
              {QStringLiteral("Power"), QStringLiteral("Trends")},
              {QStringLiteral("Thermal"), QStringLiteral("Actions")}
          }),
          this)
    , m_energy(
          QStringLiteral("Energy, Power & Battery Subsystems"),
          QStringLiteral("Track battery state-of-charge, energy flow, power demand, charging readiness, and subsystem efficiency metrics."),
          makeCards(this, {
              {QStringLiteral("Batteries"), QStringLiteral("Overview")},
              {QStringLiteral("Power Flow"), QStringLiteral("Status")},
              {QStringLiteral("Efficiency"), QStringLiteral("Trends")},
              {QStringLiteral("Cooling"), QStringLiteral("Actions")}
          }),
          this)
    , m_fcs(
          QStringLiteral("Flight Control System (FCS) & Automation"),
          QStringLiteral("Check flight control system status, automation modes, sensor fusion health and guidance status."),
          makeCards(this, {
              {QStringLiteral("FCS Modes"), QStringLiteral("Overview")},
              {QStringLiteral("Sensors"), QStringLiteral("Status")},
              {QStringLiteral("Automation"), QStringLiteral("Trends")},
              {QStringLiteral("Commands"), QStringLiteral("Actions")}
          }),
          this)
    , m_safety(
          QStringLiteral("Safety, Health & Environmental Monitoring"),
          QStringLiteral("Review safety alerts, environmental sensors, health monitoring, and fault detection readiness."),
          makeCards(this, {
              {QStringLiteral("Alerts"), QStringLiteral("Overview")},
              {QStringLiteral("Health"), QStringLiteral("Status")},
              {QStringLiteral("Enviro"), QStringLiteral("Trends")},
              {QStringLiteral("Compliance"), QStringLiteral("Actions")}
          }),
          this)
{
    // Fan out external source updates to each telemetry-backed QML view model.
    QObject::connect(m_telemetryNotifier, SIGNAL(telemetryChanged()),
                     &m_primaryFlight, SIGNAL(telemetryChanged()));
    QObject::connect(m_telemetryNotifier, SIGNAL(telemetryChanged()),
                     &m_propulsionSystem, SIGNAL(telemetryChanged()));
    QObject::connect(m_telemetryNotifier, SIGNAL(telemetryChanged()),
                     &m_energySystem, SIGNAL(telemetryChanged()));
    QObject::connect(m_telemetryNotifier, SIGNAL(telemetryChanged()),
                     &m_flightControlSystem, SIGNAL(telemetryChanged()));
    QObject::connect(m_telemetryNotifier, SIGNAL(telemetryChanged()),
                     &m_flightControlSystem, SIGNAL(stateChanged()));
    QObject::connect(m_telemetryNotifier, SIGNAL(telemetryChanged()),
                     &m_safetySystem, SIGNAL(telemetryChanged()));
    QObject::connect(m_telemetryNotifier, SIGNAL(telemetryChanged()),
                     &m_safetySystem, SIGNAL(stateChanged()));
}

/// @brief Returns the primary flight telemetry view model.
/// @return Registry-owned primary flight view model.
PrimaryFlightViewModel *AirTaxiModuleRegistry::primaryFlight() const
{
    return const_cast<PrimaryFlightViewModel *>(&m_primaryFlight);
}

/// @brief Returns the propulsion feature overview.
/// @return Registry-owned propulsion overview.
FeatureOverviewViewModel *AirTaxiModuleRegistry::propulsion() const
{
    return const_cast<FeatureOverviewViewModel *>(&m_propulsion);
}

/// @brief Returns the energy feature overview.
/// @return Registry-owned energy overview.
FeatureOverviewViewModel *AirTaxiModuleRegistry::energy() const
{
    return const_cast<FeatureOverviewViewModel *>(&m_energy);
}

/// @brief Returns the flight-control-system feature overview.
/// @return Registry-owned flight-control overview.
FeatureOverviewViewModel *AirTaxiModuleRegistry::fcs() const
{
    return const_cast<FeatureOverviewViewModel *>(&m_fcs);
}

/// @brief Returns the safety feature overview.
/// @return Registry-owned safety overview.
FeatureOverviewViewModel *AirTaxiModuleRegistry::safety() const
{
    return const_cast<FeatureOverviewViewModel *>(&m_safety);
}

/// @brief Returns the flight session view model.
/// @return Registry-owned session view model.
FlightSessionViewModel *AirTaxiModuleRegistry::session() const
{
    return const_cast<FlightSessionViewModel *>(&m_session);
}

/// @brief Returns the propulsion subsystem view model.
/// @return Registry-owned propulsion view model.
PropulsionSystemViewModel *AirTaxiModuleRegistry::propulsionSystem() const
{
    return const_cast<PropulsionSystemViewModel *>(&m_propulsionSystem);
}

/// @brief Returns the energy subsystem view model.
/// @return Registry-owned energy view model.
EnergySystemViewModel *AirTaxiModuleRegistry::energySystem() const
{
    return const_cast<EnergySystemViewModel *>(&m_energySystem);
}

/// @brief Returns the flight-control-system view model.
/// @return Registry-owned flight-control view model.
FlightControlSystemViewModel *AirTaxiModuleRegistry::flightControlSystem() const
{
    return const_cast<FlightControlSystemViewModel *>(&m_flightControlSystem);
}

/// @brief Returns the safety subsystem view model.
/// @return Registry-owned safety view model.
SafetySystemViewModel *AirTaxiModuleRegistry::safetySystem() const
{
    return const_cast<SafetySystemViewModel *>(&m_safetySystem);
}