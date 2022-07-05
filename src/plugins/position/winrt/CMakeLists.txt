# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

# Generated from winrt.pro.

#####################################################################
## QGeoPositionInfoSourceFactoryWinRT Plugin:
#####################################################################

qt_internal_add_plugin(QGeoPositionInfoSourceFactoryWinRTPlugin
    OUTPUT_NAME qtposition_winrt
    CLASS_NAME QGeoPositionInfoSourceFactoryWinRT
    PLUGIN_TYPE position
    SOURCES
        qgeopositioninfosource_winrt.cpp qgeopositioninfosource_winrt_p.h
        qgeopositioninfosourcefactory_winrt.cpp qgeopositioninfosourcefactory_winrt.h
    LIBRARIES
        Qt::Core
        Qt::CorePrivate
        Qt::Positioning
)

#### Keys ignored in scope 1:.:.:winrt.pro:<TRUE>:
# OTHER_FILES = "plugin.json"

## Scopes:
#####################################################################

qt_internal_extend_target(QGeoPositionInfoSourceFactoryWinRTPlugin CONDITION MSVC AND NOT WINRT
    LIBRARIES
        runtimeobject
)
