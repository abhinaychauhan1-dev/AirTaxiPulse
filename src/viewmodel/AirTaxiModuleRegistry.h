#ifndef AIRTAXIMODULEREGISTRY_H
#define AIRTAXIMODULEREGISTRY_H

#include <QObject>

#include "../services/FlightSimulationService.h"
#include "FeatureOverviewViewModel.h"
#include "PrimaryFlightViewModel.h"

class AirTaxiModuleRegistry : public QObject
{
    Q_OBJECT

    Q_PROPERTY(PrimaryFlightViewModel* primaryFlight READ primaryFlight CONSTANT)
    Q_PROPERTY(FeatureOverviewViewModel* propulsion READ propulsion CONSTANT)
    Q_PROPERTY(FeatureOverviewViewModel* energy READ energy CONSTANT)
    Q_PROPERTY(FeatureOverviewViewModel* fcs READ fcs CONSTANT)
    Q_PROPERTY(FeatureOverviewViewModel* safety READ safety CONSTANT)

public:
    explicit AirTaxiModuleRegistry(QObject *parent = nullptr);

    PrimaryFlightViewModel *primaryFlight() const;
    FeatureOverviewViewModel *propulsion() const;
    FeatureOverviewViewModel *energy() const;
    FeatureOverviewViewModel *fcs() const;
    FeatureOverviewViewModel *safety() const;

private:
    FlightSimulationService m_flightSimulation;
    PrimaryFlightViewModel m_primaryFlight;
    FeatureOverviewViewModel m_propulsion;
    FeatureOverviewViewModel m_energy;
    FeatureOverviewViewModel m_fcs;
    FeatureOverviewViewModel m_safety;
};

#endif // AIRTAXIMODULEREGISTRY_H