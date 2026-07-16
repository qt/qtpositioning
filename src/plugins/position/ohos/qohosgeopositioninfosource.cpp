// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qohosgeopositioninfosource.h"
#include "qohospositioncommon.h"
#include <QtCore/private/qcore_ohos_p.h>
#include <QtCore/private/qohoscommon_p.h>
#include <QtCore/private/qohosjstools_p.h>
#include <QtCore/private/qohoslogger_p.h>
#include <QtCore/qtimer.h>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <functional>
#include <memory>
#include <optional>
#include <utility>

QT_BEGIN_NAMESPACE

namespace {

enum class UserActivityScenario
{
    NAVIGATION,
    SPORT,
    TRANSPORT,
    DAILY_LIFE_SERVICE,
};

enum class LocationError
{
    LOCATING_FAILED_DEFAULT,
    LOCATING_FAILED_LOCATION_PERMISSION_DENIED,
    LOCATING_FAILED_BACKGROUND_PERMISSION_DENIED,
    LOCATING_FAILED_LOCATION_SWITCH_OFF,
    LOCATING_FAILED_INTERNET_ACCESS_FAILURE,
};

constexpr int coldStartMarginMs = 2 * 60 * 1000;
constexpr int continuousUpdateTimerIntervalMs = 30 * 1000;

const char *toStaticString(UserActivityScenario scenario)
{
    switch (scenario) {
    case UserActivityScenario::NAVIGATION:
        return "NAVIGATION";
    case UserActivityScenario::SPORT:
        return "SPORT";
    case UserActivityScenario::TRANSPORT:
        return "TRANSPORT";
    case UserActivityScenario::DAILY_LIFE_SERVICE:
        return "DAILY_LIFE_SERVICE";
    }

    qOhosReportFatalErrorAndAbort(
        "%s received invalid UserActivityScenario: %d", Q_FUNC_INFO, scenario);
}

const char *toStaticString(LocationError locationError)
{
    switch (locationError) {
    case LocationError::LOCATING_FAILED_DEFAULT:
        return "LOCATING_FAILED_DEFAULT";
    case LocationError::LOCATING_FAILED_LOCATION_PERMISSION_DENIED:
        return "LOCATING_FAILED_LOCATION_PERMISSION_DENIED";
    case LocationError::LOCATING_FAILED_BACKGROUND_PERMISSION_DENIED:
        return "LOCATING_FAILED_BACKGROUND_PERMISSION_DENIED";
    case LocationError::LOCATING_FAILED_LOCATION_SWITCH_OFF:
        return "LOCATING_FAILED_LOCATION_SWITCH_OFF";
    case LocationError::LOCATING_FAILED_INTERNET_ACCESS_FAILURE:
        return "LOCATING_FAILED_INTERNET_ACCESS_FAILURE";
    }

    qOhosReportFatalErrorAndAbort(
        "%s received invalid LocationError: %d", Q_FUNC_INFO, locationError);
}

std::optional<UserActivityScenario> tryMapPositioningMethodsToUserActivityScenario(
    QGeoPositionInfoSource::PositioningMethods positioningMethods)
{
    qOhosPrintfDebug("%s: %d", Q_FUNC_INFO, static_cast<int>(positioningMethods));

    if (positioningMethods.testFlag(QGeoPositionInfoSource::SatellitePositioningMethods))
        return UserActivityScenario::NAVIGATION;

    if (positioningMethods.testFlag(QGeoPositionInfoSource::NonSatellitePositioningMethods))
        return UserActivityScenario::DAILY_LIFE_SERVICE;

    return std::nullopt;
}

QGeoPositionInfo convertLocationObjectToPositionInfo(const QNapi::Object &locationObject)
{
    auto dateTime = QDateTime::fromMSecsSinceEpoch(
        locationObject.get<QNapi::Number>("timeStamp").Int64Value());
    QGeoCoordinate coordinate(
        locationObject.get<QNapi::Number>("latitude"),
        locationObject.get<QNapi::Number>("longitude"),
        locationObject.get<QNapi::Number>("altitude"));

    QGeoPositionInfo positionInfo(coordinate, dateTime);

    positionInfo.setAttribute(
        QGeoPositionInfo::Direction, locationObject.get<QNapi::Number>("direction"));
    positionInfo.setAttribute(
        QGeoPositionInfo::HorizontalAccuracy, locationObject.get<QNapi::Number>("accuracy"));
    positionInfo.setAttribute(
        QGeoPositionInfo::VerticalAccuracy, locationObject.get<QNapi::Number>("altitudeAccuracy"));
    // NOTE: QGeoPositionInfo distinguishes GroundSpeed and VerticalSpeed, while OHOS provides only
    // Location_BasicInfo::speed. Here, we assume, that since Location_BasicInfo::accuracy relates
    // to HorizontalAccuracy, the Location_BasicInfo::speed relates to GroundSpeed (although, there
    // is no information about that anywhere in the documentation).
    positionInfo.setAttribute(
        QGeoPositionInfo::GroundSpeed, locationObject.get<QNapi::Number>("speed"));

    return positionInfo;
}

std::shared_ptr<void> registerLocationChangeUpdateHandler(
    QObject *contextObject, long intervalSec,
    UserActivityScenario userActivityScenario,
    QOhosConsumer<const QGeoPositionInfo &> positionInfoUpdateConsumer)
{
    auto sharedPositionInfoUpdateConsumer = QtOhos::moveToSharedPtr(std::move(positionInfoUpdateConsumer));

    auto weakPositionInfoUpdateConsumer = QtOhos::makeWeakPtr(sharedPositionInfoUpdateConsumer);
    auto contextObjectRef = QtOhos::makeQThreadSafeRef(contextObject);

    auto registrationHandle = QOhosJsThreadGateway::eval(
        [&](QOhosJsState &jsState) {
            qOhosPrintfWarning("%s Interval in Seconds: %li", Q_FUNC_INFO, intervalSec);
            auto continuousLocationRequest = QNapi::makeObject(
                jsState.env(),
                {
                    {"interval", intervalSec},
                    {"locationScenario", jsState.mapOhosEnumToJs(userActivityScenario)},
                });

            qOhosPrintfDebug(
                "%s registering callback with scenario: %s",
                Q_FUNC_INFO, toStaticString(userActivityScenario));

            return QtOhos::makeProxyWithJsThreadDeleter(
                registerQOhosOnOffMethodsBasedEventHandler(
                    getGeoLocationManagerObject(jsState), "locationChange",
                    [contextObjectRef, weakPositionInfoUpdateConsumer](const QOhosCallbackInfo &cbInfo) {
                        auto location = cbInfo.getFirstArg<QNapi::Object>(Q_FUNC_INFO);
                        auto positionInfo = convertLocationObjectToPositionInfo(location);

                        contextObjectRef.visitInQtThreadIfAlive(
                            [weakPositionInfoUpdateConsumer, positionInfo](auto &) {
                                auto sharedPositionInfoUpdateConsumer = weakPositionInfoUpdateConsumer.lock();
                                if (sharedPositionInfoUpdateConsumer)
                                    (*sharedPositionInfoUpdateConsumer)(positionInfo);
                            });
                    },
                    {
                        .extraOnArg = std::make_optional<QNapi::ValueWrapper>(continuousLocationRequest),
                    }));
        });

    return QtOhos::moveToSharedPtr(
        std::make_pair(sharedPositionInfoUpdateConsumer, registrationHandle));
}

std::shared_ptr<void> makeContinuousPositionInfoUpdateProducer(
    QObject *contextObject, int intervalMs, QGeoPositionInfoSource::PositioningMethods positioningMethods,
    QOhosConsumer<const QGeoPositionInfo &, QGeoPositionInfoSource::PositioningMethods> positionInfoUpdateConsumer)
{
    auto userActivityScenario =
        tryMapPositioningMethodsToUserActivityScenario(positioningMethods)
        .value_or(UserActivityScenario::DAILY_LIFE_SERVICE);
    auto intervalSec = std::lround(static_cast<double>(intervalMs) / 1000.0);

    return registerLocationChangeUpdateHandler(
        contextObject, intervalSec, userActivityScenario,
        [positionInfoUpdateConsumer = std::move(positionInfoUpdateConsumer), positioningMethods](const QGeoPositionInfo &positionInfo) {
            positionInfoUpdateConsumer(positionInfo, positioningMethods);
        });
}

std::shared_ptr<void> makeSinglePositionInfoUpdateProducer(
    QObject *contextObject, int timeoutMs, QGeoPositionInfoSource::PositioningMethods positioningMethods,
    QOhosConsumer<std::optional<QGeoPositionInfo>, QGeoPositionInfoSource::PositioningMethods> positionInfoUpdateConsumer)
{
    qOhosDebug(QtForOhos) << Q_FUNC_INFO << contextObject << timeoutMs;

    struct Context
    {
        QOhosConsumer<std::optional<QGeoPositionInfo>, QGeoPositionInfoSource::PositioningMethods> positionInfoUpdateConsumer;
        QtOhos::QThreadSafeRef<QObject> contextObjectRef;
    };

    auto sharedContext = QtOhos::moveToSharedPtr(Context{
        .positionInfoUpdateConsumer = std::move(positionInfoUpdateConsumer),
        .contextObjectRef = QtOhos::makeQThreadSafeRef(contextObject)
    });

    auto weakContext = QtOhos::makeWeakPtr(sharedContext);
    auto positionInfoUpdateConsumerJsProxy =
        [weakContext, positioningMethods](std::optional<QGeoPositionInfo> optPositionInfo) {
            auto sharedContext = weakContext.lock();
            if (sharedContext) {
                sharedContext->contextObjectRef.visitInQtThreadIfAlive(
                    [weakContext, optPositionInfo, positioningMethods](auto &) {
                        auto sharedContext = weakContext.lock();
                        if (sharedContext)
                            sharedContext->positionInfoUpdateConsumer(optPositionInfo, positioningMethods);
                    });
            }
        };

    QOhosJsThreadGateway::runAndWait(
        [&](QOhosJsState &jsState) {
            auto userActivityScenario =
                tryMapPositioningMethodsToUserActivityScenario(positioningMethods)
                    .value_or(UserActivityScenario::DAILY_LIFE_SERVICE);
            auto currentLocationRequest = QNapi::makeObject(
                jsState.env(),
                {
                    {"scenario", jsState.mapOhosEnumToJs(userActivityScenario)},
                    {"timeoutMs", timeoutMs},
                });

            getGeoLocationManagerObject(jsState)
            .evalToPromiseOrRejectOnThrow("getCurrentLocation(*)", {currentLocationRequest})
            .withContext(std::move(positionInfoUpdateConsumerJsProxy))
            .onThenWithContext([](const QOhosCallbackInfo &cbInfo, auto &positionInfoUpdateConsumerJsProxy) {
                auto location = cbInfo.getFirstArg<QNapi::Object>("getCurrentLocation");
                positionInfoUpdateConsumerJsProxy(convertLocationObjectToPositionInfo(location));
            })
            .onCatchWithContext([](const QOhosCallbackInfo &cbInfo, auto &positionInfoUpdateConsumerJsProxy) {
                QtOhos::logJsCallbackError(cbInfo, "getCurrentLocation() failed");
                positionInfoUpdateConsumerJsProxy(std::nullopt);
            });
        });

    return sharedContext;
}

std::shared_ptr<void> registerLocationErrorHandler(
    QObject *contextObject, QOhosConsumer<LocationError> errorConsumer)
{
    qOhosDebug(QtForOhos) << Q_FUNC_INFO << contextObject;

    auto sharedErrorConsumer = QtOhos::moveToSharedPtr(std::move(errorConsumer));

    auto contextObjectRef = QtOhos::makeQThreadSafeRef(contextObject);
    auto weakErrorConsumer = QtOhos::makeWeakPtr(sharedErrorConsumer);

    auto registrationHandle = QOhosJsThreadGateway::eval(
        [&](QOhosJsState &jsState) {
            return QtOhos::makeProxyWithJsThreadDeleter(
                registerQOhosOnOffMethodsBasedEventHandler(
                    getGeoLocationManagerObject(jsState), "locationError",
                    [contextObjectRef, weakErrorConsumer](const QOhosCallbackInfo &cbInfo) {
                        auto jsLocationError = cbInfo.getFirstArg<QNapi::Number>(Q_FUNC_INFO);
                        auto locationError = cbInfo.jsState().mapOhosEnumFromJs<LocationError>(jsLocationError);
                        contextObjectRef.visitInQtThreadIfAlive(
                            [weakErrorConsumer, locationError](auto &) {
                                auto sharedErrorConsumer = weakErrorConsumer.lock();
                                if (sharedErrorConsumer)
                                    (*sharedErrorConsumer)(locationError);
                            });
                    }));
            });

    return QtOhos::makeSharedPtrWithAttachedExtraData(registrationHandle, sharedErrorConsumer);
}

QGeoPositionInfoSource::Error convertLocationErrorToQtError(LocationError error)
{
    switch (error) {
    case LocationError::LOCATING_FAILED_DEFAULT:
        return QGeoPositionInfoSource::UnknownSourceError;
    case LocationError::LOCATING_FAILED_LOCATION_SWITCH_OFF:
        return QGeoPositionInfoSource::ClosedError;
    case LocationError::LOCATING_FAILED_LOCATION_PERMISSION_DENIED:
    case LocationError::LOCATING_FAILED_BACKGROUND_PERMISSION_DENIED:
    case LocationError::LOCATING_FAILED_INTERNET_ACCESS_FAILURE:
        return QGeoPositionInfoSource::AccessError;
    }

    qOhosPrintfWarning("%s received unknown error: %d", Q_FUNC_INFO, static_cast<int>(error));
    return QGeoPositionInfoSource::UnknownSourceError;
}

class QOhosGeoPositionInfoSource : public QGeoPositionInfoSource
{
public:
    static QOhosGeoPositionInfoSource *tryMakeInstance(QObject *parent);

