// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root

    signal showHelp

    component Entry: ItemDelegate {
        id: entryRoot

        property alias source: entryRoot.icon.source
        readonly property int iconSize: entryRoot.font.pixelSize

        icon.height: iconSize
        icon.width: iconSize
        display: AbstractButton.TextBesideIcon
        LayoutMirroring.enabled: true
        LayoutMirroring.childrenInherit: true

        background: Rectangle {
            height: 1
            color: "black"
            anchors.bottom: entryRoot.bottom
        }
    }

    ColumnLayout {
        anchors {
            fill: parent
            topMargin: 5
        }
        spacing: 0

        Entry {
            text: qsTr("Settings")
            bottomPadding: 20
            Layout.alignment: Qt.AlignRight
            Layout.fillWidth: true
        }

        Entry {
            text: qsTr("Light Mode")
            source: "icons/lightmode.svg"
            Layout.alignment: Qt.AlignRight
            Layout.fillWidth: true
        }

        Entry {
            text: qsTr("Dark Mode")
            source: "icons/darkmode.svg"
            Layout.alignment: Qt.AlignRight
            Layout.fillWidth: true
        }

        Entry {
            text: qsTr("Help")
            source: "icons/help.svg"
            Layout.alignment: Qt.AlignRight
            Layout.fillWidth: true
            onClicked: root.showHelp()
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
