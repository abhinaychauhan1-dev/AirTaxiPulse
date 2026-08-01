#include "AirTaxiModuleRegistry.h"

#include <QPair>

static QList<FeatureOverviewCardData*> makeCards(QObject *parent, const QList<QPair<QString, QString>> &items)
{
    QList<FeatureOverviewCardData*> cards;
    cards.reserve(items.size());
    for (const auto &item : items) {
        cards.append(new FeatureOverviewCardData(item.first, item.second, parent));
    }
    return cards;
}

AirTaxiModuleRegistry::AirTaxiModuleRegistry(QObject *parent)
    : QObject(parent)
    , m_ownedFlightSimulation(std::make_unique<FlightSimulationService>(this))
    , m_telemetrySource(m_ownedFlightSimulation.get())
    , m_telemetryNotifier(m_ownedFlightSimulation.get())
    , m_primaryFlight(QStringLiteral("Primary Flight & Navigation Data"), *m_telemetrySource, this)
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
    QObject::connect(m_telemetryNotifier, SIGNAL(telemetryChanged()),
                     &m_primaryFlight, SIGNAL(telemetryChanged()));
}

AirTaxiModuleRegistry::AirTaxiModuleRegistry(IFlightTelemetrySource &telemetrySource,
                                             QObject &telemetryNotifier,
                                             QObject *parent)
    : QObject(parent)
    , m_ownedFlightSimulation(nullptr)
    , m_telemetrySource(&telemetrySource)
    , m_telemetryNotifier(&telemetryNotifier)
    , m_primaryFlight(QStringLiteral("Primary Flight & Navigation Data"), *m_telemetrySource, this)
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
    QObject::connect(m_telemetryNotifier, SIGNAL(telemetryChanged()),
                     &m_primaryFlight, SIGNAL(telemetryChanged()));
}

PrimaryFlightViewModel *AirTaxiModuleRegistry::primaryFlight() const
{
    return const_cast<PrimaryFlightViewModel *>(&m_primaryFlight);
}

FeatureOverviewViewModel *AirTaxiModuleRegistry::propulsion() const
{
    return const_cast<FeatureOverviewViewModel *>(&m_propulsion);
}

FeatureOverviewViewModel *AirTaxiModuleRegistry::energy() const
{
    return const_cast<FeatureOverviewViewModel *>(&m_energy);
}

FeatureOverviewViewModel *AirTaxiModuleRegistry::fcs() const
{
    return const_cast<FeatureOverviewViewModel *>(&m_fcs);
}

FeatureOverviewViewModel *AirTaxiModuleRegistry::safety() const
{
    return const_cast<FeatureOverviewViewModel *>(&m_safety);
}