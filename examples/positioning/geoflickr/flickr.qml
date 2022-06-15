// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick 2.0
import QtQml.XmlListModel
import "flickrcommon" as Common
import "flickrmobile" as Mobile

Item {
    id: screen; width: 320; height: 480
    property bool inListView : false

    Rectangle {
        id: background
        anchors.fill: parent; color: "#343434";

        Image { source: "flickrmobile/images/stripes.png"; fillMode: Image.Tile; anchors.fill: parent; opacity: 0.3 }

        Common.RestModel {
            id: restModel
            coordinate: geoTab.coordinate
        }

        Item {
            id: views
            x: 2; width: parent.width - 4
            anchors.top: titleBar.bottom; anchors.bottom: toolBar.top

            Text {
                text: qsTr("Network error")
                font.pixelSize: 48
                fontSizeMode: Text.HorizontalFit
                anchors.centerIn: parent
                width: parent.width * 0.9
                visible: restModel.status === XmlListModel.Error

            }

            Mobile.GridDelegate { id: gridDelegate }
            GridView {
                x: (width/4-79)/2; y: x
                id: photoGridView; model: restModel; delegate: gridDelegate; cacheBuffer: 100
                cellWidth: (parent.width-2)/4; cellHeight: cellWidth; width: parent.width; height: parent.height - 1; z: 6
            }
            Mobile.ListDelegate { id: listDelegate }
            ListView {
                id: photoListView; model: restModel; delegate: listDelegate; z: 6
                width: parent.width; height: parent.height; x: -(parent.width * 1.5); cacheBuffer: 100;
            }
            states: State {
                name: "ListView"; when: screen.inListView == true
                PropertyChanges { target: photoListView; x: 0 }
                PropertyChanges { target: photoGridView; x: -(parent.width * 1.5) }
            }

            transitions: Transition {
                NumberAnimation { properties: "x"; duration: 500; easing.type: Easing.InOutQuad }
            }
        }
        Mobile.ImageDetails { id: imageDetails; width: parent.width; anchors.left: views.right; height: parent.height; z:1 }
        Mobile.TitleBar { id: titleBar; z: 5; width: parent.width; height: 40; opacity: 0.9 }
        Mobile.GeoTab {
            id: geoTab;
            x: 15; y:50;
        }
        Mobile.ToolBar {
            id: toolBar; z: 5
            height: 40; anchors.bottom: parent.bottom; width: parent.width; opacity: 0.9
            button1Label: "Update"; button2Label: "View mode"
            onButton1Clicked: restModel.reload()
            onButton2Clicked: if (screen.inListView == true) screen.inListView = false; else screen.inListView = true
        }
        Connections {
            target: imageDetails
            function onClosed() {
                if (background.state == "DetailedView") {
                    background.state = '';
                    imageDetails.photoUrl = "";
                }
            }
        }

        states: State {
            name: "DetailedView"
            PropertyChanges { target: views; x: -parent.width }
            PropertyChanges { target: geoTab; x: -parent.width }
            PropertyChanges { target: toolBar; button1Label: "More..." }
            PropertyChanges {
                target: toolBar
                onButton1Clicked: if (imageDetails.state=='') imageDetails.state='Back'; else imageDetails.state=''
            }
            PropertyChanges { target: toolBar; button2Label: "Back" }
            PropertyChanges { target: toolBar; onButton2Clicked: imageDetails.closed() }
        }

        transitions: Transition {
            NumberAnimation { properties: "x"; duration: 500; easing.type: Easing.InOutQuad }
        }
    }
}
