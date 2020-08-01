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

#ifndef LCSERIALIZER_H
#define LCSERIALIZER_H

#include <QByteArray>
#include <QVariant>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QObject>
#include <QMetaObject>
#include <QMetaProperty>
#include <QRegularExpression>
#include <QLoggingCategory>
#ifdef QT_DEBUG
#include <QDebug>
#endif

#include "../deps/lqtutils/lqtutils_prop.h"

Q_DECLARE_LOGGING_CATEGORY(lserializer)

#define L_RW_PROP_ARRAY_WITH_ADDER(type, name, setter)                                 \
    L_RW_PROP3(QList<type>, name, setter)                                              \
    public:                                                                            \
    Q_INVOKABLE void add_##name(QObject* o) { m_##name.append(static_cast<type>(o)); }

class QObject;

class LSerializer
{
public:
    LSerializer();
    QJsonObject serialize(QObject* object);

protected:
    QJsonValue serializeObject(QObject* value);
    QJsonArray serializeArray(const QSequentialIterable &it);
    QJsonValue serializeValue(const QVariant& value);
};

template<class T>
class LDeserializer
{
public:
    LDeserializer(const QHash<QString, QMetaObject>& factory = QHash<QString, QMetaObject>());
    T* deserialize(const QJsonObject& json, QObject* parent = nullptr);
    T* deserialize(const QString& jsonString, QObject* parent = nullptr);

protected:
    void deserializeJson(QJsonObject json, QObject* object);
    void deserializeValue(const QJsonValue& value,
                          const QMetaProperty& metaProp,
                          QObject* dest);
    void deserializeArray(const QJsonArray& array,
                          const QMetaProperty& metaProp,
                          QObject* dest);
    void deserializeObjectArray(const QJsonArray& array,
                                const QString& propName,
                                const QString& type,
                                QObject* dest);

private:
    QHash<QString, QMetaObject> m_factory;
    QRegularExpression m_arrayTypeRegex;
};

class LCArrayHolder : public QObject
{
    Q_OBJECT
public:
    LCArrayHolder(QObject* parent = nullptr) : QObject(parent) {}
};

template<class T>
QVariant deserialize_array(const QJsonArray& array, std::function<T(const QJsonValue& jsonValue)> convert)
{
    QJsonArray::const_iterator it = array.constBegin();
    QList<T> list;
    while (it != array.constEnd()) {
        list.append(convert(*it));
        ++it;
    }

    return QVariant::fromValue(list);
}

template<class T>
LDeserializer<T>::LDeserializer(const QHash<QString, QMetaObject>& factory) :
    m_factory(factory),
    m_arrayTypeRegex(QStringLiteral("^(QList)<([^\\*]+(\\*){0,1})>$"))
{}

template<class T>
T* LDeserializer<T>::deserialize(const QJsonObject& json, QObject* parent)
{
    T* t = new T(parent);
    deserializeJson(json, t);
    return t;
}

template<class T>
T* LDeserializer<T>::deserialize(const QString& jsonString, QObject *parent)
{
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8());
    QJsonObject json = doc.object();
    return deserialize(json, parent);
}

template<class T>
void LDeserializer<T>::deserializeJson(QJsonObject json, QObject* object)
{
    QJsonObject::const_iterator it = json.constBegin();
    const QMetaObject* metaObj = object->metaObject();
    while (it != json.constEnd()) {
        const QString key = it.key();
        for (int i = 0; i < metaObj->propertyCount(); i++)
            if (metaObj->property(i).name() == key)
                deserializeValue(it.value(), metaObj->property(i), object);
        ++it;
    }
}

