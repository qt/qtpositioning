/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtPositioning module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativepositionsource_p.h"
#include "qdeclarativeposition_p.h"

#include <QtCore/QCoreApplication>
#include <QtQml/qqmlinfo.h>
#include <QtQml/qqml.h>
#include <qdeclarativepluginparameter_p.h>
#include <QFile>
#include <QtNetwork/QTcpSocket>
#include <QTimer>

QT_BEGIN_NAMESPACE

/*!
    \qmltype PositionSource
    //! \instantiates QDeclarativePositionSource
    \inqmlmodule QtPositioning
    \since 5.2

    \brief The PositionSource type provides the device's current position.

    The PositionSource type provides information about the user device's
    current position. The position is available as a \l{Position} type, which
    contains all the standard parameters typically available from GPS and other
    similar systems, including longitude, latitude, speed and accuracy details.

    As different position sources are available on different platforms and
    devices, these are categorized by their basic type (Satellite, NonSatellite,
    and AllPositioningMethods). The available methods for the current platform
    can be enumerated in the \l{supportedPositioningMethods} property.

    To indicate which methods are suitable for your application, set the
    \l{preferredPositioningMethods} property. If the preferred methods are not
    available, the default source of location data for the platform will be
    chosen instead. If no default source is available (because none are installed
    for the runtime platform, or because it is disabled), the \l{valid} property
    will be set to false.

    The \l updateInterval property can then be used to indicate how often your
    application wishes to receive position updates. The \l{start}(),
    \l{stop}() and \l{update}() methods can be used to control the operation
    of the PositionSource, as well as the \l{active} property, which when set
    is equivalent to calling \l{start}() or \l{stop}().

    When the PositionSource is active, position updates can be retrieved
    either by simply using the \l{position} property in a binding (as the
    value of another item's property), or by providing an implementation of
    the \c {onPositionChanged} signal-handler.

    \section2 Example Usage

    The following example shows a simple PositionSource used to receive
    updates every second and print the longitude and latitude out to
    the console.

    \code
    PositionSource {
        id: src
        updateInterval: 1000
        active: true

        onPositionChanged: {
            var coord = src.position.coordinate;
            console.log("Coordinate:", coord.longitude, coord.latitude);
        }
    }
    \endcode

    The \l{geoflickr}{GeoFlickr} example application shows how to use
    a PositionSource in your application to retrieve local data for users
    from a REST web service.

    \sa {QtPositioning::Position}, {QGeoPositionInfoSource}, {PluginParameter}

*/

QDeclarativePositionSource::QDeclarativePositionSource()
{
    m_position.setValueBypassingBindings(new QDeclarativePosition(this));
}

QDeclarativePositionSource::~QDeclarativePositionSource()
{
    delete m_positionSource;
}


/*!
    \qmlproperty string PositionSource::name

    This property holds the unique internal name for the plugin currently
    providing position information.

    Setting the property causes the PositionSource to use a particular positioning provider.  If
    the PositionSource is active at the time that the name property is changed, it will become
    inactive.  If the specified positioning provider cannot be loaded the position source will
    become invalid.

    Changing the name property may cause the \l {updateInterval}, \l {supportedPositioningMethods}
    and \l {preferredPositioningMethods} properties to change as well.
*/


QString QDeclarativePositionSource::name() const
{
    return m_sourceName;
}

void QDeclarativePositionSource::setName(const QString &newName)
{
    m_sourceName.removeBindingUnlessInWrapper();
    if (m_positionSource && m_positionSource->sourceName() == newName)
        return;

    if (newName.isEmpty() && m_defaultSourceUsed)
        return; // previously attached to a default source, now requesting the same.

    const QString previousName = name();

    if (!m_componentComplete || !m_parametersInitialized) {
        if (previousName != newName) {
            m_sourceName.setValueBypassingBindings(newName);
            m_sourceName.notify();
        }
        return;
    }

    // tryAttach() will update the m_sourceName correctly
    tryAttach(newName, false);
}

QBindable<QString> QDeclarativePositionSource::bindableName()
{
    return QBindable<QString>(&m_sourceName);
}

QBindable<QDeclarativePosition *> QDeclarativePositionSource::bindablePosition() const
{
    return QBindable<QDeclarativePosition *>(&m_position);
}

