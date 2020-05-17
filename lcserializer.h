#ifndef LCSERIALIZER_H
#define LCSERIALIZER_H

#include <QByteArray>
#include <QVariant>
#include <QJsonObject>

#include <optional>

class QObject;

template<class T>
class LCSerializer
{
public:
    LCSerializer();

    QJsonObject serialize(QObject* object);
    QSharedPointer<T> deserialize(QJsonObject json);

protected:
    T* deserializeJson(QJsonObject json);
    QJsonObject serializeObject(QObject* value);
};

#endif // LCSERIALIZER_H
