# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(tst_qgeopositioninfo LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

# Generated from qgeopositioninfo.pro.

#####################################################################
## tst_qgeopositioninfo Test:
#####################################################################

qt_internal_add_test(tst_qgeopositioninfo
    SOURCES
        tst_qgeopositioninfo.cpp
    LIBRARIES
        Qt::Core
        Qt::Positioning
)

#### Keys ignored in scope 1:.:.:qgeopositioninfo.pro:<TRUE>:
# TEMPLATE = "app"
