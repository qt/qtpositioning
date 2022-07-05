# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

# special case begin

qt_internal_add_benchmark(tst_bench_qgeopositioninfo
    SOURCES
        tst_bench_qgeopositioninfo.cpp
    PUBLIC_LIBRARIES
        Qt::Core
        Qt::Positioning
        Qt::Test
)

# special case end
