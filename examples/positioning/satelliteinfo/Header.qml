// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick
import QtQuick.Layouts

Rectangle {
    id: root

    property bool simulation: false
    property string statusString
    readonly property int __margin: 5

    implicitWidth: Math.max(applicationName.implicitWidth,
                            applicationMode.implicitWidth,
                            applicationStatus.implicitWidth)
    implicitHeight: applicationName.implicitHeight
                    + applicationMode.implicitHeight
                    + applicationStatus.implicitHeight
                    + 2 * rootLayout.spacing
                    + 2 * root.__margin

    ColumnLayout {
        id: rootLayout
        anchors {
            fill: parent
            topMargin: root.__margin
            bottomMargin: root.__margin
        }

        Text {
            id: applicationName
            text: qsTr("Satellite Info")
            Layout.alignment: Qt.AlignHCenter
        }
        Text {
            id: applicationMode
            text: root.simulation ? qsTr("Simulation mode") : qsTr("Realtime mode")
            Layout.alignment: Qt.AlignHCenter
        }
        Text {
            id: applicationStatus
            text: root.statusString
            Layout.alignment: Qt.AlignHCenter
        }
    }
}
