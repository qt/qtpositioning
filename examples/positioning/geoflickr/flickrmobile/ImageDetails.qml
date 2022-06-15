// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick 2.0
import "../flickrcommon" as Common

Flipable {
    id: container

    property var frontContainer: containerFront
    property string photoTitle: ""
    property string photoDate
    property string photoUrl
    property real prevScale: 1.0

    signal closed

    transform: Rotation {
        id: itemRotation
        origin.x: container.width / 2;
        axis.y: 1; axis.z: 0
    }

    front: Item {
        id: containerFront; anchors.fill: container

        Rectangle {
            anchors.fill: parent
            color: "black"; opacity: 0.4
        }

        Column {
            spacing: 10
            anchors {
                left: parent.left; leftMargin: 20
                right: parent.right; rightMargin: 20
                top: parent.top; topMargin: 180
            }
            Text { font.bold: true; color: "white"; elide: Text.ElideRight; text: container.photoTitle }
            Text { color: "white"; elide: Text.ElideRight; text: "<b>Published:</b> " + container.photoDate }
        }
    }

    back: Item {
        anchors.fill: container

        Rectangle { anchors.fill: parent; color: "black"; opacity: 0.4 }

        Common.Progress {
            anchors.centerIn: parent; width: 200; height: 18
            progress: bigImage.progress; visible: bigImage.status != Image.Ready
        }

        Flickable {
            id: flickable; anchors.fill: parent; clip: true
            contentWidth: imageContainer.width; contentHeight: imageContainer.height

            Item {
                id: imageContainer
                width: Math.max(bigImage.width * bigImage.scale, flickable.width);
                height: Math.max(bigImage.height * bigImage.scale, flickable.height);

                Image {
                    id: bigImage; // source: container.photoUrl
                    source: container.photoUrl
                    scale: slider.value
                    anchors.centerIn: parent; smooth: !flickable.movingVertically
                    onStatusChanged : {
                        // Default scale shows the entire image.
                        if (status == Image.Ready && width != 0) {
                            slider.minimum = Math.min(flickable.width / width, flickable.height / height);
                            prevScale = Math.min(slider.minimum, 1);
                            slider.value = prevScale;
                        }
                    }
                }
            }
        }

        Text {
            text: "Image Unavailable"
            visible: bigImage.status == Image.Error
            anchors.centerIn: parent; color: "white"; font.bold: true
        }

        Common.Slider {
            id: slider; visible: { bigImage.status == Image.Ready && maximum > minimum }
            anchors {
                bottom: parent.bottom; bottomMargin: 65
                left: parent.left; leftMargin: 25
                right: parent.right; rightMargin: 25
            }
            onValueChanged: {
                if (bigImage.width * value > flickable.width) {
                    var xoff = (flickable.width/2 + flickable.contentX) * value / prevScale;
                    flickable.contentX = xoff - flickable.width/2;
                }
                if (bigImage.height * value > flickable.height) {
                    var yoff = (flickable.height/2 + flickable.contentY) * value / prevScale;
                    flickable.contentY = yoff - flickable.height/2;
                }
                prevScale = value;
            }
        }
    }

    states: State {
        name: "Back"
        PropertyChanges { target: itemRotation; angle: 180 }
    }

    transitions: Transition {
        SequentialAnimation {
            PropertyAction { target: bigImage; property: "smooth"; value: false }
            NumberAnimation { easing.type: Easing.InOutQuad; properties: "angle"; duration: 500 }
            PropertyAction { target: bigImage; property: "smooth"; value: !flickable.movingVertically }
        }
    }
}
