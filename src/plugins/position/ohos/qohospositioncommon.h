// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOHOSPOSITIONCOMMON_H
#define QOHOSPOSITIONCOMMON_H

#include <QtCore/private/qcore_ohos_p.h>
#include <QtCore/private/qnapi_p.h>
#include <QtCore/qtimer.h>
#include <memory>

QT_BEGIN_NAMESPACE

QNapi::Object getGeoLocationManagerObject(QOhosJsState &jsState);

bool checkLocationOrApproximatelyLocationPermissionGranted();

std::unique_ptr<QTimer> makeSingleShotUpdateTimeoutTimer(
    int timeoutMs, std::function<void()> timeoutCallback);

QT_END_NAMESPACE

#endif // QOHOSPOSITIONCOMMON_H
