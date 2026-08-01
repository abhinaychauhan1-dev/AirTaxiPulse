import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import "features"

ApplicationWindow {
    id: window
    visible: true
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
        return Math.max(700, Math.round(availableHeight * 0.88))
    }
    title: qsTr("Air Taxi eVTOL Pulse")

    property int currentIndex: 0
    property date currentDateTime: new Date()
    property int elapsedSeconds: 0
    property real headerPhase: 0.0
    property bool simPanelHovered: false
    property bool simDetailedMode: false

    readonly property int simCruiseKnots: 120 + Math.round(Math.sin(window.headerPhase * 0.9) * 18)
    readonly property int simAltitudeFeet: 1800 + Math.round((Math.sin(window.headerPhase * 0.55) + 1) * 1500)

    readonly property string currentDateText: Qt.formatDate(window.currentDateTime, "ddd, dd MMM yyyy")
    readonly property string currentTimeText: Qt.formatTime(window.currentDateTime, "hh:mm:ss")
    readonly property string elapsedText: {
        var total = Math.max(0, window.elapsedSeconds)
        var hours = Math.floor(total / 3600)
        var minutes = Math.floor((total % 3600) / 60)
        var seconds = total % 60
        function pad2(value) {
            return value < 10 ? "0" + value : "" + value
        }
        return pad2(hours) + ":" + pad2(minutes) + ":" + pad2(seconds)
    }

    Timer {
        interval: 1000
        running: true
        repeat: true
        onTriggered: {
            window.currentDateTime = new Date()
            window.elapsedSeconds += 1
        }
    }

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
        color: "#1f2833"
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            width: parent.width
            height: 80
            color: "#0b1620"
            border.color: "#4b79a1"
            border.width: 1

            RowLayout {
                anchors.fill: parent
                anchors.margins: 14
                spacing: 14

                Rectangle {
                    Layout.preferredWidth: 165
                    Layout.fillHeight: true
                    radius: 10
                    color: "#102131"
                    border.color: "#2f4f67"
                    border.width: 1

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 8
                        spacing: 2

                        Label {
                            text: qsTr("Date")
                            color: "#8fb0c8"
                            font.pixelSize: 9
                        }
                        Label {
                            text: window.currentDateText
                            color: "#d8ebfa"
                            font.pixelSize: 11
                            font.bold: true
                        }
                        Label {
                            text: window.currentTimeText + "   ETD " + window.elapsedText
                            color: "#9dc6e3"
                            font.pixelSize: 10
                        }
                    }
                }

                ColumnLayout {
                    spacing: 1
                    Label {
                        text: qsTr("LIVE OPERATIONS")
                        color: "#78b6dd"
                        font.pixelSize: 9
                        font.bold: true
                    }
                    Label {
                        text: qsTr("Air Taxi eVTOL Pulse")
                        font.pixelSize: 23
                        font.bold: true
                        color: "#ade8f4"
                    }
                    Label {
                        text: qsTr("Real-time mission, safety, and route status overview")
                        color: "#c8d8e4"
                        font.pixelSize: 12
                    }
                }

                Item { Layout.fillWidth: true }

                Rectangle {
                    id: simPanel
                    Layout.preferredWidth: 220
                    Layout.preferredHeight: 60
                    radius: 10
                    color: window.simPanelHovered ? "#122a3d" : "#102131"
                    border.color: window.simPanelHovered ? "#5f97bf" : "#2f4f67"
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
                        anchors.topMargin: 16
                        anchors.bottomMargin: 7
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

                    Rectangle {
                        anchors.left: parent.left
                        anchors.leftMargin: 10
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 4
                        radius: 5
                        color: "#18344a"
                        border.color: "#335a76"
                        border.width: 1
                        width: 78
                        height: 15

                        Label {
                            anchors.centerIn: parent
                            text: "SPD " + window.simCruiseKnots + " kt"
                            color: "#cde7fb"
                            font.pixelSize: 8
                        }
                    }

                    Rectangle {
                        anchors.left: parent.left
                        anchors.leftMargin: 92
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 4
                        radius: 5
                        color: "#1c3448"
                        border.color: "#3a607a"
                        border.width: 1
                        width: 94
                        height: 15

                        Label {
                            anchors.centerIn: parent
                            text: "ALT " + window.simAltitudeFeet + " ft"
                            color: "#d9edff"
                            font.pixelSize: 8
                        }
                    }

                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#16222a"
            border.color: "#4b79a1"
            border.width: 1
            anchors.margins: 10
            anchors.leftMargin: 10
            anchors.rightMargin: 10
            anchors.topMargin: 8
            anchors.bottomMargin: 10

            ColumnLayout {
                anchors.fill: parent
                spacing: 3

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 6
                    Layout.margins: 3
                    Layout.minimumHeight: 58

                    Repeater {
                        model: [
                            airTaxiModules.primaryFlight.title,
                            airTaxiModules.propulsion.title,
                            airTaxiModules.energy.title,
                            airTaxiModules.fcs.title,
                            airTaxiModules.safety.title
                        ]
                        TabButton {
                            text: modelData
                            checked: window.currentIndex === index
                            onClicked: window.currentIndex = index
                            font.pixelSize: 12
                            Layout.fillWidth: true
                            Layout.preferredWidth: 176
                            Layout.minimumWidth: 136
                            Layout.maximumWidth: 250
                            Layout.alignment: Qt.AlignVCenter
                            Layout.minimumHeight: 42
                            background: Rectangle {
                                anchors.fill: parent
                                radius: 14
                                color: checked ? "#3d6f94" : "#122238"
                                border.color: checked ? "#81b6dc" : "#2f4961"
                                border.width: 1
                            }
                            contentItem: Text {
                                text: parent.text
                                color: parent.checked ? "#ffffff" : "#d4e6f4"
                                font.pixelSize: 11
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

                    PrimaryFlightPage {
                        flightModel: airTaxiModules.primaryFlight
                        missionTimeText: window.currentTimeText
                        missionDateText: window.currentDateText
                        missionEtaText: window.elapsedText
                    }
                    PropulsionPage { moduleRegistry: airTaxiModules }
                    EnergyPage { moduleRegistry: airTaxiModules }
                    FcsPage { moduleRegistry: airTaxiModules }
                    SafetyPage { moduleRegistry: airTaxiModules }
                }
            }
        }
    }
}
