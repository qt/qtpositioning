// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick 2.0

Item {
    id: titleBar
    BorderImage { source: "images/titlebar.sci"; width: parent.width; height: parent.height + 14; y: -7 }

    Item {
        id: container
        width: (parent.width * 2) - 55 ; height: parent.height

        Image {
            id: quitButton
            anchors.left: parent.left//; anchors.leftMargin: 0
            anchors.verticalCenter: parent.verticalCenter
            source: "images/quit.png"
            MouseArea {
                anchors.fill: parent
                onClicked: Qt.quit()
            }
        }

        Text {
            id: categoryText
            anchors {
                left: quitButton.right; leftMargin: 10; rightMargin: 10
                verticalCenter: parent.verticalCenter
            }
            elide: Text.ElideLeft
            text: "GeoFlickr (QML)"
            font.bold: true; color: "White"; style: Text.Raised; styleColor: "Black"
        }
    }

    transitions: Transition {
        NumberAnimation { properties: "x"; easing.type: Easing.InOutQuad }
    }
}
