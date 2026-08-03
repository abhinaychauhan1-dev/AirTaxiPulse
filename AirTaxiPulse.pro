QT += quick quickcontrols2
CONFIG += c++17

SOURCES += \
    src/app/main.cpp \
    src/data/FeatureOverviewCardData.cpp \
    src/services/FlightSimulationService.cpp \
    src/viewmodel/AirTaxiModuleRegistry.cpp \
    src/viewmodel/FeatureOverviewViewModel.cpp \
    src/viewmodel/OperationalViewModels.cpp \
    src/viewmodel/PrimaryFlightViewModel.cpp \
    src/model/FlightTelemetryData.cpp
HEADERS += \
    src/data/FeatureOverviewCardData.h \
    src/services/FlightSimulationService.h \
    src/viewmodel/AirTaxiModuleRegistry.h \
    src/viewmodel/FeatureOverviewViewModel.h \
    src/viewmodel/OperationalViewModels.h \
    src/viewmodel/PrimaryFlightViewModel.h \
    src/model/FlightTelemetryData.h

contains(QT_CONFIG, mqtt) {
    QT += mqtt concurrent
    DEFINES += AIRTAXI_HAS_QT_MQTT
    SOURCES += \
        src/services/MqttFlightTelemetryService.cpp \
        src/services/MqttTelemetryClient.cpp
    HEADERS += \
        src/services/MqttFlightTelemetryService.h \
        src/services/MqttTelemetryClient.h
} else {
    message("Qt MQTT module not found. Building without MQTT telemetry support.")
}
QML_SOURCES += \
    src/view/main.qml \
    src/view/features/FlightSimulationCard.qml \
    src/view/features/PrimaryFlightPage.qml \
    src/view/features/PrimaryFlightInstrumentsPage.qml \
    src/view/features/PropulsionPage.qml \
    src/view/features/DepPropulsionPage.qml \
    src/view/features/EnergyPage.qml \
    src/view/features/EnergySystemsPage.qml \
    src/view/features/FcsPage.qml \
    src/view/features/SafetyPage.qml

QML_FILES += $$QML_SOURCES
OTHER_FILES += $$QML_SOURCES

TARGET = AirTaxiPulse
TEMPLATE = app

# Use this if Qt installation has separate modules
# QT += quickcontrols2
