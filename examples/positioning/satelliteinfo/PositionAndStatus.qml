// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick
import QtQuick.Layouts

Rectangle {
    id: root

    property alias latitudeString: latValue.text
    property alias longitudeString: lonValue.text
    property alias statusString: statusValue.text
    property bool simulation: false

    height: rootLayout.implicitHeight + rootLayout.anchors.bottomMargin

    RowLayout {
        id: rootLayout
        anchors {
            fill: parent
            leftMargin: 5
            rightMargin: 5
            bottomMargin: 5
        }
        spacing: 5
        Rectangle {
            implicitWidth: (parent.width - parent.spacing) / 2
            implicitHeight: Math.max(statusLayout.actualHeight, posLayout.actualHeight)
            border {
                color: "black"
                width: 1
            }
            radius: 5
            GridLayout {
                id: posLayout
                property real actualHeight: implicitHeight + anchors.margins * 2
                anchors {
                    fill: parent
                    margins: 5
                }
                columns: 2
                Text {
                    text: qsTr("Current Position")
                    font.bold: true
                    Layout.alignment: Qt.AlignHCenter
                    Layout.columnSpan: 2
                }
                Text {
                    text: qsTr("lat:")
                }
                Text {
                    id: latValue
                    text: qsTr("N/A")
                }
                Text {
                    text: qsTr("lon:")
                }
                Text {
                    id: lonValue
                    text: qsTr("N/A")
                }
            }
        }
        Rectangle {
            implicitWidth: (parent.width - parent.spacing) / 2
            implicitHeight: Math.max(statusLayout.actualHeight, posLayout.actualHeight)
            border {
                color: "black"
                width: 1
            }
            radius: 5
            ColumnLayout {
                id: statusLayout
                property real actualHeight: implicitHeight + anchors.margins * 2
                anchors {
                    fill: parent
                    margins: 5
                }
                Text {
                    text: qsTr("Status")
                    font.bold: true
                    Layout.alignment: Qt.AlignHCenter
                }
                Text {
                    id: statusValue
                    text: qsTr("N/A")
                    wrapMode: Text.WordWrap
                    Layout.alignment: Qt.AlignHCenter
                }
                Text {
                    id: simulationValue
                    text: root.simulation ? qsTr("Simulation Mode") : "";
                    Layout.alignment: Qt.AlignHCenter
                }
            }
        }
    }
}
