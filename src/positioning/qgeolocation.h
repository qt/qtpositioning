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

#ifndef QGEOLOCATION_H
#define QGEOLOCATION_H

#include <QtCore/QSharedDataPointer>
#include <QtCore/QMetaType>
#include <QtPositioning/qpositioningglobal.h>

QT_BEGIN_NAMESPACE

class QGeoAddress;
class QGeoCoordinate;
class QGeoShape;
class QGeoLocationPrivate;
QT_DECLARE_QSDP_SPECIALIZATION_DTOR_WITH_EXPORT(QGeoLocationPrivate, Q_POSITIONING_EXPORT)

class Q_POSITIONING_EXPORT QGeoLocation
{
public:
    QGeoLocation();
    QGeoLocation(const QGeoLocation &other);
    QGeoLocation(QGeoLocation &&other) noexcept = default;
    ~QGeoLocation();

    QGeoLocation &operator=(const QGeoLocation &other);
    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_PURE_SWAP(QGeoLocation)

    void swap(QGeoLocation &other) noexcept { d.swap(other.d); }

    friend bool operator==(const QGeoLocation &lhs, const QGeoLocation &rhs)
    {
        return equals(lhs, rhs);
    }
    friend bool operator!=(const QGeoLocation &lhs, const QGeoLocation &rhs)
    {
        return !equals(lhs, rhs);
    }

    QGeoAddress address() const;
    void setAddress(const QGeoAddress &address);
    QGeoCoordinate coordinate() const;
    void setCoordinate(const QGeoCoordinate &position);
    QGeoShape boundingShape() const;
    void setBoundingShape(const QGeoShape &shape);
    QVariantMap extendedAttributes() const;
    void setExtendedAttributes(const QVariantMap &data);

    bool isEmpty() const;

private:
    static bool equals(const QGeoLocation &lhs, const QGeoLocation &rhs);
    QSharedDataPointer<QGeoLocationPrivate> d;
};

Q_POSITIONING_EXPORT size_t qHash(const QGeoLocation &location, size_t seed = 0) noexcept;

Q_DECLARE_SHARED(QGeoLocation)

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QGeoLocation)

#endif
