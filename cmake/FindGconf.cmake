# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

find_package(PkgConfig QUIET)
if (PkgConfig_FOUND)
    pkg_check_modules(Gconf gconf-2.0 IMPORTED_TARGET)

    if (TARGET PkgConfig::Gconf)
        mark_as_advanced(Gconf_LIBRARIES Gconf_INCLUDE_DIRS)
        if (NOT TARGET Gconf::Gconf)
            add_library(Gconf::Gconf INTERFACE IMPORTED)
            target_link_libraries(Gconf::Gconf INTERFACE PkgConfig::Gconf)
        endif()
    else()
        set(Gconf_FOUND 0)
    endif()
endif()