template<class T>
void LDeserializer<T>::deserializeArray(const QJsonArray& array, const QMetaProperty& metaProp, QObject* dest)
{
    qDebug() << "Deserialize array:" << metaProp.typeName() << metaProp.name();
    QRegularExpressionMatch match = m_arrayTypeRegex.match(metaProp.typeName());
    if (!match.hasMatch()) {
        qWarning() << "Failed to deserialize array with type:" << metaProp.typeName();
        return;
    }

    QString container = match.captured(1);
    QString type = match.captured(2);
    if (type == QStringLiteral("int"))
        dest->setProperty(metaProp.name(), deserialize_array<int>(array, [] (const QJsonValue& jsonValue) -> int {
            return jsonValue.toInt();
        }));
    else if (type == QStringLiteral("long"))
        dest->setProperty(metaProp.name(), deserialize_array<long>(array, [] (const QJsonValue& jsonValue) -> long {
            return jsonValue.toInt();
        }));
    else if (type == QStringLiteral("float"))
        qDebug() << "List of floats";
    else if (type == QStringLiteral("double"))
        qDebug() << "List of doubles";
    else if (type == QStringLiteral("QString"))
        dest->setProperty(metaProp.name(), deserialize_array<QString>(array, [] (const QJsonValue& jsonValue) -> QString {
            return jsonValue.toString();
        }));
    else if (type == QStringLiteral("bool"))
        dest->setProperty(metaProp.name(), deserialize_array<bool>(array, [] (const QJsonValue& jsonValue) -> bool {
            return jsonValue.toBool();
        }));
    else {
        if (m_factory.contains(type))
            deserializeObjectArray(array, metaProp.name(), type, dest);
        else
            qWarning() << type << "is not known";
    }

    return;
}

template<class T>
void LDeserializer<T>::deserializeValue(const QJsonValue& value, const QMetaProperty& metaProp, QObject* dest)
{
    switch (value.type()) {
    case QJsonValue::Null:
    case QJsonValue::Undefined:
        dest->setProperty(metaProp.name(), QVariant());
        break;
    case QJsonValue::Bool:
        dest->setProperty(metaProp.name(), value.toBool());
        break;
    case QJsonValue::Double:
        qDebug() << "Settings:" << metaProp.name() << value.toInt();
        dest->setProperty(metaProp.name(), value.toDouble());
        break;
    case QJsonValue::String:
        dest->setProperty(metaProp.name(), value.toString());
        break;
    case QJsonValue::Array:
        deserializeArray(value.toArray(), metaProp, dest);
        break;
    case QJsonValue::Object:
        QString className(metaProp.typeName());
        if (!m_factory.contains(className)) {
            qCDebug(lserializer) << "Factory does not contain class name:"
                                 << className;
            break;
        }

        QObject* child = m_factory[className].newInstance(Q_ARG(QObject*, dest));
        dest->setProperty(metaProp.name(), QVariant::fromValue<QObject*>(child));
        deserializeJson(value.toObject(), child);

        // TODO: Handle errors.

        break;
    }
}

template<class T>
void LDeserializer<T>::deserializeObjectArray(const QJsonArray& array, const QString& propName, const QString& type, QObject* dest)
{
    QMetaMethod addMethod;
    for (int i = 0; i < dest->metaObject()->methodCount(); i++)
        if (dest->metaObject()->method(i).name() == QString("add_%1").arg(propName))
            addMethod = dest->metaObject()->method(i);
    if (!addMethod.isValid()) {
        qWarning() << "Could not find add method";
        return;
    }

    QJsonArray::const_iterator it = array.constBegin();
    while (it != array.constEnd()) {
        if ((*it).type() == QJsonValue::Null || (*it).type() == QJsonValue::Undefined)
            addMethod.invoke(dest, Qt::DirectConnection, Q_ARG(QObject*, nullptr));
        else {
            QObject* child = m_factory[type].newInstance(Q_ARG(QObject*, dest));
            deserializeJson((*it).toObject(), child);
            addMethod.invoke(dest, Qt::DirectConnection, Q_ARG(QObject*, child));
        }

        ++it;
    }
}

#endif // LCSERIALIZER_H