    ~QOhosGeoPositionInfoSource() override;

    void setUpdateInterval(int msec) override;
    QGeoPositionInfo lastKnownPosition(bool fromSatellitePositioningMethodsOnly) const override;
    PositioningMethods supportedPositioningMethods() const override;
    void setPreferredPositioningMethods(PositioningMethods methods) override;
    int minimumUpdateInterval() const override;
    Error error() const override;

public:
    void startUpdates() override;
    void stopUpdates() override;

    void requestUpdate(int timeout) override;

protected:
    QOhosGeoPositionInfoSource(QObject *parent);

private:
    void setErrorHelper(QGeoPositionInfoSource::Error error);
    void continuousUpdateTimeout();

    void updatePositionInfo(const QGeoPositionInfo &positionInfo, PositioningMethods positioningMethods);

    QGeoPositionInfoSource::Error m_error = QGeoPositionInfoSource::NoError;
    QGeoPositionInfo m_lastUpdatedPositionInfo;
    QGeoPositionInfo m_lastUpdatedPositionInfoFromSatelliteSource;

    std::shared_ptr<void> m_locationErrorHandle;

    std::shared_ptr<void> m_continuousLocationUpdatesHandle;
    QTimer m_continuousUpdateTimeoutTimer;
    qint64 m_lastUpdateTime = 0;
    bool m_continuousUpdateTimeoutErrorRaised = false;

