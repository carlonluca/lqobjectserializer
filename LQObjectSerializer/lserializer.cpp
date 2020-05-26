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

#include "lserializer.h"

Q_LOGGING_CATEGORY(lserializer, "lserializer")

LSerializer::LSerializer()
{

}

QJsonObject LSerializer::serialize(QObject* object)
{
    return !object ? QJsonObject() : serializeObject(object).toObject();
}

QJsonValue LSerializer::serializeObject(QObject* object)
{
    QJsonObject json;

    const QMetaObject* metaObj = object->metaObject();
    for (int i = 0; i < metaObj->propertyCount(); ++i) {
        const char* propertyName = metaObj->property(i).name();
        QVariant value = object->property(propertyName);
        QJsonValue jsonValue = serializeValue(value);
        json[propertyName] = jsonValue;
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
    if (value.canConvert<QVariantList>())
        return serializeArray(value.value<QSequentialIterable>());
    else if (value.canConvert<QObject*>()) {
        if (!value.value<QObject*>())
            return QJsonValue::Null;
        else
            return serializeObject(value.value<QObject*>());
    }
    else {
        switch (static_cast<QMetaType::Type>(value.type())) {
        case QMetaType::QString:
            return QJsonValue(value.value<QString>());
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
        default:
            break;
        }
    }

    return QJsonValue();
}
