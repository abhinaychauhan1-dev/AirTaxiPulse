import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    implicitWidth: 320
    implicitHeight: 180
    property string cardTitle: ""
    property string cardSubtitle: ""
    property string iconType: "generic"
    property color accentColor: "#74b2e0"
    property real progressValue: 0.0
    property string progressLabel: ""
    property bool showProgress: true
    property var sparkValues: []
    property real motionPhase: 0.0
    property real motionRate: 0.035
    property real motionSeed: 0.0
    default property alias content: contentColumn.data

    property bool hovered: false

    readonly property string animationStyle: root.iconType

    scale: hovered ? 1.01 : 1.0

    Behavior on scale {
        NumberAnimation {
            duration: 160
            easing.type: Easing.OutCubic
        }
    }

    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#12202a" }
            GradientStop { position: 1.0; color: "#0f1d29" }
        }
        radius: 12
        border.color: root.hovered ? root.accentColor : "#2f4f67"
        border.width: 1
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        acceptedButtons: Qt.NoButton
        onEntered: root.hovered = true
        onExited: root.hovered = false
    }

    Timer {
        interval: 60
        running: true
        repeat: true
        onTriggered: {
            root.motionPhase += root.motionRate
            if (root.motionPhase > Math.PI * 2) {
                root.motionPhase -= Math.PI * 2
            }
            iconCanvas.requestPaint()
            sparkline.requestPaint()
            progressCanvas.requestPaint()
        }
    }

    Component.onCompleted: {
        root.motionPhase = root.motionSeed
    }

    ColumnLayout {
        id: contentColumn
        anchors.fill: parent
        anchors.margins: 8
        spacing: 5

        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Rectangle {
                width: 30
                height: 30
                radius: 15
                color: "#14222d"
                border.color: root.accentColor
                border.width: 1

                Rectangle {
                    anchors.centerIn: parent
                    width: 18 + 5 * Math.max(0, Math.sin(root.motionPhase))
                    height: width
                    radius: width / 2
                    color: "transparent"
                    border.color: root.accentColor
                    border.width: 1
                    opacity: 0.25
                }

                Canvas {
                    id: iconCanvas
                    anchors.fill: parent
                    antialiasing: true
                    renderTarget: Canvas.Image

                    onPaint: {
                        var ctx = getContext("2d")
                        ctx.clearRect(0, 0, width, height)
                        ctx.strokeStyle = root.accentColor
                        ctx.fillStyle = root.accentColor
                        ctx.lineWidth = 1.8
                        ctx.lineCap = "round"
                        ctx.lineJoin = "round"

                        function drawCircle(x, y, r) {
                            ctx.beginPath()
                            ctx.arc(x, y, r, 0, Math.PI * 2)
                            ctx.stroke()
                        }

                        if (root.iconType === "speed") {
                            ctx.beginPath()
                            ctx.arc(15, 15, 8.5, Math.PI * 1.1, Math.PI * 1.95)
                            ctx.stroke()
                            ctx.beginPath()
                            ctx.moveTo(15, 15)
                            ctx.lineTo(20, 11)
                            ctx.stroke()
                        } else if (root.iconType === "altitude") {
                            ctx.fillRect(9, 11, 4, 9)
                            ctx.fillRect(17, 8, 4, 12)
                            ctx.beginPath()
                            ctx.moveTo(15, 21)
                            ctx.lineTo(15, 10)
                            ctx.moveTo(12, 13)
                            ctx.lineTo(15, 10)
                            ctx.lineTo(18, 13)
                            ctx.stroke()
                        } else if (root.iconType === "vertical") {
                            ctx.beginPath()
                            ctx.moveTo(15, 8)
                            ctx.lineTo(15, 23)
                            ctx.moveTo(11, 12)
                            ctx.lineTo(15, 8)
                            ctx.lineTo(19, 12)
                            ctx.moveTo(11, 19)
                            ctx.lineTo(15, 23)
                            ctx.lineTo(19, 19)
                            ctx.stroke()
                        } else if (root.iconType === "attitude") {
                            drawCircle(15, 15, 8.2)
                            ctx.beginPath()
                            ctx.moveTo(9, 17)
                            ctx.lineTo(21, 17)
                            ctx.moveTo(12, 12)
                            ctx.lineTo(18, 12)
                            ctx.stroke()
                        } else if (root.iconType === "heading") {
                            drawCircle(15, 15, 8.2)
                            ctx.beginPath()
                            ctx.moveTo(15, 9)
                            ctx.lineTo(18, 16)
                            ctx.lineTo(15, 14)
                            ctx.lineTo(12, 16)
                            ctx.closePath()
                            ctx.fill()
                        } else if (root.iconType === "fpv") {
                            ctx.beginPath()
                            ctx.moveTo(9, 20)
                            ctx.quadraticCurveTo(14, 15, 21, 10)
                            ctx.stroke()
                            drawCircle(21, 10, 2.2)
                            ctx.beginPath()
                            ctx.moveTo(13, 19)
                            ctx.lineTo(17, 15)
                            ctx.stroke()
                        } else {
                            ctx.beginPath()
                            ctx.moveTo(9, 15)
                            ctx.lineTo(21, 15)
                            ctx.stroke()
                        }
                    }

                    onWidthChanged: requestPaint()
                    onHeightChanged: requestPaint()
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 1

                Text {
                    text: root.cardTitle
                    font.pixelSize: 13
                    font.bold: true
                    color: "#ffffff"
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }

                Text {
                    visible: root.cardSubtitle.length > 0
                    text: root.cardSubtitle
                    font.pixelSize: 9
                    color: "#9fb8c8"
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                }
            }

            Rectangle {
                width: 8
                height: 8
                radius: 4
                color: root.accentColor
                opacity: 0.35

                SequentialAnimation on opacity {
                    running: true
                    loops: Animation.Infinite
                    NumberAnimation { to: 1.0; duration: 850; easing.type: Easing.InOutSine }
                    NumberAnimation { to: 0.35; duration: 850; easing.type: Easing.InOutSine }
                }
            }
        }

        Text {
            visible: root.progressLabel.length > 0
            text: root.progressLabel
            font.pixelSize: 9
            color: root.accentColor
            Layout.fillWidth: true
        }

        Canvas {
            id: sparkline
            visible: root.sparkValues && root.sparkValues.length > 1
            Layout.fillWidth: true
            Layout.preferredHeight: 34
            antialiasing: true

            onPaint: {
                var ctx = getContext("2d")
                ctx.clearRect(0, 0, width, height)

                var values = root.sparkValues || []
                if (values.length < 2) {
                    return
                }

                var style = root.animationStyle

                ctx.strokeStyle = "rgba(175, 205, 225, 0.08)"
                ctx.lineWidth = 1
                for (var grid = 0; grid <= 4; ++grid) {
                    var yGrid = 4 + grid * (height - 8) / 4
                    ctx.beginPath()
                    ctx.moveTo(0, yGrid)
                    ctx.lineTo(width, yGrid)
                    ctx.stroke()
                }

                var minValue = values[0]
                var maxValue = values[0]
                for (var i = 1; i < values.length; ++i) {
                    minValue = Math.min(minValue, values[i])
                    maxValue = Math.max(maxValue, values[i])
                }

                if (Math.abs(maxValue - minValue) < 0.001) {
                    maxValue = minValue + 1
                }

                var step = width / (values.length - 1)

                if (style === "speed") {
                    ctx.beginPath()
                    for (var s = 0; s < values.length; ++s) {
                        var speedNormalized = (values[s] - minValue) / (maxValue - minValue)
                        var speedY = height - 6 - speedNormalized * (height - 12)
                        var speedX = s * step
                        if (s === 0) {
                            ctx.moveTo(speedX, speedY)
                        } else {
                            ctx.lineTo(speedX, speedY)
                        }
                    }
                    ctx.strokeStyle = root.accentColor
                    ctx.lineWidth = 2.2
                    ctx.stroke()
                    ctx.strokeStyle = "rgba(114, 184, 230, 0.14)"
                    ctx.lineWidth = 1
                    ctx.beginPath()
                    ctx.moveTo(0, height - 8)
                    ctx.lineTo(width, height - 8)
                    ctx.stroke()
                } else if (style === "altitude") {
                    var ladder = 5
                    ctx.strokeStyle = "rgba(136, 197, 255, 0.18)"
                    ctx.lineWidth = 1
                    for (var rung = 0; rung < ladder; ++rung) {
                        var rungY = 4 + rung * (height - 8) / (ladder - 1)
                        ctx.beginPath()
                        ctx.moveTo(0, rungY)
                        ctx.lineTo(width, rungY)
                        ctx.stroke()
                    }
                    ctx.beginPath()
                    for (var a = 0; a < values.length; ++a) {
                        var altNormalized = (values[a] - minValue) / (maxValue - minValue)
                        var altY = height - 5 - altNormalized * (height - 10)
                        var altX = a * step
                        if (a === 0) {
                            ctx.moveTo(altX, altY)
                        } else {
                            ctx.lineTo(altX, altY)
                        }
                    }
                    ctx.strokeStyle = root.accentColor
                    ctx.lineWidth = 2.2
                    ctx.stroke()
                    ctx.beginPath()
                    ctx.moveTo(width * 0.78, 3)
                    ctx.lineTo(width * 0.78, height - 3)
                    ctx.strokeStyle = "rgba(136, 197, 255, 0.65)"
                    ctx.lineWidth = 1.4
                    ctx.stroke()
                } else if (style === "vertical") {
                    ctx.strokeStyle = "rgba(121, 213, 122, 0.14)"
                    ctx.lineWidth = 1
                    ctx.beginPath()
                    ctx.moveTo(width * 0.5, 3)
                    ctx.lineTo(width * 0.5, height - 3)
                    ctx.stroke()
                    for (var b = 0; b < values.length; ++b) {
                        var vsNormalized = (values[b] - minValue) / (maxValue - minValue)
                        var vsX = b * step
                        var vsY = height - 5 - vsNormalized * (height - 10)
                        var barHeight = height - vsY - 5
                        ctx.fillStyle = vsNormalized >= 0.5 ? root.accentColor : "rgba(121, 213, 122, 0.6)"
                        ctx.fillRect(vsX - 1.2, vsY, 2.4, barHeight)
                    }
                } else if (style === "attitude") {
                    ctx.strokeStyle = "rgba(174, 205, 225, 0.16)"
                    ctx.lineWidth = 1
                    ctx.beginPath()
                    ctx.moveTo(0, height * 0.5)
                    ctx.lineTo(width, height * 0.5)
                    ctx.stroke()
                    ctx.beginPath()
                    for (var c = 0; c < values.length; ++c) {
                        var attNormalized = (values[c] - minValue) / (maxValue - minValue)
                        var attX = c * step
                        var attY = height * 0.65 - attNormalized * (height * 0.38)
                        if (c === 0) {
                            ctx.moveTo(attX, attY)
                        } else {
                            ctx.lineTo(attX, attY)
                        }
                    }
                    ctx.strokeStyle = root.accentColor
                    ctx.lineWidth = 2.2
                    ctx.stroke()
                    var rollAngle = Math.sin(root.motionPhase) * 12
                    ctx.save()
                    ctx.translate(width * 0.5, height * 0.52)
                    ctx.rotate(rollAngle * Math.PI / 180)
                    ctx.strokeStyle = "rgba(116, 178, 224, 0.85)"
                    ctx.lineWidth = 1.6
                    ctx.beginPath()
                    ctx.moveTo(-width * 0.18, 0)
                    ctx.lineTo(width * 0.18, 0)
                    ctx.stroke()
                    ctx.restore()
                } else if (style === "heading") {
                    ctx.strokeStyle = "rgba(158, 196, 245, 0.16)"
                    ctx.lineWidth = 1
                    ctx.beginPath()
                    ctx.moveTo(0, height - 6)
                    ctx.lineTo(width, height - 6)
                    ctx.stroke()
                    ctx.beginPath()
                    for (var d = 0; d < values.length; ++d) {
                        var hdgNormalized = (values[d] - minValue) / (maxValue - minValue)
                        var hdgX = d * step
                        var hdgY = height - 6 - hdgNormalized * (height - 10)
                        if (d === 0) {
                            ctx.moveTo(hdgX, hdgY)
                        } else {
                            ctx.lineTo(hdgX, hdgY)
                        }
                    }
                    ctx.strokeStyle = root.accentColor
                    ctx.lineWidth = 2.2
                    ctx.stroke()
                    var compassX = (root.motionPhase / (Math.PI * 2)) * width
                    ctx.strokeStyle = "rgba(158, 196, 245, 0.65)"
                    ctx.lineWidth = 1.4
                    ctx.beginPath()
                    ctx.moveTo(compassX, 3)
                    ctx.lineTo(compassX, height - 3)
                    ctx.stroke()
                    ctx.beginPath()
                    ctx.arc(width * 0.5, height * 0.5, 5, 0, Math.PI * 2)
                    ctx.stroke()
                } else if (style === "fpv") {
                    ctx.strokeStyle = "rgba(110, 200, 255, 0.16)"
                    ctx.lineWidth = 1
                    ctx.beginPath()
                    ctx.moveTo(0, height - 8)
                    ctx.lineTo(width, height - 8)
                    ctx.stroke()
                    ctx.beginPath()
                    for (var e = 0; e < values.length; ++e) {
                        var fpvNormalized = (values[e] - minValue) / (maxValue - minValue)
                        var fpvX = e * step
                        var fpvY = height * 0.78 - fpvNormalized * (height * 0.52)
                        if (e === 0) {
                            ctx.moveTo(fpvX, fpvY)
                        } else {
                            ctx.lineTo(fpvX, fpvY)
                        }
                    }
                    ctx.strokeStyle = root.accentColor
                    ctx.lineWidth = 2.2
                    ctx.stroke()
                    var markerX = (root.motionPhase / (Math.PI * 2)) * width
                    var markerY = height * 0.65 - Math.sin(root.motionPhase * 1.2) * 5
                    ctx.beginPath()
                    ctx.arc(markerX, markerY, 4, 0, Math.PI * 2)
                    ctx.fillStyle = root.accentColor
                    ctx.fill()
                } else {
                    ctx.beginPath()
                    for (var j = 0; j < values.length; ++j) {
                        var normalized = (values[j] - minValue) / (maxValue - minValue)
                        var y = height - 4 - normalized * (height - 8)
                        var x = j * step
                        if (j === 0) {
                            ctx.moveTo(x, y)
                        } else {
                            ctx.lineTo(x, y)
                        }
                    }
                    ctx.strokeStyle = root.accentColor
                    ctx.lineWidth = 2
                    ctx.stroke()
                }

                var lastValue = values[values.length - 1]
                var lastNormalized = (lastValue - minValue) / (maxValue - minValue)
                var lastY = height - 4 - lastNormalized * (height - 8)
                ctx.beginPath()
                ctx.arc(width - 2, lastY, 3.2, 0, Math.PI * 2)
                ctx.fillStyle = root.accentColor
                ctx.fill()

                if (style !== "fpv") {
                    var scanX = (root.motionPhase / (Math.PI * 2)) * width
                    var gradient = ctx.createLinearGradient(scanX - 10, 0, scanX + 10, 0)
                    gradient.addColorStop(0.0, "rgba(0, 0, 0, 0)")
                    gradient.addColorStop(0.5, root.accentColor)
                    gradient.addColorStop(1.0, "rgba(0, 0, 0, 0)")
                    ctx.strokeStyle = gradient
                    ctx.lineWidth = 1.5
                    ctx.beginPath()
                    ctx.moveTo(scanX, 2)
                    ctx.lineTo(scanX, height - 2)
                    ctx.stroke()
                }
            }

            onVisibleChanged: requestPaint()
            onWidthChanged: requestPaint()
            onHeightChanged: requestPaint()
        }

        Canvas {
            id: progressCanvas
            visible: root.showProgress
            Layout.fillWidth: true
            Layout.preferredHeight: 9
            antialiasing: true

            onPaint: {
                var ctx = getContext("2d")
                ctx.clearRect(0, 0, width, height)

                ctx.strokeStyle = "#29455b"
                ctx.fillStyle = "#1a2b38"
                ctx.lineWidth = 1
                roundRect(ctx, 0.5, 0.5, width - 1, height - 1, 5)
                ctx.fill()
                ctx.stroke()

                var pct = Math.max(0.0, Math.min(1.0, root.progressValue))
                var fillWidth = Math.max(2, width * pct)
                var style = root.animationStyle

                if (style === "speed") {
                    var grad = ctx.createLinearGradient(0, 0, fillWidth, 0)
                    grad.addColorStop(0.0, root.accentColor)
                    grad.addColorStop(1.0, "#dff4ff")
                    ctx.fillStyle = grad
                    roundRect(ctx, 1, 1, fillWidth - 2, height - 2, 5)
                    ctx.fill()
                    ctx.fillStyle = "rgba(255,255,255,0.25)"
                    ctx.fillRect(fillWidth * 0.55, 2, Math.max(1, width * 0.04), height - 4)
                } else if (style === "altitude") {
                    var segments = 8
                    var segmentWidth = width / segments
                    for (var s = 0; s < segments; ++s) {
                        if (s * segmentWidth < fillWidth) {
                            ctx.fillStyle = s % 2 === 0 ? root.accentColor : "#d2ecff"
                            ctx.fillRect(s * segmentWidth + 1, 1, segmentWidth - 2, height - 2)
                        }
                    }
                    ctx.fillStyle = "rgba(255,255,255,0.22)"
                    ctx.fillRect(fillWidth * 0.82, 1, 2, height - 2)
                } else if (style === "vertical") {
                    ctx.fillStyle = root.accentColor
                    roundRect(ctx, 1, 1, fillWidth - 2, height - 2, 5)
                    ctx.fill()
                    ctx.strokeStyle = "rgba(255,255,255,0.24)"
                    ctx.beginPath()
                    ctx.moveTo(width * 0.5, 1)
                    ctx.lineTo(width * 0.5, height - 1)
                    ctx.stroke()
                    ctx.fillStyle = "rgba(255,255,255,0.32)"
                    ctx.beginPath()
                    ctx.moveTo(fillWidth - 5, 1)
                    ctx.lineTo(fillWidth + 3, height * 0.5)
                    ctx.lineTo(fillWidth - 5, height - 1)
                    ctx.fill()
                } else if (style === "attitude") {
                    ctx.fillStyle = root.accentColor
                    roundRect(ctx, 1, 1, fillWidth - 2, height - 2, 5)
                    ctx.fill()
                    var tilt = (Math.sin(root.motionPhase) * 0.5 + 0.5) * fillWidth
                    ctx.fillStyle = "rgba(255,255,255,0.24)"
                    ctx.fillRect(tilt, 1, 3, height - 2)
                } else if (style === "heading") {
                    ctx.fillStyle = root.accentColor
                    roundRect(ctx, 1, 1, fillWidth - 2, height - 2, 5)
                    ctx.fill()
                    ctx.strokeStyle = "rgba(255,255,255,0.3)"
                    ctx.beginPath()
                    ctx.moveTo(width * 0.25, 1)
                    ctx.lineTo(width * 0.25, height - 1)
                    ctx.moveTo(width * 0.5, 1)
                    ctx.lineTo(width * 0.5, height - 1)
                    ctx.moveTo(width * 0.75, 1)
                    ctx.lineTo(width * 0.75, height - 1)
                    ctx.stroke()
                } else if (style === "fpv") {
                    ctx.fillStyle = root.accentColor
                    roundRect(ctx, 1, 1, fillWidth - 2, height - 2, 5)
                    ctx.fill()
                    ctx.fillStyle = "rgba(255,255,255,0.36)"
                    var dotX = Math.max(4, fillWidth - 6)
                    ctx.beginPath()
                    ctx.arc(dotX, height * 0.5, 2.6, 0, Math.PI * 2)
                    ctx.fill()
                } else {
                    ctx.fillStyle = root.accentColor
                    roundRect(ctx, 1, 1, fillWidth - 2, height - 2, 5)
                    ctx.fill()
                }
            }

            function roundRect(ctx, x, y, w, h, r) {
                var radius = Math.min(r, w / 2, h / 2)
                ctx.beginPath()
                ctx.moveTo(x + radius, y)
                ctx.lineTo(x + w - radius, y)
                ctx.quadraticCurveTo(x + w, y, x + w, y + radius)
                ctx.lineTo(x + w, y + h - radius)
                ctx.quadraticCurveTo(x + w, y + h, x + w - radius, y + h)
                ctx.lineTo(x + radius, y + h)
                ctx.quadraticCurveTo(x, y + h, x, y + h - radius)
                ctx.lineTo(x, y + radius)
                ctx.quadraticCurveTo(x, y, x + radius, y)
                ctx.closePath()
            }

            onVisibleChanged: requestPaint()
            onWidthChanged: requestPaint()
            onHeightChanged: requestPaint()
        }
    }
}
