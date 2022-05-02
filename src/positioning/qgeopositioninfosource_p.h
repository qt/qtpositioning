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

#ifndef QGEOPOSITIONINFOSOURCE_P_H
#define QGEOPOSITIONINFOSOURCE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/private/qobject_p.h>
#include <QtCore/private/qproperty_p.h>
#include <QtPositioning/private/qpositioningglobal_p.h>
#include "qgeopositioninfosource.h"
#include "qgeopositioninfosourcefactory.h"
#include <QJsonObject>
#include <QString>
#include <QMultiHash>
#include <QList>

QT_BEGIN_NAMESPACE

class QGeoPositionInfoSourcePrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QGeoPositionInfoSource)
public:
    virtual ~QGeoPositionInfoSourcePrivate();

    static QGeoPositionInfoSourceFactory *loadFactory(const QJsonObject &meta);
    static QGeoPositionInfoSource *createSourceReal(const QJsonObject &meta,
                                                    const QVariantMap &parameters,
                                                    QObject *parent);

    void setPositioningMethods(QGeoPositionInfoSource::PositioningMethods methods)
    {
        q_func()->setPreferredPositioningMethods(methods);
    }

    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(QGeoPositionInfoSourcePrivate, int, interval, 0)
    Q_OBJECT_COMPAT_PROPERTY_WITH_ARGS(QGeoPositionInfoSourcePrivate,
                                       QGeoPositionInfoSource::PositioningMethods, methods,
                                       &QGeoPositionInfoSourcePrivate::setPositioningMethods,
                                       QGeoPositionInfoSource::NoPositioningMethods)
    QString sourceName;

    static QMultiHash<QString, QJsonObject> plugins(bool reload = false);
    static void loadPluginMetadata(QMultiHash<QString, QJsonObject> &list);
    static QList<QJsonObject> pluginsSorted();
};

QT_END_NAMESPACE

#endif // QGEOPOSITIONINFOSOURCE_P_H
