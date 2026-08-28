# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(tst_qgeopath LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

# Generated from qgeopath.pro.

#####################################################################
## tst_qgeopath Test:
#####################################################################

qt_internal_add_test(tst_qgeopath
    SOURCES
        tst_qgeopath.cpp
    LIBRARIES
        Qt::Core
        Qt::PositioningPrivate
)

#### Keys ignored in scope 1:.:.:qgeopath.pro:<TRUE>:
# TEMPLATE = "app"
