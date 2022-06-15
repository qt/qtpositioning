// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef SATELLITEMODEL_H
#define SATELLITEMODEL_H

#include <QAbstractListModel>
#include <QSet>
#include <QtQml/qqml.h>
#include <QtQml/QQmlParserStatus>
#include <QtPositioning/QGeoSatelliteInfoSource>

QT_FORWARD_DECLARE_CLASS(QTimer)
QT_FORWARD_DECLARE_CLASS(QGeoSatelliteInfoSource)

//! [0]
class SatelliteModel : public QAbstractListModel, public QQmlParserStatus
{
    Q_OBJECT
    Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY runningChanged)
    Q_PROPERTY(bool satelliteInfoAvailable READ canProvideSatelliteInfo NOTIFY canProvideSatelliteInfoChanged)
    Q_PROPERTY(int entryCount READ entryCount NOTIFY entryCountChanged)
    Q_PROPERTY(bool singleRequestMode READ isSingleRequest WRITE setSingleRequest NOTIFY singleRequestChanged)
    Q_INTERFACES(QQmlParserStatus)
    QML_ELEMENT
public:
    explicit SatelliteModel(QObject *parent = 0);

    enum {
        IdentifierRole = Qt::UserRole + 1,
        InUseRole,
        SignalStrengthRole,
        ElevationRole,
        AzimuthRole
    };

    //From QAbstractListModel
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    //From QQmlParserStatus
    void classBegin() override {}
    void componentComplete() override;
//! [0]

    bool running() const;
    void setRunning(bool isActive);

    bool isSingleRequest() const;
    void setSingleRequest(bool single);

    int entryCount() const;

    bool canProvideSatelliteInfo() const;

//! [1]
signals:
    void runningChanged();
    void entryCountChanged();
    void errorFound(int code);
    void canProvideSatelliteInfoChanged();
    void singleRequestChanged();

public slots:
    void clearModel();
    void updateDemoData();
//! [1]

private slots:
    void error(QGeoSatelliteInfoSource::Error);
    void satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &infos);
    void satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &infos);

private:
    QGeoSatelliteInfoSource *source;
    bool m_componentCompleted;
    bool m_running;
    bool m_runningRequested;
    QList <QGeoSatelliteInfo> knownSatellites;
    QSet<int> knownSatelliteIds;
    QSet<int> satellitesInUse;
    bool demo;
    QTimer *timer;
    bool isSingle;
    bool singleRequestServed;
//! [2]
};
//! [2]

QML_DECLARE_TYPE(SatelliteModel)

#endif // SATELLITEMODEL_H
