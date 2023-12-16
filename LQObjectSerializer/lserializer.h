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

#ifndef LSERIALIZER_H
#define LSERIALIZER_H

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
#include <QSequentialIterable>
#include <QDebug>

#include "../deps/lqtutils/lqtutils_prop.h"
#include "../deps/lqtutils/lqtutils_string.h"

Q_DECLARE_LOGGING_CATEGORY(lserializer)

#define L_RW_PROP_ARRAY_WITH_ADDER(type, name, setter)                                 \
    L_RW_PROP3(QList<type>, name, setter)                                              \
    public:                                                                            \
    Q_INVOKABLE void add_##name(void* o) { m_##name.append(reinterpret_cast<type>(o)); }

class QObject;

namespace lqo {

///
/// \brief The LStringifier class is an interface for objects used to automatically
/// stringify or destringify objects when serializing/deserializing.
///
class Stringifier
{
public:
    virtual QString stringify(const QVariant&) { return QString(); }
    virtual QVariant destringify(const QString&) { return QVariant(); }
};

///
/// \brief The LRectStringifier class is a stringifier for the QRectF and QRect types. It
/// serializes QRectF to x,y,w,h by using the dot as the decimal separator.
///
class RectStringifier : public Stringifier
{
public:
    QString stringify(const QVariant& v) override {
        if (v.isNull() || !v.canConvert<QRectF>())
            return QString();
        return lqt::string_from_rect(v.value<QRectF>());
    }

    QVariant destringify(const QString& s) override {
        QRectF ret = lqt::string_to_rect(s);
        return QVariant::fromValue(ret);
    }
};

class PointStringifier : public Stringifier
{
public:
    QString stringify(const QVariant& v) override {
        if (v.isNull() || !v.canConvert<QPointF>())
            return QString();
        return lqt::string_from_point(v.value<QPointF>());
    }

    QVariant destringify(const QString& s) override {
        QPointF p = lqt::string_to_point(s);
        return QVariant::fromValue(p);
    }
};

class Serializer
{
public:
    Serializer(const QHash<QString, QSharedPointer<Stringifier>>& stringifiers = QHash<QString, QSharedPointer<Stringifier>>());
    template<class T> QJsonObject serialize(T* object);
    template<class T> QJsonArray serialize(const QList<T>& array, const QMetaObject* metaObject = nullptr);

public:
    QJsonValue serializeObject(const void* value, const QMetaObject* metaObj);
    QJsonArray serializeArray(const QSequentialIterable& it, const QMetaObject* metaObject);
    template<typename T>
    QJsonValue serializeDictionary(const T& variant);
    QJsonValue serializeValue(const char* propName, const QVariant& value, const QMetaObject* metaObject);

private:
    QHash<QString, QSharedPointer<Stringifier>> m_stringifiers;
};

template<typename T>
QJsonValue Serializer::serializeDictionary(const T& variant)
{
    QJsonObject ret;
    for (auto it = variant.constBegin(), end = variant.constEnd(); it != end; it++) {
        const QJsonValue v = serializeValue(it.key().toLocal8Bit(), *it, nullptr);
        if (v.isNull())
            continue;
        ret[it.key()] = v;
    }

    return ret;
}

template<class T>
QJsonObject Serializer::serialize(T* object)
{
    return !object ? QJsonObject() : serializeObject(object, &T::staticMetaObject).toObject();
}

template<class T>
QJsonArray Serializer::serialize(const QList<T>& array, const QMetaObject* metaObject)
{
    QVariant list = QVariant::fromValue(array);
    return serializeArray(list.value<QSequentialIterable>(), metaObject);
}

template<class T>
class Deserializer
{
public:
    Deserializer(const QHash<QString, QSharedPointer<Stringifier>>& stringifiers = QHash<QString, QSharedPointer<Stringifier>>());
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
                          bool isGadget,
                          const QMetaObject* metaObject);
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
    QVariant destringify(const QString& value,
                         const QMetaProperty& metaProp,
                         const QMetaObject* metaObject);

protected:
    void writeProp(const QMetaProperty& metaProp, void* dest, const QVariant& value, bool isGadget);
    int metatype_from_name(const QString& typeName) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        return QMetaType::fromName(typeName.toLatin1()).id();
#else
        return QMetaType::type(typeName.toLatin1().data());
#endif
    }

private:
    QRegularExpression m_arrayTypeRegex;
    QHash<QString, QSharedPointer<Stringifier>> m_stringifiers;
};

class LCArrayHolder : public QObject
{
    Q_OBJECT
public:
    LCArrayHolder(QObject* parent = nullptr) : QObject(parent) {}
};

