#include <QObject>
#include <QMetaObject>
#include <QMetaProperty>
#ifdef QT_DEBUG
#include <QDebug>
#endif

#include "lcserializer.h"

template<class T>
LCSerializer<T>::LCSerializer()
{

}

template<class T>
QJsonObject LCSerializer<T>::serialize(QObject* object)
{
	return serializeObject(object);
}

template<class T>
QSharedPointer<T> LCSerializer<T>::deserialize(QJsonObject json)
{
}

template<class T>
T* LCSerializer<T>::deserializeJson(QJsonObject json)
{

}

template<class T>
QJsonObject LCSerializer<T>::serializeObject(QObject* object)
{
	QJsonObject json;

	const QMetaObject* metaObj = object->metaObject();
	for (int i = 0; i < metaObj->propertyCount(); ++i) {
		const char* propertyName = metaObj->property(i).name();
		QVariant value = object->property(propertyName);
#ifdef QT_DEBUG
		qDebug() << "Name:" << propertyName << ", value:" << value;
#endif
		if (value.canConvert<QObject*>())
			json[propertyName] = serializeObject(value.value<QObject*>());
		else {
			switch (static_cast<QMetaType::Type>(value.type())) {
			case QMetaType::QString:
				json[propertyName] = value.value<QString>(); break;
			case QMetaType::Int:
				json[propertyName] = value.toInt(); break;
			case QMetaType::UInt:
				json[propertyName] = static_cast<qint64>(value.toUInt()); break;
			case QMetaType::Long:
				json[propertyName] = static_cast<qint64>(value.value<long>()); break;
			case QMetaType::LongLong:
				json[propertyName] = static_cast<qint64>(value.value<long long>()); break;
			}
		}
	}

	return json;
}
