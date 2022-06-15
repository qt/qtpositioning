// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick 2.0

Item {
    width: 480; height: 320

    Loader {
        y: 320; rotation: -90
        transformOrigin: Item.TopLeft
        source: "flickr.qml"
    }
}
