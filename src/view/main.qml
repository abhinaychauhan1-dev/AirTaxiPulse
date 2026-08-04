import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import "features"

ApplicationWindow {
    id: window
    visible: true
    minimumWidth: 1024
    minimumHeight: 680
    width: {
        var availableWidth = (window.screen && window.screen.availableGeometry)
            ? window.screen.availableGeometry.width
            : Screen.desktopAvailableWidth
        return Math.max(1024, Math.round(availableWidth * 0.9))
    }
    height: {
        var availableHeight = (window.screen && window.screen.availableGeometry)
            ? window.screen.availableGeometry.height
            : Screen.desktopAvailableHeight
        return Math.max(680, Math.round(availableHeight * 0.87))
    }
    title: qsTr("Air Taxi eVTOL Pulse")

    property int currentIndex: 0
    property real headerPhase: 0.0
    property bool simPanelHovered: false
    property bool simDetailedMode: false
    readonly property bool mqttAvailable: (typeof mqttTelemetryAvailable !== "undefined") && mqttTelemetryAvailable
    readonly property bool mqttConnected: mqttAvailable && mqttTelemetry ? mqttTelemetry.connected : false
    readonly property int mqttParserWorkers: mqttAvailable && mqttTelemetry ? mqttTelemetry.maxConcurrentParsers : 0
    readonly property int mqttPendingCap: mqttAvailable && mqttTelemetry ? mqttTelemetry.maxPendingMessages : 0
    readonly property int mqttDroppedCount: mqttAvailable && mqttTelemetry ? mqttTelemetry.droppedMessageCount : 0
    readonly property real motorTempAverage: airTaxiModules.primaryFlight.averageMotorTemperature
    readonly property string gpsText: airTaxiModules.primaryFlight.gpsText

    readonly property int simCruiseKnots: Math.round(airTaxiModules.primaryFlight.tas)
    readonly property int simAltitudeFeet: Math.round(airTaxiModules.primaryFlight.altBaro)

    readonly property string currentDateText: airTaxiModules.session.currentDateText
    readonly property string currentTimeText: airTaxiModules.session.currentTimeText
    readonly property string elapsedText: airTaxiModules.session.elapsedText

    Timer {
        interval: 60
        running: true
        repeat: true
        onTriggered: {
            window.headerPhase += 0.05
            if (window.headerPhase > Math.PI * 2) {
                window.headerPhase -= Math.PI * 2
            }
            airTaxiHeaderCanvas.requestPaint()
        }
    }

    Rectangle {
        anchors.fill: parent
        color: "#05090c"
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 96
            color: "#080f13"
            border.color: "#2c3b42"
            border.width: 1

            RowLayout {
                anchors.fill: parent
                anchors.margins: 8
                spacing: 12

                Rectangle {
                    Layout.preferredWidth: 154
                    Layout.preferredHeight: 72
                    Layout.alignment: Qt.AlignVCenter
                    radius: 2
                    color: "#0b151a"
                    border.color: "#304047"
                    border.width: 1

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 8
                        spacing: 3

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 6

                            Label {
                                text: qsTr("DATE")
                                color: "#8fb0c8"
                                font.pixelSize: 9
                                Layout.preferredWidth: 28
                            }

                            Label {
                                text: window.currentDateText
                                color: "#d8ebfa"
                                font.pixelSize: 10
                                font.bold: true
                                Layout.fillWidth: true
                                elide: Label.ElideRight
                            }
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 6

                            Label {
                                text: qsTr("TIME")
                                color: "#8fb0c8"
                                font.pixelSize: 9
                                Layout.preferredWidth: 28
                            }

                            Label {
                                text: window.currentTimeText
                                color: "#9dc6e3"
                                font.pixelSize: 9
                                Layout.fillWidth: true
                            }
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 6

                            Label {
                                text: qsTr("ETD")
                                color: "#8fb0c8"
                                font.pixelSize: 9
                                Layout.preferredWidth: 28
                            }

                            Label {
                                text: window.elapsedText
                                color: "#9dc6e3"
                                font.pixelSize: 9
                                Layout.fillWidth: true
                            }
                        }
                    }
                }

                ColumnLayout {
                    Layout.alignment: Qt.AlignVCenter
                    spacing: 1
                    Label {
                        text: qsTr("AIR TAXI  •  VEHICLE 01  •  SIMULATION")
                        color: "#5ed8ff"
                        font.pixelSize: 8
                        font.bold: true
                        font.letterSpacing: 1
                    }
                    Label {
                        text: qsTr("eVTOL FLIGHT DECK")
                        font.pixelSize: 21
                        font.bold: true
                        color: "#e8f7fc"
                    }
                    Label {
                        text: qsTr("PRIMARY SYSTEMS / TELEMETRY / MISSION CONTROL")
                        color: "#718892"
                        font.pixelSize: 9
                        font.letterSpacing: 1
                    }
                }

                Item { Layout.fillWidth: true }

                Rectangle {
                    Layout.preferredWidth: 328
                    Layout.preferredHeight: 76
                    Layout.alignment: Qt.AlignVCenter
                    radius: 2
                    color: "#0a1419"
                    border.color: mqttConnected ? "#65f5a5" : "#3b4d55"
                    border.width: 1

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 8
                        spacing: 3

                        RowLayout {
                            Layout.fillWidth: true

                            Label {
                                text: qsTr("MQTT TELEMETRY")
                                color: "#9bc1df"
                                font.pixelSize: 9
                                font.bold: true
                            }

                            Rectangle {
                                Layout.alignment: Qt.AlignVCenter
                                Layout.preferredWidth: 8
                                Layout.preferredHeight: 8
                                radius: 4
                                color: mqttConnected ? "#7ee082" : (mqttAvailable ? "#ffb26f" : "#8a98a8")
                                border.color: "#d8ebfa"
                                border.width: 1
                            }

                            Item { Layout.fillWidth: true }

                            Label {
                                text: mqttConnected ? qsTr("ONLINE") : (mqttAvailable ? qsTr("OFFLINE") : qsTr("UNAVAILABLE"))
                                color: mqttConnected ? "#9af0a8" : "#d0dfec"
                                font.pixelSize: 9
                                font.bold: true
                            }
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 6

                            Label {
                                text: "SoC"
                                color: "#8fb0c8"
                                font.pixelSize: 9
                                Layout.preferredWidth: 26
                            }

                            Label {
                                text: Math.round(airTaxiModules.primaryFlight.batterySoc) + "%"
                                color: "#d7e9f9"
                                font.pixelSize: 9
                                Layout.preferredWidth: 42
                            }

                            Label {
                                text: "MTR"
                                color: "#8fb0c8"
                                font.pixelSize: 9
                                Layout.preferredWidth: 28
                            }

                            Label {
                                text: Math.round(window.motorTempAverage) + " C"
                                color: "#d7e9f9"
                                font.pixelSize: 9
                                Layout.fillWidth: true
                                elide: Label.ElideRight
                            }
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 6

                            Label {
                                text: "GPS"
                                color: "#8fb0c8"
                                font.pixelSize: 9
                                Layout.preferredWidth: 26
                            }

                            Label {
                                text: window.gpsText
                                color: "#b4d3ea"
                                font.pixelSize: 9
                                Layout.fillWidth: true
                                elide: Label.ElideRight
                            }
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 4

                            Label {
                                text: "P"
                                color: "#8fb0c8"
                                font.pixelSize: 9
                                Layout.preferredWidth: 10
                            }

                            Label {
                                text: String(window.mqttParserWorkers)
                                color: "#9fc5e1"
                                font.pixelSize: 9
                                Layout.preferredWidth: 20
                            }

                            Label {
                                text: "Q"
                                color: "#8fb0c8"
                                font.pixelSize: 9
                                Layout.preferredWidth: 10
                            }

                            Label {
                                text: String(window.mqttPendingCap)
                                color: "#9fc5e1"
                                font.pixelSize: 9
                                Layout.preferredWidth: 36
                            }

                            Label {
                                text: "D"
                                color: "#8fb0c8"
                                font.pixelSize: 9
                                Layout.preferredWidth: 10
                            }

                            Label {
                                text: String(window.mqttDroppedCount)
                                color: "#9fc5e1"
                                font.pixelSize: 9
                                Layout.fillWidth: true
                                elide: Label.ElideRight
                            }

                            ToolButton {
                                text: "-P"
                                enabled: mqttAvailable
                                onClicked: {
                                    if (mqttTelemetry)
                                        mqttTelemetry.decreaseParserConcurrency()
                                }
                            }

                            ToolButton {
                                text: "+P"
                                enabled: mqttAvailable
                                onClicked: {
                                    if (mqttTelemetry)
                                        mqttTelemetry.increaseParserConcurrency()
                                }
                            }

                            ToolButton {
                                text: "+Q"
                                enabled: mqttAvailable
                                onClicked: {
                                    if (mqttTelemetry)
                                        mqttTelemetry.increasePendingCapacity()
                                }
                            }
                        }
                    }
                }

                Rectangle {
                    id: simPanel
                    Layout.preferredWidth: 210
                    Layout.preferredHeight: 72
                    Layout.alignment: Qt.AlignVCenter
                    radius: 2
                    color: window.simPanelHovered ? "#10242c" : "#0a151a"
                    border.color: window.simPanelHovered ? "#54d8ff" : "#304047"
                    border.width: 1
                    scale: window.simPanelHovered ? 1.02 : 1.0

                    Behavior on scale {
                        NumberAnimation {
                            duration: 140
                            easing.type: Easing.OutCubic
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onEntered: window.simPanelHovered = true
                        onExited: window.simPanelHovered = false
                        onClicked: window.simDetailedMode = !window.simDetailedMode
                    }

                    Canvas {
                        id: airTaxiHeaderCanvas
                        anchors.fill: parent
                        anchors.leftMargin: 7
                        anchors.rightMargin: 7
                        anchors.topMargin: 18
                        anchors.bottomMargin: 22
                        antialiasing: true

                        onPaint: {
                            var ctx = getContext("2d")
                            ctx.clearRect(0, 0, width, height)

                            var yMid = height * 0.62
                            var phase = window.headerPhase

                            ctx.strokeStyle = "rgba(120, 186, 232, 0.28)"
                            ctx.lineWidth = 1
                            ctx.beginPath()
                            ctx.moveTo(0, yMid)
                            ctx.lineTo(width, yMid)
                            ctx.stroke()

                            for (var i = 0; i <= 10; ++i) {
                                var tickX = i * (width / 10)
                                var tickH = i % 2 === 0 ? 6 : 3
                                ctx.beginPath()
                                ctx.moveTo(tickX, yMid)
                                ctx.lineTo(tickX, yMid - tickH)
                                ctx.stroke()
                            }

                            var taxiX = width * 0.5 + Math.sin(phase * 0.8) * (width * 0.22)
                            var taxiY = yMid - 4 - Math.sin(phase * 1.2) * 1.2

                            ctx.strokeStyle = "#87c8f4"
                            ctx.lineWidth = 1.6
                            ctx.beginPath()
                            ctx.moveTo(taxiX - 14, taxiY)
                            ctx.lineTo(taxiX + 14, taxiY)
                            ctx.moveTo(taxiX - 9, taxiY + 3)
                            ctx.lineTo(taxiX + 9, taxiY + 3)
                            ctx.moveTo(taxiX, taxiY - 7)
                            ctx.lineTo(taxiX, taxiY + 5)
                            ctx.stroke()

                            var rotorPulse = 5 + Math.max(0, Math.sin(phase * 2.1)) * 1.0
                            ctx.beginPath()
                            ctx.arc(taxiX, taxiY - 7, rotorPulse, 0, Math.PI * 2)
                            ctx.strokeStyle = "rgba(135, 200, 244, 0.35)"
                            ctx.lineWidth = 1
                            ctx.stroke()

                            var markerX = width * 0.08 + (phase / (Math.PI * 2)) * (width * 0.84)
                            ctx.beginPath()
                            ctx.arc(markerX, yMid + 8, 2.2, 0, Math.PI * 2)
                            ctx.fillStyle = "#7ee082"
                            ctx.fill()

                            if (window.simDetailedMode) {
                                var marker2X = width * 0.08 + ((phase * 1.35) % (Math.PI * 2)) / (Math.PI * 2) * (width * 0.84)
                                ctx.beginPath()
                                ctx.arc(marker2X, yMid + 4, 1.8, 0, Math.PI * 2)
                                ctx.fillStyle = "#ffb26f"
                                ctx.fill()

                                ctx.strokeStyle = "rgba(255, 178, 111, 0.35)"
                                ctx.lineWidth = 1
                                ctx.setLineDash([3, 2])
                                ctx.beginPath()
                                ctx.moveTo(marker2X, yMid + 4)
                                ctx.lineTo(taxiX, taxiY)
                                ctx.stroke()
                                ctx.setLineDash([])
                            }
                        }
                    }

                    Label {
                        anchors.left: parent.left
                        anchors.leftMargin: 10
                        anchors.top: parent.top
                        anchors.topMargin: 4
                        text: qsTr("SIM TRAJECTORY")
                        color: "#8fb0c8"
                        font.pixelSize: 9
                        font.bold: true
                    }

                    Label {
                        anchors.right: parent.right
                        anchors.rightMargin: 10
                        anchors.top: parent.top
                        anchors.topMargin: 4
                        text: window.simDetailedMode ? qsTr("DETAIL") : qsTr("STD")
                        color: window.simDetailedMode ? "#ffcc95" : "#9bc1df"
                        font.pixelSize: 8
                    }

                    RowLayout {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.bottom: parent.bottom
                        anchors.leftMargin: 10
                        anchors.rightMargin: 10
                        anchors.bottomMargin: 4
                        spacing: 4

                        Rectangle {
                            radius: 5
                            color: "#18344a"
                            border.color: "#335a76"
                            border.width: 1
                            Layout.preferredWidth: 84
                            Layout.preferredHeight: 15

                            Label {
                                anchors.centerIn: parent
                                text: "SPD " + window.simCruiseKnots + " kt"
                                color: "#cde7fb"
                                font.pixelSize: 8
                            }
                        }

                        Rectangle {
                            radius: 5
                            color: "#1c3448"
                            border.color: "#3a607a"
                            border.width: 1
                            Layout.preferredWidth: 98
                            Layout.preferredHeight: 15

                            Label {
                                anchors.centerIn: parent
                                text: "ALT " + window.simAltitudeFeet + " ft"
                                color: "#d9edff"
                                font.pixelSize: 8
                            }
                        }

                        Item { Layout.fillWidth: true }
                    }

                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#070d10"
            border.color: "#2c3b42"
            border.width: 1
            Layout.leftMargin: 6
            Layout.rightMargin: 6
            Layout.topMargin: 6
            Layout.bottomMargin: 6

            ColumnLayout {
                anchors.fill: parent
                spacing: 1

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 2
                    Layout.margins: 3
                    Layout.minimumHeight: 42

                    Repeater {
                        model: [
                            airTaxiModules.primaryFlight.title,
                            airTaxiModules.propulsion.title,
                            airTaxiModules.energy.title,
                            airTaxiModules.fcs.title,
                            airTaxiModules.safety.title
                        ]
                        TabButton {
                            id: moduleTab
                            text: modelData
                            checked: window.currentIndex === index
                            onClicked: window.currentIndex = index
                            font.pixelSize: 12
                            Layout.fillWidth: true
                            Layout.preferredWidth: 176
                            Layout.minimumWidth: 136
                            Layout.maximumWidth: 250
                            Layout.alignment: Qt.AlignVCenter
                            Layout.minimumHeight: 36
                            background: Rectangle {
                                anchors.fill: parent
                                radius: 1
                                color: moduleTab.checked ? "#102d38" : "#091318"
                                border.color: moduleTab.checked ? "#54d8ff" : "#27363d"
                                border.width: 1

                                Rectangle {
                                    anchors.left: parent.left
                                    anchors.right: parent.right
                                    anchors.bottom: parent.bottom
                                    height: 2
                                    color: moduleTab.checked ? "#54d8ff" : "transparent"
                                }
                            }
                            contentItem: Text {
                                text: moduleTab.text
                                color: moduleTab.checked ? "#dff8ff" : "#81949d"
                                font.pixelSize: 10
                                font.bold: moduleTab.checked
                                font.letterSpacing: 1
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                wrapMode: Text.WordWrap
                                anchors.fill: parent
                                anchors.margins: 10
                            }
                        }
                    }
                }

                StackLayout {
                    id: pageStack
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    currentIndex: window.currentIndex

                    PrimaryFlightInstrumentsPage {
                        flightModel: airTaxiModules.primaryFlight
                        missionTimeText: window.currentTimeText
                        missionDateText: window.currentDateText
                        missionEtaText: window.elapsedText
                    }
                    DepPropulsionPage { moduleRegistry: airTaxiModules }
                    EnergySystemsPage { moduleRegistry: airTaxiModules }
                    FcsPage { moduleRegistry: airTaxiModules }
                    SafetyPage { moduleRegistry: airTaxiModules }
                }
            }
        }
    }
}
