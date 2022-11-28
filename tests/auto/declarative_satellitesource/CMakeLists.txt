# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

file(GLOB_RECURSE test_data_glob
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.qml)
list(APPEND test_data ${test_data_glob})

qt_internal_add_test(tst_declarative_satellitesource
    QMLTEST
    SOURCES
        main.cpp
    LIBRARIES
        Qt::Positioning
        Qt::Qml
        Qt::Quick
    TESTDATA
        ${test_data}
)

qt_add_qml_module(tst_declarative_satellitesource
    URI TestFactory
    VERSION 1.0
    QML_FILES
        tst_satellite.qml
    NO_RESOURCE_TARGET_PATH
)
