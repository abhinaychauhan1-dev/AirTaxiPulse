pragma ComponentBehavior: Bound

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    required property var moduleRegistry

    readonly property var flightModel: root.moduleRegistry.primaryFlight
    readonly property var propulsionModel: root.moduleRegistry.propulsionSystem
    readonly property var rpmValues: root.flightModel.motorRpmValues || []
    readonly property var tempValues: root.flightModel.motorTemperatures || []
    readonly property var thrustValues: root.flightModel.thrustOutputs || []
    readonly property var inverterVoltageValues: root.flightModel.inverterVoltages || []
    readonly property var inverterCurrentValues: root.flightModel.inverterCurrents || []
    readonly property var inverterHealthValues: root.flightModel.inverterHealth || []
    readonly property bool compact: width < 620
    readonly property int pageMargin: 4
    readonly property int panelSpacing: 6
    readonly property real dashboardAvailableHeight: Math.max(410,
        root.height - root.pageMargin * 2 - pageHeader.implicitHeight - root.panelSpacing)
    readonly property real topPanelHeight: Math.max(236,
        Math.min(286, (root.dashboardAvailableHeight - root.panelSpacing) * 0.58))
    readonly property real bottomPanelHeight: Math.max(168,
        root.dashboardAvailableHeight - root.panelSpacing - root.topPanelHeight)

    property real rotorPhase: 0.0
    property real powerFlowPhase: 0.0

    readonly property real averageRpm: root.propulsionModel.averageRpm
    readonly property real averageMotorTemp: root.propulsionModel.averageMotorTemperature
    readonly property real inverterHealthAverage: root.propulsionModel.inverterHealthAverage
    readonly property real frontThrust: root.propulsionModel.frontThrust
    readonly property real rearThrust: root.propulsionModel.rearThrust
    readonly property real combinedThrust: root.propulsionModel.combinedThrust
    readonly property real thrustBalance: root.propulsionModel.thrustBalance
    readonly property real electricalPowerKw: root.propulsionModel.electricalPowerKw
    readonly property string propulsionState: root.propulsionModel.stateLabel
    readonly property color stateColor: root.propulsionState === "ALL CHANNELS NOMINAL" ? "#69d6a0" : "#ff9f72"

    Timer {
        interval: 32
        running: root.visible
        repeat: true
        onTriggered: {
            root.rotorPhase = (root.rotorPhase + 0.035 + root.averageRpm / 36000.0) % (Math.PI * 2)
            root.powerFlowPhase = (root.powerFlowPhase + 0.018) % 1.0
            powerNetwork.requestPaint()
        }
    }

    implicitHeight: propulsionContent.implicitHeight + root.pageMargin * 2

    ColumnLayout {
        id: propulsionContent
        anchors.fill: parent
        anchors.margins: root.pageMargin
        spacing: root.panelSpacing

        GridLayout {
            id: pageHeader
            Layout.fillWidth: true
            columns: root.compact ? 1 : 2
            columnSpacing: 10
            rowSpacing: 5

            ColumnLayout {
                Layout.fillWidth: true
                Layout.minimumWidth: 0
                spacing: 1

                Text {
                    text: root.moduleRegistry.propulsion.title
                    color: "#f5fbff"
                    font.pixelSize: 18
                    font.bold: true
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }
                Text {
                    text: root.moduleRegistry.propulsion.description
                    color: "#9db7c6"
                    font.pixelSize: 10
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }
            }

            Rectangle {
                Layout.preferredWidth: 178
                Layout.preferredHeight: 30
                Layout.alignment: root.compact ? Qt.AlignLeft : Qt.AlignRight | Qt.AlignVCenter
                color: "#10282a"
                border.color: root.stateColor
                border.width: 1

                Row {
                    anchors.centerIn: parent
                    spacing: 7
                    Rectangle { width: 7; height: 7; radius: 4; color: root.stateColor }
                    Label { text: root.propulsionState; color: root.stateColor; font.pixelSize: 9; font.bold: true }
                }
            }
        }

        GridLayout {
            id: dashboardGrid
            Layout.fillWidth: true
            Layout.fillHeight: true
            columns: root.compact ? 1 : 2
            columnSpacing: root.panelSpacing
            rowSpacing: root.panelSpacing

            Rectangle {
                Layout.fillWidth: true
                Layout.minimumWidth: 0
                Layout.preferredWidth: root.compact ? dashboardGrid.width : (dashboardGrid.width - dashboardGrid.columnSpacing) / 2
                Layout.preferredHeight: root.topPanelHeight
                color: "#0b1921"
                border.color: "#31566d"
                border.width: 1

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 4

                    RowLayout {
                        Layout.fillWidth: true
                        Label { text: "DISTRIBUTED PROPULSION MAP"; color: "#73c9ef"; font.pixelSize: 10; font.bold: true }
                        Item { Layout.fillWidth: true }
                        Label { text: Number(root.averageRpm).toFixed(0) + " RPM AVG"; color: "#d9f2ff"; font.pixelSize: 9 }
                    }

                    Item {
                        id: propulsionMap
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        Canvas {
                            id: powerNetwork
                            anchors.fill: parent
                            antialiasing: true

                            onPaint: {
                                var ctx = getContext("2d")
                                ctx.clearRect(0, 0, width, height)
                                var centerX = width * 0.5
                                var centerY = height * 0.5
                                var motorPoints = [
                                    [width * 0.14, height * 0.24],
                                    [width * 0.86, height * 0.24],
                                    [width * 0.14, height * 0.76],
                                    [width * 0.86, height * 0.76]
                                ]

                                ctx.lineWidth = 8
                                ctx.lineCap = "round"
                                ctx.strokeStyle = "rgba(46, 92, 112, 0.38)"
                                ctx.beginPath()
                                ctx.moveTo(centerX, height * 0.14)
                                ctx.lineTo(centerX, height * 0.86)
                                ctx.stroke()

                                for (var i = 0; i < motorPoints.length; ++i) {
                                    ctx.lineWidth = 2
                                    ctx.strokeStyle = "rgba(83, 164, 191, 0.48)"
                                    ctx.beginPath()
                                    ctx.moveTo(centerX, centerY)
                                    ctx.lineTo(motorPoints[i][0], motorPoints[i][1])
                                    ctx.stroke()

                                    var pulse = (root.powerFlowPhase + i * 0.21) % 1.0
                                    var pulseX = centerX + (motorPoints[i][0] - centerX) * pulse
                                    var pulseY = centerY + (motorPoints[i][1] - centerY) * pulse
                                    ctx.beginPath()
                                    ctx.arc(pulseX, pulseY, 3.2, 0, Math.PI * 2)
                                    ctx.fillStyle = "#7de0e1"
                                    ctx.fill()
                                }

                                ctx.fillStyle = "#132d38"
                                ctx.strokeStyle = "#5aa7bd"
                                ctx.lineWidth = 1.5
                                ctx.beginPath()
                                ctx.moveTo(centerX, height * 0.18)
                                ctx.lineTo(centerX + 24, centerY)
                                ctx.lineTo(centerX, height * 0.82)
                                ctx.lineTo(centerX - 24, centerY)
                                ctx.closePath()
                                ctx.fill()
                                ctx.stroke()
                            }

                            onWidthChanged: requestPaint()
                            onHeightChanged: requestPaint()
                        }

                        Repeater {
                            model: 4

                            Item {
                                id: motorNode
                                required property int index
                                readonly property real rpm: Number(root.rpmValues[motorNode.index] || 0)
                                readonly property real health: Number(root.inverterHealthValues[motorNode.index] || 0)
                                width: 78
                                height: 74
                                x: motorNode.index % 2 === 0 ? propulsionMap.width * 0.14 - width * 0.5 : propulsionMap.width * 0.86 - width * 0.5
                                y: motorNode.index < 2 ? propulsionMap.height * 0.24 - height * 0.5 : propulsionMap.height * 0.76 - height * 0.5

                                Rectangle {
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    width: 46
                                    height: 46
                                    radius: 23
                                    color: "#0e252d"
                                    border.color: motorNode.health >= 90 ? "#67d7cf" : "#ff9f72"
                                    border.width: 2
                                }

                                Item {
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    width: 38
                                    height: 38
                                    rotation: root.rotorPhase * 57.2958 * (0.55 + motorNode.rpm / 2800.0)
                                              * (motorNode.index % 2 === 0 ? 1 : -1)

                                    Rectangle { anchors.centerIn: parent; width: 38; height: 3; radius: 1; color: "#d2f4ff" }
                                    Rectangle { anchors.centerIn: parent; width: 3; height: 38; radius: 1; color: "#d2f4ff" }
                                    Rectangle { anchors.centerIn: parent; width: 9; height: 9; radius: 5; color: "#67d7cf" }
                                }

                                Label {
                                    anchors.bottom: parent.bottom
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    text: "M" + (motorNode.index + 1) + "  " + motorNode.rpm.toFixed(0)
                                    color: "#d8edf7"
                                    font.pixelSize: 9
                                    font.bold: true
                                }
                            }
                        }

                        Rectangle {
                            anchors.centerIn: parent
                            width: 78
                            height: 42
                            color: "#102832"
                            border.color: "#4d879b"
                            border.width: 1
                            Column {
                                anchors.centerIn: parent
                                spacing: 1
                                Label { anchors.horizontalCenter: parent.horizontalCenter; text: "HV BUS"; color: "#7fa8ba"; font.pixelSize: 8 }
                                Text { anchors.horizontalCenter: parent.horizontalCenter; text: Number(root.electricalPowerKw).toFixed(0) + " kW"; color: "#ecf9ff"; font.pixelSize: 14; font.bold: true }
                            }
                        }
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.minimumWidth: 0
                Layout.preferredWidth: root.compact ? dashboardGrid.width : (dashboardGrid.width - dashboardGrid.columnSpacing) / 2
                Layout.preferredHeight: root.topPanelHeight
                color: "#111b20"
                border.color: "#4e6268"
                border.width: 1

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 8

                    Label { text: "VECTOR AND THERMAL CONTROL"; color: "#f2bc6b"; font.pixelSize: 10; font.bold: true }

                    RowLayout {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        spacing: 12

                        Item {
                            Layout.fillWidth: true
                            Layout.minimumWidth: 0
                            Layout.preferredWidth: 1
                            Layout.fillHeight: true

                            Label { anchors.top: parent.top; anchors.horizontalCenter: parent.horizontalCenter; text: "NACELLE ACTUATOR"; color: "#8fa6ae"; font.pixelSize: 8 }

                            Item {
                                anchors.centerIn: parent
                                width: 112
                                height: 112

                                Rectangle {
                                    anchors.centerIn: parent
                                    width: 84
                                    height: 84
                                    radius: 42
                                    color: "transparent"
                                    border.color: "#455b61"
                                    border.width: 8
                                }
                                Rectangle {
                                    anchors.centerIn: parent
                                    width: 8
                                    height: 42
                                    radius: 4
                                    color: "#f2bc6b"
                                    transformOrigin: Item.Bottom
                                    rotation: 90 - Number(root.flightModel.tiltAngleDeg)

                                    Behavior on rotation {
                                        NumberAnimation { duration: 380; easing.type: Easing.OutCubic }
                                    }
                                }
                                Rectangle { anchors.centerIn: parent; width: 14; height: 14; radius: 7; color: "#ffe1ae" }
                            }

                            Text {
                                anchors.bottom: parent.bottom
                                anchors.horizontalCenter: parent.horizontalCenter
                                text: Number(root.flightModel.tiltAngleDeg).toFixed(1) + "°"
                                color: "#ffe4b6"
                                font.pixelSize: 20
                                font.bold: true
                            }
                        }

                        Rectangle { Layout.fillHeight: true; Layout.preferredWidth: 1; color: "#34474d" }

                        ColumnLayout {
                            Layout.fillWidth: true
                            Layout.minimumWidth: 0
                            Layout.preferredWidth: 1
                            Layout.fillHeight: true
                            spacing: 4

                            Label { text: "MOTOR TEMPERATURE BANK"; color: "#8fa6ae"; font.pixelSize: 8 }

                            RowLayout {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                spacing: 8

                                Repeater {
                                    model: 4

                                    ColumnLayout {
                                        id: thermalChannel
                                        required property int index
                                        readonly property real temperature: Number(root.tempValues[thermalChannel.index] || 0)
                                        Layout.fillWidth: true
                                        Layout.fillHeight: true
                                        spacing: 3

                                        Item {
                                            Layout.fillWidth: true
                                            Layout.fillHeight: true

                                            Rectangle {
                                                anchors.horizontalCenter: parent.horizontalCenter
                                                anchors.bottom: parent.bottom
                                                width: 16
                                                height: parent.height
                                                color: "#1a272b"
                                                border.color: "#465c62"
                                            }
                                            Rectangle {
                                                anchors.horizontalCenter: parent.horizontalCenter
                                                anchors.bottom: parent.bottom
                                                width: 12
                                                height: Math.max(4, parent.height * Math.min(1, thermalChannel.temperature / 100.0))
                                                color: thermalChannel.temperature >= 85 ? "#ff785f" : "#f2ad62"

                                                Behavior on height { NumberAnimation { duration: 300 } }
                                            }
                                        }
                                        Label { Layout.alignment: Qt.AlignHCenter; text: "M" + (thermalChannel.index + 1); color: "#8199a1"; font.pixelSize: 8 }
                                        Label { Layout.alignment: Qt.AlignHCenter; text: thermalChannel.temperature.toFixed(0) + "°"; color: "#ffd2a2"; font.pixelSize: 9; font.bold: true }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.minimumWidth: 0
                Layout.preferredWidth: root.compact ? dashboardGrid.width : (dashboardGrid.width - dashboardGrid.columnSpacing) / 2
                Layout.preferredHeight: root.bottomPanelHeight
                color: "#0d1c23"
                border.color: "#31566d"
                border.width: 1

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 8

                    RowLayout {
                        Layout.fillWidth: true
                        Label { text: "THRUST ALLOCATION"; color: "#73d8cb"; font.pixelSize: 10; font.bold: true }
                        Item { Layout.fillWidth: true }
                        Text { text: Number(root.combinedThrust).toFixed(1) + " kN"; color: "#e8fffb"; font.pixelSize: 18; font.bold: true }
                    }

                    Repeater {
                        model: 2

                        ColumnLayout {
                            id: thrustGroup
                            required property int index
                            readonly property real thrust: Number(root.thrustValues[thrustGroup.index] || 0)
                            Layout.fillWidth: true
                            spacing: 3

                            RowLayout {
                                Layout.fillWidth: true
                                spacing: 6
                                Label { text: thrustGroup.index === 0 ? "FRONT ROTOR GROUP" : "REAR ROTOR GROUP"; color: "#91aeb5"; font.pixelSize: 8 }
                                Item { Layout.fillWidth: true }
                                Label {
                                    Layout.preferredWidth: 62
                                    text: thrustGroup.thrust.toFixed(1) + " kN"
                                    color: "#b7f4ea"
                                    font.pixelSize: 10
                                    font.bold: true
                                    horizontalAlignment: Text.AlignRight
                                }
                            }
                            Item {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 22

                                Rectangle { anchors.fill: parent; color: "#14272e"; border.color: "#31515a" }
                                Rectangle {
                                    anchors.left: parent.left
                                    anchors.top: parent.top
                                    anchors.bottom: parent.bottom
                                    anchors.margins: 3
                                    width: Math.max(3, (parent.width - 6) * Math.min(1, thrustGroup.thrust / 14.0))
                                    color: thrustGroup.index === 0 ? "#55c9bd" : "#3fa7c4"

                                    Behavior on width { NumberAnimation { duration: 360; easing.type: Easing.OutCubic } }
                                }
                            }
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        Label { text: "BALANCE"; color: "#78949d"; font.pixelSize: 8 }
                        Item { Layout.fillWidth: true }
                        Label {
                            text: root.propulsionModel.thrustBalanceLabel
                            color: root.propulsionModel.thrustBalanced ? "#69d6a0" : "#f2bc6b"
                            font.pixelSize: 9
                            font.bold: true
                        }
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.minimumWidth: 0
                Layout.preferredWidth: root.compact ? dashboardGrid.width : (dashboardGrid.width - dashboardGrid.columnSpacing) / 2
                Layout.preferredHeight: root.bottomPanelHeight
                color: "#101a22"
                border.color: "#3b5365"
                border.width: 1

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 5

                    RowLayout {
                        Layout.fillWidth: true
                        Label { text: "INVERTER CONTROL"; color: "#8dc8ff"; font.pixelSize: 10; font.bold: true }
                        Item { Layout.fillWidth: true }
                        Label {
                            text: Number(root.electricalPowerKw).toFixed(0) + " kW DC OUTPUT"
                            color: "#b9dcff"
                            font.pixelSize: 9
                            font.bold: true
                        }
                    }

                    GridLayout {
                        id: inverterGrid
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        columns: 2
                        columnSpacing: 6
                        rowSpacing: 6

                        Repeater {
                            model: 4

                            Rectangle {
                                id: inverterTile
                                required property int index
                                readonly property real voltage: Number(root.inverterVoltageValues[inverterTile.index] || 0)
                                readonly property real current: Number(root.inverterCurrentValues[inverterTile.index] || 0)
                                readonly property real health: Number(root.inverterHealthValues[inverterTile.index] || 0)
                                readonly property real powerKw: inverterTile.voltage * inverterTile.current / 1000.0
                                readonly property int loadSegments: Math.round(Math.min(1, inverterTile.current / 220.0) * 10)
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                Layout.minimumWidth: 0
                                Layout.preferredWidth: (inverterGrid.width - inverterGrid.columnSpacing) / 2
                                Layout.preferredHeight: (inverterGrid.height - inverterGrid.rowSpacing) / 2
                                color: inverterTile.index % 2 === 0 ? "#122732" : "#11242e"
                                border.color: inverterTile.health >= 90 ? "#285568" : "#8a493f"
                                border.width: 1

                                ColumnLayout {
                                    anchors.fill: parent
                                    anchors.margins: 6
                                    spacing: 2

                                    RowLayout {
                                        Layout.fillWidth: true
                                        spacing: 5

                                        Rectangle {
                                            Layout.preferredWidth: 6
                                            Layout.preferredHeight: 6
                                            radius: 3
                                            color: inverterTile.health >= 90 ? "#69d6a0" : "#ff846d"
                                        }
                                        Label {
                                            text: "INV-" + (inverterTile.index + 1)
                                            color: "#e5f4fd"
                                            font.pixelSize: 9
                                            font.bold: true
                                        }
                                        Item { Layout.fillWidth: true }
                                        Label {
                                            text: inverterTile.health.toFixed(1) + "%"
                                            color: inverterTile.health >= 90 ? "#75e5ae" : "#ff9f72"
                                            font.pixelSize: 9
                                            font.bold: true
                                        }
                                    }

                                    RowLayout {
                                        Layout.fillWidth: true
                                        spacing: 4

                                        Label { text: inverterTile.voltage.toFixed(0); color: "#d5ebf8"; font.pixelSize: 12; font.bold: true }
                                        Label { text: "V"; color: "#6f93a7"; font.pixelSize: 7 }
                                        Rectangle { Layout.preferredWidth: 1; Layout.preferredHeight: 12; color: "#31505e" }
                                        Label { text: inverterTile.current.toFixed(0); color: "#d5ebf8"; font.pixelSize: 12; font.bold: true }
                                        Label { text: "A"; color: "#6f93a7"; font.pixelSize: 7 }
                                        Item { Layout.fillWidth: true }
                                        Label { text: inverterTile.powerKw.toFixed(0) + " kW"; color: "#75cfff"; font.pixelSize: 9; font.bold: true }
                                    }

                                    Row {
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: 6
                                        spacing: 2

                                        Repeater {
                                            model: 10

                                            Rectangle {
                                                id: powerSegment
                                                required property int index
                                                width: Math.max(2, (powerSegment.parent.width - 18) / 10)
                                                height: 6
                                                radius: 1
                                                color: powerSegment.index < inverterTile.loadSegments ? "#52bde9" : "#1b3946"
                                                opacity: powerSegment.index === Math.floor(root.powerFlowPhase * 10) ? 1.0 : 0.68
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
