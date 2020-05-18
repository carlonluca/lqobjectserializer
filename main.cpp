#include <QCoreApplication>
#include <QObject>
#include <QJsonDocument>
#include <QDebug>
#include <QMetaObject>

#include "lcserializer.h"

class SomeQObjectChild : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString someString READ someString WRITE setSomeString NOTIFY someStringChanegd)
	QString m_someString;

public:
	SomeQObjectChild(QObject* parent = nullptr) : QObject(parent) {}
	QString someString() const { return m_someString; }
	void setSomeString(QString s) { m_someString = s; }
signals:
	void someStringChanegd(QString someString);
};

class SomeQObject : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString someString READ someString WRITE setSomeString NOTIFY someStringChanged)
	Q_PROPERTY(SomeQObjectChild* child1 READ child1 WRITE setChild1 NOTIFY child1Changed)
public:
	SomeQObject(QObject* parent = nullptr) : QObject(parent) {}

	QString someString() const { return m_someString; }
	SomeQObjectChild* child1() const { return m_child1; }

public slots:
	void setSomeString(QString someString) { m_someString = someString; }
	void setChild1(SomeQObjectChild* child1) { m_child1 = child1; }

signals:
	void someStringChanged();

	void child1Changed(SomeQObjectChild* child1);

private:
	QString m_someString;
	SomeQObjectChild* m_child1;
};

Q_DECLARE_METATYPE(SomeQObject*);
Q_DECLARE_METATYPE(SomeQObjectChild*);

int main(int argc, char** argv)
{
	QCoreApplication a(argc, argv);

    LCSerializer serializer;
	SomeQObjectChild childObj;
	childObj.setObjectName(QStringLiteral("HELLO2"));
	childObj.setSomeString(QStringLiteral("SOME STRING"));
	SomeQObject someObj;
	someObj.setSomeString(QStringLiteral("HELLO"));
	someObj.setChild1(&childObj);
	QJsonObject json = serializer.serialize(&someObj);
	QJsonDocument doc(json);

	qDebug() << QString(doc.toJson(QJsonDocument::Indented));

    LCDeserializer<SomeQObject> deserializer;
    QSharedPointer<SomeQObject> res = deserializer.deserialize(json);

	qDebug() << "Deserialized:" << res->someString()
			 << res->child1()->someString();

    return 0;
}

#include "main.moc"
