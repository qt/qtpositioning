// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <QObject>
#include <QList>
#include <QGeoCoordinate>
#include <qqml.h>

QT_BEGIN_NAMESPACE

class Properties : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QGeoCoordinate coordinate READ coordinate WRITE setCoordinate NOTIFY coordinateChanged)
    Q_PROPERTY(QList<QGeoCoordinate> coordinates READ coordinates WRITE setCoordinates NOTIFY coordinatesChanged)
    QML_ELEMENT

public:
    explicit Properties(QObject *parent = nullptr);

    QGeoCoordinate coordinate() const { return m_coordinate; }
    void setCoordinate(const QGeoCoordinate &c);

    QList<QGeoCoordinate> coordinates() const { return m_coordinates; }
    void setCoordinates(const QList<QGeoCoordinate> &values);

signals:
    void coordinateChanged(const QGeoCoordinate &);
    void coordinatesChanged(const QList<QGeoCoordinate> &);

private:
    QGeoCoordinate m_coordinate;
    QList<QGeoCoordinate> m_coordinates;
};

QT_END_NAMESPACE

#endif // FACTORY_H
