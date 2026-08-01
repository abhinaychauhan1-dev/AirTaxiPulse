import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    required property var moduleRegistry

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 12

        Text {
            text: moduleRegistry.energy.title
            font.pixelSize: 24
            color: "#ffffff"
            wrapMode: Text.WordWrap
        }

        Text {
            wrapMode: Text.Wrap
            text: moduleRegistry.energy.description
            font.pixelSize: 16
            color: "#d7e3ef"
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#12202a"
            radius: 14
            border.color: "#2f4f67"
            border.width: 1

            Flow {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 18
                clip: true

                Repeater {
                    model: moduleRegistry.energy.cards
                    Rectangle {
                        width: Math.min(340, Math.max(240, parent.width / 2 - 18))
                        height: 150
                        radius: 14
                        color: "#1f3040"
                        border.color: "#2f4f65"
                        border.width: 1
                        opacity: 0.98

                        Column {
                            anchors.fill: parent
                            anchors.margins: 18
                            spacing: 10

                            Text {
                                text: modelData.title
                                font.pixelSize: 17
                                font.bold: true
                                color: "#ffffff"
                            }
                            Text {
                                width: parent.width
                                wrapMode: Text.WordWrap
                                text: modelData.body
                                font.pixelSize: 13
                                color: "#c1d6ef"
                            }
                        }
                    }
                }
            }
        }
    }
}
