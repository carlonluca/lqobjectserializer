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
#include <QMutex>
#include <QMetaMethod>
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
    template<class T> QJsonObject serialize(void* object);

protected:
    QJsonValue serializeObject(const void* value, const QMetaObject* metaObj);
    QJsonArray serializeArray(const QSequentialIterable &it);
    QJsonValue serializeValue(const QVariant& value);
};

template<class T>
QJsonObject LSerializer::serialize(void* object)
{
    return !object ? QJsonObject() : serializeObject(object, &T::staticMetaObject).toObject();
}

template<class T>
class LDeserializer
{
public:
    LDeserializer();
    T* deserialize(const QJsonObject& json);
    T* deserialize(const QString& jsonString);
    QList<QString> deserializeStringArray(const QJsonArray& array);
    QList<double>  deserializeNumberArray(const QJsonArray& array);
    QList<bool>    deserializeBoolArray(const QJsonArray& array);
    QList<T*>      deserializeObjectArray(const QJsonArray &array);

    static void lserializerRegisterObject(const QMetaObject& metaObject);

protected:
    void deserializeJson(QJsonObject json,
                         void* dest,
                         const QMetaObject* metaObject);
    void deserializeValue(const QJsonValue& value,
                          const QMetaProperty& metaProp,
                          void* dest,
                          bool isGadget);
    void deserializeArray(const QJsonArray& array,
                          const QMetaProperty& metaProp,
                          void* dest);
    void deserializeObjectArray(const QJsonArray& array,
                                const QString& propName,
                                const QString& type,
                                QObject* dest);

protected:
    void writeProp(const QMetaProperty& metaProp, void* dest, const QVariant& value, bool isGadget);

private:
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
LDeserializer<T>::LDeserializer() :
    m_arrayTypeRegex(QStringLiteral("^(QList)<([^\\*]+(\\*){0,1})>$"))
{}

template<class T>
T* LDeserializer<T>::deserialize(const QJsonObject& json)
{
    T* t = new T;
    deserializeJson(json, t, &T::staticMetaObject);
    return t;
}

template<class T>
T* LDeserializer<T>::deserialize(const QString& jsonString)
{
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8());
    QJsonObject json = doc.object();
    return deserialize(json);
}

template<class T>
QList<QString> LDeserializer<T>::deserializeStringArray(const QJsonArray& array)
{
    return deserialize_array<QString>(array, [] (const QJsonValue& jsonValue) -> QString {
        return jsonValue.toString();
    }).toStringList();
}

template<class T>
QList<double> LDeserializer<T>::deserializeNumberArray(const QJsonArray& array)
{
    QVariant v = deserialize_array<double>(array, [] (const QJsonValue& jsonValue) -> double {
        return jsonValue.toDouble();
    });
    return v.value<QList<double>>();
}

template<class T>
QList<bool> LDeserializer<T>::deserializeBoolArray(const QJsonArray &array)
{
    QVariant v = deserialize_array<bool>(array, [] (const QJsonValue& jsonValue) -> bool {
        return jsonValue.toBool();
    });
    return v.value<QList<bool>>();
}

template<class T>
QList<T*> LDeserializer<T>::deserializeObjectArray(const QJsonArray& array)
{
    QVariant v = deserialize_array<T*>(array, [this] (const QJsonValue& jsonValue) -> T* {
        // TODO: Mem management.
        T* t = new T();
        deserializeJson(jsonValue.toObject(), t, &T::staticMetaObject);
        return t;
    });
    return v.value<QList<T*>>();
}

template<class T>
void LDeserializer<T>::deserializeJson(QJsonObject json, void* dest, const QMetaObject* metaObject)
{
    bool isGadget = !metaObject->inherits(&QObject::staticMetaObject);
    QJsonObject::const_iterator it = json.constBegin();
    while (it != json.constEnd()) {
        const QString key = it.key();
        for (int i = 0; i < metaObject->propertyCount(); i++)
            if (metaObject->property(i).name() == key)
                deserializeValue(it.value(), metaObject->property(i), dest, isGadget);
        ++it;
    }
}

