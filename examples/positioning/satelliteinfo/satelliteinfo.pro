# Copyright (C) 2023 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

TEMPLATE = app
TARGET = satelliteinfo

QT += quick positioning positioningquick

SOURCES += main.cpp

qml_resources.files = \
    qmldir \
    Button.qml \
    Main.qml \
    RssiView.qml \
    PositionAndStatus.qml \
    SkyView.qml \
    ViewSwitch.qml

qml_resources.prefix = /qt/qml/SatelliteInformation

RESOURCES += qml_resources

data_resources.files = \
    nmealog.txt

data_resources.prefix = /

RESOURCES += data_resources

ios: QMAKE_INFO_PLIST = ../shared/Info.qmake.ios.plist

android {
    # explicitly link with serialport in order to
    # deploy it as a dependecy for nmea plugin
    QT += serialport
}

target.path = $$[QT_INSTALL_EXAMPLES]/positioning/satelliteinfo
INSTALLS += target
