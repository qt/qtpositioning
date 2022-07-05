# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

# Generated from position.pro.

add_subdirectory(positionpoll)
if(TARGET Qt::DBus AND (FREEBSD OR LINUX OR OPENBSD OR NETBSD OR HURD))
    add_subdirectory(geoclue2)
endif()
if(QT_FEATURE_gypsy)
    add_subdirectory(gypsy)
endif()
if(QT_FEATURE_winrt_geolocation)
    add_subdirectory(winrt)
endif()
if(IOS OR MACOS OR TVOS)
    add_subdirectory(corelocation)
endif()
if(ANDROID)
    add_subdirectory(android)
endif()
if(TARGET Qt::SerialPort AND TARGET Qt::Network)
    add_subdirectory(nmea)
endif()
