#include "lcserializer.h"

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
		}
	}

	return QJsonValue();
}
