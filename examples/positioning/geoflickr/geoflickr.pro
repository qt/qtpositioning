TEMPLATE = app
TARGET = geoflickr

QT += qml quick network positioning
SOURCES += qmllocationflickr.cpp

RESOURCES += \
    flickr.qrc

OTHER_FILES += flickr.qml \
               flickrcommon/* \
               flickrmobile/*

ios: QMAKE_INFO_PLIST = ../shared/Info.qmake.ios.plist

target.path = $$[QT_INSTALL_EXAMPLES]/positioning/geoflickr
INSTALLS += target

android {
    # explicitly link with serialport on mobile platforms in order to
    # deploy it as a dependecy for nmea plugin
    QT += serialport
}
