// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick

Rectangle {
    id: root
    border {
        width: 1
        color: "black"
    }
    color: mouseArea.pressed ? "lightGray" : "white"
    radius: 5
    implicitWidth: textItem.implicitWidth + textItem.anchors.margins * 2
    implicitHeight: textItem.implicitHeight + textItem.anchors.margins * 2

    property alias text: textItem.text
    property alias bold: textItem.font.bold

    signal clicked

    Text {
        id: textItem
        anchors.centerIn: parent
        anchors.margins: 5
    }
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: root.clicked()
    }
}
