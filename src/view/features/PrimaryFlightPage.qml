import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    required property var flightModel
    property string missionTimeText: ""
    property string missionDateText: ""
    property string missionEtaText: ""

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
    implicitHeight: primaryContent.implicitHeight + 16

    ColumnLayout {
        id: primaryContent
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 6
        spacing: 4

        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Text {
                text: "Air Taxi Flight Overview"
                font.pixelSize: 18
                font.bold: true
                color: "#ffffff"
                wrapMode: Text.WordWrap
            }

            Item { Layout.fillWidth: true }

            Rectangle {
                Layout.preferredHeight: 28
                Layout.preferredWidth: 410
                radius: 8
                color: "#112637"
                border.color: "#2f4f67"
                border.width: 1

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 6
                    spacing: 8

                    Label {
                        text: "Phase: " + flightModel.flightModeLabel
                        color: "#d7e9f8"
                        font.pixelSize: 10
                    }

                    Label {
                        text: "Date: " + missionDateText
                        color: "#9fd0f0"
                        font.pixelSize: 10
                    }

                    Label {
                        text: "Time: " + missionTimeText
                        color: "#d7e9f8"
                        font.pixelSize: 10
                    }

                    Item { Layout.fillWidth: true }

                    Label {
                        text: "ETA: " + missionEtaText
                        color: "#7ee082"
                        font.pixelSize: 10
                        font.bold: true
                    }
                }
            }
        }

        Text {
            text: "Live trip status for takeoff, climb, cruise and approach with speed, altitude and route confidence."
            font.pixelSize: 10
            color: "#d7e3ef"
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 24
            radius: 14
            color: "#142636"
            border.color: "#2f4f67"
            border.width: 1

            RowLayout {
                anchors.fill: parent
                anchors.margins: 6
                spacing: 7

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
            Layout.fillHeight: false
            Layout.alignment: Qt.AlignTop
            rowSpacing: 6
            columnSpacing: 6

            FlightSimulationCard {
                cardTitle: "Trip Speed"
                cardSubtitle: "Lift-off and cruise speed during your Air Taxi ride"
                iconType: "speed"
                accentColor: "#72b8e6"
                motionSeed: 0.15
                motionRate: 0.021
                progressValue: flightModel.casProgress
                progressLabel: "Speed confidence"
                sparkValues: flightModel.casHistory
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredWidth: Math.max(220, Math.min(320, cardGrid.width / cardColumns - 12))
                Layout.preferredHeight: Math.max(200, implicitHeight)
                Layout.minimumHeight: 200

                RowLayout {
                    spacing: 8
                    Label { text: "Lift-off"; color: "#9ec4f5"; font.pixelSize: 9 }
                    Text { text: Number(cas).toFixed(0) + " kt"; color: "#e1f3ff"; font.pixelSize: 16; font.bold: true }
                }
                RowLayout {
                    spacing: 8
                    Label { text: "Cruise"; color: "#9ec4f5"; font.pixelSize: 9 }
                    Text { text: Number(tas).toFixed(0) + " kt"; color: "#e1f3ff"; font.pixelSize: 16; font.bold: true }
                }
            }

            FlightSimulationCard {
                cardTitle: "Altitude Window"
                cardSubtitle: "Current cabin altitude profile and clearance"
                iconType: "altitude"
                accentColor: "#88c5ff"
                motionSeed: 1.35
                motionRate: 0.029
                progressValue: flightModel.altProgress
                progressLabel: "Altitude confidence"
                sparkValues: flightModel.altHistory
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredWidth: Math.max(220, Math.min(320, cardGrid.width / cardColumns - 12))
                Layout.preferredHeight: Math.max(200, implicitHeight)
                Layout.minimumHeight: 200

                RowLayout {
                    spacing: 8
                    Label { text: "Flight level"; color: "#9ec4f5"; font.pixelSize: 9 }
                    Text { text: Number(altBaro).toFixed(0) + " ft"; color: "#e1f3ff"; font.pixelSize: 16; font.bold: true }
                }
                RowLayout {
                    spacing: 8
                    Label { text: "Ground gap"; color: "#9ec4f5"; font.pixelSize: 9 }
                    Text { text: Number(altRadar).toFixed(0) + " ft"; color: "#e1f3ff"; font.pixelSize: 16; font.bold: true }
                }
            }

            FlightSimulationCard {
                cardTitle: "Climb and Descent"
                cardSubtitle: "How fast the Air Taxi is rising or lowering"
                iconType: "vertical"
                accentColor: flightModel.vsAccentColor
                motionSeed: 2.05
                motionRate: 0.043
                progressValue: flightModel.vsProgress
                progressLabel: flightModel.vsTrendLabel
                sparkValues: flightModel.vsHistory
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredWidth: Math.max(220, Math.min(320, cardGrid.width / cardColumns - 12))
                Layout.preferredHeight: Math.max(200, implicitHeight)
                Layout.minimumHeight: 200

                Text { text: flightSimulation.vsValueText; color: "#e1f3ff"; font.pixelSize: 17; font.bold: true }
                Text { text: flightSimulation.vsTrendLabel; font.pixelSize: 10; color: "#9ec4f5" }
            }

            FlightSimulationCard {
                cardTitle: "Ride Stability"
                cardSubtitle: "Cabin balance and comfort during the trip"
                iconType: "attitude"
                accentColor: "#f2b15d"
                motionSeed: 3.0
                motionRate: 0.037
                progressValue: flightModel.attitudeProgress
                progressLabel: "Comfort stability"
                sparkValues: flightModel.attitudeHistory
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredWidth: Math.max(220, Math.min(320, cardGrid.width / cardColumns - 12))
                Layout.preferredHeight: {
                    var cardW = Math.max(220, Math.min(320, cardGrid.width / cardColumns - 12))
                    if (cardW < 255)
                        return Math.max(236, implicitHeight)
                    if (cardW < 285)
                        return Math.max(232, implicitHeight)
                    return Math.max(228, implicitHeight)
                }
                Layout.minimumHeight: Layout.preferredHeight

                RowLayout {
                    spacing: 7
                    ColumnLayout {
                        spacing: 1
                        Text { text: "Nose tilt"; color: "#9ec4f5"; font.pixelSize: 9 }
                        Text { text: Number(pitch).toFixed(1) + "°"; color: "#e1f3ff"; font.pixelSize: 14; font.bold: true }
                    }
                    ColumnLayout {
                        spacing: 1
                        Text { text: "Side tilt"; color: "#9ec4f5"; font.pixelSize: 9 }
                        Text { text: Number(roll).toFixed(1) + "°"; color: "#e1f3ff"; font.pixelSize: 14; font.bold: true }
                    }
                    ColumnLayout {
                        spacing: 1
                        Text { text: "Yaw turn"; color: "#9ec4f5"; font.pixelSize: 9 }
                        Text { text: Number(yaw).toFixed(1) + "°"; color: "#e1f3ff"; font.pixelSize: 14; font.bold: true }
                    }
                }
                Rectangle {
                    Layout.fillWidth: true
                    height: 32
                    color: "#0f1f2e"
                    radius: 8
                    border.color: "#2f4f65"
                    border.width: 1
                    Row {
                        anchors.centerIn: parent
                        spacing: 3
                        Rectangle { width: 8; height: 16; color: "#f2b15d"; radius: 4 }
                        Text { text: "Cabin balance"; color: "#f7d7a5"; font.pixelSize: 10 }
                    }
                }
            }

            FlightSimulationCard {
                cardTitle: "Route Direction"
                cardSubtitle: "Current heading and route alignment"
                iconType: "heading"
                accentColor: "#66d7d1"
                motionSeed: 4.1
                motionRate: 0.019
                progressValue: flightModel.headingProgress
                progressLabel: "Route alignment"
                sparkValues: flightModel.headingHistory
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredWidth: Math.max(220, Math.min(320, cardGrid.width / cardColumns - 12))
                Layout.preferredHeight: Math.max(200, implicitHeight)
                Layout.minimumHeight: 200

                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.minimumHeight: 42

                    RowLayout {
                        anchors.centerIn: parent
                        spacing: 7

                        ColumnLayout {
                            spacing: 1
                            Text { text: "Direction"; color: "#98ece8"; font.pixelSize: 9 }
                            Text { text: Number(heading).toFixed(0) + "°M"; color: "#e1f3ff"; font.pixelSize: 14; font.bold: true }
                        }

                        ColumnLayout {
                            spacing: 1
                            Text { text: "Route"; color: "#98ece8"; font.pixelSize: 9 }
                            Text { text: Number(track).toFixed(0) + "°"; color: "#e1f3ff"; font.pixelSize: 14; font.bold: true }
                        }
                    }
                }
            }

            FlightSimulationCard {
                cardTitle: "Path Guidance"
                cardSubtitle: "How closely the vehicle follows the planned path"
                iconType: "fpv"
                accentColor: "#ff8f70"
                motionSeed: 5.2
                motionRate: 0.051
                progressValue: flightModel.fpvProgress
                progressLabel: "Path confidence"
                sparkValues: flightModel.fpvHistory
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredWidth: Math.max(220, Math.min(320, cardGrid.width / cardColumns - 12))
                Layout.preferredHeight: Math.max(212, implicitHeight)
                Layout.minimumHeight: 212

                Rectangle {
                    Layout.fillWidth: true
                    height: 64
                    color: "#0f1f2e"
                    radius: 8
                    border.width: 1
                    border.color: "#2f4f65"
                    Column {
                        anchors.centerIn: parent
                        width: parent.width * 0.9
                        spacing: 6
                        Rectangle { anchors.horizontalCenter: parent.horizontalCenter; width: 100; height: 4; color: "#ff8f70"; radius: 2 }
                        Text {
                            text: "Guidance marker showing route-following quality for this Air Taxi leg."
                            color: "#ffc9b9"
                            font.pixelSize: 10
                            wrapMode: Text.WordWrap
                            width: parent.width
                            horizontalAlignment: Text.AlignHCenter
                        }
                    }
                }
            }
        }
    }
}
