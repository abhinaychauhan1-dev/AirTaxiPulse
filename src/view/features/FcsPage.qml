pragma ComponentBehavior: Bound

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    required property var moduleRegistry

    readonly property var flightModel: root.moduleRegistry.primaryFlight
    property bool autopilotEngaged: true
    property int guidanceMode: 0
    property int commandedFlightMode: -1
    property bool automaticRouteSequence: true
    property int selectedWaypoint: 1
    property int hoveredWaypoint: -1
    property real animationPhase: 0.0
    property real manualLeftElevon: 0.0
    property real manualRightElevon: 0.0
    property real manualRuddervator: 0.0
    property real manualTiltActuator: 45.0
    property var waypoints: [
        { code: "PAD A", name: "Downtown Vertiport", latitude: 37.7749, longitude: -122.4194 },
        { code: "TRN-1", name: "Transition Gate", latitude: 37.7792, longitude: -122.4124 },
        { code: "CRZ-2", name: "Bay Corridor", latitude: 37.7868, longitude: -122.4012 },
        { code: "APP-1", name: "Approach Fix", latitude: 37.7935, longitude: -122.3928 },
        { code: "PAD B", name: "Harbor Vertiport", latitude: 37.7978, longitude: -122.3860 }
    ]

    readonly property string flightMode: {
        var sourceMode = String(root.flightModel.flightModeLabel)
        if (sourceMode.indexOf("Hover") >= 0 || sourceMode.indexOf("Lift-Off") >= 0)
            return "HOVER"
        if (sourceMode.indexOf("Transition") >= 0)
            return "TRANSITION"
        if (sourceMode.indexOf("Cruise") >= 0)
            return "CRUISE"
        return "VERTICAL LANDING"
    }
    readonly property int telemetryFlightModeIndex: root.flightMode === "HOVER" ? 0
                                                    : (root.flightMode === "TRANSITION" ? 1
                                                    : (root.flightMode === "CRUISE" ? 2 : 3))
    readonly property int flightModeIndex: root.commandedFlightMode >= 0
                                           ? root.commandedFlightMode : root.telemetryFlightModeIndex
    readonly property real telemetryLeftElevon: root.clamp(Number(root.flightModel.pitch) * 0.55
                                                           - Number(root.flightModel.roll) * 0.75, -18, 18)
    readonly property real telemetryRightElevon: root.clamp(Number(root.flightModel.pitch) * 0.55
                                                            + Number(root.flightModel.roll) * 0.75, -18, 18)
    readonly property real telemetryRuddervator: root.clamp(Number(root.flightModel.yaw) * 0.8, -20, 20)
    readonly property real telemetryTiltActuator: Number(root.flightModel.tiltAngleDeg)
    readonly property real leftElevon: root.autopilotEngaged ? root.telemetryLeftElevon : root.manualLeftElevon
    readonly property real rightElevon: root.autopilotEngaged ? root.telemetryRightElevon : root.manualRightElevon
    readonly property real ruddervator: root.autopilotEngaged ? root.telemetryRuddervator : root.manualRuddervator
    readonly property real tiltActuator: root.autopilotEngaged ? root.telemetryTiltActuator : root.manualTiltActuator
    readonly property int automaticWaypoint: Math.min(root.waypoints.length - 1, root.flightModeIndex + 1)
    readonly property int navigationWaypoint: root.automaticRouteSequence
                                              ? root.automaticWaypoint : root.selectedWaypoint
    readonly property int activeWaypoint: root.hoveredWaypoint >= 0
                                          ? root.hoveredWaypoint : root.navigationWaypoint
    readonly property real nextWaypointDistance: root.distanceToWaypoint(root.navigationWaypoint)
    readonly property string routeLegStatus: root.automaticRouteSequence
                                              ? "AUTO SEQUENCE" : "DIRECT TO"

    function clamp(value, minimum, maximum) {
        return Math.max(minimum, Math.min(maximum, value))
    }

    function modeName(index) {
        return ["HOVER", "TRANSITION", "CRUISE", "VERTICAL LANDING"][index]
    }

    function guidanceName(index) {
        return ["ROUTE", "HEADING", "ATTITUDE"][index]
    }

    function actuatorName(index) {
        return ["LEFT ELEVON", "RIGHT ELEVON", "RUDDERVATOR", "TILT ACTUATOR"][index]
    }

    function actuatorValue(index) {
        return [root.leftElevon, root.rightElevon, root.ruddervator, root.tiltActuator][index]
    }

    function actuatorMinimum(index) {
        return index === 3 ? 0 : (index === 2 ? -20 : -18)
    }

    function actuatorMaximum(index) {
        return index === 3 ? 90 : (index === 2 ? 20 : 18)
    }

    function toggleAutopilot() {
        if (root.autopilotEngaged) {
            root.manualLeftElevon = root.telemetryLeftElevon
            root.manualRightElevon = root.telemetryRightElevon
            root.manualRuddervator = root.telemetryRuddervator
            root.manualTiltActuator = root.telemetryTiltActuator
        }
        root.autopilotEngaged = !root.autopilotEngaged
    }

    function setManualActuator(index, value) {
        var boundedValue = root.clamp(value, root.actuatorMinimum(index), root.actuatorMaximum(index))
        if (index === 0)
            root.manualLeftElevon = boundedValue
        else if (index === 1)
            root.manualRightElevon = boundedValue
        else if (index === 2)
            root.manualRuddervator = boundedValue
        else
            root.manualTiltActuator = boundedValue
    }

    function selectDirectTo(index) {
        root.selectedWaypoint = index
        root.automaticRouteSequence = false
        root.guidanceMode = 0
        root.hoveredWaypoint = -1
        routeCanvas.requestPaint()
    }

    function resumeAutomaticRoute() {
        root.automaticRouteSequence = true
        root.selectedWaypoint = root.automaticWaypoint
        root.guidanceMode = 0
        routeCanvas.requestPaint()
    }

    function distanceToWaypoint(index) {
        var waypoint = root.waypoints[index]
        var latitude = Number(root.flightModel.gpsLatitude)
        var longitude = Number(root.flightModel.gpsLongitude)
        if (!waypoint || !isFinite(latitude) || !isFinite(longitude))
            return 0
        var radians = Math.PI / 180
        var latitudeDelta = (waypoint.latitude - latitude) * radians
        var longitudeDelta = (waypoint.longitude - longitude) * radians
        var a = Math.sin(latitudeDelta / 2) * Math.sin(latitudeDelta / 2)
                + Math.cos(latitude * radians) * Math.cos(waypoint.latitude * radians)
                * Math.sin(longitudeDelta / 2) * Math.sin(longitudeDelta / 2)
        return 3440.065 * 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1 - a))
    }

    Timer {
        interval: 50
        running: root.visible
        repeat: true
        onTriggered: {
            root.animationPhase = (root.animationPhase + 0.018) % 1.0
            routeCanvas.requestPaint()
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
                    text: "Flight Control System & Automation"
                    color: "#f2f8fb"
                    font.pixelSize: 18
                    font.bold: true
                    elide: Text.ElideRight
                }
                Text {
                    Layout.fillWidth: true
                    text: "Guidance, control-law authority, actuator response and active route management"
                    color: "#8faab4"
                    font.pixelSize: 9
                    elide: Text.ElideRight
                }
            }

            Rectangle {
                    Layout.preferredWidth: 76
                    Layout.preferredHeight: 28
                    color: root.commandedFlightMode >= 0 ? "#312a19" : "#14262c"
                    border.color: root.commandedFlightMode >= 0 ? "#d4ad62" : "#41616a"
                    border.width: 1
                    Label {
                        anchors.centerIn: parent
                        text: root.commandedFlightMode >= 0 ? "MODE CMD" : "MODE AUTO"
                        color: root.commandedFlightMode >= 0 ? "#f1cd82" : "#8eb5bf"
                        font.pixelSize: 7
                        font.bold: true
                    }
                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: root.commandedFlightMode = -1
                    }
                }

                Rectangle {
                Layout.preferredWidth: 172
                Layout.preferredHeight: 28
                color: root.autopilotEngaged ? "#102b25" : "#2c2118"
                border.color: root.autopilotEngaged ? "#63d6a2" : "#e2a35f"
                border.width: 1

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 6
                    spacing: 6
                    Rectangle {
                        Layout.preferredWidth: 8
                        Layout.preferredHeight: 8
                        radius: 4
                        color: root.autopilotEngaged ? "#63d6a2" : "#e2a35f"
                    }
                    Label {
                        Layout.fillWidth: true
                        text: root.autopilotEngaged ? "AUTOPILOT ENGAGED" : "FLY-BY-WIRE MANUAL"
                        color: root.autopilotEngaged ? "#b9f1d8" : "#f3ca99"
                        font.pixelSize: 8
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 54
            color: "#0d191d"
            border.color: "#30484e"
            border.width: 1

            RowLayout {
                anchors.fill: parent
                anchors.margins: 5
                spacing: 5

                Repeater {
                    model: 4
                    Rectangle {
                        id: modeStep
                        required property int index
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        color: root.flightModeIndex === modeStep.index ? "#214f54" : "#13262b"
                        border.color: root.flightModeIndex === modeStep.index ? "#75d9d0"
                                      : (modeStep.index < root.flightModeIndex ? "#4a8175" : "#29444b")
                        border.width: root.flightModeIndex === modeStep.index ? 2 : 1

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 6
                            spacing: 6
                            Rectangle {
                                Layout.preferredWidth: 18
                                Layout.preferredHeight: 18
                                radius: 9
                                color: modeStep.index <= root.flightModeIndex ? "#63c7b6" : "#253e44"
                                Label {
                                    anchors.centerIn: parent
                                    text: modeStep.index + 1
                                    color: modeStep.index <= root.flightModeIndex ? "#08191a" : "#76939a"
                                    font.pixelSize: 8
                                    font.bold: true
                                }
                            }
                            Label {
                                Layout.fillWidth: true
                                text: root.modeName(modeStep.index)
                                color: root.flightModeIndex === modeStep.index ? "#e3fffa" : "#83a1a6"
                                font.pixelSize: 8
                                minimumPixelSize: 6
                                fontSizeMode: Text.Fit
                                font.bold: true
                            }
                        }

                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: root.commandedFlightMode = modeStep.index
                        }
                    }
                }
            }
        }

        GridLayout {
            id: dashboard
            Layout.fillWidth: true
            Layout.fillHeight: true
            columns: 5
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
                color: "#101a20"
                border.color: root.autopilotEngaged ? "#3f756b" : "#715334"
                border.width: 1

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 7

                    RowLayout {
                        Layout.fillWidth: true
                        Label { text: "AUTOPILOT & CONTROL LAW"; color: "#72d1bc"; font.pixelSize: 10; font.bold: true }
                        Item { Layout.fillWidth: true }
                        Label { text: "FBW 3-LANE"; color: "#6f9298"; font.pixelSize: 7; font.bold: true }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 44
                        color: root.autopilotEngaged ? "#174036" : "#3a291b"
                        border.color: root.autopilotEngaged ? "#69d9ac" : "#e4a45e"
                        border.width: 1
                        Column {
                            anchors.centerIn: parent
                            spacing: 1
                            Label { anchors.horizontalCenter: parent.horizontalCenter; text: root.autopilotEngaged ? "ENGAGED" : "DISENGAGED"; color: root.autopilotEngaged ? "#aaf0d0" : "#ffd09c"; font.pixelSize: 12; font.bold: true }
                            Label { anchors.horizontalCenter: parent.horizontalCenter; text: root.autopilotEngaged ? "AUTONOMOUS GUIDANCE" : "PILOT COMMAND"; color: "#7f9aa0"; font.pixelSize: 7 }
                        }
                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: root.toggleAutopilot()
                        }
                    }

                    Label { text: "GUIDANCE TARGET"; color: "#69868d"; font.pixelSize: 7 }
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 4
                        Repeater {
                            model: 3
                            Rectangle {
                                id: guidanceButton
                                required property int index
                                Layout.fillWidth: true
                                Layout.preferredHeight: 28
                                color: root.guidanceMode === guidanceButton.index ? "#28535f" : "#17282e"
                                border.color: root.guidanceMode === guidanceButton.index ? "#75c9df" : "#30474e"
                                border.width: 1
                                Label {
                                    anchors.centerIn: parent
                                    text: root.guidanceName(guidanceButton.index)
                                    color: root.guidanceMode === guidanceButton.index ? "#e5f9ff" : "#79939a"
                                    font.pixelSize: 7
                                    font.bold: true
                                }
                                MouseArea {
                                    anchors.fill: parent
                                    cursorShape: Qt.PointingHandCursor
                                    onClicked: root.guidanceMode = guidanceButton.index
                                }
                            }
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        Label { text: "CONTROL AUTHORITY"; color: "#69868d"; font.pixelSize: 7 }
                        Item { Layout.fillWidth: true }
                        Label { text: root.autopilotEngaged ? "92%" : "38%"; color: "#cbe9e4"; font.pixelSize: 9; font.bold: true }
                    }
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 6
                        radius: 3
                        color: "#223238"
                        Rectangle {
                            width: parent.width * (root.autopilotEngaged ? 0.92 : 0.38)
                            height: parent.height
                            radius: 3
                            color: root.autopilotEngaged ? "#64c6b2" : "#d79b55"
                            Behavior on width { NumberAnimation { duration: 220 } }
                        }
                    }
                }
            }

            Rectangle {
                Layout.row: 0
                Layout.column: 2
                Layout.columnSpan: 3
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumWidth: 0
                Layout.minimumHeight: 180
                color: "#101a20"
                border.color: "#3d5c65"
                border.width: 1

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 6
                    RowLayout {
                        Layout.fillWidth: true
                        Label { text: "CONTROL SURFACE / ACTUATOR DEFLECTION"; color: "#76cbe3"; font.pixelSize: 10; font.bold: true }
                        Item { Layout.fillWidth: true }
                        Label {
                            text: root.autopilotEngaged ? "LIVE COMMAND / FEEDBACK" : "MANUAL FBW COMMAND"
                            color: root.autopilotEngaged ? "#688991" : "#e0ad63"
                            font.pixelSize: 7
                            font.bold: !root.autopilotEngaged
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        spacing: 8

                        Repeater {
                            model: 4
                            Rectangle {
                                id: actuatorGauge
                                required property int index
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                color: "#13252b"
                                border.color: actuatorGauge.index === 3 ? "#665d42" : "#2d505a"
                                border.width: 1

                                readonly property real value: root.actuatorValue(actuatorGauge.index)
                                readonly property real minimum: root.actuatorMinimum(actuatorGauge.index)
                                readonly property real maximum: root.actuatorMaximum(actuatorGauge.index)
                                readonly property real normalized: (value - minimum) / (maximum - minimum)

                                ColumnLayout {
                                    anchors.fill: parent
                                    anchors.margins: 6
                                    spacing: 3
                                    Label {
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: 13
                                        text: root.actuatorName(actuatorGauge.index)
                                        color: actuatorGauge.index === 3 ? "#d5bd77" : "#83bdcc"
                                        font.pixelSize: 7
                                        minimumPixelSize: 6
                                        fontSizeMode: Text.Fit
                                        font.bold: true
                                        horizontalAlignment: Text.AlignHCenter
                                    }
                                    Item {
                                        id: actuatorTrack
                                        Layout.fillWidth: true
                                        Layout.fillHeight: true
                                        Rectangle { anchors.horizontalCenter: parent.horizontalCenter; anchors.top: parent.top; anchors.bottom: parent.bottom; width: 8; radius: 4; color: "#20373e" }
                                        Rectangle {
                                            anchors.left: parent.left
                                            anchors.right: parent.right
                                            height: 2
                                            y: (1.0 - actuatorGauge.normalized) * Math.max(0, parent.height - height)
                                            color: actuatorGauge.index === 3 ? "#f0c969" : "#7ed8ed"
                                            Behavior on y { NumberAnimation { duration: 320; easing.type: Easing.OutCubic } }
                                        }
                                        Rectangle { anchors.horizontalCenter: parent.horizontalCenter; anchors.verticalCenter: parent.verticalCenter; width: 20; height: 1; color: "#50646a"; visible: actuatorGauge.index !== 3 }
                                        MouseArea {
                                            anchors.fill: parent
                                            enabled: !root.autopilotEngaged
                                            hoverEnabled: true
                                            cursorShape: enabled ? Qt.SizeVerCursor : Qt.ArrowCursor
                                            onPressed: function(mouse) {
                                                var normalized = 1.0 - mouse.y / Math.max(1, height)
                                                root.setManualActuator(actuatorGauge.index,
                                                    actuatorGauge.minimum + normalized * (actuatorGauge.maximum - actuatorGauge.minimum))
                                            }
                                            onPositionChanged: function(mouse) {
                                                if (!pressed)
                                                    return
                                                var normalized = 1.0 - mouse.y / Math.max(1, height)
                                                root.setManualActuator(actuatorGauge.index,
                                                    actuatorGauge.minimum + normalized * (actuatorGauge.maximum - actuatorGauge.minimum))
                                            }
                                        }
                                    }
                                    Text {
                                        Layout.fillWidth: true
                                        text: actuatorGauge.value.toFixed(1) + "°"
                                        color: "#eef9fb"
                                        font.pixelSize: 12
                                        font.bold: true
                                        horizontalAlignment: Text.AlignHCenter
                                    }
                                }
                            }
                        }
                    }
                }
            }

            Rectangle {
                Layout.row: 1
                Layout.column: 0
                Layout.columnSpan: 5
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumWidth: 0
                Layout.minimumHeight: 190
                color: "#0f191e"
                border.color: "#405961"
                border.width: 1

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 10

                    ColumnLayout {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        spacing: 5
                        RowLayout {
                            Layout.fillWidth: true
                            Label { text: "FLIGHT PLAN & ACTIVE ROUTE"; color: "#e0b66d"; font.pixelSize: 10; font.bold: true }
                            Item { Layout.fillWidth: true }
                            Rectangle {
                                Layout.preferredWidth: 76
                                Layout.preferredHeight: 20
                                color: root.automaticRouteSequence ? "#1d4a43" : "#4b3822"
                                border.color: root.automaticRouteSequence ? "#62cbb1" : "#deb15e"
                                Label {
                                    anchors.centerIn: parent
                                    text: root.routeLegStatus
                                    color: root.automaticRouteSequence ? "#c9f5e8" : "#f5d18a"
                                    font.pixelSize: 7
                                    font.bold: true
                                }
                                MouseArea {
                                    anchors.fill: parent
                                    cursorShape: Qt.PointingHandCursor
                                    onClicked: root.resumeAutomaticRoute()
                                }
                            }
                            Label { text: "TRACK " + Number(root.flightModel.track).toFixed(0) + "°"; color: "#82b8c7"; font.pixelSize: 8; font.bold: true }
                        }

                        Canvas {
                            id: routeCanvas
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            antialiasing: true

                            onPaint: {
                                var context = getContext("2d")
                                context.clearRect(0, 0, width, height)
                                var points = [[width * 0.08, height * 0.72], [width * 0.27, height * 0.46], [width * 0.49, height * 0.57], [width * 0.71, height * 0.30], [width * 0.92, height * 0.42]]
                                context.strokeStyle = "rgba(55, 93, 103, 0.34)"
                                context.lineWidth = 1
                                for (var gridX = 0; gridX < width; gridX += 34) {
                                    context.beginPath(); context.moveTo(gridX, 0); context.lineTo(gridX, height); context.stroke()
                                }
                                for (var gridY = 0; gridY < height; gridY += 28) {
                                    context.beginPath(); context.moveTo(0, gridY); context.lineTo(width, gridY); context.stroke()
                                }
                                context.strokeStyle = "#4ca8bd"
                                context.lineWidth = 3
                                context.beginPath(); context.moveTo(points[0][0], points[0][1])
                                for (var pointIndex = 1; pointIndex < points.length; ++pointIndex)
                                    context.lineTo(points[pointIndex][0], points[pointIndex][1])
                                context.stroke()

                                var segment = Math.min(3, root.flightModeIndex)
                                var progress = root.flightModeIndex === 0 ? 0.18 : (root.flightModeIndex === 1 ? 0.48 : (root.flightModeIndex === 2 ? 0.68 : 0.88))
                                var start = points[segment]
                                var end = points[Math.min(segment + 1, points.length - 1)]
                                var aircraftX = start[0] + (end[0] - start[0]) * progress
                                var aircraftY = start[1] + (end[1] - start[1]) * progress

                                for (var index = 0; index < points.length; ++index) {
                                    context.fillStyle = index === root.activeWaypoint ? "#ffd37b" : "#79d3df"
                                    context.beginPath(); context.arc(points[index][0], points[index][1], index === root.activeWaypoint ? 6 : 4, 0, Math.PI * 2); context.fill()
                                    context.fillStyle = "#b8d0d5"; context.font = "8px sans-serif"
                                    context.fillText(root.waypoints[index].code, points[index][0] + 8, points[index][1] - 7)
                                }

                                context.save(); context.translate(aircraftX, aircraftY); context.rotate(Number(root.flightModel.track) * Math.PI / 180)
                                context.fillStyle = "#f5fbff"; context.beginPath(); context.moveTo(0, -9); context.lineTo(6, 7); context.lineTo(0, 4); context.lineTo(-6, 7); context.closePath(); context.fill(); context.restore()
                                var pulse = 3 + 3 * Math.abs(Math.sin(root.animationPhase * Math.PI * 2))
                                context.strokeStyle = "rgba(126, 221, 231, 0.7)"; context.lineWidth = 1; context.beginPath(); context.arc(aircraftX, aircraftY, 8 + pulse, 0, Math.PI * 2); context.stroke()
                            }
                            onWidthChanged: requestPaint()
                            onHeightChanged: requestPaint()
                        }
                    }

                    Rectangle {
                        Layout.preferredWidth: Math.min(300, Math.max(220, parent.width * 0.28))
                        Layout.fillHeight: true
                        color: "#132329"
                        border.color: "#344f57"
                        border.width: 1

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 8
                            spacing: 4
                            RowLayout {
                                Layout.fillWidth: true
                                ColumnLayout {
                                    Layout.fillWidth: true
                                    spacing: 0
                                    Label { text: root.automaticRouteSequence ? "NEXT WAYPOINT" : "DIRECT TO"; color: "#718e95"; font.pixelSize: 7 }
                                    Text { Layout.fillWidth: true; text: root.waypoints[root.navigationWaypoint].code; color: "#ffd481"; font.pixelSize: 16; font.bold: true }
                                }
                                ColumnLayout {
                                    spacing: 0
                                    Label { text: "DISTANCE"; color: "#718e95"; font.pixelSize: 7 }
                                    Text { text: root.nextWaypointDistance.toFixed(2) + " NM"; color: "#e9f6f7"; font.pixelSize: 14; font.bold: true }
                                }
                            }

                            Repeater {
                                model: root.waypoints
                                Rectangle {
                                    id: waypointRow
                                    required property int index
                                    required property var modelData
                                    Layout.fillWidth: true
                                    Layout.fillHeight: true
                                    Layout.minimumHeight: 22
                                    color: root.activeWaypoint === waypointRow.index ? "#294b51" : "#182b31"
                                    border.color: root.navigationWaypoint === waypointRow.index ? "#e5ba68" : "#294047"
                                    border.width: root.navigationWaypoint === waypointRow.index ? 2 : 1

                                    RowLayout {
                                        anchors.fill: parent
                                        anchors.margins: 5
                                        spacing: 6
                                        Rectangle { Layout.preferredWidth: 7; Layout.preferredHeight: 7; radius: 4; color: waypointRow.index < root.navigationWaypoint ? "#5fc89d" : (waypointRow.index === root.navigationWaypoint ? "#f0c46d" : "#48656d") }
                                        Label { text: waypointRow.modelData.code; color: "#dcecee"; font.pixelSize: 8; font.bold: true; Layout.preferredWidth: 42 }
                                        Label { Layout.fillWidth: true; text: waypointRow.modelData.name; color: "#78959b"; font.pixelSize: 7; elide: Text.ElideRight }
                                    }

                                    MouseArea {
                                        anchors.fill: parent
                                        hoverEnabled: true
                                        cursorShape: Qt.PointingHandCursor
                                        onEntered: root.hoveredWaypoint = waypointRow.index
                                        onExited: if (root.hoveredWaypoint === waypointRow.index) root.hoveredWaypoint = -1
                                        onClicked: root.selectDirectTo(waypointRow.index)
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