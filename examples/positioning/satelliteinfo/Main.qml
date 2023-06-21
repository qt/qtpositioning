// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtCore
import QtQuick
import QtQuick.Window

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Satellite Info")

    id: root

    LocationPermission {
        id: permission
        accuracy: LocationPermission.Precise
        availability: LocationPermission.WhenInUse
    }

    PermissionsScreen {
        anchors.fill: parent
        visible: permission.status !== Qt.PermissionStatus.Granted
        requestDenied: permission.status === Qt.PermissionStatus.Denied
        onRequestPermission: permission.request()
    }

    Component {
        id: applicationComponent
        ApplicationScreen {
        }
    }

    Loader {
        anchors.fill: parent
        active: permission.status === Qt.PermissionStatus.Granted
        sourceComponent: applicationComponent
    }
}
