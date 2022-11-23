// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qdeclarativesatellitesource_p.h"

QT_BEGIN_NAMESPACE

QDeclarativeSatelliteSource::QDeclarativeSatelliteSource()
    : m_active(0), m_componentComplete(0), m_parametersInitialized(0),
      m_startRequested(0), m_defaultSourceUsed(0), m_regularUpdates(0),
      m_singleUpdate(0), m_singleUpdateRequested(0)
{
}

QDeclarativeSatelliteSource::~QDeclarativeSatelliteSource() = default;

bool QDeclarativeSatelliteSource::isActive() const
{
    return m_active;
}

bool QDeclarativeSatelliteSource::isValid() const
{
    return m_source != nullptr;
}

int QDeclarativeSatelliteSource::updateInterval() const
{
    return m_source ? m_source->updateInterval() : m_updateInterval;
}

QDeclarativeSatelliteSource::SourceError QDeclarativeSatelliteSource::sourceError() const
{
    return m_error;
}

QString QDeclarativeSatelliteSource::name() const
{
    return m_source ? m_source->sourceName() : m_name;
}

QQmlListProperty<QDeclarativePluginParameter> QDeclarativeSatelliteSource::parameters()
{
    return QQmlListProperty<QDeclarativePluginParameter>(this, nullptr,
                                                         parameter_append,
                                                         parameter_count,
                                                         parameter_at,
                                                         parameter_clear);
}

QList<QGeoSatelliteInfo> QDeclarativeSatelliteSource::satellitesInUse() const
{
    return m_satellitesInUse;
}

QList<QGeoSatelliteInfo> QDeclarativeSatelliteSource::satellitesInView() const
{
    return m_satellitesInView;
}

void QDeclarativeSatelliteSource::setUpdateInterval(int updateInterval)
{
    if (m_updateInterval == updateInterval)
        return;

    const auto oldInterval = m_updateInterval;

    if (m_source) {
        m_source->setUpdateInterval(updateInterval);
        // The above call can set some other interval, for example if desired
        // updateInterval is less than minimum supported update interval. So
        // we need to read the value back explicitly.
        m_updateInterval = m_source->updateInterval();
    } else {
        m_updateInterval = updateInterval;
    }
    if (oldInterval != m_updateInterval)
        emit updateIntervalChanged();
}

void QDeclarativeSatelliteSource::setActive(bool active)
{
    if (active == m_active)
        return;

    if (active)
        start();
    else
        stop();
}

void QDeclarativeSatelliteSource::setName(const QString &name)
{
    if ((m_name == name) || (name.isEmpty() && m_defaultSourceUsed))
        return;

    if (m_componentComplete && m_parametersInitialized) {
        createSource(name); // it will update name and emit, if needed
    } else {
        m_name = name;
        emit nameChanged();
    }
}

void QDeclarativeSatelliteSource::componentComplete()
{
    m_componentComplete = true;
    m_parametersInitialized = true;
    for (QDeclarativePluginParameter *p: std::as_const(m_parameters)) {
        if (!p->isInitialized()) {
            m_parametersInitialized = false;
            connect(p, &QDeclarativePluginParameter::initialized,
                    this, &QDeclarativeSatelliteSource::onParameterInitialized,
                    Qt::SingleShotConnection);
        }
    }

    if (m_parametersInitialized)
        createSource(m_name);
}

bool QDeclarativeSatelliteSource::setBackendProperty(const QString &name, const QVariant &value)
{
    if (m_source)
        return m_source->setBackendProperty(name, value);
    return false;
}

QVariant QDeclarativeSatelliteSource::backendProperty(const QString &name) const
{
    return m_source ? m_source->backendProperty(name) : QVariant{};
}

void QDeclarativeSatelliteSource::update(int timeout)
{
    if (m_componentComplete && m_parametersInitialized) {
        executeSingleUpdate(timeout);
    } else {
        m_singleUpdateDesiredTimeout = timeout;
        m_singleUpdateRequested = true;
    }
}

void QDeclarativeSatelliteSource::start()
{
    if (m_componentComplete && m_parametersInitialized)
        executeStart();
    else
        m_startRequested = true;
}

void QDeclarativeSatelliteSource::stop()
{
    if (m_source) {
        m_source->stopUpdates();
        m_regularUpdates = false;

        if (m_active && !m_singleUpdate) {
            m_active = false;
            emit activeChanged();
        }
    }
}

void QDeclarativeSatelliteSource::sourceErrorReceived(const QGeoSatelliteInfoSource::Error error)
{
    const auto oldError = m_error;
    m_error = static_cast<SourceError>(error);
    if (m_error != oldError)
        emit sourceErrorChanged();

    // if an error occurred during single update, the update is stopped, so we
    // need to change the active state.
    if (m_singleUpdate) {
        m_singleUpdate = false;
        if (m_active && !m_regularUpdates) {
            m_active = false;
            emit activeChanged();
        }
    }
}

