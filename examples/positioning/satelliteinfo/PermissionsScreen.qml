// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick

Rectangle {
    id: root

    property bool requestDenied: false

    signal requestPermission

    Text {
        id: textItem
        width: parent.width
        anchors.verticalCenter: parent.verticalCenter
        padding: 20
        wrapMode: Text.WordWrap
        text: root.requestDenied
              ? qsTr("The application cannot run because the Location permission "
                   + "was not granted.\n"
                   + "Please grant the permission and restart the application.")
              : qsTr("The application requires the Location permission to get "
                   + "the position and satellite information.\n"
                   + "Please press the button to request the permission.")
    }

    Button {
        visible: !root.requestDenied
        anchors {
            top: textItem.bottom
            horizontalCenter: parent.horizontalCenter
        }
        text: qsTr("Request Permission")
        onClicked: root.requestPermission()
    }
}