inline Stringifier* find_stringifier(const QMetaObject* metaObject,
                                     const char* propName,
                                     const QHash<QString, QSharedPointer<Stringifier>>& stringifiers)
{
    if (!metaObject)
        return nullptr;

    const int classInfoIndex = metaObject->indexOfClassInfo(propName);
    if (classInfoIndex < 0)
        return nullptr;

    const QString stringifierName = QString(metaObject->classInfo(classInfoIndex).value());
    if (!stringifiers.contains(stringifierName))
        return nullptr;

    return stringifiers.value(stringifierName).data();
}

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
Deserializer<T>::Deserializer(const QHash<QString, QSharedPointer<Stringifier>>& stringifiers) :
    m_arrayTypeRegex(QStringLiteral("^(QList)<([^\\*]+(\\*){0,1})>$"))
  , m_stringifiers(stringifiers) {}

template<class T>
T* Deserializer<T>::deserialize(const QJsonObject& json)
{
    T* t = new T;
    deserializeJson(json, t, &T::staticMetaObject);
    return t;
}

template<class T>
T* Deserializer<T>::deserialize(const QString& jsonString)
{
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8());
    QJsonObject json = doc.object();
    return deserialize(json);
}

template<class T>
QList<QString> Deserializer<T>::deserializeStringArray(const QJsonArray& array)
{
    return deserialize_array<QString>(array, [] (const QJsonValue& jsonValue) -> QString {
        return jsonValue.toString();
    }).toStringList();
}

template<class T>
QList<double> Deserializer<T>::deserializeNumberArray(const QJsonArray& array)
{
    QVariant v = deserialize_array<double>(array, [] (const QJsonValue& jsonValue) -> double {
        return jsonValue.toDouble();
    });
    return v.value<QList<double>>();
}

template<class T>
QList<bool> Deserializer<T>::deserializeBoolArray(const QJsonArray &array)
{
    QVariant v = deserialize_array<bool>(array, [] (const QJsonValue& jsonValue) -> bool {
        return jsonValue.toBool();
    });
    return v.value<QList<bool>>();
}

template<class T>
QList<T*> Deserializer<T>::deserializeObjectArray(const QJsonArray& array)
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
void Deserializer<T>::deserializeJson(QJsonObject json, void* dest, const QMetaObject* metaObject)
{
    bool isGadget = !metaObject->inherits(&QObject::staticMetaObject);
    QJsonObject::const_iterator it = json.constBegin();
    while (it != json.constEnd()) {
        const QString& key = it.key();
        for (int i = 0; i < metaObject->propertyCount(); i++)
            if (metaObject->property(i).name() == key) {
                deserializeValue(it.value(), metaObject->property(i), dest, isGadget, metaObject);
                break;
            }
        ++it;
    }
}

template<class T>
void Deserializer<T>::deserializeArray(const QJsonArray& array, const QMetaProperty& metaProp, void* dest)
{
#ifdef DEBUG_LQOBJECTSERIALIZER
    qDebug() << "Deserialize array:" << metaProp.typeName() << metaProp.name();
#endif
    QString container;
    QString type;
    if (metaProp.typeName() == QStringLiteral("QStringList")) {
        container = QStringLiteral("QList");
        type = QStringLiteral("QString");
    }
    else {
        QRegularExpressionMatch match = m_arrayTypeRegex.match(metaProp.typeName());
        if (!match.hasMatch()) {
            qWarning() << "Failed to deserialize array with type:" << metaProp.typeName();
            return;
        }

        container = match.captured(1);
        type = match.captured(2);
    }

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
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        if (QMetaType::fromName(type.toLatin1()).id() != QMetaType::UnknownType)
#else
        if (QMetaType::type(type.toLatin1().data()) != QMetaType::UnknownType)
#endif
            deserializeObjectArray(array, metaProp.name(), type, reinterpret_cast<QObject*>(dest));
        else
            qWarning() << type << "is not known";
    }

    return;
}

template<class T>
void* Deserializer<T>::instantiateObject(const QJsonValue& value, const QMetaType& metaType, bool isGadget, QObject* parent)
{
    const QMetaObject* metaObject = metaType.metaObject();
    if (metaType.id() == QMetaType::UnknownType) {
        qCDebug(lserializer) << "Class not registered:"
                             << metaType.name() << metaObject->className();
        return nullptr;
    }

    if (!isGadget) {
        QObject* child = metaObject->newInstance();
        if (parent)
            child->setParent(parent);
        deserializeJson(value.toObject(), child, metaObject);
        return child;
    }
    else {
        // TODO: mem?
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        void* gadget = QMetaType::fromName(metaObject->className()).create(nullptr);
#else
        void* gadget = QMetaType::create(QMetaType::type(metaObject->className()));
#endif
        deserializeJson(value.toObject(), gadget, metaObject);
        return gadget;
    }

    return nullptr;
}