void QDeclarativeSatelliteSource::onParameterInitialized()
{
    m_parametersInitialized = true;
    for (QDeclarativePluginParameter *p: std::as_const(m_parameters)) {
        if (!p->isInitialized()) {
            m_parametersInitialized = false;
            break;
        }
    }

    // m_componentComplete == true here
    if (m_parametersInitialized)
        createSource(m_name);
}

void QDeclarativeSatelliteSource::satellitesInViewUpdateReceived(const QList<QGeoSatelliteInfo> &satellites)
{
    m_satellitesInView = satellites;
    emit satellitesInViewChanged();
    handleSingleUpdateReceived();
}

void QDeclarativeSatelliteSource::satellitesInUseUpdateReceived(const QList<QGeoSatelliteInfo> &satellites)
{
    m_satellitesInUse = satellites;
    emit satellitesInUseChanged();
    handleSingleUpdateReceived();
}

QVariantMap QDeclarativeSatelliteSource::parameterMap() const
{
    QVariantMap map;
    for (const auto *parameter : std::as_const(m_parameters))
        map.insert(parameter->name(), parameter->value());
    return map;
}

void QDeclarativeSatelliteSource::createSource(const QString &newName)
{
    if (m_source && m_source->sourceName() == newName)
        return;

    const auto oldName = name();
    const bool oldIsValid = isValid();
    const bool oldActive = isActive();
    const auto oldUpdateInterval = updateInterval();

    if (m_source) {
        m_source->disconnect(this);
        m_source->stopUpdates();
        m_source.reset(nullptr);
        m_active = false;
    }

    if (!newName.isEmpty()) {
        m_source.reset(QGeoSatelliteInfoSource::createSource(newName, parameterMap(), nullptr));
        m_defaultSourceUsed = false;
    } else {
        m_source.reset(QGeoSatelliteInfoSource::createDefaultSource(parameterMap(), nullptr));
        m_defaultSourceUsed = true;
    }

    if (m_source) {
        connect(m_source.get(), &QGeoSatelliteInfoSource::errorOccurred,
                this, &QDeclarativeSatelliteSource::sourceErrorReceived);
        connect(m_source.get(), &QGeoSatelliteInfoSource::satellitesInViewUpdated,
                this, &QDeclarativeSatelliteSource::satellitesInViewUpdateReceived);
        connect(m_source.get(), &QGeoSatelliteInfoSource::satellitesInUseUpdated,
                this, &QDeclarativeSatelliteSource::satellitesInUseUpdateReceived);

        m_name = m_source->sourceName();
        m_source->setUpdateInterval(m_updateInterval);
        m_updateInterval = m_source->updateInterval();
    } else {
        m_name = newName;
        m_defaultSourceUsed = false;
    }

    if (oldName != name())
        emit nameChanged();

    if (oldIsValid != isValid())
        emit validityChanged();

    if (oldActive != isActive())
        emit activeChanged();

    if (oldUpdateInterval != updateInterval())
        emit updateIntervalChanged();

    if (m_startRequested) {
        m_startRequested = false;
        executeStart();
    }
    if (m_singleUpdateRequested) {
        m_singleUpdateRequested = false;
        executeSingleUpdate(m_singleUpdateDesiredTimeout);
    }
}

void QDeclarativeSatelliteSource::handleSingleUpdateReceived()
{
    if (m_singleUpdate) {
        m_singleUpdate = false;
        if (m_active && !m_regularUpdates) {
            m_active = false;
            emit activeChanged();
        }
    }
}

void QDeclarativeSatelliteSource::executeStart()
{
    if (m_source) {
        m_source->startUpdates();
        m_regularUpdates = true;

        if (!m_active) {
            m_active = true;
            emit activeChanged();
        }
    }
}

void QDeclarativeSatelliteSource::executeSingleUpdate(int timeout)
{
    if (m_source) {
        m_singleUpdate = true;
        m_source->requestUpdate(timeout);
        if (!m_active) {
            m_active = true;
            emit activeChanged();
        }
    }
}

void QDeclarativeSatelliteSource::parameter_append(PluginParameterProperty *prop,
                                                   QDeclarativePluginParameter *parameter)
{
    auto *src = static_cast<QDeclarativeSatelliteSource *>(prop->object);
    src->m_parameters.append(parameter);
}

qsizetype QDeclarativeSatelliteSource::parameter_count(PluginParameterProperty *prop)
{
    return static_cast<QDeclarativeSatelliteSource *>(prop->object)->m_parameters.size();
}

QDeclarativePluginParameter *
QDeclarativeSatelliteSource::parameter_at(PluginParameterProperty *prop, qsizetype index)
{
    return static_cast<QDeclarativeSatelliteSource *>(prop->object)->m_parameters[index];
}

void QDeclarativeSatelliteSource::parameter_clear(PluginParameterProperty *prop)
{
    auto *src = static_cast<QDeclarativeSatelliteSource *>(prop->object);
    src->m_parameters.clear();
}

QT_END_NAMESPACE
