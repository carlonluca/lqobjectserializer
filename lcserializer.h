#ifndef LCSERIALIZER_H
#define LCSERIALIZER_H

#include <QByteArray>
#include <QVariant>
#include <QJsonObject>
#include <QJsonArray>
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
	QJsonValue serializeObject(QObject* value);
	QJsonArray serializeArray(const QSequentialIterable &it);
	QJsonValue serializeValue(const QVariant& value);
};

template<class T>
class LCDeserializer
{
public:
    LCDeserializer(const QHash<QString, QMetaObject>& factory);
    QSharedPointer<T> deserialize(const QJsonObject& json);

protected:
    void deserializeJson(QJsonObject json, QObject* object);
    void deserializeArray(QJsonArray array);

private:
    QHash<QString, QMetaObject> m_factory;
};

template<class T>
LCDeserializer<T>::LCDeserializer(const QHash<QString, QMetaObject>& factory) :
    m_factory(factory)
{}

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
                    qDebug() << metaObj->property(i).type() << metaObj->property(i).typeName();
                    break;
                case QJsonValue::Object:
                    QString className(metaObj->property(i).typeName());
                    QObject* child = m_factory[className].newInstance(Q_ARG(QObject*, object));
                    object->setProperty(metaObj->property(i).name(), QVariant::fromValue<QObject*>(child));
                    deserializeJson(it.value().toObject(), child);

					// TODO: Handle errors.

                    break;
                }
            }
        }

        ++it;
    }
}

#endif // LCSERIALIZER_H
