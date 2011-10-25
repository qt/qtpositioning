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

#include "qgeocodereply.h"
#include "qgeocodereply_p.h"

QT_BEGIN_NAMESPACE
/*!
    \class QGeocodeReply
    \inmodule QtLocation
    \ingroup QtLocation-geocoding
    \since QtLocation 5.0

    \brief The QGeocodeReply class manages an operation started by an
    instance of QGeocodingManager.

    Instances of QGeocodeReply manage the state and results of these
    operations.

    The isFinished(), error() and errorString() methods provide information
    on whether the operation has completed and if it completed successfully.

    The finished() and error(QGeocodeReply::Error,QString)
    signals can be used to monitor the progress of the operation.

    It is possible that a newly created QGeocodeReply may be in a finished
    state, most commonly because an error has occurred. Since such an instance
    will never emit the finished() or
    error(QGeocodeReply::Error,QString) signals, it is
    important to check the result of isFinished() before making the connections
    to the signals. The documentation for QGeocodingManager demonstrates how
    this might be carried out.

    If the operation completes successfully the results will be able to be
    accessed with locations().
*/

/*!
    \enum QGeocodeReply::Error

    Describes an error which prevented the completion of the operation.

    \value NoError
        No error has occurred.
    \value EngineNotSetError
        The geocoding manager that was used did not have a QGeocodingManagerEngine instance associated with it.
    \value CommunicationError
        An error occurred while communicating with the service provider.
    \value ParseError
        The response from the service provider was in an unrecognizable format.
    \value UnsupportedOptionError
        The requested operation or one of the options for the operation are not
        supported by the service provider.
    \value CombinationError
        An error occurred while results where being combined from multiple sources.
    \value UnknownError
        An error occurred which does not fit into any of the other categories.
*/

/*!
    Constructs a geocode reply with the specified \a parent.
*/
QGeocodeReply::QGeocodeReply(QObject *parent)
    : QObject(parent),
      d_ptr(new QGeocodeReplyPrivate()) {}

/*!
    Constructs a geocode reply with a given \a error and \a errorString and the specified \a parent.
*/
QGeocodeReply::QGeocodeReply(Error error, const QString &errorString, QObject *parent)
    : QObject(parent),
      d_ptr(new QGeocodeReplyPrivate(error, errorString)) {}

/*!
    Destroys this reply object.
*/
QGeocodeReply::~QGeocodeReply()
{
    delete d_ptr;
}

/*!
    Sets whether or not this reply has finished to \a finished.

    If \a finished is true, this will cause the finished() signal to be
    emitted.

    If the operation completed successfully, QGeocodeReply::setLocations()
    should be called before this function. If an error occurred,
    QGeocodeReply::setError() should be used instead.
*/
void QGeocodeReply::setFinished(bool finished)
{
    d_ptr->isFinished = finished;
    if (d_ptr->isFinished)
        emit this->finished();
}

/*!
    Return true if the operation completed successfully or encountered an
    error which cause the operation to come to a halt.
*/
bool QGeocodeReply::isFinished() const
{
    return d_ptr->isFinished;
}

/*!
    Sets the error state of this reply to \a error and the textual
    representation of the error to \a errorString.

    This wil also cause error() and finished() signals to be emitted, in that
    order.
*/
void QGeocodeReply::setError(QGeocodeReply::Error error, const QString &errorString)
{
    d_ptr->error = error;
    d_ptr->errorString = errorString;
    emit this->error(error, errorString);
    setFinished(true);
}

/*!
    Returns the error state of this reply.

    If the result is QGeocodeReply::NoError then no error has occurred.
*/
QGeocodeReply::Error QGeocodeReply::error() const
{
    return d_ptr->error;
}

/*!
    Returns the textual representation of the error state of this reply.

    If no error has occurred this will return an empty string.  It is possible
    that an error occurred which has no associated textual representation, in
    which case this will also return an empty string.

    To determine whether an error has occurred, check to see if
    QGeocodeReply::error() is equal to QGeocodeReply::NoError.
*/
QString QGeocodeReply::errorString() const
{
    return d_ptr->errorString;
}

/*!
    Sets the viewport which contains the results to \a viewport.
*/
void QGeocodeReply::setViewport(QGeoBoundingArea *viewport)
{
    d_ptr->viewport = viewport;
}

/*!
    Returns the viewport which contains the results.

    This function will return 0 if no viewport bias
    was specified in the QGeocodingManager function which created this reply.
*/
QGeoBoundingArea* QGeocodeReply::viewport() const
{
    return d_ptr->viewport;
}

/*!
    Returns a list of locations.

    The locations are the results of the operation corresponding to the
    QGeocodingManager function which created this reply.
*/
QList<QGeoLocation> QGeocodeReply::locations() const
{
    return d_ptr->locations;
}

/*!
    Adds \a location to the list of locations in this reply.
*/
void QGeocodeReply::addLocation(const QGeoLocation &location)
{
    d_ptr->locations.append(location);
}

/*!
    Sets the list of \a locations in the reply.
*/
void QGeocodeReply::setLocations(const QList<QGeoLocation> &locations)
{
    d_ptr->locations = locations;
}

/*!
    Cancels the operation immediately.

    This will do nothing if the reply is finished.
*/
void QGeocodeReply::abort()
{
    if (!isFinished())
        setFinished(true);
}

/*!
    Returns the limit on the number of responses from each data source.

    If no limit was set this function will return -1.

    This may be more than locations().length() if the number of responses
    was less than the number requested.
*/
int QGeocodeReply::limit() const
{
    return d_ptr->limit;
}

/*!
    Returns the offset into the entire result set at which to start
    fetching results.
*/
int QGeocodeReply::offset() const
{
    return d_ptr->offset;
}

/*!
    Sets the limit on the number of responses from each data source to \a limit.

    If \a limit is -1 then all available responses will be returned.
*/
void QGeocodeReply::setLimit(int limit)
{
    d_ptr->limit = limit;
}

/*!
    Sets the offset in the entire result set at which to start
    fetching result to \a offset.
*/
void QGeocodeReply::setOffset(int offset)
{
    d_ptr->offset = offset;
}

/*!
    \fn void QGeocodeReply::finished()

    This signal is emitted when this reply has finished processing.

    If error() equals QGeocodeReply::NoError then the processing
    finished successfully.

    This signal and QGeocodingManager::finished() will be
    emitted at the same time.

    \note Do no delete this reply object in the slot connected to this
    signal. Use deleteLater() instead.
*/
/*!
    \fn void QGeocodeReply::error(QGeocodeReply::Error error, const QString &errorString)

    This signal is emitted when an error has been detected in the processing of
    this reply. The finished() signal will probably follow.

    The error will be described by the error code \a error. If \a errorString is
    not empty it will contain a textual description of the error.

    This signal and QGeocodingManager::error() will be emitted at the same time.

    \note Do no delete this reply object in the slot connected to this
    signal. Use deleteLater() instead.
*/

/*******************************************************************************
*******************************************************************************/

QGeocodeReplyPrivate::QGeocodeReplyPrivate()
    : error(QGeocodeReply::NoError),
      errorString(QLatin1String("")),
      isFinished(false),
      viewport(0),
      limit(-1),
      offset(0) {}

QGeocodeReplyPrivate::QGeocodeReplyPrivate(QGeocodeReply::Error error, const QString &errorString)
    : error(error),
      errorString(errorString),
      isFinished(true),
      viewport(0),
      limit(-1),
      offset(0) {}

QGeocodeReplyPrivate::~QGeocodeReplyPrivate() {}


#include "moc_qgeocodereply.cpp"

QT_END_NAMESPACE
