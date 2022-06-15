// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick 2.0

Item {
    id: container

    property string weatherIcon: "sunny"

    Image {
        id: img
        source: "../icons/weather-" + container.weatherIcon + ".png"
        smooth: true
        anchors.fill: parent
    }
}
