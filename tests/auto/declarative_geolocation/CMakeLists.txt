# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(tst_declarative_geolocation LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

# special case begin

# Collect test data

qt_internal_add_test(tst_declarative_geolocation
    QMLTEST
    SOURCES
        main.cpp
    LIBRARIES
        Qt::Positioning
        Qt::Quick
    TESTDATA
        tst_declarativegeolocation.qml
)

# special case end
