/**
 * MIT License
 *
 * Copyright (c) 2020 Luca Carlon
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 **/

#include <QMutex>

#include "../deps/lqtutils/lqtutils_autoexec.h"

#include "lserializer.h"

Q_LOGGING_CATEGORY(lserializer, "lserializer")

LSerializer::LSerializer()
{}

QJsonValue LSerializer::serializeObject(const void* object, const QMetaObject* metaObj)
{
    QJsonObject json;
    bool isGadget = !metaObj->inherits(&QObject::staticMetaObject);

    for (int i = 0; i < metaObj->propertyCount(); ++i) {
        QMetaProperty metaProp = metaObj->property(i);
        QVariant value;
        if (isGadget)
            value = metaProp.readOnGadget(object);
        else
            value = metaProp.read(reinterpret_cast<const QObject*>(object));

        // This is the case of objectName. Only add it to the json if it is not empty.
        if (metaProp.enclosingMetaObject() == &QObject::staticMetaObject && value.toString().isEmpty())
            continue;

        QJsonValue jsonValue = serializeValue(value);
        json[metaProp.name()] = jsonValue;
    }

    return json;
}

QJsonArray LSerializer::serializeArray(const QSequentialIterable& it)
{
    QJsonArray ret;
    for (const QVariant& variant: it)
        ret.append(serializeValue(variant));
    return ret;
}

QJsonValue LSerializer::serializeValue(const QVariant& value)
{
    if (value.isNull())
        return QJsonValue::Undefined;

    QMetaType metaType(value.userType());
    switch (metaType.id()) {
    case QMetaType::QVariantList:
        return serializeArray(value.value<QSequentialIterable>());
    case QMetaType::QVariant:
        // Try to convert.
        break;
    case QMetaType::QVariantHash:
        return serializeDictionary(value.toHash());
    case QMetaType::QVariantMap:
        return serializeDictionary(value.toMap());
    case QMetaType::QString:
        if (value.toString().isNull())
            return QJsonValue::Undefined;
        return QJsonValue(value.toString());
    case QMetaType::Int:
    case QMetaType::UInt:
    case QMetaType::Long:
    case QMetaType::LongLong:
    case QMetaType::Float:
    case QMetaType::Double:
    case QMetaType::Short:
    case QMetaType::ULong:
    case QMetaType::ULongLong:
    case QMetaType::UShort:
        return QJsonValue(value.toDouble());
    case QMetaType::Bool:
        return QJsonValue(value.toBool());
    case QMetaType::QObjectStar:
        if (!value.value<QObject*>())
            return QJsonValue::Null;
        else {
            QObject* obj = value.value<QObject*>();
            return serializeObject(obj, obj->metaObject());
        }
    default:
        if (metaType.flags().testFlag(QMetaType::PointerToQObject)) {
            if (!value.value<QObject*>())
                return QJsonValue::Null;
            else {
                QObject* obj = value.value<QObject*>();
                return serializeObject(obj, obj->metaObject());
            }
        }

        if (metaType.flags().testFlag(QMetaType::PointerToGadget)) {
            // NOTE: this could probably be improved.
            uintptr_t pgadget;
            metaType.construct(&pgadget, value.constData());
            void* gadget = reinterpret_cast<void*>(pgadget);
            lqt::AutoExec exec([&metaType, gadget] {
                metaType.destruct(gadget);
            });
            if (!gadget)
                return QJsonValue::Null;
            else
                return serializeObject(gadget, metaType.metaObject());
        }

        break;
    }

    if (value.canConvert<QVariantList>())
        return serializeArray(value.value<QSequentialIterable>());
    if (value.canConvert<QVariantHash>())
        return serializeDictionary(value.value<QVariantHash>());
    if (value.canConvert<QVariantMap>())
        return serializeDictionary(value.value<QVariantMap>());
    if (value.canConvert<QString>())
        return QJsonValue(value.toString());

    qCDebug(lserializer) << "Unable to serialize type:" << metaType.name();
    return QJsonValue();
}
