TARGET  = declarative_location
TARGETPATH = QtLocation

include(qlocationimport.pri)

QT += declarative-private network location qtquick1 declarative qt3d

DESTDIR = $$QT.location.imports/$$TARGETPATH
target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

#INCLUDEPATH += ../../../../qtdeclarative/include
INCLUDEPATH += ../../../../qtdeclarative/include/QtDeclarative/5.0.0/QtDeclarative
INCLUDEPATH += ../../../../qtdeclarative/include/QtDeclarative/5.0.0/QtDeclarative/private
INCLUDEPATH *= $$PWD

# On some platforms, build both versions because debug and release
# versions are incompatible
#win32|mac:!wince*:!win32-msvc:!macx-xcode:CONFIG += debug_and_release

HEADERS += qdeclarativeposition_p.h \
           qdeclarativepositionsource_p.h \
           qdeclarativecoordinate_p.h \
           qdeclarativegeolocation_p.h \
           qdeclarativegeomapitemview_p.h \
           qdeclarativegeomapmouseevent_p.h \
           qdeclarativegeomapmousearea_p.h \
           qdeclarativegeoserviceprovider_p.h \
           qdeclarativegeoaddress_p.h \
           qdeclarativegeoboundingarea_p.h \
           qdeclarativegeoboundingbox_p.h \
           qdeclarativegeocodemodel_p.h \
           qdeclarativegeoroutemodel_p.h \
           qdeclarativegeoroute_p.h \
           qdeclarativegeoroutesegment_p.h \
           qdeclarativegeomaneuver_p.h \
           qdeclarativegeoboundingcircle_p.h \
           qdeclarative3dgraphicsgeomap_p.h \
           qdeclarativegeomapflickable_p.h \
           qdeclarativegeomappincharea_p.h \
           qdeclarativegeomapitem_p.h

SOURCES += qdeclarativeposition.cpp \
           location.cpp \
           qdeclarativepositionsource.cpp \
           qdeclarativecoordinate.cpp \
           qdeclarativegeolocation.cpp \
           qdeclarativegeomapitemview.cpp \
           qdeclarativegeomapmouseevent.cpp \
           qdeclarativegeomapmousearea.cpp \
           qdeclarativegeoserviceprovider.cpp \
           qdeclarativegeoaddress.cpp \
           qdeclarativegeoboundingarea.cpp \
           qdeclarativegeoboundingbox.cpp \
           qdeclarativegeocodemodel.cpp \
           qdeclarativegeoroutemodel.cpp \
           qdeclarativegeoroute.cpp \
           qdeclarativegeoroutesegment.cpp \
           qdeclarativegeomaneuver.cpp \
           qdeclarativegeoboundingcircle.cpp \
           qdeclarative3dgraphicsgeomap.cpp \
           qdeclarativegeomapflickable.cpp \
           qdeclarativegeomappincharea.cpp \
           qdeclarativegeomapitem.cpp

include(declarativeplaces/declarativeplaces.pri)

# Tell qmake to create such makefile that qmldir and target (i.e. declarative_location)
# are both copied to qt/imports/QtMobility/location -directory,
# as the "/imports" is the default place where qmlviewer looks for plugins
# (otherwise qmlviewer -I <path> -option is needed)

qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

INSTALLS += target qmldir

# manually enable mouse area and map items (shader effect source) if you know symbols to be
# present in your environment
#message('Will try to build QML Map 3D with QML2 user interaction elements and Map Items.')
#DEFINES += QQUICKMOUSEAREA_AVAILABLE
#DEFINES += QQUICKSHADEREFFECTSOURCE_AVAILABLE
message('QML2 3D Map user interaction elements and MapItems not available. Check location.pro - file for instructions.')

