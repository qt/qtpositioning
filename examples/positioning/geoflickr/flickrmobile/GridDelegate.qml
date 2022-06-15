// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

 import QtQuick 2.0
 import QtQuick.Window 2.0

 Component {
     id: photoDelegate
     Item {
         id: wrapper; width: 79; height: 79

         function photoClicked() {
             imageDetails.photoTitle = title;
             imageDetails.photoDate = datetaken;
             imageDetails.photoUrl = "http://farm" + farm + ".static.flickr.com/" + server + "/" + id + "_" + secret + ".jpg";
             console.log(imageDetails.photoUrl);
             scaleMe.state = "Details";
         }

         Item {
             anchors.centerIn: parent
             scale: 0.0
             Behavior on scale { NumberAnimation { easing.type: Easing.InOutQuad} }
             id: scaleMe

             Rectangle { height: 79; width: 79; id: blackRect;  anchors.centerIn: parent; color: "black"; smooth: true }
             Rectangle {
                 id: whiteRect; width: 76; height: 76; anchors.centerIn: parent; color: "#dddddd"; smooth: true
                 Image { id: thumb;
                         // source: imagePath;
                         source: imageDetails.photoUrl = "http://farm" + farm + ".static.flickr.com/" + server + "/" + id + "_" + secret + "_t.jpg"
                         width: parent.width; height: parent.height
                         x: 1; y: 1; smooth: true}
                 Image { source: "images/gloss.png" }
             }

             Connections {
                 target: toolBar
                 function onButton2Clicked() {
                     if (scaleMe.state == 'Details' ) scaleMe.state = 'Show'
                 }
             }

             states: [
                 State {
                     name: "Show"; when: thumb.status == Image.Ready
                     PropertyChanges { target: scaleMe; scale: Math.round(Screen.pixelDensity / 4) }
                 },
                 State {
                     name: "Details"
                     PropertyChanges { target: scaleMe; scale: Math.round(Screen.pixelDensity / 4)}
                     ParentChange { target: wrapper; parent: imageDetails.frontContainer }
                     PropertyChanges { target: wrapper; x: 20; y: 60; z: 1000 }
                     PropertyChanges { target: background; state: "DetailedView" }
                 }
             ]
             transitions: [
                 Transition {
                     from: "Show"; to: "Details"
                     ParentAnimation {
                         NumberAnimation { properties: "x,y"; duration: 500; easing.type: Easing.InOutQuad }
                     }
                 },
                 Transition {
                     from: "Details"; to: "Show"
                     SequentialAnimation {
                         ParentAnimation {
                            NumberAnimation { properties: "x,y"; duration: 500; easing.type: Easing.InOutQuad }
                         }
                         PropertyAction { targets: wrapper; properties: "z" }
                     }
                 }
             ]
         }
         MouseArea { anchors.fill: wrapper; onClicked: { photoClicked() } }
     }
 }
