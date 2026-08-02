import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    implicitWidth: 320
    implicitHeight: Math.max(186, contentColumn.implicitHeight + 12)
    property string cardTitle: ""
    property string cardSubtitle: ""
    property string iconType: "generic"
    property string visualTheme: "default"
    property color accentColor: "#74b2e0"
    property real progressValue: 0.0
    property string progressLabel: ""
    property bool showProgress: true
    property var sparkValues: []
    property real motionPhase: 0.0
    property real motionRate: 0.035
    property real motionSeed: 0.0
    default property alias content: customContentColumn.data

    readonly property real normalizedProgress: Math.max(0.0, Math.min(1.0, root.progressValue))
    readonly property string confidenceText: normalizedProgress >= 0.72 ? "High" : (normalizedProgress >= 0.45 ? "Moderate" : "Low")
    readonly property string trendText: {
        var values = root.sparkValues || []
        if (values.length < 2) {
            return "Stable"
        }
        var delta = values[values.length - 1] - values[values.length - 2]
        if (delta > 0.001) {
            return "Rising"
        }
        if (delta < -0.001) {
            return "Falling"
        }
        return "Stable"
    }

    property bool hovered: false

    readonly property string animationStyle: root.visualTheme === "propulsion" ? "propulsion" : root.iconType

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
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 6
        spacing: 3

        RowLayout {
            Layout.fillWidth: true
            spacing: 7

            Rectangle {
                width: 28
                height: 28
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
                        } else if (root.iconType === "propulsion") {
                            ctx.beginPath()
                            ctx.arc(15, 15, 7.2, 0, Math.PI * 2)
                            ctx.stroke()
                            ctx.beginPath()
                            ctx.moveTo(15, 7)
                            ctx.lineTo(15, 23)
                            ctx.moveTo(7, 15)
                            ctx.lineTo(23, 15)
                            ctx.moveTo(9.5, 9.5)
                            ctx.lineTo(20.5, 20.5)
                            ctx.moveTo(20.5, 9.5)
                            ctx.lineTo(9.5, 20.5)
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
                spacing: 0

                Text {
                    text: root.cardTitle
                    font.pixelSize: 12
                    font.bold: true
                    color: "#ffffff"
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }

                Text {
                    visible: root.cardSubtitle.length > 0
                    text: root.cardSubtitle
                    font.pixelSize: 8
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
            font.pixelSize: 8
            color: root.accentColor
            Layout.fillWidth: true
        }

        Canvas {
            id: sparkline
            visible: root.sparkValues && root.sparkValues.length > 1
            Layout.fillWidth: true
            Layout.preferredHeight: root.animationStyle === "attitude" ? 34 : (root.animationStyle === "heading" ? 32 : 30)
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
                    var speedPoints = []
                    for (var s = 0; s < values.length; ++s) {
                        var speedNormalized = (values[s] - minValue) / (maxValue - minValue)
                        var speedY = height - 6 - speedNormalized * (height - 12)
                        var speedX = s * step
                        speedPoints.push({ x: speedX, y: speedY })
                    }

                    if (speedPoints.length > 1) {
                        var speedArea = ctx.createLinearGradient(0, 2, 0, height)
                        speedArea.addColorStop(0.0, "rgba(114, 184, 230, 0.34)")
                        speedArea.addColorStop(1.0, "rgba(114, 184, 230, 0.04)")
                        ctx.fillStyle = speedArea
                        ctx.beginPath()
                        ctx.moveTo(speedPoints[0].x, height - 4)
                        ctx.lineTo(speedPoints[0].x, speedPoints[0].y)
                        for (var sp = 1; sp < speedPoints.length; ++sp) {
                            var prev = speedPoints[sp - 1]
                            var curr = speedPoints[sp]
                            var midX = (prev.x + curr.x) * 0.5
                            var midY = (prev.y + curr.y) * 0.5
                            ctx.quadraticCurveTo(prev.x, prev.y, midX, midY)
                        }
                        var lastPoint = speedPoints[speedPoints.length - 1]
                        ctx.quadraticCurveTo(lastPoint.x, lastPoint.y, lastPoint.x, lastPoint.y)
                        ctx.lineTo(lastPoint.x, height - 4)
                        ctx.closePath()
                        ctx.fill()

                        ctx.beginPath()
                        ctx.moveTo(speedPoints[0].x, speedPoints[0].y)
                        for (var sl = 1; sl < speedPoints.length; ++sl) {
                            var prevLine = speedPoints[sl - 1]
                            var currLine = speedPoints[sl]
                            var midLineX = (prevLine.x + currLine.x) * 0.5
                            var midLineY = (prevLine.y + currLine.y) * 0.5
                            ctx.quadraticCurveTo(prevLine.x, prevLine.y, midLineX, midLineY)
                        }
                        ctx.lineTo(lastPoint.x, lastPoint.y)
                        ctx.strokeStyle = root.accentColor
                        ctx.lineWidth = 2.4
                        ctx.stroke()

                        ctx.strokeStyle = "rgba(230, 245, 255, 0.45)"
                        ctx.lineWidth = 1
                        ctx.beginPath()
                        ctx.moveTo(speedPoints[0].x, speedPoints[0].y - 1.1)
                        for (var sh = 1; sh < speedPoints.length; ++sh) {
                            var prevHigh = speedPoints[sh - 1]
                            var currHigh = speedPoints[sh]
                            var midHighX = (prevHigh.x + currHigh.x) * 0.5
                            var midHighY = (prevHigh.y + currHigh.y) * 0.5 - 1.1
                            ctx.quadraticCurveTo(prevHigh.x, prevHigh.y - 1.1, midHighX, midHighY)
                        }
                        ctx.stroke()
                    }

                    ctx.strokeStyle = "rgba(114, 184, 230, 0.14)"
                    ctx.lineWidth = 1
                    ctx.beginPath()
                    ctx.moveTo(0, height - 8)
                    ctx.lineTo(width, height - 8)
                    ctx.stroke()
                } else if (style === "altitude") {
                    var ladder = 5
                    ctx.strokeStyle = "rgba(220, 232, 242, 0.18)"
                    ctx.lineWidth = 1
                    for (var rung = 0; rung < ladder; ++rung) {
                        var rungY = 4 + rung * (height - 8) / (ladder - 1)
                        ctx.beginPath()
                        ctx.moveTo(0, rungY)
                        ctx.lineTo(width, rungY)
                        ctx.stroke()
                    }

                    ctx.fillStyle = "rgba(180, 220, 190, 0.08)"
                    ctx.fillRect(0, 4, width, height * 0.22)

                    var previousAltY = height - 6
                    var altitudeBarWidth = Math.max(2.0, step * 0.62)

                    function altitudeLevel(normalized) {
                        if (normalized < 0.35) {
                            return "red"
                        }
                        if (normalized < 0.7) {
                            return "yellow"
                        }
                        return "green"
                    }

                    function altitudeColor(level) {
                        if (level === "red") {
                            return "rgba(255, 110, 110, 0.72)"
                        }
                        if (level === "yellow") {
                            return "rgba(255, 213, 96, 0.72)"
                        }
                        return "rgba(116, 215, 132, 0.72)"
                    }

                    function drawAltitudePattern(x, y, w, h, level) {
                        ctx.save()
                        ctx.beginPath()
                        ctx.rect(x, y, w, h)
                        ctx.clip()
                        ctx.lineWidth = 1
                        if (level === "red") {
                            ctx.strokeStyle = "rgba(255, 235, 235, 0.45)"
                            for (var d = -h; d < w + h; d += 4) {
                                ctx.beginPath()
                                ctx.moveTo(x + d, y + h)
                                ctx.lineTo(x + d + h, y)
                                ctx.stroke()
                            }
                        } else if (level === "yellow") {
                            ctx.fillStyle = "rgba(255, 246, 210, 0.42)"
                            for (var px = x + 1; px < x + w - 1; px += 4) {
                                for (var py = y + 1; py < y + h - 1; py += 4) {
                                    ctx.fillRect(px, py, 1.2, 1.2)
                                }
                            }
                        } else {
                            ctx.strokeStyle = "rgba(226, 255, 230, 0.42)"
                            for (var gy = y + 1; gy < y + h; gy += 3) {
                                ctx.beginPath()
                                ctx.moveTo(x, gy)
                                ctx.lineTo(x + w, gy)
                                ctx.stroke()
                            }
                        }
                        ctx.restore()
                    }

                    for (var a = 0; a < values.length; ++a) {
                        var altNormalized = (values[a] - minValue) / (maxValue - minValue)
                        var altY = height - 5 - altNormalized * (height - 10)
                        var altX = a * step

                        var barX = altX - altitudeBarWidth * 0.5
                        var barH = Math.max(2, height - 6 - altY)
                        var altLevel = altitudeLevel(altNormalized)
                        ctx.fillStyle = altitudeColor(altLevel)
                        ctx.fillRect(barX, altY, altitudeBarWidth, barH)
                        drawAltitudePattern(barX, altY, altitudeBarWidth, barH, altLevel)

                        previousAltY = altY
                    }

                    ctx.strokeStyle = "rgba(235, 243, 248, 0.76)"
                    ctx.lineWidth = 1.2
                    ctx.beginPath()
                    ctx.moveTo(0, previousAltY)
                    ctx.lineTo(width, previousAltY)
                    ctx.stroke()

                    var tapeX = width * 0.78
                    ctx.beginPath()
                    ctx.moveTo(tapeX, 3)
                    ctx.lineTo(tapeX, height - 3)
                    ctx.strokeStyle = "rgba(235, 243, 248, 0.66)"
                    ctx.lineWidth = 1.4
                    ctx.stroke()

                    ctx.beginPath()
                    ctx.moveTo(tapeX + 2.5, previousAltY)
                    ctx.lineTo(tapeX + 8.0, previousAltY - 2.5)
                    ctx.lineTo(tapeX + 8.0, previousAltY + 2.5)
                    ctx.closePath()
                    ctx.fillStyle = "rgba(240, 246, 250, 0.92)"
                    ctx.fill()
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
                    var centerY = height * 0.5
                    var waveAmplitude = height * 0.24
                    var midValue = (minValue + maxValue) * 0.5
                    var halfRange = Math.max(0.001, (maxValue - minValue) * 0.5)

                    ctx.fillStyle = "rgba(242, 177, 93, 0.12)"
                    ctx.fillRect(0, centerY - waveAmplitude - 2, width, waveAmplitude * 2 + 4)

                    ctx.strokeStyle = "rgba(245, 210, 148, 0.34)"
                    ctx.lineWidth = 1
                    ctx.beginPath()
                    ctx.moveTo(0, centerY)
                    ctx.lineTo(width, centerY)
                    ctx.stroke()

                    ctx.beginPath()
                    for (var c = 0; c < values.length; ++c) {
                        var signedAttitude = (values[c] - midValue) / halfRange
                        var attX = c * step
                        var attY = centerY - signedAttitude * waveAmplitude
                        if (c === 0) {
                            ctx.moveTo(attX, attY)
                        } else {
                            ctx.lineTo(attX, attY)
                        }
                    }
                    ctx.strokeStyle = root.accentColor
                    ctx.lineWidth = 2.2
                    ctx.stroke()

                    ctx.strokeStyle = "rgba(250, 220, 171, 0.34)"
                    ctx.lineWidth = 1
                    ctx.beginPath()
                    ctx.moveTo(0, centerY - waveAmplitude)
                    ctx.lineTo(width, centerY - waveAmplitude)
                    ctx.moveTo(0, centerY + waveAmplitude)
                    ctx.lineTo(width, centerY + waveAmplitude)
                    ctx.stroke()

                    ctx.fillStyle = "rgba(255, 229, 188, 0.9)"
                    ctx.fillRect(width * 0.5 - 1, centerY - 4, 2, 8)
                } else if (style === "heading") {
                    var headingNow = values[values.length - 1]
                    var tapeY = height - 7
                    var majorStep = width / 8
                    var shift = ((headingNow % 20) / 20) * majorStep

                    ctx.strokeStyle = "rgba(102, 215, 209, 0.42)"
                    ctx.lineWidth = 1.2
                    ctx.beginPath()
                    ctx.moveTo(0, tapeY)
                    ctx.lineTo(width, tapeY)
                    ctx.stroke()

                    for (var ht = -1; ht <= 10; ++ht) {
                        var baseX = ht * majorStep - shift
                        for (var minor = 0; minor < 4; ++minor) {
                            var tickX = baseX + minor * (majorStep / 4)
                            if (tickX < 0 || tickX > width) {
                                continue
                            }
                            var tickHeight = minor === 0 ? 8 : (minor === 2 ? 5 : 3)
                            ctx.beginPath()
                            ctx.moveTo(tickX, tapeY)
                            ctx.lineTo(tickX, tapeY - tickHeight)
                            ctx.stroke()
                        }
                    }

                    var compassX = width * 0.5 + Math.sin(root.motionPhase * 1.2) * (width * 0.03)
                    ctx.beginPath()
                    ctx.moveTo(compassX, 2)
                    ctx.lineTo(compassX - 4, 8)
                    ctx.lineTo(compassX + 4, 8)
                    ctx.closePath()
                    ctx.fillStyle = "rgba(204, 255, 251, 0.86)"
                    ctx.fill()
                } else if (style === "fpv") {
                    ctx.strokeStyle = "rgba(255, 143, 112, 0.22)"
                    ctx.lineWidth = 1
                    ctx.beginPath()
                    ctx.moveTo(0, height - 8)
                    ctx.lineTo(width, height - 8)
                    ctx.stroke()

                    ctx.strokeStyle = "rgba(255, 143, 112, 0.34)"
                    ctx.strokeRect(width * 0.42, height * 0.24, width * 0.16, height * 0.34)

                    for (var e = 0; e < values.length; ++e) {
                        var fpvNormalized = (values[e] - minValue) / (maxValue - minValue)
                        var fpvX = e * step
                        var fpvY = height * 0.78 - fpvNormalized * (height * 0.52)

                        var dotRadius = e === values.length - 1 ? 2.8 : (e % 3 === 0 ? 2.0 : 1.5)
                        ctx.beginPath()
                        ctx.arc(fpvX, fpvY, dotRadius, 0, Math.PI * 2)
                        ctx.fillStyle = e === values.length - 1 ? "rgba(255, 223, 212, 0.95)" : "rgba(255, 143, 112, 0.7)"
                        ctx.fill()

                        if (e > 0) {
                            var previousValue = values[e - 1]
                            var previousNormalized = (previousValue - minValue) / (maxValue - minValue)
                            var previousY = height * 0.78 - previousNormalized * (height * 0.52)
                            var deviation = Math.abs(fpvY - previousY)
                            if (deviation > 4.0) {
                                ctx.strokeStyle = "rgba(255, 168, 145, 0.32)"
                                ctx.lineWidth = 1
                                ctx.beginPath()
                                ctx.moveTo(fpvX, fpvY)
                                ctx.lineTo(fpvX, previousY)
                                ctx.stroke()
                            }
                        }
                    }

                    var markerX = (root.motionPhase / (Math.PI * 2)) * width
                    var markerY = height * 0.65 - Math.sin(root.motionPhase * 1.2) * 5
                    ctx.beginPath()
                    ctx.arc(markerX, markerY, 4, 0, Math.PI * 2)
                    ctx.fillStyle = root.accentColor
                    ctx.fill()

                    ctx.beginPath()
                    ctx.moveTo(markerX - 6, markerY)
                    ctx.lineTo(markerX + 6, markerY)
                    ctx.moveTo(markerX, markerY - 6)
                    ctx.lineTo(markerX, markerY + 6)
                    ctx.strokeStyle = "rgba(225, 246, 255, 0.65)"
                    ctx.lineWidth = 1
                    ctx.stroke()
                } else if (style === "propulsion") {
                    ctx.strokeStyle = "rgba(115, 198, 255, 0.16)"
                    ctx.lineWidth = 1
                    for (var p = 0; p <= 6; ++p) {
                        var pY = 4 + p * (height - 8) / 6
                        ctx.beginPath()
                        ctx.moveTo(0, pY)
                        ctx.lineTo(width, pY)
                        ctx.stroke()
                    }

                    for (var b = 0; b < values.length; ++b) {
                        var propulsionNormalized = (values[b] - minValue) / (maxValue - minValue)
                        var propulsionBarHeight = 3 + propulsionNormalized * (height - 8)
                        var propulsionX = b * step - 2.2
                        var propulsionY = height - 4 - propulsionBarHeight
                        ctx.fillStyle = b === values.length - 1 ? "rgba(220, 245, 255, 0.96)" : "rgba(116, 194, 255, 0.78)"
                        ctx.fillRect(propulsionX, propulsionY, 4.4, propulsionBarHeight)
                    }

                    var propulsionSweep = (root.motionPhase / (Math.PI * 2)) * width
                    ctx.strokeStyle = "rgba(234, 247, 255, 0.8)"
                    ctx.lineWidth = 1.2
                    ctx.beginPath()
                    ctx.moveTo(propulsionSweep, 2)
                    ctx.lineTo(propulsionSweep, height - 2)
                    ctx.stroke()
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
                var motionUnit = root.motionPhase / (Math.PI * 2)

                if (style === "speed") {
                    var speedScanX = motionUnit * width
                    var speedGradient = ctx.createLinearGradient(speedScanX - 12, 0, speedScanX + 12, 0)
                    speedGradient.addColorStop(0.0, "rgba(0, 0, 0, 0)")
                    speedGradient.addColorStop(0.5, "rgba(210, 241, 255, 0.65)")
                    speedGradient.addColorStop(1.0, "rgba(0, 0, 0, 0)")
                    ctx.strokeStyle = speedGradient
                    ctx.lineWidth = 1.6
                    ctx.beginPath()
                    ctx.moveTo(speedScanX, 2)
                    ctx.lineTo(speedScanX, height - 2)
                    ctx.stroke()

                    ctx.beginPath()
                    ctx.moveTo(width - 2, lastY - 4)
                    ctx.lineTo(width + 2, lastY)
                    ctx.lineTo(width - 2, lastY + 4)
                    ctx.lineTo(width - 6, lastY)
                    ctx.closePath()
                    ctx.fillStyle = "rgba(220, 245, 255, 0.9)"
                    ctx.fill()
                } else if (style === "altitude") {
                    var sweepY = 3 + motionUnit * (height - 6)
                    ctx.fillStyle = "rgba(180, 225, 255, 0.12)"
                    ctx.fillRect(0, sweepY - 1.2, width, 2.4)

                    ctx.strokeStyle = "rgba(214, 240, 255, 0.88)"
                    ctx.lineWidth = 1.4
                    ctx.beginPath()
                    ctx.moveTo(width - 9, lastY - 4)
                    ctx.lineTo(width - 3, lastY - 4)
                    ctx.lineTo(width - 3, lastY + 4)
                    ctx.lineTo(width - 9, lastY + 4)
                    ctx.stroke()
                } else if (style === "attitude") {
                    ctx.beginPath()
                    ctx.arc(width - 4, lastY, 2.4, 0, Math.PI * 2)
                    ctx.fillStyle = "rgba(255, 229, 188, 0.92)"
                    ctx.fill()
                } else if (style === "heading") {
                    var headingPulseX = motionUnit * width
                    ctx.strokeStyle = "rgba(138, 241, 235, 0.52)"
                    ctx.lineWidth = 1.2
                    ctx.beginPath()
                    ctx.moveTo(headingPulseX, 2)
                    ctx.lineTo(headingPulseX, 8)
                    ctx.stroke()

                    ctx.beginPath()
                    ctx.arc(width - 4.5, height - 7, 3.2, 0, Math.PI * 2)
                    ctx.strokeStyle = "rgba(211, 255, 252, 0.92)"
                    ctx.lineWidth = 1.2
                    ctx.stroke()
                } else if (style === "fpv") {
                    var fpvDotPulse = 1.6 + Math.max(0, Math.sin(root.motionPhase * 2.2)) * 1.1
                    ctx.beginPath()
                    ctx.arc(width - 4.5, lastY, fpvDotPulse, 0, Math.PI * 2)
                    ctx.fillStyle = "rgba(255, 214, 201, 0.9)"
                    ctx.fill()
                } else {
                    ctx.beginPath()
                    ctx.arc(width - 2, lastY, 3.2, 0, Math.PI * 2)
                    ctx.fillStyle = root.accentColor
                    ctx.fill()
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
            Layout.preferredHeight: root.animationStyle === "heading" ? 10 : (root.animationStyle === "attitude" ? 9 : 8)
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

                    ctx.strokeStyle = "rgba(223, 244, 255, 0.30)"
                    ctx.lineWidth = 1
                    var stripeOffset = (root.motionPhase % 1.0) * 9
                    for (var ss = -6; ss < fillWidth; ss += 9) {
                        ctx.beginPath()
                        ctx.moveTo(ss + stripeOffset, 1)
                        ctx.lineTo(ss + stripeOffset + 5, height - 1)
                        ctx.stroke()
                    }

                    var pulseX = Math.max(3, fillWidth - 5)
                    ctx.fillStyle = "rgba(255,255,255,0.28)"
                    ctx.fillRect(fillWidth * 0.45, 2, Math.max(1, width * 0.03), height - 4)
                    ctx.beginPath()
                    ctx.arc(pulseX, height * 0.5, 2.3, 0, Math.PI * 2)
                    ctx.fillStyle = "rgba(230, 247, 255, 0.92)"
                    ctx.fill()
                } else if (style === "altitude") {
                    var segments = 8
                    var segmentWidth = width / segments
                    var altitudeValues = root.sparkValues || []
                    var progressMin = 0.0
                    var progressMax = 1.0
                    if (altitudeValues.length > 0) {
                        progressMin = altitudeValues[0]
                        progressMax = altitudeValues[0]
                        for (var ai = 1; ai < altitudeValues.length; ++ai) {
                            progressMin = Math.min(progressMin, altitudeValues[ai])
                            progressMax = Math.max(progressMax, altitudeValues[ai])
                        }
                        if (Math.abs(progressMax - progressMin) < 0.001) {
                            progressMax = progressMin + 1
                        }
                    }

                    function progressLevel(normalized) {
                        if (normalized < 0.35) {
                            return "red"
                        }
                        if (normalized < 0.7) {
                            return "yellow"
                        }
                        return "green"
                    }

                    function progressColor(level) {
                        if (level === "red") {
                            return "#ff6d6d"
                        }
                        if (level === "yellow") {
                            return "#ffd35f"
                        }
                        return "#79d689"
                    }

                    function drawSegmentPattern(x, y, w, h, level) {
                        ctx.save()
                        ctx.beginPath()
                        ctx.rect(x, y, w, h)
                        ctx.clip()
                        ctx.lineWidth = 1
                        if (level === "red") {
                            ctx.strokeStyle = "rgba(255, 235, 235, 0.42)"
                            for (var dx = -h; dx < w + h; dx += 4) {
                                ctx.beginPath()
                                ctx.moveTo(x + dx, y + h)
                                ctx.lineTo(x + dx + h, y)
                                ctx.stroke()
                            }
                        } else if (level === "yellow") {
                            ctx.fillStyle = "rgba(255, 246, 210, 0.40)"
                            for (var sx = x + 1; sx < x + w - 1; sx += 4) {
                                ctx.fillRect(sx, y + h * 0.45, 1.2, 1.2)
                            }
                        } else {
                            ctx.strokeStyle = "rgba(226, 255, 230, 0.40)"
                            for (var sy = y + 1; sy < y + h; sy += 3) {
                                ctx.beginPath()
                                ctx.moveTo(x, sy)
                                ctx.lineTo(x + w, sy)
                                ctx.stroke()
                            }
                        }
                        ctx.restore()
                    }

                    for (var s = 0; s < segments; ++s) {
                        if (s * segmentWidth < fillWidth) {
                            var segX = s * segmentWidth + 1
                            var segW = segmentWidth - 2
                            var sampleIndex = altitudeValues.length > 0
                                ? Math.min(altitudeValues.length - 1, Math.round((s / Math.max(1, segments - 1)) * (altitudeValues.length - 1)))
                                : 0
                            var sampleNormalized = altitudeValues.length > 0
                                ? (altitudeValues[sampleIndex] - progressMin) / (progressMax - progressMin)
                                : (s / Math.max(1, segments - 1))
                            var segLevel = progressLevel(sampleNormalized)
                            ctx.fillStyle = progressColor(segLevel)
                            ctx.fillRect(segX, 1, segW, height - 2)
                            drawSegmentPattern(segX, 1, segW, height - 2, segLevel)
                        }
                    }
                    var ladderMarker = Math.max(3, fillWidth - 3)
                    ctx.strokeStyle = "rgba(235, 245, 255, 0.86)"
                    ctx.lineWidth = 1
                    ctx.beginPath()
                    ctx.moveTo(ladderMarker, 1)
                    ctx.lineTo(ladderMarker, height - 1)
                    ctx.moveTo(ladderMarker - 3, 2)
                    ctx.lineTo(ladderMarker, 2)
                    ctx.moveTo(ladderMarker - 3, height - 2)
                    ctx.lineTo(ladderMarker, height - 2)
                    ctx.stroke()
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
                    var stableWidth = Math.max(4, (width - 2) * pct)
                    var stableX = (width - stableWidth) * 0.5
                    ctx.fillStyle = "rgba(242, 177, 93, 0.24)"
                    roundRect(ctx, stableX, 1, stableWidth, height - 2, 4)
                    ctx.fill()

                    ctx.strokeStyle = "rgba(255, 230, 192, 0.58)"
                    ctx.lineWidth = 1
                    ctx.beginPath()
                    ctx.moveTo(width * 0.5, 1)
                    ctx.lineTo(width * 0.5, height - 1)
                    ctx.stroke()

                    var loadX = stableX + stableWidth - 1
                    ctx.fillStyle = "rgba(255, 233, 198, 0.78)"
                    ctx.fillRect(loadX, 1, 2, height - 2)
                } else if (style === "heading") {
                    var stripY = Math.max(1, height * 0.5 - 1)
                    ctx.strokeStyle = "rgba(199, 255, 251, 0.36)"
                    ctx.lineWidth = 1
                    for (var h = 0; h <= 10; ++h) {
                        var hx = h * (width / 10)
                        var tickH = h % 2 === 0 ? 3 : 2
                        ctx.beginPath()
                        ctx.moveTo(hx, stripY - tickH)
                        ctx.lineTo(hx, stripY + tickH)
                        ctx.stroke()
                    }

                    var headingMarkerX = 1 + pct * (width - 2)
                    ctx.strokeStyle = root.accentColor
                    ctx.lineWidth = 1.6
                    ctx.beginPath()
                    ctx.moveTo(headingMarkerX, 1)
                    ctx.lineTo(headingMarkerX, height - 1)
                    ctx.stroke()

                    ctx.beginPath()
                    ctx.moveTo(headingMarkerX, 1)
                    ctx.lineTo(headingMarkerX - 3.5, 5)
                    ctx.lineTo(headingMarkerX + 3.5, 5)
                    ctx.closePath()
                    ctx.fillStyle = "rgba(213, 255, 252, 0.92)"
                    ctx.fill()
                } else if (style === "fpv") {
                    var dotCount = 11
                    var dotSpacing = (width - 10) / (dotCount - 1)
                    var activeDots = Math.round(pct * (dotCount - 1))
                    for (var fd = 0; fd < dotCount; ++fd) {
                        var dotCenterX = 5 + fd * dotSpacing
                        var dotRadius = fd === activeDots ? 2.6 : 2.0
                        ctx.beginPath()
                        ctx.arc(dotCenterX, height * 0.5, dotRadius, 0, Math.PI * 2)
                        if (fd <= activeDots) {
                            ctx.fillStyle = fd === activeDots ? "rgba(255, 222, 212, 0.95)" : "rgba(255, 143, 112, 0.75)"
                        } else {
                            ctx.fillStyle = "rgba(255, 143, 112, 0.16)"
                        }
                        ctx.fill()
                    }

                    ctx.strokeStyle = "rgba(255, 200, 184, 0.4)"
                    ctx.lineWidth = 1
                    ctx.beginPath()
                    ctx.moveTo(5, height * 0.5)
                    ctx.lineTo(width - 5, height * 0.5)
                    ctx.stroke()
                } else if (style === "propulsion") {
                    var coilSegments = 12
                    var coilWidth = (width - 2) / coilSegments
                    for (var cg = 0; cg < coilSegments; ++cg) {
                        var coilX = 1 + cg * coilWidth
                        var coilFill = Math.min(1.0, Math.max(0.0, pct - (cg / coilSegments) * 0.35))
                        var coilHeight = Math.max(2, (height - 2) * Math.min(1.0, coilFill))
                        var coilY = height - 1 - coilHeight
                        ctx.fillStyle = cg % 2 === 0 ? "rgba(112, 198, 255, 0.8)" : "rgba(216, 244, 255, 0.92)"
                        roundRect(ctx, coilX + 1, coilY, Math.max(2, coilWidth - 2), coilHeight, 3)
                        ctx.fill()
                    }

                    ctx.strokeStyle = "rgba(218, 243, 255, 0.42)"
                    ctx.lineWidth = 1
                    ctx.beginPath()
                    ctx.moveTo(Math.max(1, fillWidth - 3), 1)
                    ctx.lineTo(Math.max(1, fillWidth - 3), height - 1)
                    ctx.stroke()

                    ctx.beginPath()
                    ctx.arc(fillWidth, height * 0.5, 2.6, 0, Math.PI * 2)
                    ctx.fillStyle = "rgba(230, 247, 255, 0.9)"
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

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 34
            radius: 6
            color: "#122536"
            border.color: "#24425a"
            border.width: 1

            GridLayout {
                anchors.fill: parent
                anchors.margins: 8
                columns: 2
                rowSpacing: 2
                columnSpacing: 8

                Text {
                    Layout.row: 0
                    Layout.column: 0
                    Layout.fillWidth: true
                    text: "Status"
                    font.pixelSize: 7
                    color: "#8db2cb"
                }

                Text {
                    Layout.row: 0
                    Layout.column: 1
                    Layout.preferredWidth: 66
                    Layout.minimumWidth: 66
                    Layout.fillWidth: true
                    text: "Trend"
                    font.pixelSize: 7
                    color: "#8db2cb"
                    horizontalAlignment: Text.AlignRight
                }

                Text {
                    Layout.row: 1
                    Layout.column: 0
                    Layout.fillWidth: true
                    text: root.confidenceText + " confidence"
                    font.pixelSize: 9
                    color: "#d8ecff"
                    font.bold: true
                }

                Text {
                    Layout.row: 1
                    Layout.column: 1
                    Layout.preferredWidth: 66
                    Layout.minimumWidth: 66
                    Layout.fillWidth: true
                    text: root.trendText
                    font.pixelSize: 9
                    color: root.accentColor
                    font.bold: true
                    horizontalAlignment: Text.AlignRight
                }
            }
        }

        Item {
            Layout.fillHeight: false
            Layout.preferredHeight: 2
        }

        ColumnLayout {
            id: customContentColumn
            Layout.fillWidth: true
            spacing: 2
        }
    }
}
