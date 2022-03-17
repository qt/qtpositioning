# Generated from positionplugintest.pro.

#####################################################################
## tst_positionplugin Test:
#####################################################################

qt_internal_add_test(tst_positionplugin
    SOURCES
        tst_positionplugin.cpp
    PUBLIC_LIBRARIES
        Qt::Core
        Qt::Positioning
)

add_dependencies(tst_positionplugin QGeoPositionInfoSourceFactoryTestPlugin)
if(ANDROID)
    set_target_properties(tst_positionplugin PROPERTIES
        QT_ANDROID_EXTRA_PLUGINS "$<TARGET_FILE_DIR:QGeoPositionInfoSourceFactoryTestPlugin>"
    )
endif()

#### Keys ignored in scope 1:.:.:positionplugintest.pro:<TRUE>:
# TEMPLATE = "app"
