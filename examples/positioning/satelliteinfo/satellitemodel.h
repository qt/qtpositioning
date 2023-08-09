// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef SATELLITEMODEL_H
#define SATELLITEMODEL_H

#include <QAbstractListModel>
#include <QGeoSatelliteInfo>
#include <QtQml/qqmlregistration.h>

class SatelliteModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int size READ rowCount NOTIFY sizeChanged)
    QML_ELEMENT
public:
    explicit SatelliteModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

public slots:
    void updateSatellitesInView(const QList<QGeoSatelliteInfo> &inView);
    void updateSatellitesInUse(const QList<QGeoSatelliteInfo> &inUse);

signals:
    void sizeChanged();

private:
    QList<QGeoSatelliteInfo> m_satellites;
    QSet<int> m_inUseIds;
    QSet<int> m_allIds;
};

#endif // SATELLITEMODEL_H
