/**
 * @file    : src/view/features/EnergySystemsPage.qml
 * @brief   : Defines the detailed battery and power systems dashboard.
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

// This dashboard provides detailed battery, thermal, and power-distribution telemetry.
Item {
    id: root
    required property var moduleRegistry

    readonly property var energyModel: root.moduleRegistry.primaryFlight
    readonly property var energySystem: root.moduleRegistry.energySystem
    readonly property bool hoverMode: root.energySystem.hoverMode
    readonly property real soc: Number(root.energyModel.batterySoc)
    readonly property real soh: Number(root.energyModel.batterySoh)
    readonly property real powerKw: Number(root.energyModel.powerConsumptionKw)
    readonly property real minTemp: Number(root.energyModel.cellTemperatureMin)
    readonly property real maxTemp: Number(root.energyModel.cellTemperatureMax)
    readonly property var cellTemperatures: root.energyModel.batteryCellTemperatures
    readonly property real busVoltage: Number(root.energyModel.busVoltage)
    readonly property real busCurrent: Number(root.energyModel.busCurrent)
    readonly property bool thermalWarning: root.energyModel.thermalRunawayWarning
    readonly property real estimatedMinutes: root.energySystem.estimatedMinutes
    readonly property color statusColor: root.energySystem.status === 2 ? "#ff665c" : (root.energySystem.status === 1 ? "#ffb05f" : "#65d69b")
    readonly property int panelSpacing: 6
    readonly property real topHeight: Math.max(210, (dashboard.height - root.panelSpacing) * 0.55)
    readonly property real bottomHeight: Math.max(170, dashboard.height - root.panelSpacing - root.topHeight)

    property int selectedMetric: 0
    property int selectedThermalZone: -1
    property int selectedBusChannel: 0
    property int hoveredBusChannel: -1
    property real flowPhase: 0.0

    readonly property int activeBusChannel: root.hoveredBusChannel >= 0
                                            ? root.hoveredBusChannel : root.selectedBusChannel

    // These accessors adapt energy-system channel data for the dashboard controls.
    function zoneTemperature(index) {
        return root.energySystem.zoneTemperatures[index]
    }

    function busChannelName(index) {
        return root.energySystem.busChannelName(index)
    }

    function busChannelCode(index) {
        return root.energySystem.busChannelCode(index)
    }

    function busChannelCurrent(index) {
        return root.energySystem.busChannelCurrents[index]
    }

    function busChannelPower(index) {
        return root.energySystem.busChannelPowers[index]
    }

    // The animation timer advances high-voltage bus flow indicators.
    Timer {
        interval: 40
        running: root.visible
        repeat: true
        onTriggered: {
            root.flowPhase = (root.flowPhase + 0.018) % 1.0
        }
    }

    // The page content arranges system status, metric selection, and energy panels.
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 4
        spacing: 5

        // The page header summarizes battery-system identity and alert state.
        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 43
            spacing: 8

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 0
                Text { text: "Energy, Power & Battery Subsystems"; color: "#f5fbff"; font.pixelSize: 18; font.bold: true; Layout.fillWidth: true; Layout.minimumWidth: 0; elide: Text.ElideRight }
                Text { text: "Live high-voltage storage, thermal and distribution telemetry"; color: "#94afbd"; font.pixelSize: 9; Layout.fillWidth: true }
            }

            Rectangle {
                Layout.preferredWidth: 178
                Layout.preferredHeight: 28
                color: root.thermalWarning ? "#351d1c" : "#102923"
                border.color: root.statusColor
                border.width: 1
                Row {
                    anchors.centerIn: parent
                    spacing: 7
                    Rectangle { width: 7; height: 7; radius: 4; color: root.statusColor }
                    Label {
                        width: 154
                        text: root.energySystem.statusLabel
                        color: root.statusColor
                        font.pixelSize: 9
                        minimumPixelSize: 7
                        fontSizeMode: Text.Fit
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
            }
        }

        // The metric selector focuses the corresponding dashboard panel.
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 30
            color: "#0d1d24"
            border.color: "#294958"
            border.width: 1

            RowLayout {
                anchors.fill: parent
                anchors.margins: 4
                spacing: 4

                Repeater {
                    model: ["CHARGE", "HEALTH", "POWER", "THERMAL", "HV BUS"]
                    Rectangle {
                        id: metricButton
                        required property int index
                        required property string modelData
                        Layout.preferredWidth: 68
                        Layout.fillHeight: true
                        color: root.selectedMetric === metricButton.index ? "#2d5e68" : "#142b33"
                        border.color: root.selectedMetric === metricButton.index ? "#69d4d0" : "#294852"
                        border.width: 1
                        Label { anchors.centerIn: parent; text: metricButton.modelData; color: root.selectedMetric === metricButton.index ? "#efffff" : "#83a2ad"; font.pixelSize: 7; font.bold: true }
                        MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: root.selectedMetric = metricButton.index }
                    }
                }

                Item { Layout.fillWidth: true }
                Label {
                    text: root.energySystem.powerProfileLabel
                    color: root.hoverMode ? "#ffbd70" : "#74d1e6"
                    font.pixelSize: 8
                    font.bold: true
                    rightPadding: 6
                }
            }
        }

        // The dashboard groups charge, thermal, consumption, health, and bus telemetry.
        GridLayout {
            id: dashboard
            Layout.fillWidth: true
            Layout.fillHeight: true
            columns: 2
            columnSpacing: root.panelSpacing
            rowSpacing: root.panelSpacing

            // The state-of-charge panel combines a radial gauge with pack occupancy.
            Rectangle {
                Layout.row: 0
                Layout.column: 0
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumWidth: 0
                Layout.preferredWidth: (dashboard.width - dashboard.columnSpacing) / 2
                Layout.preferredHeight: root.topHeight
                color: "#0b191e"
                border.color: root.selectedMetric === 0 ? "#69d4b2" : "#31545c"
                border.width: root.selectedMetric === 0 ? 2 : 1

                MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: root.selectedMetric = 0 }

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 9
                    spacing: 5

                    RowLayout {
                        Layout.fillWidth: true
                        Label { text: "BATTERY STATE OF CHARGE"; color: "#74d9b5"; font.pixelSize: 10; font.bold: true }
                        Item { Layout.fillWidth: true }
                        Label { text: root.estimatedMinutes.toFixed(0) + " MIN EST."; color: "#8aac9f"; font.pixelSize: 8 }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        spacing: 12

                        Item {
                            Layout.preferredWidth: 154
                            Layout.fillHeight: true

                            // The charge gauge renders usable battery capacity as a radial arc.
                            Canvas {
                                id: socGauge
                                anchors.centerIn: parent
                                width: Math.min(parent.width, parent.height) - 8
                                height: width
                                antialiasing: true
                                property real value: root.soc
                                onValueChanged: requestPaint()
                                onPaint: {
                                    var ctx = getContext("2d")
                                    ctx.clearRect(0, 0, width, height)
                                    var cx = width / 2
                                    var cy = height / 2
                                    var radius = Math.min(width, height) * 0.38
                                    var start = Math.PI * 0.75
                                    var span = Math.PI * 1.5
                                    ctx.lineCap = "round"
                                    ctx.lineWidth = 12
                                    ctx.strokeStyle = "#1d3638"
                                    ctx.beginPath()
                                    ctx.arc(cx, cy, radius, start, start + span)
                                    ctx.stroke()
                                    ctx.strokeStyle = root.statusColor
                                    ctx.beginPath()
                                    ctx.arc(cx, cy, radius, start, start + span * Math.max(0, Math.min(1, value / 100)))
                                    ctx.stroke()
                                }
                            }
                            Column {
                                anchors.centerIn: parent
                                spacing: 0
                                Text { anchors.horizontalCenter: parent.horizontalCenter; text: root.soc.toFixed(1) + "%"; color: "#f1fff9"; font.pixelSize: 23; font.bold: true }
                                Label { anchors.horizontalCenter: parent.horizontalCenter; text: "REMAINING"; color: "#71948a"; font.pixelSize: 7 }
                            }
                        }

                        ColumnLayout {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            spacing: 5

                            Label { text: "PACK MODULES"; color: "#6f8e87"; font.pixelSize: 7 }
                            GridLayout {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                columns: 4
                                columnSpacing: 4
                                rowSpacing: 4
                                Repeater {
                                    model: 16
                                    Rectangle {
                                        id: moduleCell
                                        required property int index
                                        Layout.fillWidth: true
                                        Layout.fillHeight: true
                                        color: moduleCell.index < Math.round(root.soc / 100 * 16) ? "#2b8f72" : "#172c2d"
                                        border.color: moduleCell.index < Math.round(root.soc / 100 * 16) ? "#67d6ab" : "#2d4b4c"
                                        border.width: 1
                                        Behavior on color { ColorAnimation { duration: 300 } }
                                    }
                                }
                            }
                            RowLayout {
                                Layout.fillWidth: true
                                Label { text: "USABLE CAPACITY"; color: "#718e87"; font.pixelSize: 7 }
                                Item { Layout.fillWidth: true }
                                Label { text: (root.soc * 4.2).toFixed(0) + " kWh"; color: "#bdebd8"; font.pixelSize: 9; font.bold: true }
                            }
                        }
                    }
                }
            }

            // The thermal panel maps module temperatures and cooling-flow boundaries.
            Rectangle {
                Layout.row: 0
                Layout.column: 1
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumWidth: 0
                Layout.preferredWidth: (dashboard.width - dashboard.columnSpacing) / 2
                Layout.preferredHeight: root.topHeight
                color: "#10191d"
                border.color: root.selectedMetric === 3 ? "#f2ad65" : "#4e5550"
                border.width: root.selectedMetric === 3 ? 2 : 1

                MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: root.selectedMetric = 3 }

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 9
                    spacing: 6

                    RowLayout {
                        Layout.fillWidth: true
                        Label { text: "CELL THERMAL MAP"; color: "#f1b46f"; font.pixelSize: 10; font.bold: true }
                        Item { Layout.fillWidth: true }
                        Label { text: root.thermalWarning ? "RUNAWAY RISK" : "STABLE"; color: root.thermalWarning ? "#ff665c" : "#68d39a"; font.pixelSize: 8; font.bold: true }
                    }

                    Item {
                        id: thermalPack
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        clip: true

                        // The thermal-flow canvas outlines the battery cooling circuit.
                        Canvas {
                            id: thermalFlow
                            anchors.top: parent.top
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.bottom: moduleSelector.top
                            anchors.bottomMargin: 2
                            antialiasing: true

                            onPaint: {
                                var ctx = getContext("2d")
                                ctx.clearRect(0, 0, width, height)
                                var inset = 10
                                var left = inset
                                var right = width - inset
                                var top = inset
                                var bottom = height - inset
                                var corner = Math.min(16, (bottom - top) * 0.18)

                                ctx.strokeStyle = "rgba(98, 186, 201, 0.28)"
                                ctx.lineWidth = 7
                                ctx.lineCap = "round"
                                ctx.beginPath()
                                ctx.moveTo(left + corner, top)
                                ctx.lineTo(right - corner, top)
                                ctx.quadraticCurveTo(right, top, right, top + corner)
                                ctx.lineTo(right, bottom - corner)
                                ctx.quadraticCurveTo(right, bottom, right - corner, bottom)
                                ctx.lineTo(left + corner, bottom)
                                ctx.quadraticCurveTo(left, bottom, left, bottom - corner)
                                ctx.lineTo(left, top + corner)
                                ctx.quadraticCurveTo(left, top, left + corner, top)
                                ctx.closePath()
                                ctx.stroke()

                                ctx.strokeStyle = "rgba(118, 224, 225, 0.52)"
                                ctx.lineWidth = 2
                                ctx.stroke()

                                ctx.fillStyle = "rgba(134, 231, 225, 0.72)"
                                ctx.beginPath()
                                ctx.arc(left + corner, top, 2.5, 0, Math.PI * 2)
                                ctx.fill()
                                ctx.beginPath()
                                ctx.arc(right - corner, bottom, 2.5, 0, Math.PI * 2)
                                ctx.fill()
                            }

                            onWidthChanged: requestPaint()
                            onHeightChanged: requestPaint()
                        }

                        Rectangle {
                            anchors.centerIn: thermalFlow
                            width: thermalFlow.width * 0.78
                            height: thermalFlow.height * 0.58
                            radius: 8
                            color: "#151f20"
                            border.color: "#54605a"
                            border.width: 2

                            GridLayout {
                                id: moduleDeck
                                anchors.fill: parent
                                anchors.margins: 7
                                columns: 3
                                columnSpacing: 6
                                rowSpacing: 6

                                Repeater {
                                    model: 6

                                    Rectangle {
                                        id: thermalZone
                                        required property int index
                                        readonly property real temperature: root.zoneTemperature(thermalZone.index)
                                        Layout.fillWidth: true
                                        Layout.fillHeight: true
                                        color: thermalZone.temperature >= 55 ? "#9f4039"
                                               : (thermalZone.temperature >= 45 ? "#8a6435" : "#285957")
                                        border.color: root.selectedThermalZone === thermalZone.index ? "#f5f0c8"
                                                      : (thermalZone.temperature >= 55 ? "#ff786d" : "#4d7b76")
                                        border.width: root.selectedThermalZone === thermalZone.index ? 2 : 1

                                        Behavior on color { ColorAnimation { duration: 280 } }

                                        Column {
                                            anchors.centerIn: parent
                                            width: parent.width - 8
                                            spacing: 1
                                            Label { width: parent.width; text: "MODULE " + (thermalZone.index + 1); color: "#a7b7ae"; font.pixelSize: 7; minimumPixelSize: 6; fontSizeMode: Text.Fit; font.bold: true; horizontalAlignment: Text.AlignHCenter }
                                            Text { width: parent.width; text: thermalZone.temperature.toFixed(1) + "°C"; color: "#f8fbf5"; font.pixelSize: 13; minimumPixelSize: 8; fontSizeMode: Text.Fit; font.bold: true; horizontalAlignment: Text.AlignHCenter }
                                            Row {
                                                anchors.horizontalCenter: parent.horizontalCenter
                                                spacing: 3
                                                Repeater {
                                                    model: 4
                                                    Rectangle {
                                                        required property int index
                                                        width: 5
                                                        height: 5
                                                        radius: 3
                                                        color: thermalZone.temperature + index * 0.5 >= 55 ? "#ff776b" : "#8cc9b9"
                                                    }
                                                }
                                            }
                                        }

                                        MouseArea {
                                            anchors.fill: parent
                                            hoverEnabled: true
                                            cursorShape: Qt.PointingHandCursor
                                            onEntered: root.selectedThermalZone = thermalZone.index
                                            onClicked: root.selectedThermalZone = thermalZone.index
                                        }
                                    }
                                }
                            }
                        }

                        Rectangle {
                            anchors.left: parent.left
                            anchors.verticalCenter: thermalFlow.verticalCenter
                            width: 10
                            height: thermalFlow.height * 0.58
                            gradient: Gradient {
                                GradientStop { position: 0.0; color: "#d65045" }
                                GradientStop { position: 0.5; color: "#b57b3c" }
                                GradientStop { position: 1.0; color: "#2f7770" }
                            }
                        }

                        Label {
                            id: moduleSelector
                            anchors.right: parent.right
                            anchors.bottom: parent.bottom
                            anchors.rightMargin: 4
                            anchors.bottomMargin: 3
                            width: Math.min(142, Math.max(104, parent.width * 0.34))
                            height: 18
                            text: root.selectedThermalZone >= 0
                                  ? "M" + (root.selectedThermalZone + 1) + "  " + root.zoneTemperature(root.selectedThermalZone).toFixed(1) + " °C"
                                  : "SELECT MODULE"
                            color: "#d8e5df"
                            font.pixelSize: 8
                            minimumPixelSize: 6
                            fontSizeMode: Text.Fit
                            font.bold: true
                            horizontalAlignment: Text.AlignRight
                            verticalAlignment: Text.AlignVCenter
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        Label { text: "MIN"; color: "#76938a"; font.pixelSize: 7 }
                        Text { Layout.preferredWidth: 72; text: root.minTemp.toFixed(1) + " °C"; color: "#92d6bd"; font.pixelSize: 12; minimumPixelSize: 8; fontSizeMode: Text.Fit; font.bold: true }
                        Item { Layout.fillWidth: true }
                        Label { Layout.preferredWidth: 92; text: "SPREAD " + (root.maxTemp - root.minTemp).toFixed(1) + " °C"; color: "#91a49e"; font.pixelSize: 8; minimumPixelSize: 6; fontSizeMode: Text.Fit; horizontalAlignment: Text.AlignHCenter }
                        Item { Layout.fillWidth: true }
                        Label { text: "MAX"; color: "#8f8170"; font.pixelSize: 7 }
                        Text { Layout.preferredWidth: 72; text: root.maxTemp.toFixed(1) + " °C"; color: "#ffc486"; font.pixelSize: 12; minimumPixelSize: 8; fontSizeMode: Text.Fit; font.bold: true; horizontalAlignment: Text.AlignRight }
                    }
                }
            }

            // The consumption panel compares hover and cruise power demand.
            Rectangle {
                Layout.row: 1
                Layout.column: 0
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumWidth: 0
                Layout.preferredWidth: (dashboard.width - dashboard.columnSpacing) / 2
                Layout.preferredHeight: root.bottomHeight
                color: "#0d1920"
                border.color: root.selectedMetric === 2 ? "#71ccec" : "#34515e"
                border.width: root.selectedMetric === 2 ? 2 : 1

                MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: root.selectedMetric = 2 }

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 9
                    spacing: 5

                    RowLayout {
                        Layout.fillWidth: true
                        Label { text: "POWER CONSUMPTION"; color: "#72caeb"; font.pixelSize: 10; font.bold: true }
                        Item { Layout.fillWidth: true }
                        Text { text: root.powerKw.toFixed(0) + " kW"; color: "#e5f8ff"; font.pixelSize: 18; font.bold: true }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        spacing: 10

                        ColumnLayout {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            Label { text: "HOVER DEMAND"; color: "#9c8a72"; font.pixelSize: 7 }
                            Item {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                Rectangle { anchors.fill: parent; color: "#1b282c" }
                                Rectangle {
                                    anchors.left: parent.left
                                    anchors.right: parent.right
                                    anchors.bottom: parent.bottom
                                    height: parent.height * Math.min(1, root.powerKw / 700.0) * (root.hoverMode ? 1.0 : 0.72)
                                    color: "#e09a50"
                                    Behavior on height { NumberAnimation { duration: 350 } }
                                }
                            }
                            Label { text: root.hoverMode ? "ACTIVE" : "REFERENCE"; color: root.hoverMode ? "#ffc17c" : "#708890"; font.pixelSize: 8; font.bold: true }
                        }

                        ColumnLayout {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            Label { text: "CRUISE DEMAND"; color: "#718d98"; font.pixelSize: 7 }
                            Item {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                Rectangle { anchors.fill: parent; color: "#1b282c" }
                                Rectangle {
                                    anchors.left: parent.left
                                    anchors.right: parent.right
                                    anchors.bottom: parent.bottom
                                    height: parent.height * Math.min(1, root.powerKw / 700.0) * (root.hoverMode ? 0.62 : 1.0)
                                    color: "#45a8c8"
                                    Behavior on height { NumberAnimation { duration: 350 } }
                                }
                            }
                            Label { text: root.hoverMode ? "REFERENCE" : "ACTIVE"; color: root.hoverMode ? "#708890" : "#74d8f5"; font.pixelSize: 8; font.bold: true }
                        }

                        ColumnLayout {
                            Layout.preferredWidth: 116
                            Layout.alignment: Qt.AlignVCenter
                            spacing: 2
                            Label { text: "FLIGHT PHASE"; color: "#718c98"; font.pixelSize: 7 }
                            Text { Layout.fillWidth: true; text: root.energyModel.flightModeLabel; color: "#d9edf5"; font.pixelSize: 10; font.bold: true; wrapMode: Text.WordWrap }
                            Label { text: "EFFICIENCY"; color: "#718c98"; font.pixelSize: 7 }
                            Text { text: root.energySystem.efficiencyPercent.toFixed(1) + "%"; color: "#79d6a7"; font.pixelSize: 12; font.bold: true }
                        }
                    }
                }
            }

            // The health and bus panel combines degradation status with channel distribution.
            Rectangle {
                Layout.row: 1
                Layout.column: 1
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumWidth: 0
                Layout.preferredWidth: (dashboard.width - dashboard.columnSpacing) / 2
                Layout.preferredHeight: root.bottomHeight
                color: "#10191f"
                border.color: root.selectedMetric === 1 || root.selectedMetric === 4 ? "#b4c875" : "#47544a"
                border.width: root.selectedMetric === 1 || root.selectedMetric === 4 ? 2 : 1

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: root.selectedMetric = root.selectedMetric === 4 ? 1 : 4
                }

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 9
                    spacing: 5

                    RowLayout {
                        Layout.fillWidth: true
                        Label { text: "BATTERY HEALTH & HV BUS"; color: "#b3c879"; font.pixelSize: 10; font.bold: true }
                        Item { Layout.fillWidth: true }
                        Label { text: root.soh >= 95 ? "EXCELLENT" : (root.soh >= 85 ? "SERVICEABLE" : "MAINTENANCE"); color: root.soh >= 95 ? "#78d7a4" : "#f2b46d"; font.pixelSize: 8; font.bold: true }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        spacing: 10

                        ColumnLayout {
                            Layout.preferredWidth: 118
                            Layout.alignment: Qt.AlignVCenter
                            spacing: 1
                            Label { text: "STATE OF HEALTH"; color: "#788d79"; font.pixelSize: 7 }
                            Text { text: root.soh.toFixed(1) + "%"; color: "#e8f4d3"; font.pixelSize: 22; font.bold: true }
                            Rectangle {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 7
                                color: "#26342a"
                                Rectangle { width: parent.width * Math.min(1, root.soh / 100); height: parent.height; color: "#91b85d" }
                            }
                            Label { text: "DEGRADATION " + (100 - root.soh).toFixed(1) + "%"; color: "#859582"; font.pixelSize: 7 }
                        }

                        Rectangle { Layout.preferredWidth: 1; Layout.fillHeight: true; color: "#34443b" }

                        ColumnLayout {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            spacing: 3

                            RowLayout {
                                Layout.fillWidth: true
                                ColumnLayout {
                                    Layout.fillWidth: true
                                    spacing: 0
                                    Label { text: "MAIN BUS"; color: "#718993"; font.pixelSize: 7 }
                                    Text { text: root.busVoltage.toFixed(0) + " V"; color: "#d9f2ff"; font.pixelSize: 16; font.bold: true }
                                }
                                ColumnLayout {
                                    Layout.fillWidth: true
                                    spacing: 0
                                    Label { text: "TOTAL CURRENT"; color: "#718993"; font.pixelSize: 7 }
                                    Text { text: root.busCurrent.toFixed(0) + " A"; color: "#8fd9ff"; font.pixelSize: 16; font.bold: true }
                                }
                            }

                            // The bus network visualizes animated power flow across selectable channels.
                            Item {
                                id: busNetwork
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                Layout.minimumHeight: 54

                                Rectangle {
                                    anchors.left: parent.left
                                    anchors.right: parent.right
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.verticalCenterOffset: -8
                                    height: 5
                                    radius: 3
                                    color: "#19323d"
                                }

                                Repeater {
                                    model: 3
                                    Rectangle {
                                        id: busPulse
                                        required property int index
                                        width: 10
                                        height: 5
                                        radius: 3
                                        y: busNetwork.height * 0.5 - 10
                                        x: ((root.flowPhase + busPulse.index / 3.0) % 1.0)
                                           * Math.max(0, busNetwork.width - width)
                                        color: "#62c9ef"
                                        opacity: 0.75
                                    }
                                }

                                Row {
                                    anchors.fill: parent

                                    Repeater {
                                    model: 5
                                    Item {
                                        id: busChannel
                                        required property int index
                                        width: busNetwork.width / 5
                                        height: busNetwork.height

                                        Rectangle {
                                            anchors.horizontalCenter: parent.horizontalCenter
                                            y: parent.height * 0.5 - 6
                                            width: 2
                                            height: 12
                                            color: root.activeBusChannel === busChannel.index ? "#8de1ff" : "#315563"
                                        }

                                        Rectangle {
                                            anchors.horizontalCenter: parent.horizontalCenter
                                            y: parent.height * 0.5 + 4
                                            width: Math.min(34, parent.width - 5)
                                            height: 20
                                            radius: 3
                                            color: root.activeBusChannel === busChannel.index ? "#23576a" : "#172b33"
                                            border.color: root.selectedBusChannel === busChannel.index ? "#8de1ff" : "#315563"
                                            border.width: root.selectedBusChannel === busChannel.index ? 2 : 1

                                            Text {
                                                anchors.fill: parent
                                                text: root.busChannelCode(busChannel.index)
                                                color: root.activeBusChannel === busChannel.index ? "#e3f8ff" : "#84a8b7"
                                                font.pixelSize: 8
                                                font.bold: true
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }
                                        }

                                        MouseArea {
                                            anchors.fill: parent
                                            hoverEnabled: true
                                            cursorShape: Qt.PointingHandCursor
                                            onEntered: root.hoveredBusChannel = busChannel.index
                                            onExited: root.hoveredBusChannel = -1
                                            onClicked: root.selectedBusChannel = busChannel.index
                                        }
                                    }
                                    }
                                }
                            }
                            Label {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 12
                                text: root.busChannelName(root.activeBusChannel)
                                      + "  |  " + root.busChannelCurrent(root.activeBusChannel).toFixed(0) + " A"
                                      + "  |  " + root.busChannelPower(root.activeBusChannel).toFixed(1) + " kW"
                                color: "#8fc6d9"
                                font.pixelSize: 8
                                minimumPixelSize: 6
                                fontSizeMode: Text.Fit
                                font.bold: true
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                        }
                    }
                }
            }
        }
    }
}