/*!
    \internal
*/
void QDeclarativePositionSource::tryAttach(const QString &newName, bool useFallback)
{
    const QString previousName = name();
    const bool sourceExisted = (m_positionSource != nullptr);

    int previousUpdateInterval = updateInterval();
    PositioningMethods previousPositioningMethods = supportedPositioningMethods();
    PositioningMethods previousPreferredPositioningMethods = preferredPositioningMethods();

    m_defaultSourceUsed = false;

    if (newName.isEmpty()) {
        setSource(QGeoPositionInfoSource::createDefaultSource(parameterMap(), this));
        m_defaultSourceUsed = true;
    } else {
        setSource(QGeoPositionInfoSource::createSource(newName, parameterMap(), this));
        if (!m_positionSource && useFallback) {
            setSource(QGeoPositionInfoSource::createDefaultSource(parameterMap(), this));
            m_defaultSourceUsed = true;
        }
    }

    if (m_positionSource) {
        m_sourceName.setValueBypassingBindings(m_positionSource->sourceName());

        connect(m_positionSource, SIGNAL(positionUpdated(QGeoPositionInfo)),
                this, SLOT(positionUpdateReceived(QGeoPositionInfo)));
        connect(m_positionSource, SIGNAL(errorOccurred(QGeoPositionInfoSource::Error)),
                this, SLOT(sourceErrorReceived(QGeoPositionInfoSource::Error)));

        m_positionSource->setUpdateInterval(m_updateInterval);
        m_positionSource->setPreferredPositioningMethods(
            static_cast<QGeoPositionInfoSource::PositioningMethods>(int(m_preferredPositioningMethods)));

        const QGeoPositionInfo &lastKnown = m_positionSource->lastKnownPosition();
        if (lastKnown.isValid())
            setPosition(lastKnown);
    } else {
        m_sourceName.setValueBypassingBindings(newName);
        m_defaultSourceUsed = false;
        if (m_active) {
            m_active = false;
            emit activeChanged();
        }
    }

    if (previousUpdateInterval != updateInterval())
        emit updateIntervalChanged();

    if (previousPreferredPositioningMethods != preferredPositioningMethods())
        emit preferredPositioningMethodsChanged();

    if (previousPositioningMethods != supportedPositioningMethods())
        notifySupportedPositioningMethodsChanged();

    const bool sourceCurrentlyExists = (m_positionSource != nullptr);
    if (sourceExisted != sourceCurrentlyExists) {
        m_isValid.notify();
        emit validityChanged();
    }

    if (m_active) { // implies m_positionSource
        if (!sourceExisted) {
            QTimer::singleShot(0, this, SLOT(start())); // delay ensures all properties have been set
        } else {
            m_active = false;
            emit activeChanged();
        }
    }

    if (previousName != m_sourceName)
        m_sourceName.notify();
}

/*!
    \qmlproperty bool PositionSource::valid

    This property is true if the PositionSource object has acquired a valid
    backend plugin to provide data. If false, other methods on the PositionSource
    will have no effect.

    Applications should check this property to determine whether positioning is
    available and enabled on the runtime platform, and react accordingly.
*/
bool QDeclarativePositionSource::isValid() const
{
    return m_isValid.value();
}

QBindable<bool> QDeclarativePositionSource::bindableIsValid() const
{
    return QBindable<bool>(&m_isValid);
}

bool QDeclarativePositionSource::isValidActualComputation() const
{
    return m_positionSource != nullptr;
}

void QDeclarativePositionSource::handleUpdateTimeout()
{
    // notify will be called by the calling method
    m_sourceError.setValueBypassingBindings(QDeclarativePositionSource::UpdateTimeoutError);

    if (!m_active)
        return;

    if (m_singleUpdate) {
        m_singleUpdate = false;

        if (!m_regularUpdates) {
            // only singleUpdate based timeouts change activity
            // continuous updates may resume again
            // (see QGeoPositionInfoSource::startUpdates())
            m_active = false;
            emit activeChanged();
        }
    }
}

/*!
    \internal
*/
void QDeclarativePositionSource::onParameterInitialized()
{
    m_parametersInitialized = true;
    for (QDeclarativePluginParameter *p: qAsConst(m_parameters)) {
        if (!p->isInitialized()) {
            m_parametersInitialized = false;
            break;
        }
    }

    // If here, componentComplete has been called.
    if (m_parametersInitialized)
        tryAttach(m_sourceName.value());
}

void QDeclarativePositionSource::notifySupportedPositioningMethodsChanged()
{
    m_supportedPositioningMethods.notify();
    emit supportedPositioningMethodsChanged();
}

void QDeclarativePositionSource::setPosition(const QGeoPositionInfo &pi)
{
    m_position.value()->setPosition(pi);
    m_position.notify();
    emit positionChanged();
}

