// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QtCore/private/qohoscommon_p.h>
#include <QtCore/private/qohoslogger_p.h>
#include "qohosjsutils.h"

QT_BEGIN_NAMESPACE

namespace QtOhos {

std::shared_ptr<void> registerOnOffMethodsBasedEventHandler(
    QNapi::Object eventSourceObject, const std::string &eventTypeName,
    QNapi::CallbackFuncWrapper eventHandler, OnOffMethodsBasedEventHandlerOptions options)
{
    struct Context
    {
        std::function<QNapi::Value(const QOhosCallbackInfo &)> eventHandler;
        std::function<bool(QNapi::Object)> eventSourceAliveCheckFunc;
        QNapi::Reference<QNapi::Value> optExtraOnArg;
        QNapi::Reference<QNapi::Value> optExtraOffArg;
    };

    auto env = eventSourceObject.Env();

    auto sharedContext = moveToSharedPtr(
        Context{
            .eventHandler = std::move(eventHandler.callbackFunc()),
            .eventSourceAliveCheckFunc = options.optEventSourceAliveCheckFunc
                ? std::move(options.optEventSourceAliveCheckFunc)
                : [](QNapi::Object) {
                    return true;
                },
            .optExtraOnArg = options.extraOnArg.has_value()
                ? QNapi::Reference<>::makePersistentFrom(
                    options.extraOnArg.value().mapToValue(env))
                : QNapi::Reference<>::makeEmpty(),
            .optExtraOffArg = options.extraOffArg.has_value()
                ? QNapi::Reference<>::makePersistentFrom(
                    options.extraOffArg.value().mapToValue(env))
                : QNapi::Reference<>::makeEmpty(),
        });

    auto jsEventHandlerRef = moveToSharedPtr(
        QNapi::Reference<>::makePersistentFrom(
            QNapi::Function::New(
                eventSourceObject.Env(),
                [eventTypeName, weakContext = makeWeakPtr(sharedContext)](const QOhosCallbackInfo &cbInfo) {
                    auto sharedContext = weakContext.lock();
                    if (sharedContext) {
                        return sharedContext->eventHandler(cbInfo);
                    } else {
                        qOhosPrintfWarning(
                            "%s: got unexpected '%s' event callback call for detached handler",
                            Q_FUNC_INFO, eventTypeName.c_str());
                        return cbInfo.Env().Undefined();
                    }
                })));

    std::vector<QNapi::ValueWrapper> onCallArgs;
    onCallArgs.push_back(eventTypeName);
    if (!sharedContext->optExtraOnArg.IsEmpty())
        onCallArgs.push_back(sharedContext->optExtraOnArg.Value());
    onCallArgs.push_back(jsEventHandlerRef->Value());
    eventSourceObject.call("on", onCallArgs);

    auto eventSourceWeakRef = moveToSharedPtr(Napi::Weak(eventSourceObject));

    return makeProxyWithJsThreadDeleter(
        QtOhos::makeDestroyNotifier(
        [eventSourceWeakRef, eventTypeName, sharedContext, jsEventHandlerRef]() {
            auto eventSourceValue = eventSourceWeakRef->Value();
            if (eventSourceValue.IsObject()) {
                auto eventSourceObject = QNapi::checkedCast<QNapi::Object>(eventSourceValue);
                if (sharedContext->eventSourceAliveCheckFunc(eventSourceObject)) {
                    try {
                        std::vector<QNapi::ValueWrapper> offCallArgs;
                        offCallArgs.push_back(eventTypeName);
                        if (!sharedContext->optExtraOffArg.IsEmpty())
                            offCallArgs.push_back(sharedContext->optExtraOffArg.Value());

                        offCallArgs.push_back(jsEventHandlerRef->Value());
                        eventSourceObject.call("off", offCallArgs);
                    } catch (const Napi::Error &e) {
                        qOhosPrintfError(
                            "%s: got exception from off(%s, ...) call (ignoring): %s",
                            Q_FUNC_INFO, eventTypeName.c_str(), e.what());
                    }
                } else {
                    qOhosPrintfDebug(
                        "%s: not calling off(%s, ...), event source 'considered' not alive",
                        Q_FUNC_INFO, eventTypeName.c_str());
                }
            } else {
                qOhosPrintfDebug(
                    "%s: not calling off(%s, ...), event source not alive",
                    Q_FUNC_INFO, eventTypeName.c_str());
            }
        }));
}

}

QT_END_NAMESPACE
