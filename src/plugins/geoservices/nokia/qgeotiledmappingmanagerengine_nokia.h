/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
** This file is part of the Ovi services plugin for the Maps and
** Navigation API.  The use of these services, whether by use of the
** plugin or by other means, is governed by the terms and conditions
** described by the file OVI_SERVICES_TERMS_AND_CONDITIONS.txt in
** this package, located in the directory containing the Ovi services
** plugin source code.
**
****************************************************************************/

#ifndef QGEOTILEDMAPPINGMANAGERENGINE_NOKIA_H
#define QGEOTILEDMAPPINGMANAGERENGINE_NOKIA_H

#include "qgeotiledmappingmanagerengine.h"
#include "qgeoboundingbox.h"
#include "qgeomaptype.h"

#include <QGeoServiceProvider>

#include <QList>
#include <QHash>
#include <QSet>

QT_BEGIN_NAMESPACE

class QByteArray;
class QGeoTileSpec;

class QGeoTiledMappingManagerEngineNokia : public QGeoTiledMappingManagerEngine
{
    Q_OBJECT

public:
    QGeoTiledMappingManagerEngineNokia(const QMap<QString, QVariant> &parameters,
                                       QGeoServiceProvider::Error *error,
                                       QString *errorString);
    ~QGeoTiledMappingManagerEngineNokia();

    virtual QGeoMapData* createMapData();
    QString evaluateCopyrightsText(const QGeoMapType::MapStyle mapStyle,
                                   const int zoomLevel,
                                   const QSet<QGeoTileSpec> &tiles);

public Q_SLOTS:
    void loadCopyrightsDescriptorsFromJson(const QByteArray &jsonData);

private:
    class CopyrightDesc
    {
    public:
        CopyrightDesc()
            : maxLevel(-1),
              minLevel(-1) {}

        qreal maxLevel;
        qreal minLevel;
        QList<QGeoBoundingBox> boxes;
        QString alt;
        QString label;
    };

    void initialize();
    void populateMapTypesDb();

    QHash<QString, QList<CopyrightDesc> > m_copyrights;
    QHash<QGeoMapType::MapStyle, QString> m_mapTypeStrings;
};

QT_END_NAMESPACE

#endif // QGEOTILEDMAPPINGMANAGERENGINE_NOKIA_H
