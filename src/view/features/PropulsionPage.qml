/**
 * @file    : src/view/features/PropulsionPage.qml
 * @brief   : Defines the propulsion system overview page.
 * @author  : Abhinay Chauhan (email: abhinay.chauhan1@gmail.com)
 * @version : 1.0.0
 *
 * Copyright (c) 2024
 * Abhinay Chauhan. All rights reserved.
 */

pragma ComponentBehavior: Bound

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

// This overview page summarizes distributed propulsion performance and health.
Item {
    id: root
    required property var moduleRegistry
    readonly property var flightModel: moduleRegistry.primaryFlight
    readonly property var propulsionModel: moduleRegistry.propulsionSystem
    readonly property var rpmValues: flightModel.motorRpmValues || []
    readonly property var tempValues: flightModel.motorTemperatures || []
    readonly property var thrustValues: flightModel.thrustOutputs || []
    readonly property var inverterVoltageValues: flightModel.inverterVoltages || []
    readonly property var inverterCurrentValues: flightModel.inverterCurrents || []
    readonly property var inverterHealthValues: flightModel.inverterHealth || []
    property int cardColumns: width > 700 ? 2 : 1
    property real rotorPhase: 0.0

    readonly property real frontGroupRpm: root.propulsionModel.frontGroupRpm
    readonly property real rearGroupRpm: root.propulsionModel.rearGroupRpm
    readonly property real averageMotorTemp: root.propulsionModel.averageMotorTemperature
    readonly property real combinedThrust: root.propulsionModel.combinedThrust
    readonly property real inverterHealthAverage: root.propulsionModel.inverterHealthAverage
    readonly property real averageRpm: root.propulsionModel.averageRpm
    readonly property string propulsionState: root.propulsionModel.stateLabel

    // The animation timer advances the rotor visualization while the page is visible.
    Timer {
        interval: 40
        running: root.visible
        repeat: true
        onTriggered: {
            root.rotorPhase = (root.rotorPhase + 0.04 + root.averageRpm / 42000.0) % (Math.PI * 2)
        }
    }

    implicitHeight: propulsionContent.implicitHeight + 16

    // The page content combines propulsion status, the powertrain summary, and metric cards.
    ColumnLayout {
        id: propulsionContent
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 6
        spacing: 4

        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Text {
                text: moduleRegistry.propulsion.title
                font.pixelSize: 18
                font.bold: true
                color: "#ffffff"
                wrapMode: Text.WordWrap
            }

            Item { Layout.fillWidth: true }

            Label {
                text: root.propulsionState
                color: root.propulsionState === "ALL CHANNELS NOMINAL" ? "#7ee082" : "#ffb26f"
                font.pixelSize: 10
                font.bold: true
            }
        }

        Text {
            text: moduleRegistry.propulsion.description
            font.pixelSize: 10
            color: "#d7e3ef"
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }

        // The powertrain summary presents total thrust, rotor state, and nacelle health.
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 112
            color: "#0d1b24"
            border.color: "#31566d"
            border.width: 1

            RowLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 12

                ColumnLayout {
                    Layout.preferredWidth: 190
                    spacing: 2

                    Label { text: "DEP POWERTRAIN"; color: "#75c9f1"; font.pixelSize: 9; font.bold: true }
                    Text {
                        text: Number(root.combinedThrust).toFixed(1) + " kN"
                        color: "#f2fbff"
                        font.pixelSize: 25
                        font.bold: true
                    }
                    Label { text: "TOTAL VECTOR OUTPUT"; color: "#7797aa"; font.pixelSize: 8 }
                    Label {
                        text: Number(root.averageRpm).toFixed(0) + " rpm average"
                        color: "#b8d7e8"
                        font.pixelSize: 10
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: "#102630"
                    border.color: "#234959"
                    border.width: 1

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 8
                        spacing: 6

                        Repeater {
                            model: 4

                            ColumnLayout {
                                required property int index
                                Layout.fillWidth: true
                                spacing: 3

                                Item {
                                    Layout.alignment: Qt.AlignHCenter
                                    Layout.preferredWidth: 42
                                    Layout.preferredHeight: 42

                                    Rectangle {
                                        anchors.centerIn: parent
                                        width: 34
                                        height: 34
                                        radius: 17
                                        color: "#0b1921"
                                        border.color: root.propulsionModel.inverterHealthWarnings[index] ? "#ff9f7a" : "#67d7cf"
                                        border.width: 2
                                    }

                                    Item {
                                        anchors.centerIn: parent
                                        width: 28
                                        height: 28
                                        rotation: root.rotorPhase * 57.2958 * (index % 2 === 0 ? 1 : -1)

                                        Rectangle { anchors.centerIn: parent; width: 28; height: 3; radius: 1; color: "#b9eaff" }
                                        Rectangle { anchors.centerIn: parent; width: 3; height: 28; radius: 1; color: "#b9eaff" }
                                        Rectangle { anchors.centerIn: parent; width: 7; height: 7; radius: 4; color: "#67d7cf" }
                                    }
                                }

                                Label {
                                    Layout.alignment: Qt.AlignHCenter
                                    text: "M" + (index + 1) + "  " + Number(root.rpmValues[index] || 0).toFixed(0)
                                    color: "#d8edf7"
                                    font.pixelSize: 9
                                    font.bold: true
                                }
                                Label {
                                    Layout.alignment: Qt.AlignHCenter
                                    text: Number(root.tempValues[index] || 0).toFixed(0) + " C"
                                    color: root.propulsionModel.motorThermalWarnings[index] ? "#ff9f7a" : "#7fa8bc"
                                    font.pixelSize: 8
                                }
                            }
                        }
                    }
                }

                ColumnLayout {
                    Layout.preferredWidth: 150
                    spacing: 5

                    Label { text: "NACELLE TILT"; color: "#7797aa"; font.pixelSize: 8 }
                    Text { text: Number(root.flightModel.tiltAngleDeg).toFixed(1) + "°"; color: "#f3bf68"; font.pixelSize: 18; font.bold: true }
                    Label { text: "INVERTER HEALTH"; color: "#7797aa"; font.pixelSize: 8 }
                    Text { text: Number(root.inverterHealthAverage).toFixed(1) + "%"; color: "#8dc8ff"; font.pixelSize: 15; font.bold: true }
                }
            }
        }

        // The propulsion card grid exposes detailed motor and inverter telemetry.
        GridLayout {
            id: cardGrid
            columns: root.cardColumns
            Layout.fillWidth: true
            Layout.fillHeight: false
            Layout.alignment: Qt.AlignTop
            rowSpacing: 6
            columnSpacing: 6

            FlightSimulationCard {
                dataModel: flightModel
                cardTitle: "Motor RPM"
                cardSubtitle: "Individual and grouped motor rotational speeds"
                iconType: "propulsion"
                visualTheme: "propulsion"
                accentColor: "#76c2ff"
                motionSeed: 0.3
                motionRate: 0.024
                progressValue: root.propulsionModel.rpmProgress
                progressLabel: "Rotor speed readiness"
                sparkValues: flightModel.propulsionRpmHistory
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredWidth: Math.max(220, Math.min(320, cardGrid.width / cardColumns - 12))
                Layout.preferredHeight: Math.max(210, implicitHeight)
                Layout.minimumHeight: 210

                RowLayout {
                    spacing: 8
                    Label { text: "Front group"; color: "#9ec4f5"; font.pixelSize: 9 }
                    Text { text: Number(root.frontGroupRpm).toFixed(0) + " rpm"; color: "#e1f3ff"; font.pixelSize: 15; font.bold: true }
                }
                RowLayout {
                    spacing: 8
                    Label { text: "Rear group"; color: "#9ec4f5"; font.pixelSize: 9 }
                    Text { text: Number(root.rearGroupRpm).toFixed(0) + " rpm"; color: "#e1f3ff"; font.pixelSize: 15; font.bold: true }
                }
                Text {
                    text: "M1 " + Number(root.rpmValues[0] || 0).toFixed(0)
                          + " | M2 " + Number(root.rpmValues[1] || 0).toFixed(0)
                          + " | M3 " + Number(root.rpmValues[2] || 0).toFixed(0)
                          + " | M4 " + Number(root.rpmValues[3] || 0).toFixed(0)
                    color: "#9ec4f5"
                    font.pixelSize: 9
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }
            }

            FlightSimulationCard {
                dataModel: flightModel
                cardTitle: "Tilt Angle"
                cardSubtitle: "Nacelle or rotor tilt position"
                iconType: "propulsion"
                visualTheme: "propulsion"
                accentColor: "#f3bf68"
                motionSeed: 1.15
                motionRate: 0.028
                progressValue: root.propulsionModel.tiltProgress
                progressLabel: "Forward-flight transition"
                sparkValues: flightModel.propulsionTiltHistory
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredWidth: Math.max(220, Math.min(320, cardGrid.width / cardColumns - 12))
                Layout.preferredHeight: Math.max(210, implicitHeight)
                Layout.minimumHeight: 210

                Text {
                    text: Number(flightModel.tiltAngleDeg).toFixed(1) + "°"
                    color: "#fff2da"
                    font.pixelSize: 26
                    font.bold: true
                }
                Text {
                    text: root.propulsionModel.tiltProfileLabel
                    color: "#f9d89f"
                    font.pixelSize: 10
                }
            }

            FlightSimulationCard {
                dataModel: flightModel
                cardTitle: "Motor Temperature"
                cardSubtitle: "Thermal telemetry per motor group"
                iconType: "propulsion"
                visualTheme: "propulsion"
                accentColor: "#ff9f7a"
                motionSeed: 2.0
                motionRate: 0.041
                progressValue: root.propulsionModel.temperatureProgress
                progressLabel: "Thermal load"
                sparkValues: flightModel.propulsionTempHistory
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredWidth: Math.max(220, Math.min(320, cardGrid.width / cardColumns - 12))
                Layout.preferredHeight: Math.max(210, implicitHeight)
                Layout.minimumHeight: 210

                RowLayout {
                    spacing: 8
                    Label { text: "Average"; color: "#ffc5b0"; font.pixelSize: 9 }
                    Text { text: Number(root.averageMotorTemp).toFixed(1) + " C"; color: "#fff1ea"; font.pixelSize: 16; font.bold: true }
                }
                Text {
                    text: "M1 " + Number(root.tempValues[0] || 0).toFixed(1)
                          + " | M2 " + Number(root.tempValues[1] || 0).toFixed(1)
                          + " | M3 " + Number(root.tempValues[2] || 0).toFixed(1)
                          + " | M4 " + Number(root.tempValues[3] || 0).toFixed(1)
                    color: "#ffc5b0"
                    font.pixelSize: 9
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }
            }

            FlightSimulationCard {
                dataModel: flightModel
                cardTitle: "Thrust Vector / Output"
                cardSubtitle: "Real-time thrust generation per motor group"
                iconType: "propulsion"
                visualTheme: "propulsion"
                accentColor: "#74e2d8"
                motionSeed: 3.1
                motionRate: 0.03
                progressValue: root.propulsionModel.thrustProgress
                progressLabel: "Total thrust authority"
                sparkValues: flightModel.propulsionThrustHistory
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredWidth: Math.max(220, Math.min(320, cardGrid.width / cardColumns - 12))
                Layout.preferredHeight: Math.max(210, implicitHeight)
                Layout.minimumHeight: 210

                RowLayout {
                    spacing: 8
                    Label { text: "Front"; color: "#a7f1ec"; font.pixelSize: 9 }
                    Text { text: Number(root.thrustValues[0] || 0).toFixed(1) + " kN"; color: "#e6fffd"; font.pixelSize: 15; font.bold: true }
                }
                RowLayout {
                    spacing: 8
                    Label { text: "Rear"; color: "#a7f1ec"; font.pixelSize: 9 }
                    Text { text: Number(root.thrustValues[1] || 0).toFixed(1) + " kN"; color: "#e6fffd"; font.pixelSize: 15; font.bold: true }
                }
            }

            FlightSimulationCard {
                dataModel: flightModel
                cardTitle: "Inverter Status"
                cardSubtitle: "Voltage, current, and health per motor controller"
                iconType: "propulsion"
                visualTheme: "propulsion"
                accentColor: "#8dc8ff"
                motionSeed: 4.4
                motionRate: 0.022
                progressValue: root.propulsionModel.inverterProgress
                progressLabel: "Controller health"
                sparkValues: flightModel.propulsionInverterHealthHistory
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredWidth: Math.max(220, Math.min(320, cardGrid.width / cardColumns - 12))
                Layout.preferredHeight: Math.max(210, implicitHeight)
                Layout.minimumHeight: 210

                Text {
                    text: "INV1 " + Number(root.inverterVoltageValues[0] || 0).toFixed(0) + " V / "
                          + Number(root.inverterCurrentValues[0] || 0).toFixed(0) + " A / "
                          + Number(root.inverterHealthValues[0] || 0).toFixed(1) + "%"
                    color: "#d9f0ff"
                    font.pixelSize: 9
                }
                Text {
                    text: "INV2 " + Number(root.inverterVoltageValues[1] || 0).toFixed(0) + " V / "
                          + Number(root.inverterCurrentValues[1] || 0).toFixed(0) + " A / "
                          + Number(root.inverterHealthValues[1] || 0).toFixed(1) + "%"
                    color: "#d9f0ff"
                    font.pixelSize: 9
                }
                Text {
                    text: "INV3 " + Number(root.inverterVoltageValues[2] || 0).toFixed(0) + " V / "
                          + Number(root.inverterCurrentValues[2] || 0).toFixed(0) + " A / "
                          + Number(root.inverterHealthValues[2] || 0).toFixed(1) + "%"
                    color: "#d9f0ff"
                    font.pixelSize: 9
                }
                Text {
                    text: "INV4 " + Number(root.inverterVoltageValues[3] || 0).toFixed(0) + " V / "
                          + Number(root.inverterCurrentValues[3] || 0).toFixed(0) + " A / "
                          + Number(root.inverterHealthValues[3] || 0).toFixed(1) + "%"
                    color: "#d9f0ff"
                    font.pixelSize: 9
                }
            }
        }
    }
}
