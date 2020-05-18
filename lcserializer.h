#ifndef LCSERIALIZER_H
#define LCSERIALIZER_H

#include <QByteArray>
#include <QVariant>
#include <QJsonObject>
#include <QObject>
#include <QMetaObject>
#include <QMetaProperty>
#ifdef QT_DEBUG
#include <QDebug>
#endif

#include <optional>

class QObject;

class LCSerializer
{
public:
    LCSerializer();
    QJsonObject serialize(QObject* object);

protected:
    QJsonObject serializeObject(QObject* value);
};

template<class T>
class LCDeserializer
{
public:
    LCDeserializer();
    QSharedPointer<T> deserialize(const QJsonObject& json);

protected:
    void deserializeJson(QJsonObject json, QObject* object);
    void deserializeArray(QJsonArray array);
};

template<class T>
LCDeserializer<T>::LCDeserializer()
{

}

template<class T>
QSharedPointer<T> LCDeserializer<T>::deserialize(const QJsonObject& json)
{
    T* t = new T;
    deserializeJson(json, t);

    return QSharedPointer<T>(t);
}

template<class T>
void LCDeserializer<T>::deserializeJson(QJsonObject json, QObject* object)
{
    QJsonObject::const_iterator it = json.constBegin();
    const QMetaObject* metaObj = object->metaObject();
    while (it != json.constEnd()) {
        const QString key = it.key();
        for (int i = 0; i < metaObj->propertyCount(); i++) {
            if (metaObj->property(i).name() == key) {
                switch (it.value().type()) {
                case QJsonValue::Null:
                case QJsonValue::Undefined:
                    object->setProperty(metaObj->property(i).name(), QVariant());
                    break;
                case QJsonValue::Bool:
                    object->setProperty(metaObj->property(i).name(), it.value().toBool());
                    break;
                case QJsonValue::Double:
                    object->setProperty(metaObj->property(i).name(), it.value().toDouble());
                    break;
                case QJsonValue::String:
                    object->setProperty(metaObj->property(i).name(), it.value().toString());
                    break;
                case QJsonValue::Array:
                    // TODO
                    break;
                case QJsonValue::Object:
                    // TODO
                    qDebug() << "Process object of class:"
                             << metaObj->property(i).typeName();
                    break;
                }
            }
        }

        ++it;
    }
}

#endif // LCSERIALIZER_H
