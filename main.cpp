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
	Q_PROPERTY(int someInt READ someInt WRITE setSomeInt NOTIFY someIntChanged)
	Q_PROPERTY(qint64 someLong READ someLong WRITE setSomeLong NOTIFY someLongChanged)
	Q_PROPERTY(bool someBool READ someBool WRITE setSomeBool NOTIFY someBoolChanged)
	Q_PROPERTY(double someDouble READ someDouble WRITE setSomeDouble NOTIFY someDoubleChanged)
	Q_PROPERTY(QString someString READ someString WRITE setSomeString NOTIFY someStringChanged)
    Q_PROPERTY(SomeQObjectChild* child1 READ child1 WRITE setChild1 NOTIFY child1Changed)
    Q_PROPERTY(SomeQObjectChild2* child2 READ child2 WRITE setChild2 NOTIFY child2Changed)
	Q_PROPERTY(QList<int> intList READ intList WRITE setIntList NOTIFY intListChanged)
    Q_PROPERTY(QList<QString> stringList READ stringList WRITE setStringList NOTIFY stringListChanged)
    Q_PROPERTY(QList<QObject*> objectList READ objectList WRITE setObjectList NOTIFY objectListChanged)
public:
	Q_INVOKABLE SomeQObject(QObject* parent = nullptr) :
		QObject(parent) {}
    ~SomeQObject() { qDebug() << Q_FUNC_INFO; }

	int someInt() const { return m_someInt; }
	qint64 someLong() const { return m_someLong; }
	bool someBool() const { return m_someBool; }
	double someDouble() const { return m_someDouble; }
	QString someString() const { return m_someString; }
	SomeQObjectChild* child1() const { return m_child1; }
    SomeQObjectChild2* child2() const { return m_child2; }
	QList<int> intList() const { return m_intList; }
	QList<QString> stringList() const { return m_stringList; }
    QList<QObject*> objectList() const { return m_objectList; }

public slots:
	void setSomeInt(int someInt) { m_someInt = someInt; }
	void setSomeLong(qint64 someLong) { m_someLong = someLong; }
	void setSomeBool(bool someBool) { m_someBool = someBool; }
	void setSomeDouble(double someDouble) { m_someDouble = someDouble; }
	void setSomeString(QString someString) { m_someString = someString; }
	void setChild1(SomeQObjectChild* child1) { m_child1 = child1; }
    void setChild2(SomeQObjectChild2* child2) { m_child2 = child2; }
	void setIntList(QList<int> intList) { m_intList = intList; }
	void setStringList(QStringList stringList) { m_stringList = stringList; }
    void setObjectList(QList<QObject*> objectList) { m_objectList = objectList; }

signals:
	void someIntChanged();
	void someLongChanged();
	void someBoolChanged();
	void someDoubleChanged();
	void someStringChanged();
	void child1Changed(SomeQObjectChild* child1);
    void child2Changed(SomeQObjectChild2* child2);
	void intListChanged();
	void stringListChanged();
    void objectListChanged();

private:
	int m_someInt = 0;
	qint64 m_someLong = 0;
	bool m_someBool = false;
	double m_someDouble = 0.0;
	QString m_someString;
    SomeQObjectChild* m_child1 = nullptr;
    SomeQObjectChild2* m_child2 = nullptr;
	QList<int> m_intList;
	QList<QString> m_stringList;
    QList<QObject*> m_objectList;
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
		someObj.setSomeInt(7);
		someObj.setSomeLong(std::numeric_limits<int>::max() + static_cast<qint64>(10));
		someObj.setSomeBool(true);
		someObj.setSomeDouble(7.6);
        someObj.setSomeString(QStringLiteral("HELLO"));
        someObj.setChild1(&childObj);
        someObj.setIntList(QList<int>()
                           << 1 << 2 << 3);
        someObj.setStringList(QStringList()
                              << "A" << "B" << "C" << "D");
        someObj.setObjectList(QList<QObject*>()
                              << new SomeQObjectChild(&someObj)
                              << new SomeQObjectChild(&someObj));

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
