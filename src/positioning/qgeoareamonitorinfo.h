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

#ifndef QGEOAREAMONITORINFO_H
#define QGEOAREAMONITORINFO_H

#include <QtPositioning/QGeoCoordinate>
#include <QtPositioning/QGeoShape>
#include <QtCore/QExplicitlySharedDataPointer>
#include <QtCore/QMetaType>
#include <QtCore/QVariantMap>

QT_BEGIN_NAMESPACE

class QDataStream;
class QGeoAreaMonitorInfo;

Q_POSITIONING_EXPORT size_t qHash(const QGeoAreaMonitorInfo &key, size_t seed = 0) noexcept;
namespace QTest
{
Q_POSITIONING_EXPORT char *toString(const QGeoAreaMonitorInfo &info);
} // namespace QTest

class QGeoAreaMonitorInfoPrivate;
QT_DECLARE_QESDP_SPECIALIZATION_DTOR_WITH_EXPORT(QGeoAreaMonitorInfoPrivate, Q_POSITIONING_EXPORT)

class Q_POSITIONING_EXPORT QGeoAreaMonitorInfo
{
public:
    explicit QGeoAreaMonitorInfo(const QString &name = QString());
    QGeoAreaMonitorInfo(const QGeoAreaMonitorInfo &other);
    QGeoAreaMonitorInfo(QGeoAreaMonitorInfo &&other) noexcept = default;
    ~QGeoAreaMonitorInfo();

    QGeoAreaMonitorInfo &operator=(const QGeoAreaMonitorInfo &other);
    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_PURE_SWAP(QGeoAreaMonitorInfo)

    void swap(QGeoAreaMonitorInfo &other) noexcept { d.swap(other.d); }

    friend bool operator==(const QGeoAreaMonitorInfo &lhs, const QGeoAreaMonitorInfo &rhs)
    {
        return equals(lhs, rhs);
    }
    friend bool operator!=(const QGeoAreaMonitorInfo &lhs, const QGeoAreaMonitorInfo &rhs)
    {
        return !equals(lhs, rhs);
    }

    QString name() const;
    void setName(const QString &name);

    QString identifier() const;
    bool isValid() const;

    QGeoShape area() const;
    void setArea(const QGeoShape &newShape);

    QDateTime expiration() const;
    void setExpiration(const QDateTime &expiry);

    bool isPersistent() const;
    void setPersistent(bool isPersistent);

    QVariantMap notificationParameters() const;
    void setNotificationParameters(const QVariantMap &parameters);

    void detach();

private:
    static bool equals(const QGeoAreaMonitorInfo &lhs, const QGeoAreaMonitorInfo &rhs);
    QExplicitlySharedDataPointer<QGeoAreaMonitorInfoPrivate> d;
    friend class QGeoAreaMonitorInfoPrivate;

#ifndef QT_NO_DATASTREAM
    friend QDataStream &operator<<(QDataStream &ds, const QGeoAreaMonitorInfo &monitor)
    {
        return dataStreamOut(ds, monitor);
    }
    friend QDataStream &operator>>(QDataStream &ds, QGeoAreaMonitorInfo &monitor)
    {
        return dataStreamIn(ds, monitor);
    }
    static QDataStream &dataStreamOut(QDataStream &ds, const QGeoAreaMonitorInfo &monitor);
    static QDataStream &dataStreamIn(QDataStream &ds, QGeoAreaMonitorInfo &monitor);
#endif
    friend Q_POSITIONING_EXPORT size_t qHash(const QGeoAreaMonitorInfo &key, size_t seed) noexcept;
    friend Q_POSITIONING_EXPORT char *QTest::toString(const QGeoAreaMonitorInfo& info);
#ifndef QT_NO_DEBUG_STREAM
    friend QDebug operator<<(QDebug dbg, const QGeoAreaMonitorInfo &monitor)
    {
        return debugStreaming(dbg, monitor);
    }
    static QDebug debugStreaming(QDebug dbg, const QGeoAreaMonitorInfo &monitor);
#endif
};

Q_DECLARE_SHARED(QGeoAreaMonitorInfo)

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QGeoAreaMonitorInfo)

#endif // QGEOAREAMONITORINFO_H
