// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick
import QtQuick.Controls

ItemDelegate {
    id: root

    property alias bold: root.font.bold

    contentItem: Text {
        text: root.text
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font: root.font
    }
    background: Rectangle {
        border {
            width: 1
            color: "black"
        }
        radius: 5
        color: root.pressed ? "lightGray" : "white"
    }
}
