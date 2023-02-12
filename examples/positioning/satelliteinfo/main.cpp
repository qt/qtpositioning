// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QtGui/qguiapplication.h>
#include <QtQml/qqmlapplicationengine.h>
#if QT_CONFIG(permissions)
#include <QtCore/qpermissions.h>
#endif

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed, &app,
                     []() { QCoreApplication::exit(1); }, Qt::QueuedConnection);

#if QT_CONFIG(permissions)
    // Check and request location permission if needed. The view is loaded even if the
    // the permission is denied; user can later navigate to system settings to enable the
    // permissions
    QLocationPermission permission;
    permission.setAccuracy(QLocationPermission::Precise);
    permission.setAvailability(QLocationPermission::WhenInUse);
    if (qApp->checkPermission(permission) == Qt::PermissionStatus::Undetermined) {
        qApp->requestPermission(permission, [&engine] (const QPermission& permission) {
            if (permission.status() != Qt::PermissionStatus::Granted)
                qWarning("Location permission not granted, app will run in simulation mode");
            engine.loadFromModule("SatelliteInformation", "Main");
        });
    } else {
        engine.loadFromModule("SatelliteInformation", "Main");
    }
#else
    engine.loadFromModule("SatelliteInformation", "Main");
#endif
    return app.exec();
}
