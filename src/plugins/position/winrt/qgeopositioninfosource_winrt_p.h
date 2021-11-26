/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtPositioning module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QGEOPOSITIONINFOSOURCEWINRT_H
#define QGEOPOSITIONINFOSOURCEWINRT_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qgeopositioninfosource.h"
#include "qgeopositioninfo.h"

#include <QtCore/qtimer.h>

#include <EventToken.h>
#include <wrl.h>

namespace ABI {
    namespace Windows {
        namespace Devices {
            namespace Geolocation{
                struct IGeolocator;
                struct IPositionChangedEventArgs;
                struct IStatusChangedEventArgs;
            }
        }
    }
}

QT_BEGIN_NAMESPACE

class QGeoPositionInfoSourceWinRTPrivate;

class QGeoPositionInfoSourceWinRT : public QGeoPositionInfoSource
{
    Q_OBJECT
public:
    QGeoPositionInfoSourceWinRT(QObject *parent = nullptr);
    ~QGeoPositionInfoSourceWinRT() override;
    int init();

    QGeoPositionInfo lastKnownPosition(bool fromSatellitePositioningMethodsOnly = false) const override;
    PositioningMethods supportedPositioningMethods() const override;

    void setPreferredPositioningMethods(PositioningMethods methods) override;

    void setUpdateInterval(int msec) override;
    int minimumUpdateInterval() const override;
    Error error() const override;

    HRESULT onPositionChanged(ABI::Windows::Devices::Geolocation::IGeolocator *locator,
                              ABI::Windows::Devices::Geolocation::IPositionChangedEventArgs *args);
    HRESULT onStatusChanged(ABI::Windows::Devices::Geolocation::IGeolocator *locator,
                              ABI::Windows::Devices::Geolocation::IStatusChangedEventArgs *args);

    bool requestAccess() const;
Q_SIGNALS:
    void nativePositionUpdate(const QGeoPositionInfo);
public slots:
    void startUpdates() override;
    void stopUpdates() override;

    void requestUpdate(int timeout = 0) override;

private slots:
    void stopHandler();
    void virtualPositionUpdate();
    void singleUpdateTimeOut();
    void updateSynchronized(const QGeoPositionInfo info);
    void reactOnError(QGeoPositionInfoSource::Error positionError);
private:
    bool startHandler();

    Q_DISABLE_COPY(QGeoPositionInfoSourceWinRT)
    void setError(QGeoPositionInfoSource::Error positionError);

    QScopedPointer<QGeoPositionInfoSourceWinRTPrivate> d_ptr;
    Q_DECLARE_PRIVATE(QGeoPositionInfoSourceWinRT)
};

QT_END_NAMESPACE

#endif // QGEOPOSITIONINFOSOURCEWINRT_H
