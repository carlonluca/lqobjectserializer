/**2
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
    Q_INVOKABLE void add_##name(void* o) { m_##name.append(reinterpret_cast<type>(o)); }

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
    void* instantiateObject(const QJsonValue& value,
                            const QMetaType& metaType,
                            bool isGadget,
                            QObject* parent);

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
            if (metaObject->property(i).name() == key) {
                deserializeValue(it.value(), metaObject->property(i), dest, isGadget);
                break;
            }
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

template<class T>
void* LDeserializer<T>::instantiateObject(const QJsonValue& value, const QMetaType& metaType, bool isGadget, QObject* parent)
{
    const QMetaObject* metaObject = QMetaType::metaObjectForType(metaType.id());
    if (metaType.id() == QMetaType::UnknownType) {
        qCDebug(lserializer) << "Class not registered:"
                             << metaType.name() << metaObject->className();
        return nullptr;
    }

    if (!isGadget) {
        QObject* child = metaObject->newInstance();
        // TODO: Check errors.
        if (parent)
            child->setParent(parent);
        deserializeJson(value.toObject(), child, metaObject);
        return child;
    }
    else {
        // TODO: mem?
        void* gadget = QMetaType::create(QMetaType::type(metaObject->className()));
        deserializeJson(value.toObject(), gadget, metaObject);
        return gadget;
    }

    return nullptr;
}

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
        int typeId = QMetaType::type(metaProp.typeName());
        QMetaType metaType(typeId);
        bool createGadget = metaType.flags().testFlag(QMetaType::PointerToGadget);
        // TODO: Check error.
        QObject* parent = !createGadget && !isGadget ? reinterpret_cast<QObject*>(dest) : nullptr;
        void* obj = instantiateObject(value, metaType, createGadget, parent);
        QVariant value_;
        if (createGadget)
            value_ = QVariant(typeId, &obj);
        else
            value_ = QVariant::fromValue<QObject*>(reinterpret_cast<QObject*>(obj));
        writeProp(metaProp, dest, value_, isGadget);

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
            addMethod.invoke(dest, Qt::DirectConnection, Q_ARG(void*, nullptr));
        else {
            int typeId = QMetaType::type(type.toLatin1());
            QMetaType metaType(typeId);
            bool createGadget = metaType.flags().testFlag(QMetaType::PointerToGadget);
            QObject* parent = !createGadget ? reinterpret_cast<QObject*>(dest) : nullptr;
            // TODO: Check error.
            void* obj = instantiateObject((*it).toObject(), metaType, createGadget, parent);
            // TODO: Set parent.
            //QVariant value_ = QVariant::fromValue(obj);

            addMethod.invoke(dest, Qt::DirectConnection, Q_ARG(void*, obj));
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
