// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick
import QtQuick.Controls as QC
import QtQuick.Layouts

QC.Popup {
    id: root
    Rectangle {
        anchors.fill: parent
        ColumnLayout {
            anchors.fill: parent
            Image {
                source: "icons/qtlogo_green.png"
                fillMode: Image.PreserveAspectFit
                Layout.alignment: Qt.AlignHCenter
            }
            Text {
                text: qsTr("SatelliteInfo")
                Layout.alignment: Qt.AlignHCenter
            }
            Text {
                text: qsTr("Explore satellite information using the SkyView and\nRSSI View. ")
                      + qsTr("Track the satellites contributing to your\nGPS fix in real-time.")
                horizontalAlignment: Text.AlignHCenter
                Layout.alignment: Qt.AlignHCenter
            }
            Item {
                Layout.fillHeight: true
            }
            Button {
                text: qsTr("Back to the Application")
                implicitWidth: root.width * 0.8
                Layout.alignment: Qt.AlignHCenter
                onClicked: root.close()
            }
            Item {
                Layout.fillHeight: true
            }
        }
    }
}
