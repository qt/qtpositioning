# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

# Generated from positionpoll.pro.

#####################################################################
## QGeoPositionInfoSourceFactoryPoll Plugin:
#####################################################################

qt_internal_add_plugin(QGeoPositionInfoSourceFactoryPollPlugin
    OUTPUT_NAME qtposition_positionpoll
    CLASS_NAME QGeoPositionInfoSourceFactoryPoll
    PLUGIN_TYPE position
    SOURCES
        positionpollfactory.cpp positionpollfactory.h
        qgeoareamonitor_polling.cpp qgeoareamonitor_polling.h
    LIBRARIES
        Qt::Core
        Qt::Positioning
)

#### Keys ignored in scope 1:.:.:positionpoll.pro:<TRUE>:
# OTHER_FILES = "plugin.json"
