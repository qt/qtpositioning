// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Shapes

Rectangle {
    id: root

    // Sample text to calculate sky view size properly
    Text {
        id: sampleText
        visible: false
        text: "270\u00B0"
    }

    property var satellitesModel

    property color inViewColor: "blue"
    property color inUseColor: "red"

    property real maxCircleRadius: width < height ? width / 2 - sampleText.contentWidth * 1.1
                                                  : height / 2 - sampleText.contentHeight * 1.1
    property real radiusStep: maxCircleRadius / 3
    property int centerX: width / 2
    property int centerY: height / 2

    // Shape for the SkyView
    Shape {
        // Circles
        component Circle: PathAngleArc {
            property real radius: 0
            centerX: root.centerX
            centerY: root.centerY
            radiusX: radius
            radiusY: radius
            startAngle: 0
            sweepAngle: 360
        }

        ShapePath {
            id: circles
            strokeColor: "black"

            Circle {
                radius: root.radiusStep
            }
            Circle {
                radius: 2 * root.radiusStep
            }
            Circle {
                radius: root.maxCircleRadius
            }
        }

        // Lines
        component Line: ShapePath {
            id: line
            strokeColor: "black"
            property real angle
            property real radianAngle: line.angle / 180 * Math.PI
            startX: root.centerX + root.maxCircleRadius * Math.cos(radianAngle)
            startY: root.centerY + root.maxCircleRadius * Math.sin(radianAngle)
            PathLine {
                x: root.centerX + root.maxCircleRadius * Math.cos(line.radianAngle + Math.PI)
                y: root.centerY + root.maxCircleRadius * Math.sin(line.radianAngle + Math.PI)
            }
        }

        Line {
            angle: 0
        }
        Line {
            angle: 45
        }
        Line {
            angle: 90
        }
        Line {
            angle: 135
        }

        // Azimuth captions
        component AzimuthCaption: Item {
            property string text
            property real angle
            property real offsetX: 0
            property real offsetY: 0
            width: textElement.implicitWidth
            height: textElement.implicitHeight
            Text {
                id: textElement
                // Subtract 90, because 0 should be on top, not on the right
                property real radianAngle: (parent.angle - 90) / 180 * Math.PI
                // end of line coordinates
                property int lineX: root.centerX + root.maxCircleRadius * Math.cos(radianAngle)
                property int lineY: root.centerY + root.maxCircleRadius * Math.sin(radianAngle)
                text: parent.text
                x: lineX + parent.offsetX
                y: lineY + parent.offsetY
            }
        }

        AzimuthCaption {
            text: angle.toFixed(0) + '\u00B0'
            angle: 0
            offsetY: -height
            offsetX: -width / 2
        }

        AzimuthCaption {
            text: angle.toFixed(0) + '\u00B0'
            angle: 45
            offsetY: -height
        }

        AzimuthCaption {
            text: angle.toFixed(0) + '\u00B0'
            angle: 90
            offsetY: -height / 2
        }

        AzimuthCaption {
            text: angle.toFixed(0) + '\u00B0'
            angle: 135
        }

        AzimuthCaption {
            text: angle.toFixed(0) + '\u00B0'
            angle: 180
            offsetX: -width / 2
        }

        AzimuthCaption {
            text: angle.toFixed(0) + '\u00B0'
            angle: 225
            offsetX: -width
        }

        AzimuthCaption {
            text: angle.toFixed(0) + '\u00B0'
            angle: 270
            offsetX: -width
            offsetY: -height / 2
        }

        AzimuthCaption {
            text: angle.toFixed(0) + '\u00B0'
            angle: 315
            offsetX: -width
            offsetY: -height / 2
        }

        // Elevation captions
        component ElevationCaption: Item {
            property string text
            property real step
            Text {
                text: parent.text
                x: root.centerX - parent.step * root.radiusStep + 5
                y: root.centerY + 5
            }
        }

        ElevationCaption {
            text: "60\u00B0"
            step: 1
        }
        ElevationCaption {
            text: "30\u00B0"
            step: 2
        }
        ElevationCaption {
            text: "0\u00B0"
            step: 3
        }
    }

    // Actual satellite positions
    Item {
        id: satelliteItemView
        anchors.fill: parent

        Repeater {
            model: root.satellitesModel
            delegate: Rectangle {
                required property var modelData
                width: 30
                height: width
                radius: width / 2
                color: modelData.inUse ? root.inUseColor : root.inViewColor
                visible: modelData.azimuth > -1 && modelData.elevation > -1
                property real angle: (modelData.azimuth - 90) / 180 * Math.PI
                property real distance: root.maxCircleRadius * (90 - modelData.elevation) / 90
                x: root.centerX + distance * Math.cos(angle) - width / 2
                y: root.centerY + distance * Math.sin(angle) - height / 2
                Text {
                    anchors.centerIn: parent
                    text: parent.modelData.id
                }
            }
        }
    }
}
