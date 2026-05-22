// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOHOSJSUTILS_P_H
#define QOHOSJSUTILS_P_H

#include <QtCore/private/qcore_ohos_p.h>
#include <QtCore/private/qnapi_p.h>
#include <QtCore/qglobal.h>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>

QT_BEGIN_NAMESPACE

namespace QtOhos {

struct OnOffMethodsBasedEventHandlerOptions
{
    std::function<bool(QNapi::Object)> optEventSourceAliveCheckFunc;
    std::optional<QNapi::ValueWrapper> extraOnArg;
    std::optional<QNapi::ValueWrapper> extraOffArg;
};

std::shared_ptr<void> registerOnOffMethodsBasedEventHandler(
    QNapi::Object eventSourceObject, const std::string &eventTypeName,
    QNapi::CallbackFuncWrapper handler, OnOffMethodsBasedEventHandlerOptions options = {});

}

QT_END_NAMESPACE

#endif // QOHOSJSUTILS_P_H
