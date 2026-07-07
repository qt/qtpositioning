// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qohosgeopositioninfosource.h"
#include "qohosgeosatelliteinfosource.h"
#include "qohospositioncommon.h"
#include "qohosjsutils.h"
#include <QtCore/private/qcore_ohos_p.h>
#include <QtCore/private/qnapi_p.h>
#include <QtCore/private/qohoscommon_p.h>
#include <QtCore/private/qohoslogger_p.h>
#include <QtCore/qset.h>
#include <QtCore/qmath.h>
#include <QtPositioning/qgeosatelliteinfo.h>
#include <algorithm>
#include <chrono>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

QT_BEGIN_NAMESPACE

namespace {

constexpr int updateAsOftenAsNecessaryInterval = 0;

enum class SatelliteConstellationCategory
{
    CONSTELLATION_CATEGORY_UNKNOWN,
    CONSTELLATION_CATEGORY_GPS,
    CONSTELLATION_CATEGORY_SBAS,
    CONSTELLATION_CATEGORY_GLONASS,
    CONSTELLATION_CATEGORY_QZSS,
    CONSTELLATION_CATEGORY_BEIDOU,
    CONSTELLATION_CATEGORY_GALILEO,
    CONSTELLATION_CATEGORY_IRNSS,
};

struct SatelliteInfo
{
    int id;
    double carrierToNoiseDensity;
    double altitude;
    double azimuth;
    std::optional<SatelliteConstellationCategory> optConstellationCategory;
    bool usedInFix;
};

std::optional<std::vector<SatelliteConstellationCategory>>
tryGetSatelliteConstelationCategoriesFromSatelliteStatusInfoObject(
    QOhosJsState &jsState, QNapi::Object satelliteStatusInfoObject)
{
    auto satelliteConstellationArray = QNapi::getOptionalPropOrEmpty<QNapi::Array>(
        satelliteStatusInfoObject, "satelliteConstellation");

    return !satelliteConstellationArray.IsEmpty()
        ? std::make_optional(
            QNapi::getArrayElements<std::vector<SatelliteConstellationCategory>, QNapi::Number>(
                satelliteConstellationArray,
                [&](auto satelliteConstellation) {
                    return jsState.mapOhosEnumFromJs<SatelliteConstellationCategory>(satelliteConstellation);
                }))
        : std::nullopt;
}

std::optional<std::vector<int>> tryGetSatelliteAdditionalInfosFromSatelliteStatusInfoObject(
    QOhosJsState &, QNapi::Object satelliteStatusInfoObject)
{
    auto satelliteAdditionalInfoArray = QNapi::getOptionalPropOrEmpty<QNapi::Array>(
        satelliteStatusInfoObject, "satelliteAdditionalInfo");

    return !satelliteAdditionalInfoArray.IsEmpty()
        ? std::make_optional(
            QNapi::getArrayElements<std::vector<int>, QNapi::Number>(satelliteAdditionalInfoArray))
        : std::nullopt;
}

bool isSatelliteUsedInFix(QOhosJsState &jsState, int satelliteAdditionalInfo)
{
    int satelliteUsedInValueFlag = jsState.eval<QNapi::Number>(
        "@ohos.geoLocationManager.SatelliteAdditionalInfo.SATELLITES_ADDITIONAL_INFO_USED_IN_FIX");

    return (satelliteAdditionalInfo & satelliteUsedInValueFlag) != 0;
}

std::vector<SatelliteInfo> convertJsSatelliteStatusInfoObjectToQGeoSatelliteInfos(
    QOhosJsState &jsState, QNapi::Object satelliteStatusInfoObject)
{
    int satellitesNumber = satelliteStatusInfoObject.get<QNapi::Number>("satellitesNumber");
    auto satelliteIds = QNapi::getArrayElements<std::vector<int>, QNapi::Number>(
        satelliteStatusInfoObject.get<QNapi::Array>("satelliteIds"));
    auto carrierToNoiseDensities = QNapi::getArrayElements<std::vector<double>, QNapi::Number>(
        satelliteStatusInfoObject.get<QNapi::Array>("carrierToNoiseDensitys"));
    auto altitudes = QNapi::getArrayElements<std::vector<double>, QNapi::Number>(
        satelliteStatusInfoObject.get<QNapi::Array>("altitudes"));
    auto azimuths = QNapi::getArrayElements<std::vector<double>, QNapi::Number>(
        satelliteStatusInfoObject.get<QNapi::Array>("azimuths"));
    auto satelliteConstellations = tryGetSatelliteConstelationCategoriesFromSatelliteStatusInfoObject(
        jsState, satelliteStatusInfoObject);
    auto satelliteAdditionalInfos = tryGetSatelliteAdditionalInfosFromSatelliteStatusInfoObject(
        jsState, satelliteStatusInfoObject);

    std::vector<SatelliteInfo> satelliteInfos;
    satelliteInfos.reserve(satellitesNumber);

    for (int i = 0; i < satellitesNumber; i++) {
        satelliteInfos.push_back(SatelliteInfo{
            .id = satelliteIds[i],
            .carrierToNoiseDensity = carrierToNoiseDensities[i],
            .altitude = altitudes[i],
            .azimuth = azimuths[i],
            .optConstellationCategory = satelliteConstellations.has_value()
                ? std::make_optional(satelliteConstellations.value()[i])
                : std::nullopt,
            .usedInFix = satelliteAdditionalInfos.has_value()
                ? isSatelliteUsedInFix(jsState, satelliteAdditionalInfos.value()[i])
                : false
        });
    }

    return satelliteInfos;
}

QGeoSatelliteInfo::SatelliteSystem convertSatelliteConstellationCategoryToQt(
    SatelliteConstellationCategory constellationCategory)
{
    switch (constellationCategory) {
    case SatelliteConstellationCategory::CONSTELLATION_CATEGORY_UNKNOWN:
        return QGeoSatelliteInfo::Undefined;
    case SatelliteConstellationCategory::CONSTELLATION_CATEGORY_GPS:
        return QGeoSatelliteInfo::GPS;
    case SatelliteConstellationCategory::CONSTELLATION_CATEGORY_GLONASS:
        return QGeoSatelliteInfo::GLONASS;
    case SatelliteConstellationCategory::CONSTELLATION_CATEGORY_SBAS:
        return QGeoSatelliteInfo::SBAS;
    case SatelliteConstellationCategory::CONSTELLATION_CATEGORY_QZSS:
        return QGeoSatelliteInfo::QZSS;
    case SatelliteConstellationCategory::CONSTELLATION_CATEGORY_BEIDOU:
        return QGeoSatelliteInfo::BEIDOU;
    case SatelliteConstellationCategory::CONSTELLATION_CATEGORY_GALILEO:
        return QGeoSatelliteInfo::GALILEO;
    case SatelliteConstellationCategory::CONSTELLATION_CATEGORY_IRNSS:
        return QGeoSatelliteInfo::IRNSS;
    }

    qOhosReportFatalErrorAndAbort(
        "%s Received an unknown SatelliteConstellationCategory: %d",
        Q_FUNC_INFO, constellationCategory);
}

std::shared_ptr<void> registerSatelliteStatusInfoProducerConsumer(
    QObject *contextObject, QOhosConsumer<std::vector<SatelliteInfo>> satelliteStatusInfoConsumer)
{
    qOhosDebug(QtForOhos) << Q_FUNC_INFO << contextObject;

    auto contextObjectRef = QtOhos::makeQThreadSafeRef(contextObject);
    auto sharedSatelliteStatusInfoConsumer =
        QtOhos::moveToSharedPtr(std::move(satelliteStatusInfoConsumer));
    auto weakSatelliteStatusInfoConsumer = QtOhos::makeWeakPtr(sharedSatelliteStatusInfoConsumer);

    auto registrationHandle = QOhosJsThreadGateway::eval(
        [&](QOhosJsState &jsState) {
            return QtOhos::registerOnOffMethodsBasedEventHandler(
                getGeoLocationManagerObject(jsState), "satelliteStatusChange",
                [contextObjectRef, weakSatelliteStatusInfoConsumer](const QOhosCallbackInfo &cbInfo) {
                    auto satelliteStatusInfo = cbInfo.getFirstArg<QNapi::Object>(Q_FUNC_INFO);
                    auto satelliteInfos = convertJsSatelliteStatusInfoObjectToQGeoSatelliteInfos(
                        cbInfo.jsState(), satelliteStatusInfo);
                    contextObjectRef.visitInQtThreadIfAlive(
                        [weakSatelliteStatusInfoConsumer, satelliteInfos](auto &) {
                            auto sharedSatelliteStatusInfoConsumer = weakSatelliteStatusInfoConsumer.lock();
                            if (sharedSatelliteStatusInfoConsumer)
                                (*sharedSatelliteStatusInfoConsumer)(satelliteInfos);
                        });
                });
        });

    return QtOhos::moveToSharedPtr(
        std::make_pair(registrationHandle, sharedSatelliteStatusInfoConsumer));
}

class QOhosGeoSatelliteInfoSource : public QGeoSatelliteInfoSource
{
public:
    QOhosGeoSatelliteInfoSource(QObject *parent);
    ~QOhosGeoSatelliteInfoSource() override;

