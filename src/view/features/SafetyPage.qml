pragma ComponentBehavior: Bound

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    required property var moduleRegistry

    readonly property var flightModel: root.moduleRegistry.primaryFlight
    readonly property var safety: root.moduleRegistry.safetySystem
    property int selectedMonitor: 0
    readonly property int selectedTraffic: root.safety.selectedTraffic
    property int radarRange: 5
    property int approachMode: 0

    readonly property int noiseLimit: root.safety.noiseLimit
    readonly property var noiseHistory: root.safety.noiseHistory
    readonly property real acousticDb: root.safety.acousticDb
    readonly property bool noiseWarning: root.safety.noiseWarning
    readonly property int satelliteCount: root.safety.satelliteCount
    readonly property real signalStrength: root.safety.signalStrength
    readonly property real navigationAccuracy: root.safety.navigationAccuracy
    readonly property bool raimAvailable: root.safety.raimAvailable
    readonly property real lateralDeviation: root.safety.lateralDeviation
    readonly property real verticalDeviation: root.safety.verticalDeviation
    readonly property real alignmentQuality: root.safety.alignmentQuality
    readonly property bool landingPhase: root.safety.landingPhase
    readonly property int parachuteState: root.safety.parachuteState
    readonly property real parachuteProgress: root.safety.parachuteProgress
    readonly property bool parachuteTestOverride: root.safety.parachuteTestOverride
    readonly property string parachuteEventTime: root.safety.parachuteEventTime
    readonly property bool parachuteEnvelopeValid: root.safety.parachuteEnvelopeValid
    readonly property bool parachuteDeployAllowed: root.safety.parachuteDeployAllowed
    readonly property string parachuteInhibitReason: root.safety.parachuteInhibitReason
    readonly property bool gnssTestMode: root.safety.gnssTestMode
    readonly property int alertCount: root.safety.alertCount

    function monitorCode(index) { return root.safety.monitorCode(index) }
    function monitorSummary(index) { return root.safety.monitorSummaries[index] }
    function signed(value) { return root.safety.signedValue(value) }
    function trafficCode(index) { return root.safety.trafficCode(index) }
    function trafficRange(index) { return root.safety.trafficRanges[index] }
    function trafficBearing(index) { return root.safety.trafficBearings[index] }
    function toggleParachuteArm() {
        root.selectedMonitor = 1
        root.safety.toggleParachuteArm()
    }
    function deployOrResetParachute() {
        root.selectedMonitor = 1
        root.safety.deployOrResetParachute()
    }

    Connections {
        target: root.safety
        function onStateChanged() {
            noiseCanvas.requestPaint()
            parachuteCanvas.requestPaint()
            trafficCanvas.requestPaint()
            alignmentCanvas.requestPaint()
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 4
        spacing: 6

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 43
            spacing: 10

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 0
                Text {
                    Layout.fillWidth: true
                    Layout.minimumWidth: 0
                    text: "Safety, Health & Environmental Monitoring"
                    color: "#f4f8f7"
                    font.pixelSize: 18
                    font.bold: true
                    elide: Text.ElideRight
                }
                Text {
                    Layout.fillWidth: true
                    text: "Live environmental exposure, emergency readiness, traffic and precision-landing integrity"
                    color: "#91aaa7"
                    font.pixelSize: 9
                    elide: Text.ElideRight
                }
            }

            Rectangle {
                Layout.preferredWidth: 158
                Layout.preferredHeight: 28
                color: root.alertCount === 0 ? "#102923" : "#35231b"
                border.color: root.alertCount === 0 ? "#61cf9a" : "#eea15e"
                border.width: 1
                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 6
                    Rectangle { Layout.preferredWidth: 8; Layout.preferredHeight: 8; radius: 4; color: root.alertCount === 0 ? "#61cf9a" : "#eea15e" }
                    Label {
                        Layout.fillWidth: true
                        text: root.alertCount === 0 ? "ALL SYSTEMS NOMINAL" : root.alertCount + " ACTIVE ADVISORY"
                        color: root.alertCount === 0 ? "#b6eed3" : "#ffd0a0"
                        font.pixelSize: 8
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 31
            color: "#0d191c"
            border.color: "#304a4c"
            border.width: 1
            RowLayout {
                anchors.fill: parent
                anchors.margins: 4
                spacing: 4
                Repeater {
                    model: 5
                    Rectangle {
                        id: monitorButton
                        required property int index
                        Layout.preferredWidth: 58
                        Layout.fillHeight: true
                        color: root.selectedMonitor === monitorButton.index ? "#315c5b" : "#15282c"
                        border.color: root.selectedMonitor === monitorButton.index ? "#76d2c6" : "#2f4a4e"
                        border.width: 1
                        Label { anchors.centerIn: parent; text: root.monitorCode(monitorButton.index); color: root.selectedMonitor === monitorButton.index ? "#effffb" : "#7e9b9d"; font.pixelSize: 7; font.bold: true }
                        MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: root.selectedMonitor = monitorButton.index }
                    }
                }
                Rectangle { Layout.preferredWidth: 1; Layout.fillHeight: true; Layout.margins: 3; color: "#30494c" }
                Label {
                    Layout.fillWidth: true
                    Layout.minimumWidth: 0
                    text: root.monitorSummary(root.selectedMonitor)
                    color: "#c9deda"
                    font.pixelSize: 9
                    minimumPixelSize: 7
                    fontSizeMode: Text.Fit
                    font.bold: true
                    elide: Text.ElideRight
                }
            }
        }

        GridLayout {
            id: safetyGrid
            Layout.fillWidth: true
            Layout.fillHeight: true
            columns: 6
            rows: 2
            columnSpacing: 6
            rowSpacing: 6

            Rectangle {
                Layout.row: 0
                Layout.column: 0
                Layout.columnSpan: 2
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumWidth: 0
                Layout.minimumHeight: 180
                color: "#101b1e"
                border.color: root.selectedMonitor === 0 ? "#e7b866" : "#42585a"
                border.width: root.selectedMonitor === 0 ? 2 : 1

                MouseArea { anchors.fill: parent; onClicked: root.selectedMonitor = 0 }
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 9
                    spacing: 5
                    RowLayout {
                        Layout.fillWidth: true
                        Label { text: "ACOUSTIC NOISE PROFILE"; color: "#e9b864"; font.pixelSize: 10; font.bold: true }
                        Item { Layout.fillWidth: true }
                        Label { text: root.noiseWarning ? "LIMIT EXCEEDED" : "COMPLIANT"; color: root.noiseWarning ? "#ff786a" : "#6bd2a0"; font.pixelSize: 7; font.bold: true }
                    }
                    RowLayout {
                        Layout.fillWidth: true
                        Text { text: root.acousticDb.toFixed(1); color: root.noiseWarning ? "#ff8a75" : "#f6d38e"; font.pixelSize: 25; font.bold: true }
                        Label { text: "dBA"; color: "#99886c"; font.pixelSize: 8; Layout.alignment: Qt.AlignBottom }
                        Item { Layout.fillWidth: true }
                        ColumnLayout {
                            spacing: 1
                            Label { text: "COMMUNITY LIMIT"; color: "#71888a"; font.pixelSize: 7 }
                            RowLayout {
                                spacing: 3
                                Repeater {
                                    model: [65, 75, 85]
                                    Rectangle {
                                        id: limitButton
                                        required property int modelData
                                        Layout.preferredWidth: 30
                                        Layout.preferredHeight: 20
                                        color: root.noiseLimit === limitButton.modelData ? "#694f2c" : "#1a2b2f"
                                        border.color: root.noiseLimit === limitButton.modelData ? "#edbd68" : "#354b4e"
                                        Label { anchors.centerIn: parent; text: limitButton.modelData; color: "#d8c196"; font.pixelSize: 7; font.bold: true }
                                        MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: root.safety.setNoiseLimit(limitButton.modelData) }
                                    }
                                }
                            }
                        }
                    }
                    Canvas {
                        id: noiseCanvas
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        antialiasing: true
                        onPaint: {
                            var context = getContext("2d")
                            context.clearRect(0, 0, width, height)
                            var minimum = 45
                            var maximum = 100
                            var limitY = height - (root.noiseLimit - minimum) / (maximum - minimum) * height
                            context.strokeStyle = "rgba(234, 184, 100, 0.45)"; context.lineWidth = 1; context.setLineDash([4, 3]); context.beginPath(); context.moveTo(0, limitY); context.lineTo(width, limitY); context.stroke(); context.setLineDash([])
                            if (root.noiseHistory.length < 2) return
                            context.strokeStyle = root.noiseWarning ? "#ff806e" : "#68cbb7"; context.lineWidth = 2; context.beginPath()
                            for (var index = 0; index < root.noiseHistory.length; ++index) {
                                var x = index * width / Math.max(1, root.noiseHistory.length - 1)
                                var y = height - (root.noiseHistory[index] - minimum) / (maximum - minimum) * height
                                if (index === 0) context.moveTo(x, y); else context.lineTo(x, y)
                            }
                            context.stroke()
                        }
                        onWidthChanged: requestPaint()
                        onHeightChanged: requestPaint()
                    }
                }
            }

            Rectangle {
                Layout.row: 0
                Layout.column: 2
                Layout.columnSpan: 2
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumWidth: 0
                Layout.minimumHeight: 180
                color: "#101b1e"
                border.color: root.selectedMonitor === 1 ? "#db9d65" : "#42585a"
                border.width: root.selectedMonitor === 1 ? 2 : 1

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 9
                    spacing: 6
                    RowLayout {
                        Layout.fillWidth: true
                        Label { text: "BALLISTIC PARACHUTE"; color: "#dfaa78"; font.pixelSize: 10; font.bold: true }
                        Item { Layout.fillWidth: true }
                        Rectangle {
                            Layout.preferredWidth: 58
                            Layout.preferredHeight: 18
                            color: root.parachuteTestOverride ? "#513b24" : "#17282c"
                            border.color: root.parachuteTestOverride ? "#e0aa59" : "#3b5053"
                            Label { anchors.centerIn: parent; text: root.parachuteTestOverride ? "TEST ON" : "TEST OFF"; color: root.parachuteTestOverride ? "#ffd28a" : "#789092"; font.pixelSize: 7; font.bold: true }
                            MouseArea { anchors.fill: parent; enabled: root.parachuteState < 2; cursorShape: Qt.PointingHandCursor; onClicked: root.safety.toggleParachuteTestOverride() }
                        }
                        Label {
                            text: root.parachuteState === 0 ? "SAFE"
                                  : (root.parachuteState === 1 ? "ARMED"
                                  : (root.parachuteState === 2 ? "DEPLOYING" : "DEPLOYED"))
                            color: root.parachuteState === 0 ? "#7fa3a0" : (root.parachuteState === 1 ? "#f1c26f" : "#ff776b")
                            font.pixelSize: 8
                            font.bold: true
                        }
                    }
                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Canvas {
                            id: parachuteCanvas
                            anchors.fill: parent
                            antialiasing: true
                            onPaint: {
                                var context = getContext("2d")
                                context.clearRect(0, 0, width, height)
                                var visualProgress = root.parachuteState === 3 ? 1 : (root.parachuteState === 2 ? root.parachuteProgress : 0.18)
                                var centerX = width / 2
                                var aircraftY = height * 0.78
                                var canopyY = aircraftY - height * (0.12 + visualProgress * 0.48)
                                var canopyRadius = Math.min(width, height) * (0.08 + visualProgress * 0.23)
                                var activeColor = root.parachuteState >= 2 ? "#ff796d" : "#d6a46f"

                                context.strokeStyle = activeColor
                                context.lineWidth = 2
                                context.beginPath()
                                context.arc(centerX, canopyY, canopyRadius, Math.PI, Math.PI * 2)
                                context.stroke()

                                context.beginPath()
                                context.moveTo(centerX - canopyRadius, canopyY)
                                context.lineTo(centerX - width * 0.06, aircraftY)
                                context.moveTo(centerX + canopyRadius, canopyY)
                                context.lineTo(centerX + width * 0.06, aircraftY)
                                context.stroke()

                                if (root.parachuteState === 2) {
                                    context.strokeStyle = "rgba(255, 121, 109, 0.45)"
                                    context.lineWidth = 1
                                    context.beginPath()
                                    context.arc(centerX, canopyY, canopyRadius + 5 + 4 * Math.sin(root.parachuteProgress * Math.PI * 8), Math.PI, Math.PI * 2)
                                    context.stroke()
                                }

                                context.fillStyle = root.parachuteState >= 2 ? "#ff796d" : "#73878a"
                                context.fillRect(centerX - width * 0.06, aircraftY, width * 0.12, height * 0.08)

                                if (root.parachuteState === 2) {
                                    context.fillStyle = "#f6c3b9"
                                    context.font = "bold 9px sans-serif"
                                    context.textAlign = "center"
                                    context.fillText(Math.round(root.parachuteProgress * 100) + "%", centerX, height * 0.16)
                                }
                            }
                        }
                    }
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 5
                        color: "#273638"
                        visible: root.parachuteState >= 2
                        Rectangle {
                            width: parent.width * root.parachuteProgress
                            height: parent.height
                            color: "#ff796d"
                            Behavior on width { NumberAnimation { duration: 70 } }
                        }
                    }
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 5
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 27
                            color: root.parachuteState === 1 ? "#654c28" : "#1a2b2f"
                            border.color: "#c99757"
                            Label { anchors.centerIn: parent; text: root.parachuteState === 1 ? "DISARM" : "ARM SYSTEM"; color: "#f0c887"; font.pixelSize: 8; font.bold: true }
                            MouseArea { anchors.fill: parent; enabled: root.parachuteState < 2; cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor; onClicked: root.toggleParachuteArm() }
                        }
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 27
                            color: root.parachuteDeployAllowed ? "#642e28" : (root.parachuteState === 3 ? "#463b25" : "#202a2d")
                            border.color: root.parachuteDeployAllowed ? "#ff796d" : (root.parachuteState === 3 ? "#d5a95d" : "#455457")
                            Label { anchors.centerIn: parent; text: root.parachuteState === 3 ? "SIM RESET" : (root.parachuteState === 2 ? "DEPLOYING" : "DEPLOY"); color: root.parachuteDeployAllowed || root.parachuteState === 3 ? "#ffd3cd" : "#778689"; font.pixelSize: 8; font.bold: true }
                            MouseArea { anchors.fill: parent; enabled: root.parachuteDeployAllowed || root.parachuteState === 3; cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor; onClicked: root.deployOrResetParachute() }
                        }
                    }
                    Label {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 10
                        text: root.parachuteState === 2 ? "CANOPY EXTRACTION IN PROGRESS"
                              : (root.parachuteState === 3 ? "DEPLOYED AT " + root.parachuteEventTime + "  |  SIMULATION RESET AVAILABLE"
                              : (root.parachuteTestOverride ? "TEST OVERRIDE ACTIVE  |  SQUIB A/B READY"
                              : root.parachuteInhibitReason + "  |  SQUIB A/B READY"))
                        color: root.parachuteEnvelopeValid || root.parachuteTestOverride ? "#7fb7a9" : "#c18d62"
                        font.pixelSize: 7
                        minimumPixelSize: 6
                        fontSizeMode: Text.Fit
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
            }

            Rectangle {
                Layout.row: 0
                Layout.column: 4
                Layout.columnSpan: 2
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumWidth: 0
                Layout.minimumHeight: 180
                color: "#101b1e"
                border.color: root.selectedMonitor === 2 ? "#ef876d" : "#42585a"
                border.width: root.selectedMonitor === 2 ? 2 : 1

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 9
                    spacing: 4
                    RowLayout {
                        Layout.fillWidth: true
                        Label { text: "COLLISION AVOIDANCE / TCAS"; color: "#ef8c73"; font.pixelSize: 10; font.bold: true }
                        Item { Layout.fillWidth: true }
                        Repeater {
                            model: [2, 5, 10]
                            Rectangle {
                                id: rangeButton
                                required property int modelData
                                Layout.preferredWidth: 24
                                Layout.preferredHeight: 18
                                color: root.radarRange === rangeButton.modelData ? "#633b32" : "#1a2b2f"
                                border.color: root.radarRange === rangeButton.modelData ? "#ef9078" : "#384d50"
                                Label { anchors.centerIn: parent; text: rangeButton.modelData; color: "#d4aaa0"; font.pixelSize: 7 }
                                MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: root.radarRange = rangeButton.modelData }
                            }
                        }
                    }
                    RowLayout {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Canvas {
                            id: trafficCanvas
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            antialiasing: true
                            onPaint: {
                                var context = getContext("2d")
                                context.clearRect(0, 0, width, height)
                                var centerX = width / 2; var centerY = height / 2; var radius = Math.min(width, height) * 0.43
                                context.strokeStyle = "#345056"; context.lineWidth = 1
                                for (var ring = 1; ring <= 3; ++ring) { context.beginPath(); context.arc(centerX, centerY, radius * ring / 3, 0, Math.PI * 2); context.stroke() }
                                context.fillStyle = "#dceff0"; context.beginPath(); context.moveTo(centerX, centerY - 7); context.lineTo(centerX + 5, centerY + 6); context.lineTo(centerX, centerY + 3); context.lineTo(centerX - 5, centerY + 6); context.closePath(); context.fill()
                                for (var index = 0; index < 3; ++index) {
                                    var scaledRange = Math.min(1, root.trafficRange(index) / root.radarRange)
                                    var angle = (root.trafficBearing(index) - 90) * Math.PI / 180
                                    var x = centerX + Math.cos(angle) * radius * scaledRange
                                    var y = centerY + Math.sin(angle) * radius * scaledRange
                                    context.fillStyle = index === root.selectedTraffic ? "#ff796d" : "#e1b766"; context.fillRect(x - 4, y - 4, 8, 8)
                                }
                            }
                            onWidthChanged: requestPaint()
                            onHeightChanged: requestPaint()
                        }
                        ColumnLayout {
                            Layout.preferredWidth: 94
                            Layout.fillHeight: true
                            spacing: 3
                            Repeater {
                                model: 3
                                Rectangle {
                                    id: trafficRow
                                    required property int index
                                    Layout.fillWidth: true
                                    Layout.fillHeight: true
                                    color: root.selectedTraffic === trafficRow.index ? "#4b302c" : "#17282c"
                                    border.color: root.selectedTraffic === trafficRow.index ? "#f0856f" : "#31484c"
                                    Column {
                                        anchors.centerIn: parent
                                        spacing: 1
                                        Label { anchors.horizontalCenter: parent.horizontalCenter; text: root.trafficCode(trafficRow.index); color: "#e7c0b8"; font.pixelSize: 8; font.bold: true }
                                        Label { anchors.horizontalCenter: parent.horizontalCenter; text: root.trafficRange(trafficRow.index).toFixed(1) + " NM"; color: "#879fa0"; font.pixelSize: 7 }
                                    }
                                    MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: { root.selectedMonitor = 2; root.safety.setSelectedTraffic(trafficRow.index); trafficCanvas.requestPaint() } }
                                }
                            }
                        }
                    }
                }
            }

            Rectangle {
                Layout.row: 1
                Layout.column: 0
                Layout.columnSpan: 3
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumWidth: 0
                Layout.minimumHeight: 185
                color: "#101b1e"
                border.color: root.selectedMonitor === 3 ? "#78c9b7" : "#42585a"
                border.width: root.selectedMonitor === 3 ? 2 : 1

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 9
                    spacing: 6
                    RowLayout {
                        Layout.fillWidth: true
                        Label { text: "GPS / GNSS INTEGRITY"; color: "#79cbb8"; font.pixelSize: 10; font.bold: true }
                        Item { Layout.fillWidth: true }
                        Label { text: root.raimAvailable ? "RAIM AVAILABLE" : "RAIM UNAVAILABLE"; color: root.raimAvailable ? "#64d59c" : "#ff826e"; font.pixelSize: 8; font.bold: true }
                    }
                    RowLayout {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        spacing: 10
                        Item {
                            Layout.preferredWidth: 150
                            Layout.fillHeight: true
                            Repeater {
                                model: 12
                                Rectangle {
                                    id: satelliteDot
                                    required property int index
                                    width: 9
                                    height: 9
                                    radius: 5
                                    x: parent.width / 2 + Math.cos(satelliteDot.index * Math.PI * 2 / 12) * parent.width * 0.38 - width / 2
                                    y: parent.height / 2 + Math.sin(satelliteDot.index * Math.PI * 2 / 12) * parent.height * 0.38 - height / 2
                                    color: satelliteDot.index < root.satelliteCount ? "#68d0ae" : "#30494a"
                                }
                            }
                            Text { anchors.centerIn: parent; text: root.satelliteCount; color: "#e7f7f2"; font.pixelSize: 26; font.bold: true }
                            Label { anchors.horizontalCenter: parent.horizontalCenter; anchors.top: parent.verticalCenter; anchors.topMargin: 24; text: "SATELLITES"; color: "#718f8e"; font.pixelSize: 7 }
                        }
                        ColumnLayout {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            spacing: 5
                            RowLayout {
                                Layout.fillWidth: true
                                Label { text: "SIGNAL"; color: "#718b8b"; font.pixelSize: 7 }
                                Item { Layout.fillWidth: true }
                                Text { text: root.signalStrength.toFixed(0) + "%"; color: "#d7efea"; font.pixelSize: 13; font.bold: true }
                            }
                            Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 7; color: "#223536"; Rectangle { width: parent.width * root.signalStrength / 100; height: parent.height; color: root.raimAvailable ? "#67c9ad" : "#e88468" } }
                            RowLayout {
                                Layout.fillWidth: true
                                Label { text: "HPL / ACCURACY"; color: "#718b8b"; font.pixelSize: 7 }
                                Item { Layout.fillWidth: true }
                                Text { text: root.navigationAccuracy.toFixed(1) + " m"; color: "#d7efea"; font.pixelSize: 13; font.bold: true }
                            }
                            Rectangle {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 28
                                color: root.gnssTestMode ? "#56372c" : "#183035"
                                border.color: root.gnssTestMode ? "#ee9374" : "#4f807b"
                                Label { anchors.centerIn: parent; text: root.gnssTestMode ? "END INTERFERENCE TEST" : "SIMULATE INTERFERENCE"; color: root.gnssTestMode ? "#ffd0c5" : "#a9d7cd"; font.pixelSize: 8; font.bold: true }
                                MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: { root.selectedMonitor = 3; root.safety.toggleGnssTestMode() } }
                            }
                        }
                    }
                }
            }

            Rectangle {
                Layout.row: 1
                Layout.column: 3
                Layout.columnSpan: 3
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumWidth: 0
                Layout.minimumHeight: 185
                color: "#101b1e"
                border.color: root.selectedMonitor === 4 ? "#72b9dd" : "#42585a"
                border.width: root.selectedMonitor === 4 ? 2 : 1

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 9
                    spacing: 5
                    RowLayout {
                        Layout.fillWidth: true
                        Label { text: "VERTIPORT ALIGNMENT GUIDANCE"; color: "#75bfe0"; font.pixelSize: 10; font.bold: true }
                        Item { Layout.fillWidth: true }
                        Label { text: root.alignmentQuality >= 80 ? "CAPTURED" : "CORRECTING"; color: root.alignmentQuality >= 80 ? "#65d29d" : "#edb15f"; font.pixelSize: 8; font.bold: true }
                    }
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 4
                        Repeater {
                            model: ["VISUAL", "GNSS", "AUTO-LAND"]
                            Rectangle {
                                id: approachButton
                                required property int index
                                required property string modelData
                                Layout.fillWidth: true
                                Layout.preferredHeight: 22
                                color: root.approachMode === approachButton.index ? "#28516a" : "#17292e"
                                border.color: root.approachMode === approachButton.index ? "#79c8e8" : "#344a4e"
                                Label { anchors.centerIn: parent; text: approachButton.modelData; color: root.approachMode === approachButton.index ? "#eaf9ff" : "#789398"; font.pixelSize: 7; font.bold: true }
                                MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: { root.selectedMonitor = 4; root.approachMode = approachButton.index } }
                            }
                        }
                    }
                    RowLayout {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Canvas {
                            id: alignmentCanvas
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            antialiasing: true
                            onPaint: {
                                var context = getContext("2d")
                                context.clearRect(0, 0, width, height)
                                var centerX = width / 2; var centerY = height / 2
                                context.strokeStyle = "#35515a"; context.lineWidth = 1
                                for (var ring = 1; ring <= 3; ++ring) { context.beginPath(); context.arc(centerX, centerY, ring * Math.min(width, height) * 0.12, 0, Math.PI * 2); context.stroke() }
                                context.strokeStyle = "#6ebbdc"; context.lineWidth = 2; context.beginPath(); context.moveTo(centerX, 4); context.lineTo(centerX, height - 4); context.moveTo(4, centerY); context.lineTo(width - 4, centerY); context.stroke()
                                var offsetX = root.lateralDeviation / 2.5 * width * 0.38
                                var offsetY = root.verticalDeviation / 2.5 * height * 0.38
                                context.strokeStyle = root.alignmentQuality >= 80 ? "#6bd5a4" : "#efb45f"; context.lineWidth = 3; context.beginPath(); context.arc(centerX + offsetX, centerY + offsetY, 9, 0, Math.PI * 2); context.stroke()
                                context.beginPath(); context.moveTo(centerX + offsetX - 14, centerY + offsetY); context.lineTo(centerX + offsetX + 14, centerY + offsetY); context.moveTo(centerX + offsetX, centerY + offsetY - 14); context.lineTo(centerX + offsetX, centerY + offsetY + 14); context.stroke()
                            }
                            onWidthChanged: requestPaint()
                            onHeightChanged: requestPaint()
                        }
                        ColumnLayout {
                            Layout.preferredWidth: 154
                            Layout.fillHeight: true
                            spacing: 3
                            RowLayout {
                                Layout.fillWidth: true
                                Label { text: "LATERAL"; color: "#718d94"; font.pixelSize: 7 }
                                Item { Layout.fillWidth: true }
                                Text { text: root.signed(root.lateralDeviation) + " m"; color: "#dceef4"; font.pixelSize: 11; font.bold: true }
                            }
                            RowLayout {
                                Layout.fillWidth: true
                                Label { text: "VERTICAL"; color: "#718d94"; font.pixelSize: 7 }
                                Item { Layout.fillWidth: true }
                                Text { text: root.signed(root.verticalDeviation) + " m"; color: "#dceef4"; font.pixelSize: 11; font.bold: true }
                            }
                            RowLayout {
                                Layout.fillWidth: true
                                Label { text: "RADAR ALT"; color: "#718d94"; font.pixelSize: 7 }
                                Item { Layout.fillWidth: true }
                                Text { text: Number(root.flightModel.altRadar).toFixed(0) + " ft"; color: "#dceef4"; font.pixelSize: 11; font.bold: true }
                            }
                            Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 1; color: "#344c52" }
                            Text { Layout.fillWidth: true; text: root.alignmentQuality.toFixed(0) + "%"; color: root.alignmentQuality >= 80 ? "#73d7aa" : "#efba6b"; font.pixelSize: 22; font.bold: true; horizontalAlignment: Text.AlignHCenter }
                            Label { Layout.fillWidth: true; text: root.landingPhase ? "TERMINAL APPROACH ACTIVE" : "GUIDANCE STANDBY"; color: "#78959b"; font.pixelSize: 7; font.bold: true; horizontalAlignment: Text.AlignHCenter }
                        }
                    }
                }
            }
        }
    }
}