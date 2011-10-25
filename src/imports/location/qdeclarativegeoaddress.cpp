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
***************************************************************************/

#include "qdeclarativegeoaddress_p.h"

QT_BEGIN_NAMESPACE

/*!
    \qmlclass Address QDeclarativeGeoAddress
    \inqmlmodule QtLocation 5
    \ingroup qml-QtLocation5-positioning
    \since QtLocation 5.0

    \brief The Address element presents an address.

    The Address element presents an address of a location.

    \sa {QGeoAddress}
*/

QDeclarativeGeoAddress::QDeclarativeGeoAddress(QObject* parent) :
        QObject(parent)
{
}

QDeclarativeGeoAddress::QDeclarativeGeoAddress(const QGeoAddress& address, QObject* parent) :
        QObject(parent), m_address(address)
{
}

QGeoAddress QDeclarativeGeoAddress::address() const
{
    return m_address;
}

void QDeclarativeGeoAddress::setAddress(const QGeoAddress& address)
{
    // Elaborate but takes care of emiting needed signals
    setCountry(address.country());
    setCountryCode(address.countryCode());
    setState(address.state());
    setCounty(address.county());
    setCity(address.city());
    setDistrict(address.district());
    setStreet(address.street());
    setPostcode(address.postcode());
    m_address = address;
}

QString QDeclarativeGeoAddress::country() const
{
    return m_address.country();
}

/*!
  \qmlproperty string Address::country

  This property holds the country of the address.

  */

void QDeclarativeGeoAddress::setCountry(const QString& country)
{
    if (m_address.country() == country)
        return;
    m_address.setCountry(country);
    emit countryChanged();
}

QString QDeclarativeGeoAddress::countryCode() const
{
    return m_address.countryCode();
}

/*!
  \qmlproperty string Address::countryCode

  This property holds the country code of the address.

  */

void QDeclarativeGeoAddress::setCountryCode(const QString& countryCode)
{
    if (m_address.countryCode() == countryCode)
        return;
    m_address.setCountryCode(countryCode);
    emit countryCodeChanged();
}

QString QDeclarativeGeoAddress::state() const
{
    return m_address.state();
}

/*!
  \qmlproperty string Address::state

  This property holds the state of the address.

  */

void QDeclarativeGeoAddress::setState(const QString& state)
{
    if (m_address.state() == state)
        return;
    m_address.setState(state);
    emit stateChanged();
}

QString QDeclarativeGeoAddress::county() const
{
    return m_address.county();
}

/*!
  \qmlproperty string Address::county

  This property holds the county of the address.

  */

void QDeclarativeGeoAddress::setCounty(const QString& county)
{
    if (m_address.county() == county)
        return;
    m_address.setCounty(county);
    emit countyChanged();
}

QString QDeclarativeGeoAddress::city() const
{
    return m_address.city();
}

/*!
  \qmlproperty string Address::city

  This property holds the city of the address.

  */

void QDeclarativeGeoAddress::setCity(const QString& city)
{
    if (m_address.city() == city)
        return;
    m_address.setCity(city);
    emit cityChanged();
}

QString QDeclarativeGeoAddress::district() const
{
    return m_address.district();
}

/*!
  \qmlproperty string Address::district

  This property holds the district of the address.

  */

void QDeclarativeGeoAddress::setDistrict(const QString& district)
{
    if (m_address.district() == district)
        return;
    m_address.setDistrict(district);
    emit districtChanged();
}

QString QDeclarativeGeoAddress::street() const
{
    return m_address.street();
}

/*!
  \qmlproperty string Address::street

  This property holds the street of the address.

  */

void QDeclarativeGeoAddress::setStreet(const QString& street)
{
    if (m_address.street() == street)
        return;
    m_address.setStreet(street);
    emit streetChanged();
}

QString QDeclarativeGeoAddress::postcode() const
{
    return m_address.postcode();
}

/*!
  \qmlproperty string Address::postcode

  This property holds the post code of the address.

  */

void QDeclarativeGeoAddress::setPostcode(const QString& postcode)
{
    if (m_address.postcode() == postcode)
        return;
    m_address.setPostcode(postcode);
    emit postcodeChanged();
}

#include "moc_qdeclarativegeoaddress_p.cpp"

QT_END_NAMESPACE
