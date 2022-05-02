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
#ifndef QNMEAPOSITIONINFOSOURCE_H
#define QNMEAPOSITIONINFOSOURCE_H

#include <QtPositioning/QGeoPositionInfoSource>

QT_BEGIN_NAMESPACE

class QIODevice;

class QNmeaPositionInfoSourcePrivate;
class Q_POSITIONING_EXPORT QNmeaPositionInfoSource : public QGeoPositionInfoSource
{
    Q_OBJECT
public:
    enum UpdateMode {
        RealTimeMode = 1,
        SimulationMode
    };

    explicit QNmeaPositionInfoSource(UpdateMode updateMode, QObject *parent = nullptr);
    ~QNmeaPositionInfoSource();

    void setUserEquivalentRangeError(double uere);
    double userEquivalentRangeError() const;

    UpdateMode updateMode() const;

    void setDevice(QIODevice *source);
    QIODevice *device() const;

    void setUpdateInterval(int msec) override;

    QGeoPositionInfo lastKnownPosition(bool fromSatellitePositioningMethodsOnly = false) const override;
    PositioningMethods supportedPositioningMethods() const override;
    int minimumUpdateInterval() const override;
    Error error() const override;


public Q_SLOTS:
    void startUpdates() override;
    void stopUpdates() override;
    void requestUpdate(int timeout = 0) override;

protected:
    virtual bool parsePosInfoFromNmeaData(const char *data,
                                          int size,
                                          QGeoPositionInfo *posInfo,
                                          bool *hasFix);
    void setError(QGeoPositionInfoSource::Error positionError);

private:
    Q_DISABLE_COPY(QNmeaPositionInfoSource)
    friend class QNmeaPositionInfoSourcePrivate;
    QNmeaPositionInfoSourcePrivate *d;
};

QT_END_NAMESPACE

#endif
