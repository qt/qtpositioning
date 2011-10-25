/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qplacemanagerengine.h"
#include "qplacemanagerengine_p.h"

QT_USE_NAMESPACE

/*!
    \class QPlaceManagerEngine
    \inmodule QtLocation
    \ingroup QtLocation-impl
    \since QtLocation 5.0

    \brief The QPlaceManagerEngine class provides an interface and convenience methods to
    implementers of QGeoServiceProvider plugins who want to provide access to place search
    functionality.

    Subclasses of QPlaceManagerEngine need to provide an implementation of getPlaceDetails(),
    getContent(), postRating(), getReviews(), searchForPlaces(), supportedSearchVisibilityScopes(),
    recommendations(), textPredictions(), savePlace(), supportedSaveVisibilityScopes(), removePlace(),
    initializeCategories() and categories().

    \sa QPlaceManager
*/

/*!
    Constructs a new engine with the specified \a parent, using \a parameters to pass any
    implementation specific data to the engine.
*/
QPlaceManagerEngine::QPlaceManagerEngine(const QMap<QString, QVariant> &parameters,
                                         QObject *parent)
:   QObject(parent), d_ptr(new QPlaceManagerEnginePrivate)
{
    Q_UNUSED(parameters)
}

/*!
    Destroys this engine.
*/
QPlaceManagerEngine::~QPlaceManagerEngine()
{
    delete d_ptr;
}

/*!
    Sets the name which this engine implementation uses to distinguish itself
    from the implementations provided by other plugins to \a managerName.

    The combination of managerName() and managerVersion() should be unique
    amongst plugin implementations.
*/
void QPlaceManagerEngine::setManagerName(const QString &managerName)
{
    d_ptr->managerName = managerName;
}

/*!
    Returns the name which this engine implementation uses to distinguish
    itself from the implementations provided by other plugins.

    The combination of managerName() and managerVersion() should be unique
    amongst plugin implementations.
*/
QString QPlaceManagerEngine::managerName() const
{
    return d_ptr->managerName;
}

/*!
    Sets the version of this engine implementation to \a managerVersion.

    The combination of managerName() and managerVersion() should be unique
    amongst plugin implementations.
*/
void QPlaceManagerEngine::setManagerVersion(int managerVersion)
{
    d_ptr->managerVersion = managerVersion;
}

/*!
    Returns the version of this engine implementation.

    The combination of managerName() and managerVersion() should be unique
    amongst plugin implementations.
*/
int QPlaceManagerEngine::managerVersion() const
{
    return d_ptr->managerVersion;
}

/*!
    Returns the manager instance used to create this engine.
*/
QPlaceManager *QPlaceManagerEngine::manager() const
{
    return d_ptr->manager;
}

/*!
    \fn void QPlaceManagerEngine::authenticationRequired(QAuthenticator *authenticator)

    This signal is emitted if authentication details are required by the manager engine
    to peform certain operations.  If the authentication was successful, the next time
    the operations are performed, the same credentials are used and the
    authenticationRequired signal is not emitted again.

    If authentication is unsuccessful, the manager engine will emit the signal again.
*/

QPlaceManagerEnginePrivate::QPlaceManagerEnginePrivate()
    :   managerVersion(-1), manager(0)
{
}

QPlaceManagerEnginePrivate::~QPlaceManagerEnginePrivate()
{
}

/*!
    \fn void QPlaceManagerEngine::placeAdded(const QString&placeId)

    This signal is emitted if a place has been added to the manager engine's datastore.

    It is generally only emitted by managers that store places locally.

*/

/*!
    \fn void QPlaceManagerEngine::placeUpdated(const QString&placeId)

    This signal is emitted if a place has been modified in the manager engine's datastore.

    It is generally only emitted by managers that store places locally.
*/

/*!
    \fn void QPlaceManagerEngine::placeRemoved(const QString&placeId)

    This signal is emitted if a place has been removed from the manager engine's datastore.

    It is generally only emitted by managers that store places locally.
*/

/*!
    \fn void QPlaceManagerEngine::categoryAdded(const QString&categoryId)

    This signal is emitted if a category has been added to the manager engine's datastore.

    It is generally only emitted by managers that store categories locally.

*/

/*!
    \fn void QPlaceManagerEngine::categoryUpdated(const QString&categoryId)

    This signal is emitted if a category has been modified in the manager engine's datastore.

    It is generally only emitted by managers that store categories locally.
*/

/*!
    \fn void QPlaceManagerEngine::categoryRemoved(const QString&categoryId)

    This signal is emitted if a category has been removed from the manager engine's datastore.

    It is generally only emitted by managers that store categories locally.
*/

#include "moc_qplacemanagerengine.cpp"

QT_END_NAMESPACE

