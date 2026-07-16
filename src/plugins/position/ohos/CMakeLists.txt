# Copyright (C) 2026 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

find_path(QT_OHOS_NODE_ADDON_API_INCLUDE_DIR
    NAMES napi.h
    PATH_SUFFIXES node-addon-api
)
if(NOT QT_OHOS_NODE_ADDON_API_INCLUDE_DIR)
    message(FATAL_ERROR
        "OHOS position plugin needs the node-addon-api headers "
        "(napi.h). Add the directory that contains "
        "include/node-addon-api/napi.h to CMAKE_PREFIX_PATH "
        "(e.g. the ohos-additional-packages prefix).")
endif()

qt_internal_add_plugin(QGeoPositionInfoSourceFactoryOhosPlugin
    OUTPUT_NAME qtposition_ohos
    CLASS_NAME QOhosGeoPositionInfoSourceFactory
    PLUGIN_TYPE position
    EXCEPTIONS
    SOURCES
        positionfactory_ohos.h positionfactory_ohos.cpp
        qohosgeopositioninfosource.h qohosgeopositioninfosource.cpp
        qohosgeosatelliteinfosource.h qohosgeosatelliteinfosource.cpp
        qohospositioncommon.h qohospositioncommon.cpp
    LIBRARIES
        Qt::Core
        Qt::CorePrivate
        Qt::Positioning
        ace_napi.z
        hilog_ndk.z
)

target_include_directories(QGeoPositionInfoSourceFactoryOhosPlugin PRIVATE
    ${QT_OHOS_NODE_ADDON_API_INCLUDE_DIR}
)

target_compile_definitions(QGeoPositionInfoSourceFactoryOhosPlugin PRIVATE
    NODE_ADDON_API_CPP_EXCEPTIONS
)
