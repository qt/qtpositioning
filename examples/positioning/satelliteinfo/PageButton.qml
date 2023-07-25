// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick
import QtQuick.Controls

ItemDelegate {
    id: root

    property alias source: root.icon.source
    property bool selected: false
    readonly property int iconSize: availableHeight - root.font.pixelSize - spacing

    // Pick an implicit height in such way that the icon has twice more space
    // then the text
    implicitHeight: topPadding + bottomPadding + spacing + 3 * root.font.pixelSize

    icon.height: iconSize
    icon.width: iconSize
    display: AbstractButton.TextUnderIcon
    font.bold: selected
}
