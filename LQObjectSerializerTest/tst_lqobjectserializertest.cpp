#include <QtTest>

#include "../LQObjectSerializer/lserializer.h"
#include "deps/lqtutils/lqtutils.h"

class SomeQObjectChild2 : public QObject
{
    Q_OBJECT
    L_RW_PROP(QString, someString, setSomeString)
    public:
        Q_INVOKABLE SomeQObjectChild2(QObject* parent = nullptr) : QObject(parent) {}
    ~SomeQObjectChild2() { qDebug() << Q_FUNC_INFO; }
};

class SomeQObjectChild : public QObject
{
    Q_OBJECT
    L_RW_PROP(QString, someString, setSomeString)
    public:
        Q_INVOKABLE SomeQObjectChild(QObject* parent = nullptr) : QObject(parent) {}
    ~SomeQObjectChild() { qDebug() << Q_FUNC_INFO; }
};

class SomeQObject : public QObject
{
    Q_OBJECT
    L_RW_PROP(int, someInt, setSomeInt)
    L_RW_PROP(qint64, someLong, setSomeLong)
    L_RW_PROP(bool, someBool, setSomeBool)
    L_RW_PROP(double, someDouble, setSomeDouble)
    L_RW_PROP(QString, someString, setSomeString)
    L_RW_PROP(SomeQObjectChild*, child1, setChild1)
    L_RW_PROP(SomeQObjectChild2*, child2, setChild2)
    L_RW_PROP(QList<int>, intList, setIntList)
    L_RW_PROP(QList<QString>, stringList, setStringList)
    L_RW_PROP(QList<SomeQObjectChild*>, objectList, setObjectList)
public:
    Q_INVOKABLE SomeQObject(QObject* parent = nullptr) :
      QObject(parent), m_someInt(0), m_child1(nullptr), m_child2(nullptr) {}
    ~SomeQObject() { qDebug() << Q_FUNC_INFO; }

public slots:
    void add_objectList(QObject* child) { m_objectList.append(static_cast<SomeQObjectChild*>(child)); }
};

Q_DECLARE_METATYPE(SomeQObject*);
Q_DECLARE_METATYPE(SomeQObjectChild*);

class LQObjectSerializerTest : public QObject
{
    Q_OBJECT
public:
    LQObjectSerializerTest();
    ~LQObjectSerializerTest();

private slots:
    void test_case1();
    void test_case2();
};

LQObjectSerializerTest::LQObjectSerializerTest()
{}

LQObjectSerializerTest::~LQObjectSerializerTest()
{}

void LQObjectSerializerTest::test_case1()
{
    SomeQObjectChild childObj;
    childObj.setObjectName(QStringLiteral("HELLO2"));
    childObj.setSomeString(QStringLiteral("SOME STRING"));
    SomeQObject someObj;
    SomeQObjectChild* childObj2 = new SomeQObjectChild(&someObj);
    childObj2->setSomeString("SOME STRING");
    SomeQObjectChild* childObj3 = new SomeQObjectChild(&someObj);
    childObj3->setSomeString("SOME STRING 2");
    someObj.setSomeInt(7);
    someObj.setSomeLong(std::numeric_limits<int>::max() + static_cast<qint64>(10));
    someObj.setSomeBool(true);
    someObj.setSomeDouble(7.6);
    someObj.setSomeString(QStringLiteral("HELLO"));
    someObj.setChild1(&childObj);
    someObj.setIntList(QList<int>()
                       << 3 << 7 << 19);
    someObj.setStringList(QStringList()
                          << "A" << "B" << "C" << "D");
    someObj.setObjectList(QList<SomeQObjectChild*>()
                          << childObj2
                          << childObj3);

    LSerializer serializer;
    QJsonObject json = serializer.serialize(&someObj);
    QJsonDocument doc(json);

    qDebug().noquote() << QString(doc.toJson(QJsonDocument::Indented));

    QHash<QString, QMetaObject> factory {
        { QStringLiteral("SomeQObject*"), SomeQObject::staticMetaObject },
        { QStringLiteral("SomeQObjectChild*"), SomeQObjectChild::staticMetaObject },
        { QStringLiteral("SomeQObjectChild2*"), SomeQObjectChild2::staticMetaObject }
    };

    LDeserializer<SomeQObject> deserializer(factory);
    QScopedPointer<SomeQObject> res(deserializer.deserialize(json));

    QCOMPARE(res->someInt(), 7);
    QCOMPARE(res->someString(), QSL("HELLO"));
    QCOMPARE(res->child1()->someString(), QSL("SOME STRING"));
    QCOMPARE(res->child2(), nullptr);

    //qDebug() << "Deserialized:" << "\n"
    //         << "Prop:" << res->someString() << "\n"
    //         << "Prop child1:" << res->child1()->someString() << "\n"
    //         << "Child2:" << res->child2() << "\n"
    //         << "Int:" << res->intList()[0] << res->intList()[1] << "\n"
    //         << "String:" << res->stringList()[0] << res->stringList()[1] << "\n"
    //         << "Object list:" << res->objectList()[0]->someString() << res->objectList()[1]->someString();
}

void LQObjectSerializerTest::test_case2()
{

}

QTEST_APPLESS_MAIN(LQObjectSerializerTest)

#include "tst_lqobjectserializertest.moc"