    std::shared_ptr<void> m_singlePositionUpdateHandle;
    std::unique_ptr<QTimer> m_singleUpdateTimeoutTimer;
};

QOhosGeoPositionInfoSource *QOhosGeoPositionInfoSource::tryMakeInstance(QObject *parent)
{
    if (!checkLocationOrApproximatelyLocationPermissionGranted()) {
        qOhosDebug(QtForOhos)
            << Q_FUNC_INFO
            << "Location permission isn't granted. Can't create QOhosGeoPositionInfoSource";
        return nullptr;
    }

    return new QOhosGeoPositionInfoSource(parent);
}

QOhosGeoPositionInfoSource::QOhosGeoPositionInfoSource(QObject *parent)
    : QGeoPositionInfoSource(parent)
{
    qOhosDebug(QtForOhos) << Q_FUNC_INFO << parent;

    m_locationErrorHandle = registerLocationErrorHandler(
        this,
        [this](LocationError error) {
            qOhosWarning(QtForOhos)
                << Q_FUNC_INFO << "Received error from callback:" << toStaticString(error);
            setErrorHelper(convertLocationErrorToQtError(error));
        });

    m_continuousUpdateTimeoutTimer.setSingleShot(false);
    m_continuousUpdateTimeoutTimer.setInterval(continuousUpdateTimerIntervalMs);
    QObject::connect(&m_continuousUpdateTimeoutTimer, &QTimer::timeout,
            this, [this]() { continuousUpdateTimeout(); });
}

QOhosGeoPositionInfoSource::~QOhosGeoPositionInfoSource() = default;

void QOhosGeoPositionInfoSource::setUpdateInterval(int msec)
{
    qOhosDebug(QtForOhos) << Q_FUNC_INFO << msec;

    QGeoPositionInfoSource::setUpdateInterval(msec);

    if (m_continuousLocationUpdatesHandle) {
        stopUpdates();
        startUpdates();
    }
}

QGeoPositionInfo QOhosGeoPositionInfoSource::lastKnownPosition(bool fromSatellitePositioningMethodsOnly) const
{
    qOhosDebug(QtForOhos) << Q_FUNC_INFO << fromSatellitePositioningMethodsOnly;

    return fromSatellitePositioningMethodsOnly
        ? m_lastUpdatedPositionInfoFromSatelliteSource
        : m_lastUpdatedPositionInfo;
}

QGeoPositionInfoSource::PositioningMethods QOhosGeoPositionInfoSource::supportedPositioningMethods() const
{
    qOhosDebug(QtForOhos) << Q_FUNC_INFO;

    return QGeoPositionInfoSource::AllPositioningMethods;
}

void QOhosGeoPositionInfoSource::setPreferredPositioningMethods(PositioningMethods methods)
{
    qOhosDebug(QtForOhos) << Q_FUNC_INFO << methods;

    QGeoPositionInfoSource::setPreferredPositioningMethods(methods);
}

int QOhosGeoPositionInfoSource::minimumUpdateInterval() const
{
    qOhosDebug(QtForOhos) << Q_FUNC_INFO;

    return 1000;
}

QGeoPositionInfoSource::Error QOhosGeoPositionInfoSource::error() const
{
    qOhosDebug(QtForOhos) << Q_FUNC_INFO;

    return m_error;
}

void QOhosGeoPositionInfoSource::startUpdates()
{
    qOhosDebug(QtForOhos) << Q_FUNC_INFO;

    if (m_continuousLocationUpdatesHandle) {
        qOhosDebug(QtForOhos) << Q_FUNC_INFO << "Continuous updates already running. Ignoring";
        return;
    }

    auto intervalMs = std::max(updateInterval(), minimumUpdateInterval());

    m_lastUpdateTime = QDateTime::currentMSecsSinceEpoch();
    m_continuousUpdateTimeoutErrorRaised = false;

    m_continuousLocationUpdatesHandle =
        makeContinuousPositionInfoUpdateProducer(
            this, intervalMs,
            preferredPositioningMethods(),
            [this](const QGeoPositionInfo &positionInfo, PositioningMethods positioningMethods) {
                updatePositionInfo(positionInfo, positioningMethods);
            });

    m_continuousUpdateTimeoutTimer.start();

    setErrorHelper(QGeoPositionInfoSource::NoError);
}

void QOhosGeoPositionInfoSource::continuousUpdateTimeout()
{
    if (!m_continuousUpdateTimeoutErrorRaised) {
        const auto now = QDateTime::currentMSecsSinceEpoch();
        if ((now - m_lastUpdateTime) > (updateInterval() + coldStartMarginMs)) {
            m_continuousUpdateTimeoutErrorRaised = true;
            setErrorHelper(QGeoPositionInfoSource::UpdateTimeoutError);
        }
    }
}

void QOhosGeoPositionInfoSource::stopUpdates()
{
    qOhosDebug(QtForOhos) << Q_FUNC_INFO;

    m_continuousLocationUpdatesHandle.reset();
    m_continuousUpdateTimeoutTimer.stop();
}

void QOhosGeoPositionInfoSource::requestUpdate(int timeoutMs)
{
    qOhosDebug(QtForOhos) << Q_FUNC_INFO << timeoutMs;

    if (m_singlePositionUpdateHandle) {
        qOhosDebug(QtForOhos) << Q_FUNC_INFO << "Update already in progress. Ignoring this one";
        return;
    }

    if (timeoutMs != 0 && timeoutMs < minimumUpdateInterval()) {
        setErrorHelper(QGeoPositionInfoSource::UpdateTimeoutError);
        return;
    }

    auto timeout = timeoutMs != 0 ? timeoutMs : coldStartMarginMs;

    m_singlePositionUpdateHandle = makeSinglePositionInfoUpdateProducer(
        this,
        updateInterval() >= minimumUpdateInterval() ? updateInterval() : minimumUpdateInterval(),
        preferredPositioningMethods(),
        [this](std::optional<QGeoPositionInfo> optPositionInfo, PositioningMethods positioningMethods) {
            m_singlePositionUpdateHandle.reset();
            m_singleUpdateTimeoutTimer.reset();
            if (optPositionInfo.has_value())
                updatePositionInfo(optPositionInfo.value(), positioningMethods);
            else
                setErrorHelper(QGeoPositionInfoSource::UpdateTimeoutError);
        });
    if (!m_singlePositionUpdateHandle) {
        qOhosWarning(QtForOhos) << Q_FUNC_INFO << "Failed to make single position info update producer";
        setErrorHelper(QGeoPositionInfoSource::UnknownSourceError);
        return;
    }

    m_singleUpdateTimeoutTimer =
        makeSingleShotUpdateTimeoutTimer(timeout, [this]() {
            m_singlePositionUpdateHandle.reset();
            setErrorHelper(QGeoPositionInfoSource::UpdateTimeoutError);
        });

    setErrorHelper(QGeoPositionInfoSource::NoError);
}

void QOhosGeoPositionInfoSource::setErrorHelper(QGeoPositionInfoSource::Error newError)
{
    qOhosDebug(QtForOhos) << Q_FUNC_INFO << newError;

    if (m_error == newError)
        return;

    m_error = newError;

    if (newError != QGeoPositionInfoSource::NoError)
        Q_EMIT QGeoPositionInfoSource::errorOccurred(m_error);
}

void QOhosGeoPositionInfoSource::updatePositionInfo(
    const QGeoPositionInfo &positionInfo, PositioningMethods positioningMethods)
{
    qOhosDebug(QtForOhos) << Q_FUNC_INFO << positionInfo;

    m_lastUpdateTime = QDateTime::currentMSecsSinceEpoch();
    m_continuousUpdateTimeoutErrorRaised = false;
    setErrorHelper(QGeoPositionInfoSource::NoError);

    if (positioningMethods.testFlag(PositioningMethod::SatellitePositioningMethods))
        m_lastUpdatedPositionInfoFromSatelliteSource = positionInfo;
    m_lastUpdatedPositionInfo = positionInfo;
    Q_EMIT positionUpdated(m_lastUpdatedPositionInfo);
}

}

