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

#include "lserializer.h"

Q_LOGGING_CATEGORY(lserializer, "lserializer")

LSerializer::LSerializer()
{}

QJsonValue LSerializer::serializeObject(const void* object, const QMetaObject* metaObj)
{
    QJsonObject json;
    bool isGadget = !metaObj->inherits(&QObject::staticMetaObject);

    for (int i = metaObj->propertyOffset(); i < metaObj->propertyCount(); ++i) {
        QMetaProperty metaProp = metaObj->property(i);
        QVariant value;
        if (isGadget)
            value = metaProp.readOnGadget(object);
        else
            value = metaProp.read(reinterpret_cast<const QObject*>(object));
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
    // TODO: Check.
    switch (metaType.id()) {
    case QMetaType::QVariantList:
        return serializeArray(value.value<QSequentialIterable>());
    case QMetaType::QString:
        if (value.toString().isNull())
            return QJsonValue::Undefined;
        return QJsonValue(value.toString());
    case QMetaType::Int:
    case QMetaType::UInt:
    case QMetaType::Long:
    case QMetaType::LongLong:
        return QJsonValue(value.toDouble());
    case QMetaType::Bool:
        return QJsonValue(value.toBool());
    case QMetaType::Float:
    case QMetaType::Double:
        return QJsonValue(value.toDouble());
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
            // TODO: leak
            uintptr_t pgadget;
            QMetaType(value.userType()).construct(&pgadget, value.constData());
            const void* gadget = reinterpret_cast<void*>(pgadget);
            if (!gadget)
                return QJsonValue::Null;
            else
                return serializeObject(gadget, metaType.metaObject());
        }

        if (value.canConvert<QVariantList>())
            return serializeArray(value.value<QSequentialIterable>());

        const QMetaObject* metaObject = metaType.metaObject();
        if (!metaObject)
            break;
        break;
    }

    return QJsonValue();
}
