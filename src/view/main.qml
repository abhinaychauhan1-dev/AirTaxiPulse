import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "features"

ApplicationWindow {
    id: window
    visible: true
    width: 1024
    height: 700
    title: qsTr("Air Taxi eVTOL Pulse")

    property int currentIndex: 0
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

                Image {
                    source: ""
                    width: 64
                    height: 64
                    fillMode: Image.PreserveAspectFit
                    visible: false
                }

                ColumnLayout {
                    spacing: 2
                    Label {
                        text: qsTr("Air Taxi eVTOL Pulse")
                        font.pixelSize: 24
                        color: "#ade8f4"
                    }
                    Label {
                        text: qsTr("Interactive main page for mission-critical systems")
                        color: "#c8d8e4"
                        font.pixelSize: 13
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

                    PrimaryFlightPage { flightModel: airTaxiModules.primaryFlight }
                    PropulsionPage { moduleRegistry: airTaxiModules }
                    EnergyPage { moduleRegistry: airTaxiModules }
                    FcsPage { moduleRegistry: airTaxiModules }
                    SafetyPage { moduleRegistry: airTaxiModules }
                }
            }
        }
    }
}