QGeoPositionInfoSource *tryMakeQOhosGeoPositionInfoSource(QObject *parent)
{
    return QOhosGeoPositionInfoSource::tryMakeInstance(parent);
}

namespace QtOhos {

template<>
struct OhosEnumMeta<UserActivityScenario>
{
    static constexpr const char *fullTypeName = "@ohos.geoLocationManager.UserActivityScenario";
    static constexpr std::array<std::pair<UserActivityScenario, const char *>, 4> enumeratorsNames = {{
        {UserActivityScenario::NAVIGATION, "NAVIGATION"},
        {UserActivityScenario::SPORT, "SPORT"},
        {UserActivityScenario::TRANSPORT, "TRANSPORT"},
        {UserActivityScenario::DAILY_LIFE_SERVICE, "DAILY_LIFE_SERVICE"},
    }};
};

template<>
struct OhosEnumMeta<LocationError>
{
    static constexpr const char *fullTypeName = "@ohos.geoLocationManager.LocationError";
    static constexpr std::array<std::pair<LocationError, const char *>, 5> enumeratorsNames = {{
        {LocationError::LOCATING_FAILED_DEFAULT, "LOCATING_FAILED_DEFAULT"},
        {LocationError::LOCATING_FAILED_LOCATION_PERMISSION_DENIED, "LOCATING_FAILED_LOCATION_PERMISSION_DENIED"},
        {LocationError::LOCATING_FAILED_BACKGROUND_PERMISSION_DENIED, "LOCATING_FAILED_BACKGROUND_PERMISSION_DENIED"},
        {LocationError::LOCATING_FAILED_LOCATION_SWITCH_OFF, "LOCATING_FAILED_LOCATION_SWITCH_OFF"},
        {LocationError::LOCATING_FAILED_INTERNET_ACCESS_FAILURE, "LOCATING_FAILED_INTERNET_ACCESS_FAILURE"},
    }};
};

}

QT_END_NAMESPACE