void QDeclarativePositionSource::setSource(QGeoPositionInfoSource *source)
{
    if (m_positionSource)
        delete m_positionSource;

    if (!source) {
        m_positionSource = nullptr;
    } else {
        m_positionSource = source;
        connect(m_positionSource, &QGeoPositionInfoSource::supportedPositioningMethodsChanged,
                this, &QDeclarativePositionSource::notifySupportedPositioningMethodsChanged);
    }
}

bool QDeclarativePositionSource::parametersReady()
{
    for (const QDeclarativePluginParameter *p: qAsConst(m_parameters)) {
        if (!p->isInitialized())
            return false;
    }
    return true;
}

/*!
    \internal
*/
QVariantMap QDeclarativePositionSource::parameterMap() const
{
    QVariantMap map;

    for (int i = 0; i < m_parameters.size(); ++i) {
        QDeclarativePluginParameter *parameter = m_parameters.at(i);
        map.insert(parameter->name(), parameter->value());
    }

    return map;
}

/*!
    \internal
*/
void QDeclarativePositionSource::setUpdateInterval(int updateInterval)
{
    if (m_positionSource) {
        int previousUpdateInterval = m_positionSource->updateInterval();

        m_updateInterval = updateInterval;

        if (previousUpdateInterval != updateInterval) {
            m_positionSource->setUpdateInterval(updateInterval);
            if (previousUpdateInterval != m_positionSource->updateInterval())
                emit updateIntervalChanged();
        }
    } else {
        if (m_updateInterval != updateInterval) {
            m_updateInterval = updateInterval;
            emit updateIntervalChanged();
        }
    }
}

/*!
    \qmlproperty int PositionSource::updateInterval

    This property holds the desired interval between updates (milliseconds).

    \sa {QGeoPositionInfoSource::updateInterval()}
*/

int QDeclarativePositionSource::updateInterval() const
{
    if (!m_positionSource)
        return m_updateInterval;

    return m_positionSource->updateInterval();
}

/*!
    \qmlproperty enumeration PositionSource::supportedPositioningMethods

    This property holds the supported positioning methods of the
    current source.

    \list
    \li PositionSource.NoPositioningMethods - No positioning methods supported (no source).
    \li PositionSource.SatellitePositioningMethods - Satellite-based positioning methods such as GPS are supported.
    \li PositionSource.NonSatellitePositioningMethods - Non-satellite-based methods are supported.
    \li PositionSource.AllPositioningMethods - Both satellite-based and non-satellite positioning methods are supported.
    \endlist

*/

QDeclarativePositionSource::PositioningMethods
QDeclarativePositionSource::supportedPositioningMethods() const
{
    return m_supportedPositioningMethods.value();
}

QDeclarativePositionSource::PositioningMethods
QDeclarativePositionSource::supportedMethodsActualComputation() const
{
    if (m_positionSource) {
        return static_cast<QDeclarativePositionSource::PositioningMethods>(
            int(m_positionSource->supportedPositioningMethods()));
    }
    return QDeclarativePositionSource::NoPositioningMethods;
}

QBindable<QDeclarativePositionSource::PositioningMethods>
QDeclarativePositionSource::bindableSupportedPositioningMethods() const
{
    return QBindable<PositioningMethods>(&m_supportedPositioningMethods);
}

/*!
    \qmlproperty enumeration PositionSource::preferredPositioningMethods

    This property holds the preferred positioning methods of the
    current source.

    \list
    \li PositionSource.NoPositioningMethods - No positioning method is preferred.
    \li PositionSource.SatellitePositioningMethods - Satellite-based positioning methods such as GPS should be preferred.
    \li PositionSource.NonSatellitePositioningMethods - Non-satellite-based methods should be preferred.
    \li PositionSource.AllPositioningMethods - Any positioning methods are acceptable.
    \endlist

*/

void QDeclarativePositionSource::setPreferredPositioningMethods(PositioningMethods methods)
{
    if (m_positionSource) {
        PositioningMethods previousPreferredPositioningMethods = preferredPositioningMethods();

        m_preferredPositioningMethods = methods;

        if (previousPreferredPositioningMethods != methods) {
            m_positionSource->setPreferredPositioningMethods(
                static_cast<QGeoPositionInfoSource::PositioningMethods>(int(methods)));
            if (previousPreferredPositioningMethods != m_positionSource->preferredPositioningMethods())
                emit preferredPositioningMethodsChanged();
        }
    } else {
        if (m_preferredPositioningMethods != methods) {
            m_preferredPositioningMethods = methods;
            emit preferredPositioningMethodsChanged();
        }
    }
}

