# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.16)
project(satelliteinfo LANGUAGES CXX)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
    set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}/positioning/satelliteinfo")

find_package(Qt6 REQUIRED COMPONENTS Core Quick PositioningQuick)
find_package(Qt6 OPTIONAL_COMPONENTS SerialPort)

qt_standard_project_setup()

qt_add_executable(satelliteinfo
    main.cpp
)

set_target_properties(satelliteinfo PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(satelliteinfo PRIVATE
    Qt::Core
    Qt::PositioningQuick
    Qt::Quick
)

# For mobile platforms we need to explicitly link with Qt::SerialPort
# to use NMEA plugin. The reason for such behavior is that, at least on Android,
# the plugin dependencies were not explicitly added to the list of deployment
# targets until Qt 6.5.
if(ANROID AND TARGET Qt::SerialPort)
    target_link_libraries(satelliteinfo PRIVATE Qt::SerialPort)
endif()

if (IOS)
    # Using absolute path for shared plist files is a Ninja bug workaround
    get_filename_component(SHARED_PLIST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../shared ABSOLUTE)
    set_target_properties(satelliteinfo PROPERTIES
       MACOSX_BUNDLE_INFO_PLIST "${SHARED_PLIST_DIR}/Info.cmake.ios.plist"
    )
endif()

qt_add_resources(satelliteinfo "resources"
    FILES nmealog.txt
)

qt_add_qml_module(satelliteinfo
    URI SatelliteInformation
    VERSION 1.0
    AUTO_RESOURCE_PREFIX
    QML_FILES
        Button.qml
        Main.qml
        RssiView.qml
        PositionAndStatus.qml
        SkyView.qml
        ViewSwitch.qml
)

install(TARGETS satelliteinfo
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