template<class T>
void LDeserializer<T>::deserializeArray(const QJsonArray& array, const QMetaProperty& metaProp, void* dest)
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
        writeProp(metaProp, dest, deserialize_array<int>(array, [] (const QJsonValue& jsonValue) -> int {
            return jsonValue.toInt();
        }), false);
    else if (type == QStringLiteral("long"))
        writeProp(metaProp, dest, deserialize_array<long>(array, [] (const QJsonValue& jsonValue) -> long {
            return jsonValue.toInt();
        }), false);
    else if (type == QStringLiteral("float"))
        writeProp(metaProp, dest, deserialize_array<float>(array, [] (const QJsonValue& jsonValue) -> float {
            return jsonValue.toDouble();
        }), false);
    else if (type == QStringLiteral("double"))
        writeProp(metaProp, dest, deserialize_array<double>(array, [] (const QJsonValue& jsonValue) -> double {
            return jsonValue.toDouble();
        }), false);
    else if (type == QStringLiteral("QString"))
        writeProp(metaProp, dest, deserialize_array<QString>(array, [] (const QJsonValue& jsonValue) -> QString {
            return jsonValue.toString();
        }), false);
    else if (type == QStringLiteral("bool"))
        writeProp(metaProp, dest, deserialize_array<bool>(array, [] (const QJsonValue& jsonValue) -> bool {
            return jsonValue.toBool();
        }), false);
    else {
        // TODO: Implement gadget.
        if (QMetaType::type(type.toLatin1().data()) != QMetaType::UnknownType)
            deserializeObjectArray(array, metaProp.name(), type, reinterpret_cast<QObject*>(dest));
        else
            qWarning() << type << "is not known";
    }

    return;
}
class LGHOwner2
{
    Q_GADGET
public:
    Q_INVOKABLE LGHOwner2() { qDebug() << Q_FUNC_INFO; }
private:
L_RW_GPROP(QString, login, setLogin, "login")
L_RW_GPROP(int, id, setId, 8)
L_RW_GPROP(QString, node_id, setNode_id, "node_id")
L_RW_GPROP(QString, avatar_url, setAvatar_url, "avatar")
L_END_GADGET
template<class T>
void LDeserializer<T>::deserializeValue(const QJsonValue& value, const QMetaProperty& metaProp, void* dest, bool isGadget)
{
    switch (value.type()) {
    case QJsonValue::Null:
    case QJsonValue::Undefined:
        writeProp(metaProp, dest, QVariant(), isGadget);
        break;
    case QJsonValue::Bool:
        writeProp(metaProp, dest, value.toBool(), isGadget);
        break;
    case QJsonValue::Double:
        writeProp(metaProp, dest, value.toDouble(), isGadget);
        break;
    case QJsonValue::String:
        writeProp(metaProp, dest, value.toString(), isGadget);
        break;
    case QJsonValue::Array:
        deserializeArray(value.toArray(), metaProp, dest);
        break;
    case QJsonValue::Object:
        const char* className = metaProp.typeName();
        int typeId = QMetaType::type(metaProp.typeName());
        if (typeId == QMetaType::UnknownType) {
            qCDebug(lserializer) << "Class not registered:"
                                 << className;
            break;
        }

        QMetaType metaType(typeId);
        const QMetaObject* metaObject = QMetaType::metaObjectForType(typeId);
        if (metaType.flags().testFlag(QMetaType::PointerToQObject)) {
            QObject* parent = reinterpret_cast<QObject*>(dest);
            QObject* child = metaObject->newInstance(Q_ARG(QObject*, parent));
            parent->setProperty(metaProp.name(), QVariant::fromValue<QObject*>(child));
            deserializeJson(value.toObject(), child, metaObject);
            return;
        }
        else if (metaType.flags().testFlag(QMetaType::PointerToGadget)) {
            // TODO: mem?
            void* gadget = QMetaType::create(QMetaType::type(metaObject->className()));
            for (int i = 0; i < metaObject->constructorCount(); i++)
                qDebug() << "Ctor:" << metaObject->constructor(i).name();
            LGHOwner2* owner = reinterpret_cast<LGHOwner2*>(gadget);
            QMetaMethod ctor = metaObject->constructor(0);
            if (!ctor.invokeOnGadget(owner))
                qWarning() << "Cannot call ctor";
            QVariant vGadget(typeId, &gadget);
            writeProp(metaProp, dest, vGadget, isGadget);
            deserializeJson(value.toObject(), gadget, metaObject);
        }

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
            int itype = QMetaType::type(type.toLatin1());
            if (itype == QMetaType::UnknownType) {
                qCDebug(lserializer) << "Class not registered:"
                                     << propName;
                break;
            }

            QObject* child = QMetaType::metaObjectForType(itype)->newInstance(Q_ARG(QObject*, dest));
            deserializeJson((*it).toObject(), child, child->metaObject());
            addMethod.invoke(dest, Qt::DirectConnection, Q_ARG(QObject*, child));
        }

        ++it;
    }
}

template<class T>
void LDeserializer<T>::writeProp(const QMetaProperty& metaProp, void* dest, const QVariant& value, bool isGadget)
{
    if (isGadget)
        metaProp.writeOnGadget(dest, value);
    else
        metaProp.write(reinterpret_cast<QObject*>(dest), value);
}

#endif // LCSERIALIZER_H
