// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtPositioning
import QtQuick
import QtQuick.Controls as QC
import QtQuick.Layouts

Rectangle {
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
            if (sourceError !== SatelliteSource.NoError) {
                // If the positionSource is in simulation mode, switch the
                // whole app into this mode. Otherwise show an error.
                if (positionSource.name !== "nmea") {
                    applicationHeader.statusString =
                            qsTr("SatelliteSource Error: %1").arg(sourceError)
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
            positionBox.latitudeString = posData[0]
            positionBox.longitudeString = posData[1]
        }
    // ![4]
        onSourceErrorChanged: {
            if (sourceError !== PositionSource.NoError) {
                // If the satelliteSource is in simulation mode, switch the
                // whole app into this mode. Otherwise show an error.
                if (satelliteSource.name !== "nmea") {
                    applicationHeader.statusString =
                            qsTr("PositionSource Error: %1").arg(sourceError)
                } else {
                    root.simulation = true
                    active = true
                }
            }
        }
    //! [5]
    }
    //! [5]

    Header {
        id: applicationHeader
        width: parent.width
        anchors.top: parent.top
        simulation: root.simulation
    }

    StackLayout {
        id: viewsLayout
        width: parent.width
        anchors {
            top: applicationHeader.bottom
            bottom: positionBox.top
        }
        currentIndex: navigationTab.currentIndex !== navigationTab.settingsIndex
                      ? navigationTab.currentIndex : currentIndex
        SkyView {
            satellitesModel: root.satellitesModel
            inViewColor: root.inViewColor
            inUseColor: root.inUseColor
        }
        SatelliteView {
        }
        RssiView {
            satellitesModel: root.satellitesModel
            inViewColor: root.inViewColor
            inUseColor: root.inUseColor
        }
    }

    PositionBox {
        id: positionBox
        width: parent.width
        anchors.bottom: modeButton.top
        visible: viewsLayout.currentIndex !== navigationTab.tableViewIndex
    }

    Button {
        id: modeButton
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: navigationTab.top
            bottomMargin: 5
        }
        width: parent.width * 0.8
        onClicked: root.toggleState()
    }

    ViewSwitch {
        id: navigationTab
        anchors.bottom: parent.bottom
        width: parent.width
        onCurrentIndexChanged: {
            if (currentIndex === navigationTab.settingsIndex)
                settingsView.open()
            else
                settingsView.close()
        }
    }

    QC.Drawer {
        id: settingsView
        dragMargin: -1
        edge: Qt.RightEdge
        height: root.height - navigationTab.height
        width: root.width / 2
        onClosed: navigationTab.currentIndex = viewsLayout.currentIndex

        SettingsView {
            anchors.fill: parent
            onShowHelp: {
                settingsView.close()
                helpPopup.open()
            }
        }
    }

    HelpPopup {
        id: helpPopup
        width: root.width
        height: root.height
    }

    Item {
        id: statesItem
        visible: false
        state: "running"
        states: [
            State {
                name: "stopped"
                PropertyChanges {
                    modeButton.text: qsTr("Single")
                }
                PropertyChanges {
                    applicationHeader.statusString: qsTr("Stopped")
                }
                StateChangeScript {
                    script: root.updateActive(false)
                }
            },
            State {
                name: "single"
                PropertyChanges {
                    modeButton.text: qsTr("Start")
                }
                PropertyChanges {
                    applicationHeader.statusString: qsTr("Single Request")
                }
                StateChangeScript {
                    script: root.enterSingle()
                }
            },
            State {
                name: "running"
                PropertyChanges {
                    modeButton.text: qsTr("Stop")
                }
                PropertyChanges {
                    applicationHeader.statusString: qsTr("Running")
                }
                StateChangeScript {
                    script: root.updateActive(true)
                }
            }
        ]
    }
}
