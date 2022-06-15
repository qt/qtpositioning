// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick 2.0
import QtPositioning

Rectangle {
    id: container
    property int maxX: parent.width; property int maxY: parent.height
//! [props]
    property var coordinate
//! [props]

    Binding {
        target: container
        property: "coordinate"
        value: positionSource.position.coordinate
    }

    width: 300; height: 130
    color: "blue"
    opacity: 0.7
    border.color: "black"
    border.width: 1
    radius: 5
    gradient: Gradient {
        GradientStop {position: 0.0; color: "grey"}
        GradientStop {position: 1.0; color: "black"}
    }
    MouseArea {
        anchors.fill: parent
        drag.target: parent
        drag.axis: Drag.XandYAxis
        drag.minimumX: -(parent.width * (2/3)); drag.maximumX: parent.maxX - (parent.width/3)
        drag.minimumY: -(parent.height/2); drag.maximumY: parent.maxY - (parent.height/2)
    }
//! [locatebutton-top]
    Button {
        id: locateButton
        text: "Locate & update"
//! [locatebutton-top]
        anchors {left: parent.left; leftMargin: 5}
        y: 3; height: 32; width: parent.width - 10
//! [locatebutton-clicked]
        onClicked: {
            if (positionSource.supportedPositioningMethods ===
                    PositionSource.NoPositioningMethods) {
                positionSource.nmeaSource = "nmealog.txt";
                sourceText.text = "(filesource): " + printableMethod(positionSource.supportedPositioningMethods);
            }
            positionSource.update();
        }
    }
//! [locatebutton-clicked]
//! [possrc]
    PositionSource {
        id: positionSource
        onPositionChanged: { planet.source = "images/sun.png"; }

        onSourceErrorChanged: {
            if (sourceError == PositionSource.UpdateTimeoutError) {
                activityText.fadeOut = true
                return
            }

            if (sourceError == PositionSource.NoError)
                return

            console.log("Source error: " + sourceError)
            activityText.fadeOut = true
            stop()
        }
    }
//! [possrc]
    function printableMethod(method) {
        if (method === PositionSource.SatellitePositioningMethods)
            return "Satellite";
        else if (method === PositionSource.NoPositioningMethods)
            return "Not available"
        else if (method === PositionSource.NonSatellitePositioningMethods)
            return "Non-satellite"
        else if (method === PositionSource.AllPositioningMethods)
            return "Multiple"
        return "source error";
    }

    Grid {
        id: locationGrid
        columns: 2
        anchors {left: parent.left; leftMargin: 5; top: locateButton.bottom; topMargin: 5}
        spacing: 5
        Text {color: "white"; font.bold: true
            text: "Lat:"; style: Text.Raised; styleColor: "black"
        }
        Text {id: latitudeValue; color: "white"; font.bold: true
            text: positionSource.position.coordinate.latitude; style: Text.Raised; styleColor: "black";
        }
        Text {color: "white"; font.bold: true
            text: "Lon:"; style: Text.Raised; styleColor: "black"
        }
        Text {id: longitudeValue; color: "white"; font.bold: true
            text: positionSource.position.coordinate.longitude; style: Text.Raised; styleColor: "black"
        }
    }
    Image {
        id: planet
        anchors {top: locationGrid.bottom; left: parent.left; leftMargin: locationGrid.anchors.leftMargin}
        source: "images/moon.png"
        width: 30; height: 30
    }
    Text {id: sourceText; color: "white"; font.bold: true;
        anchors {left: planet.right; leftMargin: 5; verticalCenter: planet.verticalCenter}
        text: "Source: " + printableMethod(positionSource.supportedPositioningMethods); style: Text.Raised; styleColor: "black";
    }

    Text {
        id: activityText; color: "white"; font.bold: true;
        anchors { top: planet.bottom; horizontalCenter: parent.horizontalCenter }
        property bool fadeOut: false

        text: {
            if (fadeOut)
                return qsTr("Timeout occurred!");
            else if (positionSource.active)
                return qsTr("Retrieving update...")
            else
                return ""
        }

        Timer {
            id: fadeoutTimer; repeat: false; interval: 3000; running: activityText.fadeOut
            onTriggered: { activityText.fadeOut = false; }
        }
    }
}
