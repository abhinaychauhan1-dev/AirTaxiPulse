/**
 * @file    : src/viewmodel/AirTaxiModuleRegistry.h
 * @brief   : Declares the registry that owns and exposes application view models.
 * @author  : Abhinay Chauhan (email: abhinay.chauhan1@gmail.com)
 * @version : 1.0.0
 *
 * Copyright (c) 2024
 * Abhinay Chauhan. All rights reserved.
 */

#ifndef AIRTAXIMODULEREGISTRY_H
#define AIRTAXIMODULEREGISTRY_H

#include <QObject>
#include <memory>

#include "../services/FlightSimulationService.h"
#include "FeatureOverviewViewModel.h"
#include "OperationalViewModels.h"
#include "PrimaryFlightViewModel.h"

/// @brief Owns the application view models and exposes the module graph to QML.
class AirTaxiModuleRegistry : public QObject
{
    Q_OBJECT

    /// @brief Primary flight telemetry view model.
    Q_PROPERTY(PrimaryFlightViewModel* primaryFlight READ primaryFlight CONSTANT)

    /// @brief Clock and elapsed-flight session view model.
    Q_PROPERTY(FlightSessionViewModel* session READ session CONSTANT)

    /// @brief Propulsion subsystem telemetry view model.
    Q_PROPERTY(PropulsionSystemViewModel* propulsionSystem READ propulsionSystem CONSTANT)

    /// @brief Energy subsystem telemetry view model.
    Q_PROPERTY(EnergySystemViewModel* energySystem READ energySystem CONSTANT)

    /// @brief Flight-control and navigation view model.
    Q_PROPERTY(FlightControlSystemViewModel* flightControlSystem READ flightControlSystem CONSTANT)

    /// @brief Safety and environmental monitoring view model.
    Q_PROPERTY(SafetySystemViewModel* safetySystem READ safetySystem CONSTANT)

    /// @brief Static propulsion feature overview.
    Q_PROPERTY(FeatureOverviewViewModel* propulsion READ propulsion CONSTANT)

    /// @brief Static energy feature overview.
    Q_PROPERTY(FeatureOverviewViewModel* energy READ energy CONSTANT)

    /// @brief Static flight-control-system feature overview.
    Q_PROPERTY(FeatureOverviewViewModel* fcs READ fcs CONSTANT)

    /// @brief Static safety feature overview.
    Q_PROPERTY(FeatureOverviewViewModel* safety READ safety CONSTANT)

public:
    /// @brief Constructs the module graph with an owned flight simulation source.
    /// @param parent Optional QObject parent for Qt ownership.
    explicit AirTaxiModuleRegistry(QObject *parent = nullptr);

    /// @brief Constructs the module graph around an externally owned telemetry source.
    /// @param telemetrySource Source that supplies aircraft telemetry values.
    /// @param telemetryNotifier QObject that emits the telemetryChanged() signal.
    /// @param parent Optional QObject parent for Qt ownership.
    AirTaxiModuleRegistry(IFlightTelemetrySource &telemetrySource,
                          QObject &telemetryNotifier,
                          QObject *parent = nullptr);

    /// @brief Returns the primary flight telemetry view model.
    /// @return Registry-owned primary flight view model.
    PrimaryFlightViewModel *primaryFlight() const;

    /// @brief Returns the flight session view model.
    /// @return Registry-owned session view model.
    FlightSessionViewModel *session() const;

    /// @brief Returns the propulsion subsystem view model.
    /// @return Registry-owned propulsion view model.
    PropulsionSystemViewModel *propulsionSystem() const;

    /// @brief Returns the energy subsystem view model.
    /// @return Registry-owned energy view model.
    EnergySystemViewModel *energySystem() const;

    /// @brief Returns the flight-control-system view model.
    /// @return Registry-owned flight-control view model.
    FlightControlSystemViewModel *flightControlSystem() const;

    /// @brief Returns the safety subsystem view model.
    /// @return Registry-owned safety view model.
    SafetySystemViewModel *safetySystem() const;

    /// @brief Returns the propulsion feature overview.
    /// @return Registry-owned propulsion overview.
    FeatureOverviewViewModel *propulsion() const;

    /// @brief Returns the energy feature overview.
    /// @return Registry-owned energy overview.
    FeatureOverviewViewModel *energy() const;

    /// @brief Returns the flight-control-system feature overview.
    /// @return Registry-owned flight-control overview.
    FeatureOverviewViewModel *fcs() const;

    /// @brief Returns the safety feature overview.
    /// @return Registry-owned safety overview.
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