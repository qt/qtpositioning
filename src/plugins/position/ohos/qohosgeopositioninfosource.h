// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOHOSGEOPOSITIONINFOSOURCE_H
#define QOHOSGEOPOSITIONINFOSOURCE_H

#include <QtPositioning/qgeopositioninfosource.h>

QT_BEGIN_NAMESPACE

QGeoPositionInfoSource *tryMakeQOhosGeoPositionInfoSource(QObject *parent = nullptr);

QT_END_NAMESPACE

#endif // QOHOSGEOPOSITIONINFOSOURCE_H
