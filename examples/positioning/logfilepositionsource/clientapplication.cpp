// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause
#include <QtCore>
#include <QTextEdit>

#include "logfilepositionsource.h"
#include "clientapplication.h"

ClientApplication::ClientApplication(QWidget *parent)
    : QMainWindow(parent)
{
    textEdit = new QTextEdit;
    setCentralWidget(textEdit);

    LogFilePositionSource *source = new LogFilePositionSource(this);
    connect(source, SIGNAL(positionUpdated(QGeoPositionInfo)),
            this, SLOT(positionUpdated(QGeoPositionInfo)));

    source->startUpdates();
}

void ClientApplication::positionUpdated(const QGeoPositionInfo &info)
{
    textEdit->append(QString("Position updated: Date/time = %1, Coordinate = %2").arg(info.timestamp().toString()).arg(info.coordinate().toString()));
}
