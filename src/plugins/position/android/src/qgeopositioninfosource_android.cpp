// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qgeopositioninfosource_android_p.h"
#include "jnipositioning.h"
#include <QGeoPositionInfo>

static constexpr int kUpdateFromColdStart = 2 * 60 * 1000;
static constexpr int kRegularUpdatesTimerInterval = 30 * 1000;

QGeoPositionInfoSourceAndroid::QGeoPositionInfoSourceAndroid(QObject *parent) :
    QGeoPositionInfoSource(parent)
{
    androidClassKeyForUpdate = AndroidPositioning::registerPositionInfoSource(this);
    androidClassKeyForSingleRequest = AndroidPositioning::registerPositionInfoSource(this);

    //by default use all methods
    setPreferredPositioningMethods(AllPositioningMethods);

    m_requestTimer.setSingleShot(true);
    connect(&m_requestTimer, &QTimer::timeout, this,
            &QGeoPositionInfoSourceAndroid::requestTimeout);

    m_regularUpdatesTimer.setSingleShot(false);
    connect(&m_regularUpdatesTimer, &QTimer::timeout, this,
            &QGeoPositionInfoSourceAndroid::regularUpdatesTimeout);
}

QGeoPositionInfoSourceAndroid::~QGeoPositionInfoSourceAndroid()
{
    stopUpdates();

    if (m_requestTimer.isActive()) {
        m_requestTimer.stop();
        AndroidPositioning::stopUpdates(androidClassKeyForSingleRequest);
    }

    AndroidPositioning::unregisterPositionInfoSource(androidClassKeyForUpdate);
    AndroidPositioning::unregisterPositionInfoSource(androidClassKeyForSingleRequest);
}

void QGeoPositionInfoSourceAndroid::setUpdateInterval(int msec)
{
    int previousInterval = updateInterval();
    msec = (((msec > 0) && (msec < minimumUpdateInterval())) || msec < 0)? minimumUpdateInterval() : msec;

    if (msec == previousInterval)
        return;

    QGeoPositionInfoSource::setUpdateInterval(msec);

    if (updatesRunning)
        reconfigureRunningSystem();
}

QGeoPositionInfo QGeoPositionInfoSourceAndroid::lastKnownPosition(bool fromSatellitePositioningMethodsOnly) const
{
    return AndroidPositioning::lastKnownPosition(fromSatellitePositioningMethodsOnly);
}

QGeoPositionInfoSource::PositioningMethods QGeoPositionInfoSourceAndroid::supportedPositioningMethods() const
{
    return AndroidPositioning::availableProviders();
}

void QGeoPositionInfoSourceAndroid::setPreferredPositioningMethods(QGeoPositionInfoSource::PositioningMethods methods)
{
    PositioningMethods previousPreferredPositioningMethods = preferredPositioningMethods();
    QGeoPositionInfoSource::setPreferredPositioningMethods(methods);
    if (previousPreferredPositioningMethods == preferredPositioningMethods())
        return;

    if (updatesRunning)
        reconfigureRunningSystem();
}

int QGeoPositionInfoSourceAndroid::minimumUpdateInterval() const
{
    return 50;
}

QGeoPositionInfoSource::Error QGeoPositionInfoSourceAndroid::error() const
{
    return m_error;
}

void QGeoPositionInfoSourceAndroid::setError(Error error)
{
    m_error = error;
    if (error != QGeoPositionInfoSource::NoError)
        emit QGeoPositionInfoSource::errorOccurred(m_error);
}

void QGeoPositionInfoSourceAndroid::startUpdates()
{
    if (updatesRunning)
        return;

    m_error = QGeoPositionInfoSource::NoError;

    if (preferredPositioningMethods() == 0) {
        setError(UnknownSourceError);
        return;
    }

    updatesRunning = true;
    // Start calculating updates from now.
    m_lastUpdateTime = QDateTime::currentMSecsSinceEpoch();
    m_regularUpdatesErrorRaised = false;
    QGeoPositionInfoSource::Error error = AndroidPositioning::startUpdates(androidClassKeyForUpdate);
    if (error != QGeoPositionInfoSource::NoError) {
        updatesRunning = false;
        setError(error);
    } else {
        m_regularUpdatesTimer.start(kRegularUpdatesTimerInterval);
    }
}

void QGeoPositionInfoSourceAndroid::stopUpdates()
{
    if (!updatesRunning)
        return;

    updatesRunning = false;
    m_regularUpdatesTimer.stop();
    AndroidPositioning::stopUpdates(androidClassKeyForUpdate);
}