QDeclarativePositionSource::PositioningMethods QDeclarativePositionSource::preferredPositioningMethods() const
{
    if (m_positionSource) {
        return static_cast<QDeclarativePositionSource::PositioningMethods>(
            int(m_positionSource->preferredPositioningMethods()));
    }
    return m_preferredPositioningMethods;
}

/*!
    \qmlmethod PositionSource::start()

    Requests updates from the location source.
    Uses \l updateInterval if set, default interval otherwise.
    If there is no source available, this method has no effect.

    \sa stop, update, active
*/

void QDeclarativePositionSource::start()
{
    if (m_positionSource) {
        m_positionSource->startUpdates();

        m_regularUpdates = true;
        if (!m_active) {
            m_active = true;
            emit activeChanged();
        }
    }
}

/*!
    \qmlmethod PositionSource::update(int timeout)

    A convenience method to request single update from the location source.
    If there is no source available, this method has no effect.

    If the position source is not active, it will be activated for as
    long as it takes to receive an update, or until the request times
    out.  The request timeout period is source-specific.

    The \a timeout is specified in milliseconds. If the \a timeout is zero
    (the default value), it defaults to a reasonable timeout period as
    appropriate for the source.

    \sa start, stop, active
*/

void QDeclarativePositionSource::update(int timeout)
{
    if (m_positionSource) {
        m_singleUpdate = true;
        if (!m_active) {
            m_active = true;
            emit activeChanged();
        }
        // Use default timeout value. Set active before calling the
        // update request because on some platforms there may
        // be results immediately.
        m_positionSource->requestUpdate(timeout);
    }
}

/*!
    \qmlmethod PositionSource::stop()

    Stops updates from the location source.
    If there is no source available or it is not active,
    this method has no effect.

    \sa start, update, active
*/

void QDeclarativePositionSource::stop()
{
    if (m_positionSource) {
        m_positionSource->stopUpdates();
        m_regularUpdates = false;
        if (m_active && !m_singleUpdate) {
            m_active = false;
            emit activeChanged();
        }
    }
}

/*!
    \qmlproperty bool PositionSource::active

    This property indicates whether the position source is active.
    Setting this property to false equals calling \l stop, and
    setting this property true equals calling \l start.

    \sa start, stop, update
*/
void QDeclarativePositionSource::setActive(bool active)
{
    if (active == m_active)
        return;

    if (active)
        QTimer::singleShot(0, this, SLOT(start())); // delay ensures all properties have been set
    else
        stop();
}

bool QDeclarativePositionSource::isActive() const
{
    return m_active;
}

/*!
    \qmlproperty Position PositionSource::position

    This property holds the last known positional data.
    It is a read-only property.

    The Position type has different positional member variables,
    whose validity can be checked with appropriate validity functions
    (for example sometimes an update does not have speed or altitude data).

    However, whenever a \c {positionChanged} signal has been received, at least
    position::coordinate::latitude, position::coordinate::longitude, and position::timestamp can
    be assumed to be valid.

    \sa start, stop, update
*/

QDeclarativePosition *QDeclarativePositionSource::position()
{
    return m_position.value();
}

void QDeclarativePositionSource::positionUpdateReceived(const QGeoPositionInfo &update)
{
    setPosition(update);

    if (m_singleUpdate && m_active) {
        // we need to reset m_singleUpdate because we got one
        m_singleUpdate = false;
        if (!m_regularUpdates) {
            // but we change the active state only if the regular updates are
            // also stopped
            m_active = false;
            emit activeChanged();
        }
    }
}


/*!
    \qmlproperty enumeration PositionSource::sourceError

    This property holds the error which last occurred with the PositionSource.

    \list
    \li PositionSource.AccessError - The connection setup to the remote positioning backend failed because the
        application lacked the required privileges.
    \li PositionSource.ClosedError - The positioning backend closed the connection, which happens for example in case
        the user is switching location services to off. As soon as the location service is re-enabled
        regular updates will resume.
    \li PositionSource.NoError - No error has occurred.
    \li PositionSource.UnknownSourceError - An unidentified error occurred.
    \li PositionSource.UpdateTimeoutError - The current position could not be
        retrieved within the specified timeout, or this PositionSource determined
        that it will not be able to provide further regular updates.
    \endlist

*/

QDeclarativePositionSource::SourceError QDeclarativePositionSource::sourceError() const
{
    return m_sourceError;
}

