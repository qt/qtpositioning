// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "positionfactory_ohos.h"
#include "qohosgeopositioninfosource.h"
#include "qohosgeosatelliteinfosource.h"
#include <QtCore/private/qohoslogger_p.h>

QT_BEGIN_NAMESPACE

QGeoPositionInfoSource *QOhosGeoPositionInfoSourceFactory::positionInfoSource(
    QObject *parent, const QVariantMap &parameters)
{
    Q_UNUSED(parameters);
    qOhosDebug(QtForOhos) << Q_FUNC_INFO << parent;

    return tryMakeQOhosGeoPositionInfoSource(parent);
}

QGeoSatelliteInfoSource *QOhosGeoPositionInfoSourceFactory::satelliteInfoSource(
    QObject *parent, const QVariantMap &parameters)
{
    Q_UNUSED(parameters);
    qOhosDebug(QtForOhos) << Q_FUNC_INFO << parent;

    return makeQOhosGeoSatelliteInfoSource(parent);
}

QGeoAreaMonitorSource *QOhosGeoPositionInfoSourceFactory::areaMonitor(
    QObject *parent, const QVariantMap &parameters)
{
    Q_UNUSED(parameters);
    qOhosDebug(QtForOhos) << Q_FUNC_INFO << parent;

    return nullptr;
}

QT_END_NAMESPACE
