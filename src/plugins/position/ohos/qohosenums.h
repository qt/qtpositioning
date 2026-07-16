// Copyright (C) 2026 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOHOSENUMS_H
#define QOHOSENUMS_H

#include <QtCore/qglobal.h>
#include <QtCore/qmetatype.h>
#include <array>
#include <info/application_target_sdk_version.h>

QT_BEGIN_NAMESPACE

namespace QtOhos {

namespace enums {

namespace ohos {

namespace geoLocationManager {

enum class LocationError {
    LOCATING_FAILED_BACKGROUND_PERMISSION_DENIED,
    LOCATING_FAILED_DEFAULT,
    LOCATING_FAILED_INTERNET_ACCESS_FAILURE,
    LOCATING_FAILED_LOCATION_PERMISSION_DENIED,
    LOCATING_FAILED_LOCATION_SWITCH_OFF,
};

enum class SatelliteConstellationCategory {
    CONSTELLATION_CATEGORY_BEIDOU,
    CONSTELLATION_CATEGORY_GALILEO,
    CONSTELLATION_CATEGORY_GLONASS,
    CONSTELLATION_CATEGORY_GPS,
    CONSTELLATION_CATEGORY_IRNSS,
    CONSTELLATION_CATEGORY_QZSS,
    CONSTELLATION_CATEGORY_SBAS,
    CONSTELLATION_CATEGORY_UNKNOWN,
};

enum class UserActivityScenario {
    DAILY_LIFE_SERVICE,
    NAVIGATION,
    SPORT,
    TRANSPORT,
};

}

}

}

template<typename Enum>
struct OhosEnumMeta;

template<>
struct OhosEnumMeta<enums::ohos::geoLocationManager::LocationError>
{
    using Enum = enums::ohos::geoLocationManager::LocationError;
    static constexpr const char *fullTypeName = "@ohos.geoLocationManager.LocationError";
    static constexpr std::array<std::pair<Enum, const char *>, 5> enumeratorsNames = {{
        {Enum::LOCATING_FAILED_BACKGROUND_PERMISSION_DENIED, "LOCATING_FAILED_BACKGROUND_PERMISSION_DENIED"},
        {Enum::LOCATING_FAILED_DEFAULT, "LOCATING_FAILED_DEFAULT"},
        {Enum::LOCATING_FAILED_INTERNET_ACCESS_FAILURE, "LOCATING_FAILED_INTERNET_ACCESS_FAILURE"},
        {Enum::LOCATING_FAILED_LOCATION_PERMISSION_DENIED, "LOCATING_FAILED_LOCATION_PERMISSION_DENIED"},
        {Enum::LOCATING_FAILED_LOCATION_SWITCH_OFF, "LOCATING_FAILED_LOCATION_SWITCH_OFF"},
    }};
};

template<>
struct OhosEnumMeta<enums::ohos::geoLocationManager::SatelliteConstellationCategory>
{
    using Enum = enums::ohos::geoLocationManager::SatelliteConstellationCategory;
    static constexpr const char *fullTypeName = "@ohos.geoLocationManager.SatelliteConstellationCategory";
    static constexpr std::array<std::pair<Enum, const char *>, 8> enumeratorsNames = {{
        {Enum::CONSTELLATION_CATEGORY_BEIDOU, "CONSTELLATION_CATEGORY_BEIDOU"},
        {Enum::CONSTELLATION_CATEGORY_GALILEO, "CONSTELLATION_CATEGORY_GALILEO"},
        {Enum::CONSTELLATION_CATEGORY_GLONASS, "CONSTELLATION_CATEGORY_GLONASS"},
        {Enum::CONSTELLATION_CATEGORY_GPS, "CONSTELLATION_CATEGORY_GPS"},
        {Enum::CONSTELLATION_CATEGORY_IRNSS, "CONSTELLATION_CATEGORY_IRNSS"},
        {Enum::CONSTELLATION_CATEGORY_QZSS, "CONSTELLATION_CATEGORY_QZSS"},
        {Enum::CONSTELLATION_CATEGORY_SBAS, "CONSTELLATION_CATEGORY_SBAS"},
        {Enum::CONSTELLATION_CATEGORY_UNKNOWN, "CONSTELLATION_CATEGORY_UNKNOWN"},
    }};
};

template<>
struct OhosEnumMeta<enums::ohos::geoLocationManager::UserActivityScenario>
{
    using Enum = enums::ohos::geoLocationManager::UserActivityScenario;
    static constexpr const char *fullTypeName = "@ohos.geoLocationManager.UserActivityScenario";
    static constexpr std::array<std::pair<Enum, const char *>, 4> enumeratorsNames = {{
        {Enum::DAILY_LIFE_SERVICE, "DAILY_LIFE_SERVICE"},
        {Enum::NAVIGATION, "NAVIGATION"},
        {Enum::SPORT, "SPORT"},
        {Enum::TRANSPORT, "TRANSPORT"},
    }};
};

}

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QT_PREPEND_NAMESPACE(QtOhos::enums::ohos::geoLocationManager::LocationError));
Q_DECLARE_METATYPE(QT_PREPEND_NAMESPACE(QtOhos::enums::ohos::geoLocationManager::SatelliteConstellationCategory));
Q_DECLARE_METATYPE(QT_PREPEND_NAMESPACE(QtOhos::enums::ohos::geoLocationManager::UserActivityScenario));

#endif