QBindable<QDeclarativePositionSource::SourceError>
QDeclarativePositionSource::bindableSourceError() const
{
    return QBindable<QDeclarativePositionSource::SourceError>(&m_sourceError);
}

QGeoPositionInfoSource *QDeclarativePositionSource::positionSource() const
{
    return m_positionSource;
}

/*!
    \qmlproperty list<PluginParameter> PositionSource::parameters
    \qmldefault

    This property holds the list of plugin parameters.

    \since QtPositioning 5.14
*/
QQmlListProperty<QDeclarativePluginParameter> QDeclarativePositionSource::parameters()
{
    return QQmlListProperty<QDeclarativePluginParameter>(this,
            0,
            parameter_append,
            parameter_count,
            parameter_at,
            parameter_clear);
}

/*!
    \internal
*/
void QDeclarativePositionSource::parameter_append(QQmlListProperty<QDeclarativePluginParameter> *prop, QDeclarativePluginParameter *parameter)
{
    QDeclarativePositionSource *p = static_cast<QDeclarativePositionSource *>(prop->object);
    p->m_parameters.append(parameter);
}

/*!
    \internal
*/
qsizetype QDeclarativePositionSource::parameter_count(QQmlListProperty<QDeclarativePluginParameter> *prop)
{
    return static_cast<QDeclarativePositionSource *>(prop->object)->m_parameters.count();
}

/*!
    \internal
*/
QDeclarativePluginParameter *QDeclarativePositionSource::parameter_at(QQmlListProperty<QDeclarativePluginParameter> *prop, qsizetype index)
{
    return static_cast<QDeclarativePositionSource *>(prop->object)->m_parameters[index];
}

/*!
    \internal
*/
void QDeclarativePositionSource::parameter_clear(QQmlListProperty<QDeclarativePluginParameter> *prop)
{
    QDeclarativePositionSource *p = static_cast<QDeclarativePositionSource *>(prop->object);
    p->m_parameters.clear();
}

void QDeclarativePositionSource::componentComplete()
{
    m_componentComplete = true;
    m_parametersInitialized = true;
    for (QDeclarativePluginParameter *p: qAsConst(m_parameters)) {
        if (!p->isInitialized()) {
            m_parametersInitialized = false;
            connect(p, &QDeclarativePluginParameter::initialized,
                    this, &QDeclarativePositionSource::onParameterInitialized);
        }
    }

    if (m_parametersInitialized)
        tryAttach(m_sourceName.value());
}

/*!
     \qmlmethod bool PositionSource::setBackendProperty(string name, Variant value)

    Sets the backend-specific property named \a name to \a value.
    Returns true on success, false otherwise, including if called on an uninitialized PositionSource.
    Supported backend-specific properties are listed and described in
    \l {Qt Positioning plugins#Default plugins}.

    \since Qt Positioning 5.14

    \sa backendProperty, QGeoPositionInfoSource::setBackendProperty
*/
bool QDeclarativePositionSource::setBackendProperty(const QString &name, const QVariant &value)
{
    if (m_positionSource)
        return m_positionSource->setBackendProperty(name, value);
    return false;
}

/*!
     \qmlmethod Variant PositionSource::backendProperty(string name)

    Returns the value of the backend-specific property named \a name, if present.
    Otherwise, including if called on an uninitialized PositionSource, the return value will be invalid.
    Supported backend-specific properties are listed and described in
    \l {Qt Positioning plugins#Default plugins}.

    \since Qt Positioning 5.14

    \sa backendProperty, QGeoPositionInfoSource::setBackendProperty
*/
QVariant QDeclarativePositionSource::backendProperty(const QString &name) const
{
    if (m_positionSource)
        return m_positionSource->backendProperty(name);
    return QVariant();
}

/*!
    \internal
*/
void QDeclarativePositionSource::sourceErrorReceived(const QGeoPositionInfoSource::Error error)
{
    if (error == QGeoPositionInfoSource::AccessError)
        m_sourceError.setValueBypassingBindings(QDeclarativePositionSource::AccessError);
    else if (error == QGeoPositionInfoSource::ClosedError)
        m_sourceError.setValueBypassingBindings(QDeclarativePositionSource::ClosedError);
    else if (error == QGeoPositionInfoSource::UpdateTimeoutError)
        handleUpdateTimeout(); // also sets m_sourceError
    else if (error == QGeoPositionInfoSource::NoError)
        return; //nothing to do
    else
        m_sourceError.setValueBypassingBindings(QDeclarativePositionSource::UnknownSourceError);

    m_sourceError.notify();
    emit sourceErrorChanged();
}

QT_END_NAMESPACE
