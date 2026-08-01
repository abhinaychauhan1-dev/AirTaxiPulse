import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    required property var flightModel

    property int cardColumns: width > 960 ? 3 : (width > 700 ? 2 : 1)

    property real cas: flightModel.cas
    property real tas: flightModel.tas
    property real altBaro: flightModel.altBaro
    property real altRadar: flightModel.altRadar
    property real vs: flightModel.vs
    property real pitch: flightModel.pitch
    property real roll: flightModel.roll
    property real yaw: flightModel.yaw
    property real heading: flightModel.heading
    property real track: flightModel.track

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 6

        Text {
            text: "Primary Flight & Navigation Data"
            font.pixelSize: 20
            font.bold: true
            color: "#ffffff"
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }

        Text {
            text: "Simulated flight instrumentation for airspeed, altitude, attitude, heading, track and flight path guidance."
            font.pixelSize: 10
            color: "#d7e3ef"
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 26
            radius: 14
            color: "#142636"
            border.color: "#2f4f67"
            border.width: 1

            RowLayout {
                anchors.fill: parent
                anchors.margins: 7
                spacing: 8

                Rectangle {
                    width: 10
                    height: 10
                    radius: 5
                    color: flightModel.vsAccentColor
                }

                Text {
                    text: flightModel.flightModeLabel
                    color: "#eaf4ff"
                    font.pixelSize: 10
                    font.bold: true
                }

                Item { Layout.fillWidth: true }

                Text {
                    text: flightModel.vsValueText
                    color: flightModel.vsAccentColor
                    font.pixelSize: 10
                    font.bold: true
                }
            }
        }

        GridLayout {
            id: cardGrid
            columns: cardColumns
            Layout.fillWidth: true
            Layout.fillHeight: true
            rowSpacing: 6
            columnSpacing: 8

            FlightSimulationCard {
                cardTitle: "Airspeed (CAS / TAS)"
                cardSubtitle: "Live speed trend and control margin"
                iconType: "speed"
                accentColor: "#72b8e6"
                motionSeed: 0.15
                motionRate: 0.021
                progressValue: flightModel.casProgress
                progressLabel: "CAS / TAS dynamics"
                sparkValues: flightModel.casHistory
                Layout.fillWidth: true
                Layout.preferredWidth: Math.max(220, Math.min(320, cardGrid.width / cardColumns - 12))
                Layout.preferredHeight: 178
                Layout.alignment: Qt.AlignTop

                RowLayout {
                    spacing: 10
                    Label { text: "CAS"; color: "#9ec4f5"; font.pixelSize: 10 }
                    Text { text: Number(cas).toFixed(0) + " kt"; color: "#e1f3ff"; font.pixelSize: 16; font.bold: true }
                }
                RowLayout {
                    spacing: 10
                    Label { text: "TAS"; color: "#9ec4f5"; font.pixelSize: 10 }
                    Text { text: Number(tas).toFixed(0) + " kt"; color: "#e1f3ff"; font.pixelSize: 16; font.bold: true }
                }
            }

            FlightSimulationCard {
                cardTitle: "Altitude"
                cardSubtitle: "Barometric and radar altitude"
                iconType: "altitude"
                accentColor: "#88c5ff"
                motionSeed: 1.35
                motionRate: 0.029
                progressValue: flightModel.altProgress
                progressLabel: "Altitude envelope"
                sparkValues: flightModel.altHistory
                Layout.fillWidth: true
                Layout.preferredWidth: Math.max(220, Math.min(320, cardGrid.width / cardColumns - 12))
                Layout.preferredHeight: 178
                Layout.alignment: Qt.AlignTop

                RowLayout {
                    spacing: 10
                    Label { text: "Baro"; color: "#9ec4f5"; font.pixelSize: 10 }
                    Text { text: Number(altBaro).toFixed(0) + " ft"; color: "#e1f3ff"; font.pixelSize: 16; font.bold: true }
                }
                RowLayout {
                    spacing: 10
                    Label { text: "Radar"; color: "#9ec4f5"; font.pixelSize: 10 }
                    Text { text: Number(altRadar).toFixed(0) + " ft"; color: "#e1f3ff"; font.pixelSize: 16; font.bold: true }
                }
            }

            FlightSimulationCard {
                cardTitle: "Vertical Speed"
                cardSubtitle: "Climb and descent rate"
                iconType: "vertical"
                accentColor: flightModel.vsAccentColor
                motionSeed: 2.05
                motionRate: 0.043
                progressValue: flightModel.vsProgress
                progressLabel: flightModel.vsTrendLabel
                sparkValues: flightModel.vsHistory
                Layout.fillWidth: true
                Layout.preferredWidth: Math.max(220, Math.min(320, cardGrid.width / cardColumns - 12))
                Layout.preferredHeight: 178
                Layout.alignment: Qt.AlignTop

                Text { text: flightSimulation.vsValueText; color: "#e1f3ff"; font.pixelSize: 17; font.bold: true }
                Text { text: flightSimulation.vsTrendLabel; font.pixelSize: 11; color: "#9ec4f5" }
            }

            FlightSimulationCard {
                cardTitle: "Attitude Indicator"
                cardSubtitle: "Pitch, roll and yaw stability"
                iconType: "attitude"
                accentColor: "#74b2e0"
                motionSeed: 3.0
                motionRate: 0.037
                progressValue: flightModel.attitudeProgress
                progressLabel: "Attitude load"
                sparkValues: flightModel.attitudeHistory
                Layout.fillWidth: true
                Layout.preferredWidth: Math.max(220, Math.min(320, cardGrid.width / cardColumns - 12))
                Layout.preferredHeight: 188
                Layout.alignment: Qt.AlignTop

                RowLayout {
                    spacing: 8
                    ColumnLayout {
                        spacing: 2
                        Text { text: "Pitch"; color: "#9ec4f5"; font.pixelSize: 10 }
                        Text { text: Number(pitch).toFixed(1) + "°"; color: "#e1f3ff"; font.pixelSize: 14; font.bold: true }
                    }
                    ColumnLayout {
                        spacing: 2
                        Text { text: "Roll"; color: "#9ec4f5"; font.pixelSize: 10 }
                        Text { text: Number(roll).toFixed(1) + "°"; color: "#e1f3ff"; font.pixelSize: 14; font.bold: true }
                    }
                    ColumnLayout {
                        spacing: 2
                        Text { text: "Yaw"; color: "#9ec4f5"; font.pixelSize: 10 }
                        Text { text: Number(yaw).toFixed(1) + "°"; color: "#e1f3ff"; font.pixelSize: 14; font.bold: true }
                    }
                }
                Rectangle {
                    Layout.fillWidth: true
                    height: 40
                    color: "#0f1f2e"
                    radius: 8
                    border.color: "#2f4f65"
                    border.width: 1
                    Row {
                        anchors.centerIn: parent
                        spacing: 3
                        Rectangle { width: 8; height: 20; color: "#74b2e0"; radius: 4 }
                        Text { text: "Horizon"; color: "#9ec4f5"; font.pixelSize: 11 }
                    }
                }
            }

            FlightSimulationCard {
                cardTitle: "Heading & Track"
                cardSubtitle: "Directional control and drift"
                iconType: "heading"
                accentColor: "#9ec4f5"
                motionSeed: 4.1
                motionRate: 0.019
                progressValue: flightModel.headingProgress
                progressLabel: "Track alignment"
                sparkValues: flightModel.headingHistory
                Layout.fillWidth: true
                Layout.preferredWidth: Math.max(220, Math.min(320, cardGrid.width / cardColumns - 12))
                Layout.preferredHeight: 178
                Layout.alignment: Qt.AlignTop

                RowLayout {
                    spacing: 8
                    ColumnLayout {
                        spacing: 2
                        Text { text: "Heading"; color: "#9ec4f5"; font.pixelSize: 10 }
                        Text { text: Number(heading).toFixed(0) + "°M"; color: "#e1f3ff"; font.pixelSize: 14; font.bold: true }
                    }
                    ColumnLayout {
                        spacing: 2
                        Text { text: "Track"; color: "#9ec4f5"; font.pixelSize: 10 }
                        Text { text: Number(track).toFixed(0) + "°"; color: "#e1f3ff"; font.pixelSize: 14; font.bold: true }
                    }
                }
            }

            FlightSimulationCard {
                cardTitle: "Flight Path Vector (FPV)"
                cardSubtitle: "Trajectory and path guidance"
                iconType: "fpv"
                accentColor: "#6ec8ff"
                motionSeed: 5.2
                motionRate: 0.051
                progressValue: flightModel.fpvProgress
                progressLabel: "Path deviation"
                sparkValues: flightModel.fpvHistory
                Layout.fillWidth: true
                Layout.preferredWidth: Math.max(220, Math.min(320, cardGrid.width / cardColumns - 12))
                Layout.preferredHeight: 178
                Layout.alignment: Qt.AlignTop

                Rectangle {
                    Layout.fillWidth: true
                    height: 80
                    color: "#0f1f2e"
                    radius: 8
                    border.width: 1
                    border.color: "#2f4f65"
                    Column {
                        anchors.fill: parent
                        anchors.margins: 8
                        spacing: 6
                        Rectangle { anchors.horizontalCenter: parent.horizontalCenter; width: 100; height: 4; color: "#74b2e0"; radius: 2 }
                        Text {
                            text: "Trajectory marker aligned with current attitude and track."
                            color: "#9ec4f5"
                            font.pixelSize: 11
                            wrapMode: Text.WordWrap
                            anchors.horizontalCenter: parent.horizontalCenter
                            width: parent.width * 0.9
                        }
                    }
                }
            }
        }
    }
}
