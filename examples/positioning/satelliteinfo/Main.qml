// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtPositioning
import QtQuick
import QtQuick.Layouts

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Satellite Info")

    id: root

    property color inUseColor: "#7FFF0000"
    property color inViewColor: "#7F0000FF"

    property bool simulation: false

    // The model structure is:
    // { "id": 1, "rssi": 10, "azimuth": 150, "elevation": 25, "inUse": false }
    property var satellitesModel: []
    property var inUseIds: new Set()

    function updateModel() {
        let intermediateModel = []
        intermediateModel.length = satelliteSource.satellitesInView.length
        for (var i = 0; i < satelliteSource.satellitesInView.length; ++i) {
            let sat = satelliteSource.satellitesInView[i]
            intermediateModel[i] = {
                "id": sat.satelliteIdentifier,
                "rssi": sat.signalStrength,
                "azimuth": sat.attribute(GeoSatelliteInfo.Azimuth),
                "elevation": sat.attribute(GeoSatelliteInfo.Elevation),
                "inUse": inUseIds.has(sat.satelliteIdentifier)
            }
        }
        satellitesModel = intermediateModel
    }

    function toggleState() {
        switch (statesItem.state) {
        case "stopped":
            statesItem.state = "single"
            break
        case "single":
            statesItem.state = "running"
            break
        case "running":
            statesItem.state = "stopped"
            break
        }
    }

    function updateActive(state) {
        satelliteSource.active = state
        positionSource.active = state
    }

    function enterSingle() {
        updateActive(false)
        satelliteSource.update()
        positionSource.update()
    }

    //! [0]
    SatelliteSource {
        id: satelliteSource
        name: root.simulation ? "nmea" : ""
    //! [0]
        PluginParameter {
            name: "nmea.source"
            value: "qrc:///nmealog.txt"
        }
    //! [1]
        onSatellitesInViewChanged: root.updateModel()
        onSatellitesInUseChanged: {
            root.inUseIds.clear()
            for (var i = 0; i < satellitesInUse.length; ++i)
                root.inUseIds.add(satellitesInUse[i].satelliteIdentifier)

            root.updateModel()
        }
    //! [1]
        onSourceErrorChanged: {
            if (sourceError != SatelliteSource.NoError) {
                // If the positionSource is in simulation mode, switch the
                // whole app into this mode. Otherwise show an error.
                if (positionSource.name !== "nmea") {
                    positionAndStatus.statusString = qsTr("SatelliteSource Error: %1").arg(sourceError)
                } else {
                    root.simulation = true
                    active = true
                }
            }
        }
    //! [2]
    }
    //! [2]

    //! [3]
    PositionSource {
        id: positionSource
        name: root.simulation ? "nmea" : ""
    //! [3]
        PluginParameter {
            name: "nmea.source"
            value: "qrc:///nmealog.txt"
        }
    //! [4]
        onPositionChanged: {
            let posData = position.coordinate.toString().split(", ")
            positionAndStatus.latitudeString = posData[0]
            positionAndStatus.longitudeString = posData[1]
        }
    // ![4]
        onSourceErrorChanged: {
            if (sourceError != PositionSource.NoError) {
                // If the satelliteSource is in simulation mode, switch the
                // whole app into this mode. Otherwise show an error.
                if (satelliteSource.name !== "nmea") {
                    positionAndStatus.statusString = qsTr("PositionSource Error: %1").arg(sourceError)
                } else {
                    root.simulation = true
                    active = true
                }
            }
        }
    //! [5]
    }
    //! [5]

    ViewSwitch {
        id: navigationTab
        anchors.top: parent.top
        width: parent.width
    }

    StackLayout {
        id: viewsLayout
        width: parent.width
        anchors {
            top: navigationTab.bottom
            bottom: positionAndStatus.top
        }
        currentIndex: navigationTab.currentIndex
        SkyView {
            satellitesModel: root.satellitesModel
            inViewColor: root.inViewColor
            inUseColor: root.inUseColor
        }
        RssiView {
            satellitesModel: root.satellitesModel
            inViewColor: root.inViewColor
            inUseColor: root.inUseColor
        }
    }

    PositionAndStatus {
        id: positionAndStatus
        width: parent.width
        anchors {
            bottom: modeButton.top
        }
        simulation: root.simulation
    }

    Button {
        id: modeButton
        anchors {
            left: parent.left
            leftMargin: 5
            right: parent.right
            rightMargin: 5
            bottom: parent.bottom
            bottomMargin: 5
        }
        onClicked: root.toggleState()
    }

    Item {
        id: statesItem
        visible: false
        state: "running"
        states: [
            State {
                name: "stopped"
                PropertyChanges {
                    target: modeButton
                    text: qsTr("Single")
                }
                PropertyChanges {
                    target: positionAndStatus
                    statusString: qsTr("Stopped")
                }
                StateChangeScript {
                    script: root.updateActive(false)
                }
            },
            State {
                name: "single"
                PropertyChanges {
                    target: modeButton
                    text: qsTr("Start")
                }
                PropertyChanges {
                    target: positionAndStatus
                    statusString: qsTr("Single Request")
                }
                StateChangeScript {
                    script: root.enterSingle()
                }
            },
            State {
                name: "running"
                PropertyChanges {
                    target: modeButton
                    text: qsTr("Stop")
                }
                PropertyChanges {
                    target: positionAndStatus
                    statusString: qsTr("Running")
                }
                StateChangeScript {
                    script: root.updateActive(true)
                }
            }
        ]
    }
}