void QGeoPositionInfoSourceAndroid::requestUpdate(int timeout)
{
    if (m_requestTimer.isActive())
        return;

    m_error = QGeoPositionInfoSource::NoError;

    if (timeout != 0 && timeout < minimumUpdateInterval()) {
        setError(QGeoPositionInfoSource::UpdateTimeoutError);
        return;
    }

    if (timeout == 0)
        timeout = kUpdateFromColdStart;

    m_requestTimer.start(timeout);

    // if updates already running with interval equal to timeout
    // then we wait for next update coming through
    // assume that a single update will not be quicker than regular updates anyway
    if (updatesRunning && updateInterval() <= timeout)
        return;

    QGeoPositionInfoSource::Error error = AndroidPositioning::requestUpdate(androidClassKeyForSingleRequest);
    if (error != QGeoPositionInfoSource::NoError) {
        m_requestTimer.stop();
        setError(error);
    }
}

void QGeoPositionInfoSourceAndroid::processPositionUpdate(const QGeoPositionInfo &pInfo)
{
    //single update request and served as part of regular update
    if (m_requestTimer.isActive())
        m_requestTimer.stop();

    m_lastUpdateTime = QDateTime::currentMSecsSinceEpoch();
    m_regularUpdatesErrorRaised = false;

    emit positionUpdated(pInfo);
}

// Might still be called multiple times (once for each provider)
void QGeoPositionInfoSourceAndroid::processSinglePositionUpdate(const QGeoPositionInfo &pInfo)
{
    //timeout but we received a late update -> ignore
    if (!m_requestTimer.isActive())
        return;

    queuedSingleUpdates.append(pInfo);
}

void QGeoPositionInfoSourceAndroid::locationProviderDisabled()
{
    if (updatesRunning && !m_regularUpdatesErrorRaised) {
        m_regularUpdatesErrorRaised = true;
        setError(QGeoPositionInfoSource::UpdateTimeoutError);
    }

    setError(QGeoPositionInfoSource::ClosedError);
}

void QGeoPositionInfoSourceAndroid::locationProvidersChanged()
{
    emit supportedPositioningMethodsChanged();
}

void QGeoPositionInfoSourceAndroid::requestTimeout()
{
    AndroidPositioning::stopUpdates(androidClassKeyForSingleRequest);
    //no queued update to process -> timeout

    if (queuedSingleUpdates.isEmpty()) {
        setError(QGeoPositionInfoSource::UpdateTimeoutError);
        return;
    }

    //pick best
    QGeoPositionInfo best = queuedSingleUpdates[0];
    for (qsizetype i = 1; i < queuedSingleUpdates.size(); ++i) {
        const QGeoPositionInfo info = queuedSingleUpdates[i];

        //anything newer by 20s is always better
        const qint64 timeDelta = best.timestamp().secsTo(info.timestamp());
        if (abs(timeDelta) > 20) {
            if (timeDelta > 0)
                best = info;
            continue;
        }

        //compare accuracy
        if (info.hasAttribute(QGeoPositionInfo::HorizontalAccuracy) &&
                best.hasAttribute(QGeoPositionInfo::HorizontalAccuracy))
        {
            best = info.attribute(QGeoPositionInfo::HorizontalAccuracy) <
                    best.attribute(QGeoPositionInfo::HorizontalAccuracy) ? info : best;
            continue;
        }

        //prefer info with accuracy information
        if (info.hasAttribute(QGeoPositionInfo::HorizontalAccuracy))
            best = info;
    }

    queuedSingleUpdates.clear();
    emit positionUpdated(best);
}

void QGeoPositionInfoSourceAndroid::regularUpdatesTimeout()
{
    if (!m_regularUpdatesErrorRaised) {
        const auto now = QDateTime::currentMSecsSinceEpoch();
        if ((now - m_lastUpdateTime) > (updateInterval() + kUpdateFromColdStart)) {
            m_regularUpdatesErrorRaised = true;
            setError(QGeoPositionInfoSource::UpdateTimeoutError);
        }
    }
}

/*
  Updates the system assuming that updateInterval
  and/or preferredPositioningMethod have changed.
 */
void QGeoPositionInfoSourceAndroid::reconfigureRunningSystem()
{
    if (!updatesRunning)
        return;

    stopUpdates();
    startUpdates();
}