    void setUpdateInterval(int msec) override;
    int minimumUpdateInterval() const override;
    Error error() const override;

public:
    void startUpdates() override;
    void stopUpdates() override;

    void requestUpdate(int timeout) override;

private:
    enum UpdateType
    {
        SingleUpdate,
        ContinuousUpdate,
    };

    void startUpdatesHelper();
    void setErrorHelper(Error error);

    void handleSatelliteStatusInfoUpdate(std::vector<SatelliteInfo> updatedSatelliteStatusInfo);
    void handleSingleUpdateFinished();
    void handleUpdateFinished();

    QSet<UpdateType> m_updateTypes;
    std::shared_ptr<void> m_updateSatelliteStatusInfoProducerHandle;
    std::unique_ptr<QGeoPositionInfoSource> m_positionInfoSource;

    std::unique_ptr<QTimer> m_singleUpdateTimeoutTimer;

    std::optional<QList<QGeoSatelliteInfo>> m_lastUpdatedSatellitesInView;
    std::optional<QList<QGeoSatelliteInfo>> m_lastUpdatedSatellitesInUse;

    QGeoSatelliteInfoSource::Error m_error = QGeoSatelliteInfoSource::NoError;
};

QOhosGeoSatelliteInfoSource::QOhosGeoSatelliteInfoSource(QObject *parent)
    : QGeoSatelliteInfoSource(parent)
{
    qOhosDebug(QtForOhos) << Q_FUNC_INFO;
}

QOhosGeoSatelliteInfoSource::~QOhosGeoSatelliteInfoSource() = default;

void QOhosGeoSatelliteInfoSource::setUpdateInterval(int msec)
{
    qOhosDebug(QtForOhos) << Q_FUNC_INFO << msec;

    if (msec != updateAsOftenAsNecessaryInterval)
        qOhosWarning(QtForOhos) << Q_FUNC_INFO << "Specific update intervals are not supported";
}

int QOhosGeoSatelliteInfoSource::minimumUpdateInterval() const
{
    qOhosDebug(QtForOhos) << Q_FUNC_INFO;

    return updateAsOftenAsNecessaryInterval;
}

QGeoSatelliteInfoSource::Error QOhosGeoSatelliteInfoSource::error() const
{
    qOhosDebug(QtForOhos) << Q_FUNC_INFO;

    return m_error;
}

void QOhosGeoSatelliteInfoSource::startUpdates()
{
    qOhosDebug(QtForOhos) << Q_FUNC_INFO;

    if (!checkLocationOrApproximatelyLocationPermissionGranted()) {
        qOhosWarning(QtForOhos) << Q_FUNC_INFO << "Location permission not granted. Can't start updates";
        setErrorHelper(QGeoSatelliteInfoSource::AccessError);
        return;
    }

    if (m_updateTypes.contains(UpdateType::ContinuousUpdate))
        return;

    m_updateTypes.insert(UpdateType::ContinuousUpdate);

    if (!m_updateSatelliteStatusInfoProducerHandle && !m_positionInfoSource)
        startUpdatesHelper();
}

void QOhosGeoSatelliteInfoSource::stopUpdates()
{
    qOhosDebug(QtForOhos) << Q_FUNC_INFO;

    m_updateTypes.remove(UpdateType::ContinuousUpdate);

    handleUpdateFinished();
}

void QOhosGeoSatelliteInfoSource::requestUpdate(int timeout)
{
    qOhosDebug(QtForOhos) << Q_FUNC_INFO << timeout;

    if (!checkLocationOrApproximatelyLocationPermissionGranted()) {
        qOhosWarning(QtForOhos) << Q_FUNC_INFO << "Location permission not granted. Can't request update";
        setErrorHelper(QGeoSatelliteInfoSource::AccessError);
        return;
    }

    if (m_updateTypes.contains(UpdateType::SingleUpdate))
        return;

    m_updateTypes.insert(UpdateType::SingleUpdate);

    if (!m_updateSatelliteStatusInfoProducerHandle && !m_positionInfoSource)
        startUpdatesHelper();

    m_singleUpdateTimeoutTimer = makeSingleShotUpdateTimeoutTimer(
        timeout, [this]() {
            if (m_lastUpdatedSatellitesInUse.has_value() && m_lastUpdatedSatellitesInView.has_value()) {
                Q_EMIT satellitesInUseUpdated(m_lastUpdatedSatellitesInUse.value());
                Q_EMIT satellitesInViewUpdated(m_lastUpdatedSatellitesInView.value());
            } else {
                Q_EMIT errorOccurred(QGeoSatelliteInfoSource::UpdateTimeoutError);
            }

            handleSingleUpdateFinished();
        });
}

void QOhosGeoSatelliteInfoSource::startUpdatesHelper()
{
    qOhosDebug(QtForOhos) << Q_FUNC_INFO;

    m_updateSatelliteStatusInfoProducerHandle =
        registerSatelliteStatusInfoProducerConsumer(
            this, [this](auto updatedSatelliteInfos) {
                handleSatelliteStatusInfoUpdate(updatedSatelliteInfos);
            });

    m_positionInfoSource = std::unique_ptr<QGeoPositionInfoSource>(tryMakeQOhosGeoPositionInfoSource());
    if (!m_positionInfoSource)
        qOhosReportFatalErrorAndAbort("%s positionInfoSource is invalid", Q_FUNC_INFO);

    m_positionInfoSource->setPreferredPositioningMethods(QGeoPositionInfoSource::SatellitePositioningMethods);
    m_positionInfoSource->startUpdates();
}

void QOhosGeoSatelliteInfoSource::setErrorHelper(Error error)
{
    qOhosWarning(QtForOhos) << Q_FUNC_INFO << error;

    if (m_error == error)
        return;

    m_error = error;

    if (error != QGeoSatelliteInfoSource::NoError)
        Q_EMIT QGeoSatelliteInfoSource::errorOccurred(m_error);
}

void QOhosGeoSatelliteInfoSource::handleSatelliteStatusInfoUpdate(
    std::vector<SatelliteInfo> updatedSatellites)
{
    qOhosDebug(QtForOhos) << Q_FUNC_INFO << "found" << updatedSatellites.size() << "satellites";

    QList<QGeoSatelliteInfo> satellitesInView;
    QList<QGeoSatelliteInfo> satellitesInUse;

    QSet<std::pair<int, int>> seenIds;

    for (const auto &updatedSatellite : updatedSatellites) {
        if (qFuzzyIsNull(updatedSatellite.carrierToNoiseDensity))
            continue;

        QGeoSatelliteInfo satelliteInfo;
        satelliteInfo.setSatelliteIdentifier(updatedSatellite.id);
        satelliteInfo.setSatelliteSystem(
            updatedSatellite.optConstellationCategory.has_value()
                ? convertSatelliteConstellationCategoryToQt(updatedSatellite.optConstellationCategory.value())
                : QGeoSatelliteInfo::SatelliteSystem::Undefined);
        satelliteInfo.setSignalStrength(updatedSatellite.carrierToNoiseDensity);
        satelliteInfo.setAttribute(QGeoSatelliteInfo::Azimuth, updatedSatellite.azimuth);
        satelliteInfo.setAttribute(QGeoSatelliteInfo::Elevation, updatedSatellite.altitude);

        const auto uid = std::make_pair(static_cast<int>(satelliteInfo.satelliteSystem()),
                                   satelliteInfo.satelliteIdentifier());
        if (seenIds.contains(uid))
            continue;
        seenIds.insert(uid);

        satellitesInView.append(satelliteInfo);
        if (updatedSatellite.usedInFix)
            satellitesInUse.append(satelliteInfo);
    }

    Q_EMIT satellitesInUseUpdated(satellitesInUse);
    Q_EMIT satellitesInViewUpdated(satellitesInView);

    m_lastUpdatedSatellitesInView = satellitesInView;
    m_lastUpdatedSatellitesInUse = satellitesInUse;

    if (m_updateTypes.contains(UpdateType::SingleUpdate))
        handleSingleUpdateFinished();
}

void QOhosGeoSatelliteInfoSource::handleSingleUpdateFinished()
{
    m_updateTypes.remove(UpdateType::SingleUpdate);

    m_singleUpdateTimeoutTimer.reset();

    handleUpdateFinished();
}

void QOhosGeoSatelliteInfoSource::handleUpdateFinished()
{
    if (m_updateTypes.empty()) {
        m_positionInfoSource.reset();
        m_updateSatelliteStatusInfoProducerHandle.reset();
        m_lastUpdatedSatellitesInView.reset();
        m_lastUpdatedSatellitesInUse.reset();
    }
}

}

