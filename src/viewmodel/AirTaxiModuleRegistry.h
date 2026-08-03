#ifndef AIRTAXIMODULEREGISTRY_H
#define AIRTAXIMODULEREGISTRY_H

#include <QObject>
#include <memory>

#include "../services/FlightSimulationService.h"
#include "FeatureOverviewViewModel.h"
#include "OperationalViewModels.h"
#include "PrimaryFlightViewModel.h"

class AirTaxiModuleRegistry : public QObject
{
    Q_OBJECT

    Q_PROPERTY(PrimaryFlightViewModel* primaryFlight READ primaryFlight CONSTANT)
    Q_PROPERTY(FlightSessionViewModel* session READ session CONSTANT)
    Q_PROPERTY(PropulsionSystemViewModel* propulsionSystem READ propulsionSystem CONSTANT)
    Q_PROPERTY(EnergySystemViewModel* energySystem READ energySystem CONSTANT)
    Q_PROPERTY(FlightControlSystemViewModel* flightControlSystem READ flightControlSystem CONSTANT)
    Q_PROPERTY(SafetySystemViewModel* safetySystem READ safetySystem CONSTANT)
    Q_PROPERTY(FeatureOverviewViewModel* propulsion READ propulsion CONSTANT)
    Q_PROPERTY(FeatureOverviewViewModel* energy READ energy CONSTANT)
    Q_PROPERTY(FeatureOverviewViewModel* fcs READ fcs CONSTANT)
    Q_PROPERTY(FeatureOverviewViewModel* safety READ safety CONSTANT)

public:
    explicit AirTaxiModuleRegistry(QObject *parent = nullptr);
    AirTaxiModuleRegistry(IFlightTelemetrySource &telemetrySource,
                          QObject &telemetryNotifier,
                          QObject *parent = nullptr);

    PrimaryFlightViewModel *primaryFlight() const;
    FlightSessionViewModel *session() const;
    PropulsionSystemViewModel *propulsionSystem() const;
    EnergySystemViewModel *energySystem() const;
    FlightControlSystemViewModel *flightControlSystem() const;
    SafetySystemViewModel *safetySystem() const;
    FeatureOverviewViewModel *propulsion() const;
    FeatureOverviewViewModel *energy() const;
    FeatureOverviewViewModel *fcs() const;
    FeatureOverviewViewModel *safety() const;

private:
    std::unique_ptr<FlightSimulationService> m_ownedFlightSimulation;
    IFlightTelemetrySource *m_telemetrySource;
    QObject *m_telemetryNotifier;
    PrimaryFlightViewModel m_primaryFlight;
    FlightSessionViewModel m_session;
    PropulsionSystemViewModel m_propulsionSystem;
    EnergySystemViewModel m_energySystem;
    FlightControlSystemViewModel m_flightControlSystem;
    SafetySystemViewModel m_safetySystem;
    FeatureOverviewViewModel m_propulsion;
    FeatureOverviewViewModel m_energy;
    FeatureOverviewViewModel m_fcs;
    FeatureOverviewViewModel m_safety;
};

#endif // AIRTAXIMODULEREGISTRY_H