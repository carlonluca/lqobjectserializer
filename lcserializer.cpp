#include "lcserializer.h"

LCSerializer::LCSerializer()
{

}

QJsonObject LCSerializer::serialize(QObject* object)
{
	return serializeObject(object);
}

QJsonObject LCSerializer::serializeObject(QObject* object)
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
