// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#ifndef QNMEASATELLITEINFOSOURCE_H
#define QNMEASATELLITEINFOSOURCE_H

#include <QtPositioning/QGeoSatelliteInfoSource>

QT_BEGIN_NAMESPACE

class QIODevice;

class QNmeaSatelliteInfoSourcePrivate;
class Q_POSITIONING_EXPORT QNmeaSatelliteInfoSource : public QGeoSatelliteInfoSource
{
    Q_OBJECT
public:
    enum class UpdateMode {
        RealTimeMode = 1,
        SimulationMode
    };

    static QString SimulationUpdateInterval;

    explicit QNmeaSatelliteInfoSource(UpdateMode mode, QObject *parent = nullptr);
    ~QNmeaSatelliteInfoSource() override;

    UpdateMode updateMode() const;

    void setDevice(QIODevice *source);
    QIODevice *device() const;

    void setUpdateInterval(int msec) override;
    int minimumUpdateInterval() const override;
    Error error() const override;

    bool setBackendProperty(const QString &name, const QVariant &value) override;
    QVariant backendProperty(const QString &name) const override;

public Q_SLOTS:
    void startUpdates() override;
    void stopUpdates() override;
    void requestUpdate(int timeout = 0) override;

protected:
    virtual QGeoSatelliteInfo::SatelliteSystem
    parseSatellitesInUseFromNmea(const char *data, int size, QList<int> &pnrsInUse);
    enum SatelliteInfoParseStatus {
        NotParsed = 0,
        PartiallyParsed,
        FullyParsed
    };
    virtual SatelliteInfoParseStatus parseSatelliteInfoFromNmea(const char *data, int size,
                                                                QList<QGeoSatelliteInfo> &infos,
                                                                QGeoSatelliteInfo::SatelliteSystem &system);

    QNmeaSatelliteInfoSourcePrivate *d;
    void setError(QGeoSatelliteInfoSource::Error satelliteError);

    friend class QNmeaSatelliteInfoSourcePrivate;
    Q_DISABLE_COPY(QNmeaSatelliteInfoSource)

    // for using the SatelliteInfoParseStatus enum
    friend class QLocationUtils;
};

QT_END_NAMESPACE

#endif // QNMEASATELLITEINFOSOURCE_H
