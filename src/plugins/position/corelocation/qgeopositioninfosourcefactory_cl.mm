// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial

#include "qgeopositioninfosource_cl_p.h"
#include "qgeopositioninfosourcefactory_cl.h"

QGeoPositionInfoSource *QGeoPositionInfoSourceFactoryCL::positionInfoSource(QObject *parent, const QVariantMap &parameters)
{
    Q_UNUSED(parameters)
    return new QGeoPositionInfoSourceCL(parent);
}

QGeoSatelliteInfoSource *QGeoPositionInfoSourceFactoryCL::satelliteInfoSource(QObject *parent, const QVariantMap &parameters)
{
    Q_UNUSED(parent)
    Q_UNUSED(parameters)
    return nullptr;
}

QGeoAreaMonitorSource *QGeoPositionInfoSourceFactoryCL::areaMonitor(QObject *parent, const QVariantMap &parameters)
{
    Q_UNUSED(parent)
    Q_UNUSED(parameters)
    return nullptr;
}