template<class T>
QVariant Deserializer<T>::destringify(const QString& value,
                                       const QMetaProperty& metaProp,
                                       const QMetaObject* metaObject)
{
    Stringifier* strigifier = find_stringifier(metaObject, metaProp.name(), m_stringifiers);
    if (!strigifier)
        return QVariant();

    return strigifier->destringify(value);
}

template<class T>
void Deserializer<T>::deserializeValue(const QJsonValue& value,
                                        const QMetaProperty& metaProp,
                                        void* dest,
                                        bool isGadget,
                                        const QMetaObject* metaObject)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    int typeId = metaProp.metaType().id();
#else
    int typeId = QMetaType::type(metaProp.typeName());
#endif

    switch (typeId) {
    case QMetaType::QVariant:
        writeProp(metaProp, dest, value.toVariant(), isGadget);
        return;
    case QMetaType::QVariantHash:
        writeProp(metaProp, dest, value.toVariant().toHash(), isGadget);
        return;
    case QMetaType::QVariantMap:
        writeProp(metaProp, dest, value.toVariant().toMap(), isGadget);
        return;
    case QMetaType::QVariantList:
        writeProp(metaProp, dest, value.toVariant().toList(), isGadget);
        return;
    default:
        break;
    }

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
    case QJsonValue::String: {
        const QVariant destringified = destringify(value.toString(), metaProp, metaObject);
        if (!destringified.isNull())
            writeProp(metaProp, dest, destringified, isGadget);
        else
            writeProp(metaProp, dest, value.toString(), isGadget);
        break;
    }
    case QJsonValue::Array:
        deserializeArray(value.toArray(), metaProp, dest);
        break;
    case QJsonValue::Object:
        QMetaType metaType(typeId);
        bool createGadget = metaType.flags().testFlag(QMetaType::PointerToGadget);
        // TODO: Check error.
        QObject* parent = !createGadget && !isGadget ? reinterpret_cast<QObject*>(dest) : nullptr;
        void* obj = instantiateObject(value, metaType, createGadget, parent);
        QVariant value_;
        if (createGadget)
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            value_ = QVariant(metaType, &obj);
#else
            value_ = QVariant(typeId, &obj);
#endif
        else
            value_ = QVariant::fromValue<QObject*>(reinterpret_cast<QObject*>(obj));
        writeProp(metaProp, dest, value_, isGadget);

        // TODO: Handle errors.

        break;
    }
}

template<class T>
void Deserializer<T>::deserializeObjectArray(const QJsonArray& array, const QString& propName, const QString& type, QObject* dest)
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
        if ((*it).type() == QJsonValue::Null || (*it).type() == QJsonValue::Undefined) {
            if (!addMethod.invoke(dest, Qt::DirectConnection, Q_ARG(void*, nullptr)))
                qWarning(lserializer) << "Failed to invoke add method";
        }
        else {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            QMetaType metaType = QMetaType::fromName(type.toLatin1());
#else
            int typeId = QMetaType::type(type.toLatin1());
            QMetaType metaType(typeId);
#endif
            const bool createGadget = metaType.flags().testFlag(QMetaType::PointerToGadget);
            QObject* parent = !createGadget ? reinterpret_cast<QObject*>(dest) : nullptr;
            void* obj = instantiateObject((*it).toObject(), metaType, createGadget, parent);
            if (!obj)
                continue;

            if (!addMethod.invoke(dest, Qt::DirectConnection, Q_ARG(void*, obj)))
                qWarning(lserializer) << "Failed to invoke add method";
        }

        ++it;
    }
}

template<class T>
void Deserializer<T>::writeProp(const QMetaProperty& metaProp, void* dest, const QVariant& value, bool isGadget)
{
    if (!metaProp.isWritable()) {
        qCWarning(lserializer) << "Prop"
                               << metaProp.name()
                               << "must be writable to deserialize";
        return;
    }

    bool success;
    if (isGadget)
        success = metaProp.writeOnGadget(dest, value);
    else
        success = metaProp.write(reinterpret_cast<QObject*>(dest), value);

    if (!success)
        qCWarning(lserializer) << "Failed to write" << value
                               << "to" << metaProp.name();
}

} // namespace lqo

#endif // LSERIALIZER_H
