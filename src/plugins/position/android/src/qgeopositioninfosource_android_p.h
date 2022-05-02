/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtPositioning module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
******************************************************************************/

#ifndef QGEOPOSITIONINFOSOURCE_ANDROID_P_H
#define QGEOPOSITIONINFOSOURCE_ANDROID_P_H

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

#include <QGeoPositionInfoSource>
#include <QTimer>

class QGeoPositionInfoSourceAndroid : public QGeoPositionInfoSource
{
    Q_OBJECT
public:
    QGeoPositionInfoSourceAndroid(QObject *parent = 0);
    ~QGeoPositionInfoSourceAndroid();

    // From QGeoPositionInfoSource
    void setUpdateInterval(int msec) override;
    QGeoPositionInfo lastKnownPosition(bool fromSatellitePositioningMethodsOnly = false) const override;
    PositioningMethods supportedPositioningMethods() const override;
    void setPreferredPositioningMethods(PositioningMethods methods) override;
    int minimumUpdateInterval() const override;
    Error error() const override;

public Q_SLOTS:
    virtual void startUpdates() override;
    virtual void stopUpdates() override;

    virtual void requestUpdate(int timeout = 0) override;

    void processPositionUpdate(const QGeoPositionInfo& pInfo);
    void processSinglePositionUpdate(const QGeoPositionInfo& pInfo);

    void locationProviderDisabled();
    void locationProvidersChanged();
private Q_SLOTS:
    void requestTimeout();
    void regularUpdatesTimeout();

private:
    void reconfigureRunningSystem();
    void setError(Error error);

    bool updatesRunning = false;
    int androidClassKeyForUpdate;
    int androidClassKeyForSingleRequest;
    QList<QGeoPositionInfo> queuedSingleUpdates;
    Error m_error = NoError;
    QTimer m_requestTimer;
    QTimer m_regularUpdatesTimer;
    qint64 m_lastUpdateTime = 0;
    bool m_regularUpdatesErrorRaised = false;
};

#endif // QGEOPOSITIONINFOSOURCE_ANDROID_P_H
