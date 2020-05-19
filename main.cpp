#include <QCoreApplication>
#include <QObject>
#include <QJsonDocument>
#include <QDebug>
#include <QMetaObject>

#include "lcserializer.h"

class SomeQObjectChild2 : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString someString READ someString WRITE setSomeString NOTIFY someStringChanegd)
    QString m_someString;

public:
    Q_INVOKABLE SomeQObjectChild2(QObject* parent = nullptr) : QObject(parent) {}
    ~SomeQObjectChild2() { qDebug() << Q_FUNC_INFO; }

    QString someString() const { return m_someString; }
    void setSomeString(QString s) { m_someString = s; }
signals:
    void someStringChanegd(QString someString);
};

class SomeQObjectChild : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString someString READ someString WRITE setSomeString NOTIFY someStringChanegd)
	QString m_someString;

public:
    Q_INVOKABLE SomeQObjectChild(QObject* parent = nullptr) : QObject(parent) {}
    ~SomeQObjectChild() { qDebug() << Q_FUNC_INFO; }

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
    Q_PROPERTY(SomeQObjectChild2* child2 READ child2 WRITE setChild2 NOTIFY child2Changed)
public:
    Q_INVOKABLE SomeQObject(QObject* parent = nullptr) : QObject(parent) {}
    ~SomeQObject() { qDebug() << Q_FUNC_INFO; }

	QString someString() const { return m_someString; }
	SomeQObjectChild* child1() const { return m_child1; }
    SomeQObjectChild2* child2() const { return m_child2; }

public slots:
	void setSomeString(QString someString) { m_someString = someString; }
	void setChild1(SomeQObjectChild* child1) { m_child1 = child1; }
    void setChild2(SomeQObjectChild2* child2) { m_child2 = child2; }

signals:
	void someStringChanged();
	void child1Changed(SomeQObjectChild* child1);
    void child2Changed(SomeQObjectChild2* child2);

private:
	QString m_someString;
    SomeQObjectChild* m_child1 = nullptr;
    SomeQObjectChild2* m_child2 = nullptr;
};

Q_DECLARE_METATYPE(SomeQObject*);
Q_DECLARE_METATYPE(SomeQObjectChild*);

int main(int argc, char** argv)
{
	QCoreApplication a(argc, argv);

    {
        SomeQObjectChild childObj;
        childObj.setObjectName(QStringLiteral("HELLO2"));
        childObj.setSomeString(QStringLiteral("SOME STRING"));
        SomeQObject someObj;
        someObj.setSomeString(QStringLiteral("HELLO"));
        someObj.setChild1(&childObj);

        LCSerializer serializer;
        QJsonObject json = serializer.serialize(&someObj);
        QJsonDocument doc(json);

        qDebug().noquote() << QString(doc.toJson(QJsonDocument::Indented));

        QHash<QString, QMetaObject> factory {
            { QStringLiteral("SomeQObject*"), SomeQObject::staticMetaObject },
            { QStringLiteral("SomeQObjectChild*"), SomeQObjectChild::staticMetaObject },
            { QStringLiteral("SomeQObjectChild2*"), SomeQObjectChild2::staticMetaObject }
        };

        LCDeserializer<SomeQObject> deserializer(factory);
        QSharedPointer<SomeQObject> res = deserializer.deserialize(json);

        qDebug() << "Deserialized:" << "\n"
                 << "Prop:" << res->someString() << "\n"
                 << "Prop child1:" << res->child1()->someString() << "\n"
                 << "Child2:" << res->child2();
    }

    qDebug() << "DONE ;-)";

    return 0;
}

#include "main.moc"
