#ifndef LCSERIALIZER_H
#define LCSERIALIZER_H

#include <QByteArray>
#include <QVariant>
#include <QJsonObject>
#include <QJsonArray>
#include <QObject>
#include <QMetaObject>
#include <QMetaProperty>
#include <QRegularExpression>
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
    void deserializeValue(const QJsonValue& value, const QMetaProperty& metaProp, QObject* dest);
	void deserializeArray(const QJsonArray& array, const QMetaProperty& metaProp, QObject* dest);
	void deserializeArray(const QJsonArray& array, const QString& propName, const QString& type, QObject* dest);

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
LCDeserializer<T>::LCDeserializer(const QHash<QString, QMetaObject>& factory) :
	m_factory(factory),
	m_arrayTypeRegex(QStringLiteral("^(QList)<([^\\*]+(\\*){0,1})>$"))
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
        for (int i = 0; i < metaObj->propertyCount(); i++)
            if (metaObj->property(i).name() == key)
                deserializeValue(it.value(), metaObj->property(i), object);
        ++it;
    }
}

template<class T>
void LCDeserializer<T>::deserializeArray(const QJsonArray& array, const QMetaProperty& metaProp, QObject* dest)
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
		if (m_factory.contains(type)) {
#if 0
			return deserialize_array<QObject*>(array, [this, type] (const QJsonValue& jsonValue) -> QObject* {
				// TODO: set parent.
				qDebug() << type;
				QObject* child = m_factory[type].newInstance();
				deserializeJson(jsonValue.toObject(), child);
				return child;
			});
#endif
			deserializeArray(array, metaProp.name(), type, dest);
		}
		else
			qWarning() << type << "is not known";
    }

	return;
}

template<class T>
void LCDeserializer<T>::deserializeValue(const QJsonValue& value, const QMetaProperty& metaProp, QObject* dest)
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
        QObject* child = m_factory[className].newInstance(Q_ARG(QObject*, dest));
        dest->setProperty(metaProp.name(), QVariant::fromValue<QObject*>(child));
        deserializeJson(value.toObject(), child);

        // TODO: Handle errors.

        break;
    }
}

template<class T>
void LCDeserializer<T>::deserializeArray(const QJsonArray& array, const QString& propName, const QString& type, QObject* dest)
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
		QObject* child = m_factory[type].newInstance(Q_ARG(QObject*, dest));
		deserializeJson((*it).toObject(), child);
		addMethod.invoke(dest, Qt::DirectConnection, Q_ARG(QObject*, child));
		++it;
	}
}

#endif // LCSERIALIZER_H