QGeoSatelliteInfoSource *makeQOhosGeoSatelliteInfoSource(QObject *parent)
{
    return new QOhosGeoSatelliteInfoSource(parent);
}

namespace QtOhos {

template<>
struct OhosEnumMeta<SatelliteConstellationCategory>
{
    static constexpr const char *fullTypeName = "@ohos.geoLocationManager.SatelliteConstellationCategory";
    static constexpr std::array<std::pair<SatelliteConstellationCategory, const char *>, 8> enumeratorsNames = {{
        {SatelliteConstellationCategory::CONSTELLATION_CATEGORY_UNKNOWN, "CONSTELLATION_CATEGORY_UNKNOWN"},
        {SatelliteConstellationCategory::CONSTELLATION_CATEGORY_GPS, "CONSTELLATION_CATEGORY_GPS"},
        {SatelliteConstellationCategory::CONSTELLATION_CATEGORY_SBAS, "CONSTELLATION_CATEGORY_SBAS"},
        {SatelliteConstellationCategory::CONSTELLATION_CATEGORY_GLONASS, "CONSTELLATION_CATEGORY_GLONASS"},
        {SatelliteConstellationCategory::CONSTELLATION_CATEGORY_QZSS, "CONSTELLATION_CATEGORY_QZSS"},
        {SatelliteConstellationCategory::CONSTELLATION_CATEGORY_BEIDOU, "CONSTELLATION_CATEGORY_BEIDOU"},
        {SatelliteConstellationCategory::CONSTELLATION_CATEGORY_GALILEO, "CONSTELLATION_CATEGORY_GALILEO"},
        {SatelliteConstellationCategory::CONSTELLATION_CATEGORY_IRNSS, "CONSTELLATION_CATEGORY_IRNSS"},
    }};
};

}

QT_END_NAMESPACE
