# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(tst_qgeosatelliteinfosource LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

# Generated from qgeosatelliteinfosource.pro.

#####################################################################
## tst_qgeosatelliteinfosource Binary:
#####################################################################

qt_internal_add_test(tst_qgeosatelliteinfosource
    SOURCES
        ../utils/qlocationtestutils.cpp ../utils/qlocationtestutils_p.h
        testqgeosatelliteinfosource.cpp testqgeosatelliteinfosource_p.h
        tst_qgeosatelliteinfosource.cpp
    LIBRARIES
        Qt::Core
        Qt::Positioning
        Qt::TestPrivate
)

#### Keys ignored in scope 1:.:.:qgeosatelliteinfosource.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################
