// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts

Rectangle {
    id: root
    height: rootLayout.implicitHeight + rootLayout.anchors.margins * 2
    property int currentIndex: 0

    RowLayout {
        id: rootLayout
        anchors {
            fill: parent
            margins: 5
        }
        spacing: 5
        property int numElements: repeater.model.length
        property real itemWidth: (width - (numElements - 1) * spacing) / numElements
        Repeater {
            id: repeater
            model: [qsTr("Sky View"), qsTr("RSSI View")]
            Button {
                required property var modelData
                required property int index
                implicitWidth: rootLayout.itemWidth
                text: modelData
                bold: root.currentIndex === index
                onClicked: root.currentIndex = index
            }
        }
    }
}
