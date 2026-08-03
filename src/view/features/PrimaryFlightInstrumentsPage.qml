pragma ComponentBehavior: Bound

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    required property var flightModel
    property string missionTimeText: ""
    property string missionDateText: ""
    property string missionEtaText: ""
    property int selectedInstrument: 0
    property int hoveredInstrument: -1

    readonly property real headingTrackDelta: root.flightModel.headingTrackDelta
    readonly property real slipOffset: root.flightModel.slipOffset
    readonly property int panelSpacing: 6
    readonly property real instrumentWidth: Math.max(0,
        (instrumentGrid.width - root.panelSpacing * 2) / 3)
    readonly property real instrumentHeight: Math.max(0,
        (instrumentGrid.height - root.panelSpacing) / 2)
    readonly property string selectedSummary: root.flightModel.instrumentSummary(root.selectedInstrument)

    function instrumentCode(index) {
        return ["SPD", "ALT", "V/S", "ATT", "HDG", "FPV"][index]
    }

    implicitHeight: pageContent.implicitHeight + 8

    ColumnLayout {
        id: pageContent
        anchors.fill: parent
        anchors.margins: 4
        spacing: 5

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 43
            spacing: 8

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 0

                Text {
                    text: "Primary Flight & Navigation Data"
                    color: "#f3f9fd"
                    font.pixelSize: 18
                    font.bold: true
                    Layout.fillWidth: true
                    Layout.minimumWidth: 0
                    elide: Text.ElideRight
                }
                Text {
                    text: "Live aircraft state, navigation vectors and trajectory references"
                    color: "#91aebe"
                    font.pixelSize: 9
                    Layout.fillWidth: true
                }
            }

            RowLayout {
                Layout.alignment: Qt.AlignVCenter
                spacing: 12

                ColumnLayout {
                    spacing: 0
                    Label { text: "MODE"; color: "#6f8fa3"; font.pixelSize: 7 }
                    Label { text: root.flightModel.flightModeLabel; color: "#77d9a6"; font.pixelSize: 10; font.bold: true }
                }
                ColumnLayout {
                    spacing: 0
                    Label { text: "MISSION TIME"; color: "#6f8fa3"; font.pixelSize: 7 }
                    Label { text: root.missionTimeText; color: "#d7e9f4"; font.pixelSize: 10; font.bold: true }
                }
                ColumnLayout {
                    spacing: 0
                    Label { text: "ELAPSED"; color: "#6f8fa3"; font.pixelSize: 7 }
                    Label { text: root.missionEtaText; color: "#8bc8ec"; font.pixelSize: 10; font.bold: true }
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 30
            color: "#0d1c25"
            border.color: "#29485b"
            border.width: 1

            RowLayout {
                anchors.fill: parent
                anchors.margins: 4
                spacing: 4

                Repeater {
                    model: 6

                    Rectangle {
                        id: selectorButton
                        required property int index
                        Layout.preferredWidth: 42
                        Layout.fillHeight: true
                        color: root.selectedInstrument === selectorButton.index ? "#315f78" : "#132b38"
                        border.color: root.selectedInstrument === selectorButton.index ? "#79cdec" : "#294858"
                        border.width: 1

                        Label {
                            anchors.centerIn: parent
                            text: root.instrumentCode(selectorButton.index)
                            color: root.selectedInstrument === selectorButton.index ? "#f3fbff" : "#83a3b5"
                            font.pixelSize: 8
                            font.bold: true
                        }

                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: root.selectedInstrument = selectorButton.index
                        }
                    }
                }

                Rectangle { Layout.preferredWidth: 1; Layout.fillHeight: true; Layout.margins: 3; color: "#294858" }

                Label {
                    Layout.fillWidth: true
                    leftPadding: 6
                    text: root.selectedSummary
                    color: "#c7e0ee"
                    font.pixelSize: 9
                    font.bold: true
                    elide: Text.ElideRight
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }

        GridLayout {
            id: instrumentGrid
            Layout.fillWidth: true
            Layout.fillHeight: true
            columns: 3
            columnSpacing: root.panelSpacing
            rowSpacing: root.panelSpacing

            Rectangle {
                id: airspeedPanel
                Layout.row: 0
                Layout.column: 0
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumWidth: 0
                Layout.minimumHeight: 0
                Layout.preferredWidth: root.instrumentWidth
                Layout.preferredHeight: root.instrumentHeight
                color: "#0b1820"
                border.color: root.selectedInstrument === 0 ? "#70c9f2" : (root.hoveredInstrument === 0 ? "#477b94" : "#31566e")
                border.width: root.selectedInstrument === 0 ? 2 : 1
                clip: true

                Behavior on border.color { ColorAnimation { duration: 130 } }

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    acceptedButtons: Qt.LeftButton
                    cursorShape: Qt.PointingHandCursor
                    onEntered: root.hoveredInstrument = 0
                    onExited: if (root.hoveredInstrument === 0) root.hoveredInstrument = -1
                    onClicked: root.selectedInstrument = 0
                }

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 4

                    RowLayout {
                        Layout.fillWidth: true
                        Label { text: "AIRSPEED"; color: "#70c9f2"; font.pixelSize: 10; font.bold: true }
                        Item { Layout.fillWidth: true }
                        Label { text: "CAS / TAS"; color: "#66889b"; font.pixelSize: 8 }
                    }

                    Item {
                        id: speedTape
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        clip: true

                        Rectangle {
                            anchors.left: parent.left
                            anchors.top: parent.top
                            anchors.bottom: parent.bottom
                            width: parent.width * 0.46
                            color: "#0e232e"
                        }

                        Repeater {
                            model: 9

                            Item {
                                id: speedMark
                                required property int index
                                readonly property real value: root.flightModel.speedTapeValue(speedMark.index)
                                x: 0
                                y: speedTape.height * 0.5 + (Number(root.flightModel.cas) - speedMark.value) * 2.0 - 8
                                width: speedTape.width * 0.46
                                height: 16

                                Rectangle {
                                    anchors.right: parent.right
                                    anchors.verticalCenter: parent.verticalCenter
                                    width: speedMark.index % 2 === 0 ? 14 : 8
                                    height: 1
                                    color: "#5e8499"
                                }
                                Label {
                                    anchors.right: parent.right
                                    anchors.rightMargin: 18
                                    anchors.verticalCenter: parent.verticalCenter
                                    text: speedMark.value.toFixed(0)
                                    color: "#9cb9c9"
                                    font.pixelSize: 8
                                }
                            }
                        }

                        Rectangle {
                            x: speedTape.width * 0.46 - 7
                            y: speedTape.height * 0.5 - 15
                            width: Math.min(82, Math.max(68, speedTape.width * 0.24))
                            height: 30
                            color: "#102f3e"
                            border.color: "#74d7ff"
                            border.width: 1
                            clip: true

                            Row {
                                anchors.centerIn: parent
                                spacing: 4
                                Label { text: "CAS"; color: "#75a7be"; font.pixelSize: 7; anchors.verticalCenter: parent.verticalCenter }
                                Label {
                                    text: Number(root.flightModel.cas).toFixed(0)
                                    color: "#f2fbff"
                                    font.pixelSize: 17
                                    minimumPixelSize: 11
                                    fontSizeMode: Text.Fit
                                    font.bold: true
                                    anchors.verticalCenter: parent.verticalCenter
                                }
                            }
                        }

                        Column {
                            anchors.right: parent.right
                            anchors.rightMargin: 4
                            anchors.verticalCenter: parent.verticalCenter
                            width: Math.min(142, parent.width * 0.27)
                            spacing: 4
                            Label { text: "TRUE AIRSPEED"; color: "#67899b"; font.pixelSize: 7 }
                            Text { width: parent.width; text: Number(root.flightModel.tas).toFixed(0) + " kt"; color: "#8fd9ff"; font.pixelSize: 16; minimumPixelSize: 10; fontSizeMode: Text.Fit; font.bold: true }
                            Rectangle { width: parent.width; height: 1; color: "#294657" }
                            Label { text: "TAS - CAS"; color: "#67899b"; font.pixelSize: 7 }
                            Text {
                                width: parent.width
                                text: (Number(root.flightModel.tas) - Number(root.flightModel.cas) >= 0 ? "+" : "")
                                      + (Number(root.flightModel.tas) - Number(root.flightModel.cas)).toFixed(0) + " kt"
                                color: "#b6ceda"
                                font.pixelSize: 12
                                minimumPixelSize: 9
                                fontSizeMode: Text.Fit
                                font.bold: true
                            }
                        }
                    }
                }
            }

            Rectangle {
                id: altitudePanel
                Layout.row: 0
                Layout.column: 1
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumWidth: 0
                Layout.minimumHeight: 0
                Layout.preferredWidth: root.instrumentWidth
                Layout.preferredHeight: root.instrumentHeight
                color: "#0c191f"
                border.color: root.selectedInstrument === 1 ? "#9fd3ef" : (root.hoveredInstrument === 1 ? "#5d7d89" : "#3b5965")
                border.width: root.selectedInstrument === 1 ? 2 : 1
                clip: true

                Behavior on border.color { ColorAnimation { duration: 130 } }

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    acceptedButtons: Qt.LeftButton
                    cursorShape: Qt.PointingHandCursor
                    onEntered: root.hoveredInstrument = 1
                    onExited: if (root.hoveredInstrument === 1) root.hoveredInstrument = -1
                    onClicked: root.selectedInstrument = 1
                }

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 4

                    RowLayout {
                        Layout.fillWidth: true
                        Label { text: "ALTITUDE"; color: "#9fd3ef"; font.pixelSize: 10; font.bold: true }
                        Item { Layout.fillWidth: true }
                        Label { text: "BARO / RADIO"; color: "#66818e"; font.pixelSize: 8 }
                    }

                    Item {
                        id: altitudeTape
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        clip: true

                        Rectangle {
                            anchors.right: parent.right
                            anchors.top: parent.top
                            anchors.bottom: parent.bottom
                            width: parent.width * 0.47
                            color: "#10232a"
                        }

                        Repeater {
                            model: 9

                            Item {
                                id: altitudeMark
                                required property int index
                                readonly property real value: root.flightModel.altitudeTapeValue(altitudeMark.index)
                                x: altitudeTape.width * 0.53
                                y: altitudeTape.height * 0.5 + (Number(root.flightModel.altBaro) - altitudeMark.value) * 0.045 - 8
                                width: altitudeTape.width * 0.47
                                height: 16

                                Rectangle {
                                    anchors.left: parent.left
                                    anchors.verticalCenter: parent.verticalCenter
                                    width: altitudeMark.index % 2 === 0 ? 14 : 8
                                    height: 1
                                    color: "#708995"
                                }
                                Label {
                                    anchors.left: parent.left
                                    anchors.leftMargin: 18
                                    anchors.verticalCenter: parent.verticalCenter
                                    text: altitudeMark.value.toFixed(0)
                                    color: "#a7bac3"
                                    font.pixelSize: 8
                                }
                            }
                        }

                        Rectangle {
                            x: altitudeTape.width * 0.30
                            y: altitudeTape.height * 0.5 - 15
                            width: altitudeTape.width * 0.39
                            height: 30
                            color: "#193039"
                            border.color: "#b8dce9"
                            border.width: 1
                            Label {
                                anchors.centerIn: parent
                                text: Number(root.flightModel.altBaro).toFixed(0)
                                color: "#f4fbfd"
                                font.pixelSize: 15
                                minimumPixelSize: 9
                                fontSizeMode: Text.Fit
                                font.bold: true
                                width: parent.width - 6
                                horizontalAlignment: Text.AlignHCenter
                            }
                        }

                        Column {
                            anchors.left: parent.left
                            anchors.verticalCenter: parent.verticalCenter
                            width: parent.width * 0.28
                            spacing: 4
                            Label { text: "BARO"; color: "#718b96"; font.pixelSize: 7 }
                            Text { text: Number(root.flightModel.altBaro).toFixed(0) + " ft"; color: "#e0f3f8"; font.pixelSize: 13; font.bold: true }
                            Label { text: "GROUND GAP"; color: "#718b96"; font.pixelSize: 7 }
                            Text { text: Number(root.flightModel.altRadar).toFixed(0) + " ft"; color: "#75d7a1"; font.pixelSize: 13; font.bold: true }
                        }

                        Rectangle {
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.bottom: parent.bottom
                            height: Math.max(3, Math.min(18, Number(root.flightModel.altRadar) / 120))
                            color: "#41694e"
                            opacity: 0.75
                            Behavior on height { NumberAnimation { duration: 300 } }
                        }
                    }
                }
            }

            Rectangle {
                id: verticalSpeedPanel
                Layout.row: 0
                Layout.column: 2
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumWidth: 0
                Layout.minimumHeight: 0
                Layout.preferredWidth: root.instrumentWidth
                Layout.preferredHeight: root.instrumentHeight
                color: "#0a171c"
                border.color: root.selectedInstrument === 2 ? "#76d5a1" : (root.hoveredInstrument === 2 ? "#527868" : "#36535d")
                border.width: root.selectedInstrument === 2 ? 2 : 1

                Behavior on border.color { ColorAnimation { duration: 130 } }

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    acceptedButtons: Qt.LeftButton
                    cursorShape: Qt.PointingHandCursor
                    onEntered: root.hoveredInstrument = 2
                    onExited: if (root.hoveredInstrument === 2) root.hoveredInstrument = -1
                    onClicked: root.selectedInstrument = 2
                }

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 4

                    RowLayout {
                        Layout.fillWidth: true
                        Label { text: "VERTICAL SPEED"; color: "#76d5a1"; font.pixelSize: 10; font.bold: true }
                        Item { Layout.fillWidth: true }
                        Label { text: root.flightModel.vsTrendLabel; color: root.flightModel.vsAccentColor; font.pixelSize: 8; font.bold: true }
                    }

                    Item {
                        id: verticalGauge
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        Rectangle {
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.top: parent.top
                            anchors.bottom: parent.bottom
                            width: 3
                            color: "#29443d"
                        }

                        Repeater {
                            model: 7

                            RowLayout {
                                id: verticalMark
                                required property int index
                                readonly property int rate: root.flightModel.verticalSpeedMark(verticalMark.index)
                                x: verticalGauge.width * 0.5 - 48
                                y: verticalGauge.height * 0.5 - 7 + (verticalMark.index - 3) * (verticalGauge.height - 20) / 6
                                width: 96
                                height: 14
                                spacing: 4
                                Label { Layout.preferredWidth: 34; text: verticalMark.rate.toString(); color: "#718e84"; font.pixelSize: 8; horizontalAlignment: Text.AlignRight }
                                Rectangle { Layout.fillWidth: true; Layout.preferredHeight: verticalMark.index === 3 ? 2 : 1; color: verticalMark.index === 3 ? "#7dd5a3" : "#3b5a4f" }
                            }
                        }

                        Rectangle {
                            id: verticalPointer
                            x: verticalGauge.width * 0.5 + 3
                            y: verticalGauge.height * 0.5 - height * 0.5
                               - Math.max(-1, Math.min(1, Number(root.flightModel.vs) / 3000.0)) * (verticalGauge.height * 0.42)
                            width: 72
                            height: 28
                            color: "#15362d"
                            border.color: root.flightModel.vsAccentColor
                            border.width: 1

                            Behavior on y { NumberAnimation { duration: 320; easing.type: Easing.OutCubic } }

                            Label {
                                anchors.centerIn: parent
                                text: (Number(root.flightModel.vs) >= 0 ? "+" : "") + Number(root.flightModel.vs).toFixed(0)
                                color: "#e5fff0"
                                font.pixelSize: 14
                                minimumPixelSize: 9
                                fontSizeMode: Text.Fit
                                font.bold: true
                                width: parent.width - 6
                                horizontalAlignment: Text.AlignHCenter
                            }
                        }

                        Label {
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.bottom: parent.bottom
                            text: "ft/min"
                            color: "#638276"
                            font.pixelSize: 8
                        }
                    }
                }
            }

            Rectangle {
                id: attitudePanel
                Layout.row: 1
                Layout.column: 0
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumWidth: 0
                Layout.minimumHeight: 0
                Layout.preferredWidth: root.instrumentWidth
                Layout.preferredHeight: root.instrumentHeight
                color: "#0a151b"
                border.color: root.selectedInstrument === 3 ? "#efc47b" : (root.hoveredInstrument === 3 ? "#81745a" : "#5c5748")
                border.width: root.selectedInstrument === 3 ? 2 : 1
                clip: true

                Behavior on border.color { ColorAnimation { duration: 130 } }

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    acceptedButtons: Qt.LeftButton
                    cursorShape: Qt.PointingHandCursor
                    onEntered: root.hoveredInstrument = 3
                    onExited: if (root.hoveredInstrument === 3) root.hoveredInstrument = -1
                    onClicked: root.selectedInstrument = 3
                }

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 4

                    RowLayout {
                        Layout.fillWidth: true
                        Label { text: "ATTITUDE INDICATOR"; color: "#efc47b"; font.pixelSize: 10; font.bold: true }
                        Item { Layout.fillWidth: true }
                        Label { text: "P / R / Y"; color: "#8d8069"; font.pixelSize: 8 }
                    }

                    Item {
                        id: attitudeViewport
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        clip: true

                        Item {
                            id: horizonPlane
                            width: attitudeViewport.width * 1.8
                            height: attitudeViewport.height * 2.2
                            x: (attitudeViewport.width - width) * 0.5
                            y: (attitudeViewport.height - height) * 0.5 + Number(root.flightModel.pitch) * 2.4
                            rotation: -Number(root.flightModel.roll)

                            Behavior on y { NumberAnimation { duration: 280; easing.type: Easing.OutCubic } }
                            Behavior on rotation { NumberAnimation { duration: 280; easing.type: Easing.OutCubic } }

                            Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.top: parent.top; height: parent.height * 0.5; color: "#24536c" }
                            Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.bottom: parent.bottom; height: parent.height * 0.5; color: "#725641" }
                            Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.verticalCenter: parent.verticalCenter; height: 2; color: "#f4e3b7" }

                            Repeater {
                                model: 5
                                Rectangle {
                                    required property int index
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    y: parent.height * 0.5 + (index - 2) * 22
                                    width: index === 2 ? 100 : 52
                                    height: 1
                                    color: "#e8e3d0"
                                }
                            }
                        }

                        Rectangle { anchors.centerIn: parent; width: 56; height: 3; color: "#ffd66e" }
                        Rectangle { anchors.centerIn: parent; width: 3; height: 12; color: "#ffd66e" }

                        RowLayout {
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.bottom: parent.bottom
                            spacing: 5
                            Label { text: "P " + Number(root.flightModel.pitch).toFixed(1) + "°"; color: "#f0d7aa"; font.pixelSize: 8 }
                            Label { text: "R " + Number(root.flightModel.roll).toFixed(1) + "°"; color: "#f0d7aa"; font.pixelSize: 8 }
                            Label { text: "Y " + Number(root.flightModel.yaw).toFixed(1) + "°"; color: "#f0d7aa"; font.pixelSize: 8 }
                            Item { Layout.fillWidth: true }
                        }

                        Rectangle {
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.bottom: parent.bottom
                            anchors.bottomMargin: 17
                            width: 84
                            height: 8
                            radius: 4
                            color: "#182129"
                            border.color: "#70726e"
                            Rectangle {
                                width: 12
                                height: 6
                                radius: 3
                                y: 1
                                x: (parent.width - width) * (0.5 + root.slipOffset * 0.42)
                                color: "#f0c36d"
                                Behavior on x { NumberAnimation { duration: 250 } }
                            }
                        }
                    }
                }
            }

            Rectangle {
                id: headingPanel
                Layout.row: 1
                Layout.column: 1
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumWidth: 0
                Layout.minimumHeight: 0
                Layout.preferredWidth: root.instrumentWidth
                Layout.preferredHeight: root.instrumentHeight
                color: "#0a171d"
                border.color: root.selectedInstrument === 4 ? "#68d5cc" : (root.hoveredInstrument === 4 ? "#4c8580" : "#315c62")
                border.width: root.selectedInstrument === 4 ? 2 : 1

                Behavior on border.color { ColorAnimation { duration: 130 } }

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    acceptedButtons: Qt.LeftButton
                    cursorShape: Qt.PointingHandCursor
                    onEntered: root.hoveredInstrument = 4
                    onExited: if (root.hoveredInstrument === 4) root.hoveredInstrument = -1
                    onClicked: root.selectedInstrument = 4
                }

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 4

                    RowLayout {
                        Layout.fillWidth: true
                        Label { text: "HEADING & TRACK"; color: "#68d5cc"; font.pixelSize: 10; font.bold: true }
                        Item { Layout.fillWidth: true }
                        Label { text: (root.headingTrackDelta >= 0 ? "+" : "") + root.headingTrackDelta.toFixed(1) + "° DRIFT"; color: "#80aaa7"; font.pixelSize: 8 }
                    }

                    Item {
                        id: compassViewport
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        Canvas {
                            id: compassCanvas
                            anchors.fill: parent
                            antialiasing: true

                            property real heading: Number(root.flightModel.heading)
                            property real track: Number(root.flightModel.track)
                            onHeadingChanged: requestPaint()
                            onTrackChanged: requestPaint()
                            onWidthChanged: requestPaint()
                            onHeightChanged: requestPaint()

                            onPaint: {
                                var ctx = getContext("2d")
                                ctx.clearRect(0, 0, width, height)
                                var cx = width * 0.5
                                var cy = height * 0.5
                                var radius = Math.min(width, height) * 0.39
                                ctx.save()
                                ctx.translate(cx, cy)
                                ctx.strokeStyle = "#4c817d"
                                ctx.lineWidth = 1.5
                                ctx.beginPath()
                                ctx.arc(0, 0, radius, 0, Math.PI * 2)
                                ctx.stroke()

                                for (var i = 0; i < 36; ++i) {
                                    ctx.save()
                                    ctx.rotate((i * 10 - heading) * Math.PI / 180)
                                    ctx.beginPath()
                                    ctx.moveTo(0, -radius)
                                    ctx.lineTo(0, -radius + (i % 3 === 0 ? 10 : 5))
                                    ctx.strokeStyle = i % 3 === 0 ? "#b9dfdc" : "#527b79"
                                    ctx.lineWidth = i % 3 === 0 ? 1.5 : 1
                                    ctx.stroke()
                                    ctx.restore()
                                }

                                var trackAngle = (track - heading) * Math.PI / 180
                                ctx.rotate(trackAngle)
                                ctx.beginPath()
                                ctx.moveTo(0, -radius + 2)
                                ctx.lineTo(-6, -radius + 13)
                                ctx.lineTo(6, -radius + 13)
                                ctx.closePath()
                                ctx.fillStyle = "#ffbc67"
                                ctx.fill()
                                ctx.restore()
                            }
                        }

                        Rectangle {
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.top: parent.top
                            width: 3
                            height: 24
                            color: "#c5fff9"
                        }
                        Column {
                            anchors.centerIn: parent
                            spacing: 0
                            Label { anchors.horizontalCenter: parent.horizontalCenter; text: "MAG"; color: "#688c8a"; font.pixelSize: 7 }
                            Text { anchors.horizontalCenter: parent.horizontalCenter; text: Number(root.flightModel.heading).toFixed(0) + "°"; color: "#e6fffc"; font.pixelSize: 21; font.bold: true }
                            Label { anchors.horizontalCenter: parent.horizontalCenter; text: "TRK " + Number(root.flightModel.track).toFixed(0) + "°"; color: "#ffca83"; font.pixelSize: 9; font.bold: true }
                        }
                    }
                }
            }

            Rectangle {
                id: fpvPanel
                Layout.row: 1
                Layout.column: 2
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumWidth: 0
                Layout.minimumHeight: 0
                Layout.preferredWidth: root.instrumentWidth
                Layout.preferredHeight: root.instrumentHeight
                color: "#09161c"
                border.color: root.selectedInstrument === 5 ? "#ff9b79" : (root.hoveredInstrument === 5 ? "#8d665a" : "#5f4c48")
                border.width: root.selectedInstrument === 5 ? 2 : 1
                clip: true

                Behavior on border.color { ColorAnimation { duration: 130 } }

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    acceptedButtons: Qt.LeftButton
                    cursorShape: Qt.PointingHandCursor
                    onEntered: root.hoveredInstrument = 5
                    onExited: if (root.hoveredInstrument === 5) root.hoveredInstrument = -1
                    onClicked: root.selectedInstrument = 5
                }

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 4

                    RowLayout {
                        Layout.fillWidth: true
                        Label { text: "FLIGHT PATH VECTOR"; color: "#ff9b79"; font.pixelSize: 10; font.bold: true }
                        Item { Layout.fillWidth: true }
                        Label { text: "FPV / TRAJECTORY"; color: "#8e7169"; font.pixelSize: 8 }
                    }

                    Item {
                        id: fpvViewport
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        clip: true

                        Repeater {
                            model: 5
                            Rectangle {
                                required property int index
                                x: 0
                                y: fpvViewport.height * 0.5 + (index - 2) * 24 + Number(root.flightModel.pitch) * 1.2
                                width: fpvViewport.width
                                height: index === 2 ? 2 : 1
                                color: index === 2 ? "#a68d83" : "#263c43"
                            }
                        }

                        Item {
                            id: fpvMarker
                            width: 42
                            height: 28
                            x: fpvViewport.width * 0.5 - width * 0.5
                               + Math.max(-1, Math.min(1, root.headingTrackDelta / 20.0)) * fpvViewport.width * 0.28
                            y: fpvViewport.height * 0.5 - height * 0.5
                               - Math.max(-1, Math.min(1, Number(root.flightModel.vs) / 2500.0)) * fpvViewport.height * 0.30

                            Behavior on x { NumberAnimation { duration: 300; easing.type: Easing.OutCubic } }
                            Behavior on y { NumberAnimation { duration: 300; easing.type: Easing.OutCubic } }

                            Rectangle { anchors.centerIn: parent; width: 13; height: 13; radius: 7; color: "transparent"; border.color: "#ff9b79"; border.width: 2 }
                            Rectangle { anchors.left: parent.left; anchors.verticalCenter: parent.verticalCenter; width: 13; height: 2; color: "#ff9b79" }
                            Rectangle { anchors.right: parent.right; anchors.verticalCenter: parent.verticalCenter; width: 13; height: 2; color: "#ff9b79" }
                            Rectangle { anchors.horizontalCenter: parent.horizontalCenter; anchors.top: parent.top; width: 2; height: 8; color: "#ff9b79" }
                        }

                        Rectangle {
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.verticalCenter: parent.verticalCenter
                            width: 70
                            height: 2
                            color: "#d4e5e9"
                            opacity: 0.7
                        }
                        Rectangle {
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.verticalCenter: parent.verticalCenter
                            width: 2
                            height: 16
                            color: "#d4e5e9"
                            opacity: 0.7
                        }

                        RowLayout {
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.bottom: parent.bottom
                            Label { text: "PATH " + Number(root.flightModel.fpvProgress * 100).toFixed(0) + "%"; color: "#ffb49b"; font.pixelSize: 8; font.bold: true }
                            Item { Layout.fillWidth: true }
                            Label { text: root.headingTrackDelta.toFixed(1) + "° XTK"; color: "#9db8bf"; font.pixelSize: 8 }
                        }
                    }
                }
            }
        }
    }
}
