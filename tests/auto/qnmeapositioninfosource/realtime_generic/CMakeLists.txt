# Generated from qnmeapositioninfosource_realtime_generic.pro.

#####################################################################
## tst_qnmeapositioninfosource_realtime_generic Test:
#####################################################################

qt_internal_add_test(tst_qnmeapositioninfosource_realtime_generic
    SOURCES
        ../../qgeopositioninfosource/testqgeopositioninfosource.cpp ../../qgeopositioninfosource/testqgeopositioninfosource_p.h
        ../../utils/qlocationtestutils.cpp ../../utils/qlocationtestutils_p.h
        ../../utils/qnmeaproxyfactory.cpp ../../utils/qnmeaproxyfactory.h
        ../tst_qnmeapositioninfosource.cpp ../tst_qnmeapositioninfosource.h
        tst_qnmeapositioninfosource_realtime_generic.cpp
    DEFINES
        QT_DISABLE_DEPRECATED_BEFORE=0
    INCLUDE_DIRECTORIES
        ..
    LIBRARIES
        Qt::Core
        Qt::Network
        Qt::Positioning
        Qt::TestPrivate
)

# Default android position plugin is not loaded for unit-tests, so we have to
# explicitly use a test position plugin.
add_dependencies(tst_qnmeapositioninfosource_realtime_generic
    QGeoPositionInfoSourceFactoryTestPlugin)
if(ANDROID)
    set_target_properties(tst_qnmeapositioninfosource_realtime_generic PROPERTIES
        QT_ANDROID_EXTRA_PLUGINS "$<TARGET_FILE_DIR:QGeoPositionInfoSourceFactoryTestPlugin>"
    )
endif()

#### Keys ignored in scope 1:.:.:qnmeapositioninfosource_realtime_generic.pro:<TRUE>:
# TEMPLATE = "app"
# testcase.timeout = "400"
