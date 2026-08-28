# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(tst_qgeoaddress LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

# Generated from qgeoaddress.pro.

#####################################################################
## tst_qgeoaddress Test:
#####################################################################

qt_internal_add_test(tst_qgeoaddress
    SOURCES
        tst_qgeoaddress.cpp
    LIBRARIES
        Qt::Core
        Qt::Positioning
)

#### Keys ignored in scope 1:.:.:qgeoaddress.pro:<TRUE>:
# TEMPLATE = "app"
