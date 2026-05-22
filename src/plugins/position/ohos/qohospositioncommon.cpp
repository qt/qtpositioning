// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qohospositioncommon.h"
#include <QtCore/private/qohoscommon_p.h>
#include <QtCore/qcoreapplication.h>
#include <QtCore/qpermissions.h>

QT_BEGIN_NAMESPACE

QNapi::Object getGeoLocationManagerObject(QOhosJsState &jsState)
{
    return jsState.eval<QNapi::Object>("@ohos.geoLocationManager");
}

bool checkLocationOrApproximatelyLocationPermissionGranted()
{
    if (!qApp)
        return false;

    QLocationPermission perm;
    return qApp->checkPermission(perm) == Qt::PermissionStatus::Granted;
}

std::unique_ptr<QTimer> makeSingleShotUpdateTimeoutTimer(
    int timeoutMs, std::function<void()> timeoutCallback)
{
    auto timer = std::make_unique<QTimer>();
    timer->setSingleShot(true);
    timer->setInterval(timeoutMs);
    QObject::connect(timer.get(), &QTimer::timeout, timer.get(), std::move(timeoutCallback));
    timer->start();
    return timer;
}

QT_END_NAMESPACE
