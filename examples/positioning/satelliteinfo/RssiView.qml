// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

pragma ComponentBehavior: Bound
import QtQuick

Rectangle {
    id: root

    property var satellitesModel

    property color inViewColor: "blue"
    property color inUseColor: "red"

    Rectangle {
        anchors.fill: parent
        anchors.margins: rect.myMargin
        border.width: 1
        radius: 10
        border.color: "black"

        Item {
            id: rect
            anchors.fill: parent
            anchors.margins: myMargin
            property int myMargin: 5

            Row {
                id: view
                property int rows: repeater.model.length
                property int singleWidth: ((rect.width - scale.width) / rows) - rect.myMargin
                spacing: rect.myMargin

                Rectangle {
                    id: scale
                    width: strengthLabel.width + 10
                    height: rect.height
                    color: "#32cd32"
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.bottom: lawngreenRect.top
                        font.pointSize: 11
                        text: "50"
                    }
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: parent.top
                        font.pointSize: 11
                        text: "100"
                    }

                    Rectangle {
                        id: redRect
                        width: parent.width
                        color: "red"
                        height: parent.height * 10 / 100
                        anchors.bottom: parent.bottom
                        Text {
                            id: strengthLabel
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.bottom: parent.bottom
                            font.pointSize: 11
                            text: "00"
                        }
                    }
                    Rectangle {
                        id: orangeRect
                        height: parent.height * 10 / 100
                        anchors.bottom: redRect.top
                        width: parent.width
                        color: "#ffa500"
                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.bottom: parent.bottom
                            font.pointSize: 11
                            text: "10"
                        }
                    }
                    Rectangle {
                        id: goldRect
                        height: parent.height * 10 / 100
                        anchors.bottom: orangeRect.top
                        width: parent.width
                        color: "#ffd700"
                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.bottom: parent.bottom
                            font.pointSize: 11
                            text: "20"
                        }
                    }
                    Rectangle {
                        id: yellowRect
                        height: parent.height * 10 / 100
                        anchors.bottom: goldRect.top
                        width: parent.width
                        color: "yellow"
                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.bottom: parent.bottom
                            font.pointSize: 11
                            text: "30"
                        }
                    }
                    Rectangle {
                        id: lawngreenRect
                        height: parent.height * 10 / 100
                        anchors.bottom: yellowRect.top
                        width: parent.width
                        color: "#7cFc00"
                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.bottom: parent.bottom
                            font.pointSize: 11
                            text: "40"
                        }
                    }
                }

                Repeater {
                    id: repeater
                    model: root.satellitesModel
                    delegate: Rectangle {
                        required property var modelData
                        height: rect.height
                        width: view.singleWidth
                        Rectangle {
                            anchors.bottom: parent.bottom
                            width: parent.width
                            height: parent.height * parent.modelData.rssi / 100
                            color: parent.modelData.inUse ? root.inUseColor : root.inViewColor
                        }
                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.bottom: parent.bottom
                            text: parent.modelData.id
                        }
                    }
                }
            }
        }
    }
}
