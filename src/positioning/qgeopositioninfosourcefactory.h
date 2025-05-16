// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial

#ifndef QGEOPOSITIONINFOSOURCEFACTORY_H
#define QGEOPOSITIONINFOSOURCEFACTORY_H

#include <QtPositioning/QGeoPositionInfoSource>
#include <QtPositioning/QGeoSatelliteInfoSource>
#include <QtPositioning/QGeoAreaMonitorSource>
#include <QtCore/QList>

QT_BEGIN_NAMESPACE

class Q_POSITIONING_EXPORT QGeoPositionInfoSourceFactory
{
public:
    virtual ~QGeoPositionInfoSourceFactory();

    virtual QGeoPositionInfoSource *positionInfoSource(QObject *parent, const QVariantMap &parameters) = 0;
    virtual QGeoSatelliteInfoSource *satelliteInfoSource(QObject *parent, const QVariantMap &parameters) = 0;
    virtual QGeoAreaMonitorSource *areaMonitor(QObject *parent, const QVariantMap &parameters) = 0;
};

#define QT_POSITION_SOURCE_INTERFACE
Q_DECLARE_INTERFACE(QGeoPositionInfoSourceFactory,
                    "org.qt-project.qt.position.sourcefactory/6.0")

QT_END_NAMESPACE

#endif // QGEOPOSITIONINFOSOURCEFACTORY_H
