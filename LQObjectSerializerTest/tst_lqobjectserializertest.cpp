/**
 * MIT License
 *
 * Copyright (c) 2020 Luca Carlon
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 **/

#include <QtTest>
#include <QObject>

#include "../LQObjectSerializer/lserializer.h"
#include "../deps/lqtutils/lqtutils_string.h"

L_BEGIN_CLASS(SomeQObjectChild2)
L_RW_PROP(QString, someString, setSomeString, QString())
L_END_CLASS

L_BEGIN_CLASS(SomeQObjectChild)
L_RW_PROP(QString, someString, setSomeString, QString())
L_END_CLASS

L_BEGIN_CLASS(SomeQObject)
L_RW_PROP(int, someInt, setSomeInt, 0)
L_RW_PROP(qint64, someLong, setSomeLong, 0)
L_RW_PROP(bool, someBool, setSomeBool, false)
L_RW_PROP(double, someDouble, setSomeDouble, 0)
L_RW_PROP(QString, someString, setSomeString, QString())
L_RW_PROP(SomeQObjectChild*, child1, setChild1, nullptr)
L_RW_PROP(SomeQObjectChild2*, child2, setChild2, nullptr)
L_RW_PROP(QList<int>, intList, setIntList, QList<int>())
L_RW_PROP(QList<QString>, stringList, setStringList, QList<QString>())
L_RW_PROP_ARRAY_WITH_ADDER(SomeQObjectChild*, objectList, setObjectList)
L_END_CLASS

L_BEGIN_CLASS(GlossDefObj)
L_RW_PROP(QString, para, setPara, QString())
L_RW_PROP(QList<QString>, GlossSeeAlso, setGlossSeeAlso)
L_END_CLASS

L_BEGIN_CLASS(GlossEntryObj)
L_RW_PROP(QString, ID, setID)
L_RW_PROP(QString, SortAs, setSortAs)
L_RW_PROP(QString, GlossTerm, setGlossTerm)
L_RW_PROP(QString, Acronym, setAcronym)
L_RW_PROP(QString, Abbrev, setAbbrev)
L_RW_PROP(QString, GlossSee, setGlossSee)
L_RW_PROP(GlossDefObj*, GlossDef, setGlossDef, nullptr)
L_END_CLASS

L_BEGIN_CLASS(GlossListObj)
L_RW_PROP(GlossEntryObj*, GlossEntry, setGlossEntry, nullptr)
L_END_CLASS

L_BEGIN_CLASS(GlossDivObj)
L_RW_PROP(QString, title, setTitle)
L_RW_PROP(GlossListObj*, GlossList, setGlossList, nullptr)
L_END_CLASS

L_BEGIN_CLASS(Glossary)
L_RW_PROP(QString, title, setTitle)
L_RW_PROP(GlossDivObj*, GlossDiv, setGlossDiv, nullptr)
L_END_CLASS

L_BEGIN_CLASS(GlossaryRoot)
L_RW_PROP(Glossary*, glossary, setGlossary, nullptr)
L_END_CLASS

L_BEGIN_CLASS(Item)
L_RW_PROP(QString, id, setId, QString())
L_RW_PROP(QString, label, setLabel, QString())
L_END_CLASS

L_BEGIN_CLASS(Menu)
L_RW_PROP(QString, header, setHeader)
L_RW_PROP_ARRAY_WITH_ADDER(Item*, items, setItems)
L_END_CLASS

L_BEGIN_CLASS(MenuRoot)
L_RW_PROP(Menu*, menu, setMenu, nullptr)
L_END_CLASS

L_BEGIN_CLASS(FMoreInfo)
L_RW_PROP(QString, gps, setGps)
L_RW_PROP(bool, valid, setValid, false)
L_END_CLASS

L_BEGIN_CLASS(FPersonInfo)
L_RW_PROP(QString, name, setName)
L_RW_PROP(int, age, setAge, 0)
L_RW_PROP(QList<int>, identifiers, setIdentifiers)
L_RW_PROP(FMoreInfo*, more, setMore, nullptr)
L_END_CLASS

L_BEGIN_GADGET(MonitorSize)
L_RW_GPROP(int, w, setW)
L_RW_GPROP(int, h, setH)
L_END_GADGET

L_BEGIN_GADGET(Monitor)
L_RW_GPROP(QString, manufacturer, setManufacturer)
L_RW_GPROP(QString, model, setModel)
L_RW_GPROP(MonitorSize*, size, setSize, nullptr)
L_RW_GPROP(MonitorSize*, resolution, setResolution, nullptr)
L_END_GADGET

L_BEGIN_GADGET(KodiResponseItem)
L_RW_GPROP(QString, fanart, setFanart)
L_RW_GPROP(QString, label, setLabel)
L_RW_GPROP(int, id, setId)
L_RW_GPROP(QString, thumbnail, setThumbnail)
L_RW_GPROP(QString, title, setTitle)
L_RW_GPROP(QString, type, setType)
L_END_GADGET

L_BEGIN_GADGET(KodiResponseResult)
L_RW_GPROP(KodiResponseItem*, item, setItem, nullptr)
public:
    virtual ~KodiResponseResult() {
        delete m_item;
    }
L_END_GADGET

L_BEGIN_GADGET(KodiResponse)
L_RW_GPROP(int, id, setId)
L_RW_GPROP(QString, jsonrpc, setJsonrpc)
L_RW_GPROP(KodiResponseResult*, result, setResult, nullptr)
public:
    virtual ~KodiResponse() {
        delete m_result;
    }
L_END_GADGET

L_BEGIN_GADGET(KodiResponseVariant)
L_RW_GPROP(int, id, setId)
L_RW_GPROP(QString, jsonrpc, setJsonrpc)
L_RW_GPROP(QVariantHash, result, setResult)
L_END_GADGET

typedef QHash<QString, int> QHashStringInt;
typedef QMap<QString, QVariant> QMapStringInt;
typedef QMap<QString, QObject*> QMapStringObj;
Q_DECLARE_METATYPE(QHashStringInt)
Q_DECLARE_METATYPE(QMapStringInt)
Q_DECLARE_METATYPE(QMapStringObj)

L_BEGIN_GADGET(HashTest)
L_RW_GPROP_AS(QHashStringInt, test1)
L_RW_GPROP_AS(QMapStringInt, test2)
L_RW_GPROP_AS(QMapStringObj, test3)
L_RW_GPROP_AS(QList<QObject*>, test4)
L_END_GADGET

class MyRect : public QRectF
{
public:
    MyRect() : QRectF() {}
    MyRect(qreal x, qreal y, qreal w, qreal h) : QRectF(x, y, w, h) {}
    MyRect(const MyRect& r) : QRectF(r.x(), r.y(), r.width(), r.height()) {}
    MyRect& operator=(const MyRect& r) {
        this->setX(r.x());
        this->setY(r.y());
        this->setWidth(r.width());
        this->setHeight(r.height());
        return *this;
    }
    operator QString() const {
        return lqt::string_from_rect(*this);
    }
};

struct NotSerializable
{
    QString someString;
};
Q_DECLARE_METATYPE(NotSerializable)

L_BEGIN_GADGET(CustomTypes)
L_RW_GPROP_AS(MyRect, myRect, MyRect(0.1, 0.2, 0.3, 0.4))
L_RW_GPROP_AS(NotSerializable, notSerializable)
L_END_GADGET
Q_DECLARE_METATYPE(MyRect)

class LQObjectSerializerTest : public QObject
{
    Q_OBJECT
public:
    LQObjectSerializerTest();
    ~LQObjectSerializerTest();

private slots:
    void test_caseNull();
    void test_case1();
    void test_case2();
    void test_case3();
    void test_case4();
    void test_case5();
    void test_case6();
    void test_case7();
    void test_case8();
    void test_case9();
    void test_case10();
    void test_case11();
    void test_case12();
};

LQObjectSerializerTest::LQObjectSerializerTest()
{
    qRegisterMetaType<SomeQObject*>();
    qRegisterMetaType<SomeQObjectChild*>();
    qRegisterMetaType<SomeQObjectChild2*>();
    qRegisterMetaType<Glossary*>();
    qRegisterMetaType<GlossDivObj*>();
    qRegisterMetaType<GlossListObj*>();
    qRegisterMetaType<GlossEntryObj*>();
    qRegisterMetaType<GlossDefObj*>();
    qRegisterMetaType<Item*>();
    qRegisterMetaType<Menu*>();
    qRegisterMetaType<FMoreInfo*>();
    qRegisterMetaType<FPersonInfo*>();
    qRegisterMetaType<Monitor*>();
    qRegisterMetaType<MonitorSize*>();
    qRegisterMetaType<Monitor>();
    qRegisterMetaType<MonitorSize>();
    qRegisterMetaType<KodiResponseItem*>();
    qRegisterMetaType<KodiResponseResult*>();
    qRegisterMetaType<KodiResponse*>();
    qRegisterMetaType<KodiResponseItem>();
    qRegisterMetaType<KodiResponseResult>();
    qRegisterMetaType<KodiResponse>();
    qRegisterMetaType<MyRect>();
    qRegisterMetaType<QHashStringInt>();
    qRegisterMetaType<QMapStringInt>();
    qRegisterMetaType<QMapStringObj>();
    qRegisterMetaType<NotSerializable>();

    QMetaType::registerConverter<MyRect, QString>();
}

LQObjectSerializerTest::~LQObjectSerializerTest()
{}

class MyObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY labelChanged)
    QString m_id;
    QString m_label;

public:
    MyObject(QObject* parent = nullptr) : QObject(parent) {}

    const QString &id() const { return m_id; }
    void setId(const QString &newId) { if (m_id != newId) { m_id = newId; emit idChanged(); } }
    const QString &label() const { return m_label; }
    void setLabel(const QString &newLabel) { if (m_label != newLabel) { m_label = newLabel; emit labelChanged(); } }

signals:
    void idChanged();
    void labelChanged();
};

void LQObjectSerializerTest::test_caseNull()
{
    QScopedPointer<MyObject> i(new MyObject);
    i->setId("myid");
    i->setLabel(QString());

    QVERIFY(!i->id().isNull());
    QVERIFY(i->label().isNull());

    QMetaObject mo = MyObject::staticMetaObject;
    for (int j = mo.propertyOffset(); j < mo.propertyCount(); ++j) {
        QMetaProperty mp = mo.property(j);
        if (mp.name() == QString("id"))
            QVERIFY(!mp.read(i.data()).isNull());
        else if (mp.name() == QString("label")) {
            //QVERIFY(mp.read(i.data()).isNull());
            QVERIFY(mp.read(i.data()).toString().isNull());
        }
    }
}

void LQObjectSerializerTest::test_case1()
{
    SomeQObjectChild childObj;
    childObj.setObjectName(QSL("HELLO2"));
    childObj.setSomeString(QSL("SOME STRING"));
    SomeQObject someObj;
    SomeQObjectChild* childObj2 = new SomeQObjectChild(&someObj);
    childObj2->setSomeString("SOME STRING");
    SomeQObjectChild* childObj3 = new SomeQObjectChild(&someObj);
    childObj3->setSomeString("SOME STRING 2");
    someObj.setSomeInt(7);
    someObj.setSomeLong(std::numeric_limits<int>::max() + static_cast<qint64>(10));
    someObj.setSomeBool(true);
    someObj.setSomeDouble(7.6);
    someObj.setSomeString(QSL("HELLO"));
    someObj.setChild1(&childObj);
    someObj.setIntList(QList<int>()
                       << 3 << 7 << 19);
    someObj.setStringList(QStringList()
                          << "A" << "B" << "C" << "D");
    someObj.setObjectList(QList<SomeQObjectChild*>()
                          << childObj2
                          << childObj3);

    QScopedPointer<Item> i(new Item());
    QCOMPARE(i->label(), QString());
    i.reset((Item*)Item::staticMetaObject.newInstance());
    QCOMPARE(i->label(), "");
    QVERIFY(i->label() == "");
    QVERIFY(i->label().isNull());
    QVERIFY(QString().isNull());
    i->setLabel(QString());

    LSerializer serializer;
    QJsonObject json = serializer.serialize<SomeQObject>(&someObj);
    QJsonDocument doc(json);

    LDeserializer<SomeQObject> deserializer;
    QScopedPointer<SomeQObject> res(deserializer.deserialize(json));

    QCOMPARE(res->someInt(), 7);
    QCOMPARE(res->someLong(), std::numeric_limits<int>::max() + static_cast<qint64>(10));
    QCOMPARE(res->someBool(), true);
    QCOMPARE(res->someDouble(), 7.6);
    QCOMPARE(res->someString(), QSL("HELLO"));
    QCOMPARE(res->child1()->someString(), QSL("SOME STRING"));
    QCOMPARE(res->child2(), nullptr);
    QCOMPARE(res->intList().size(), 3);
    QCOMPARE(res->intList(), QList<int>() << 3 << 7 << 19);
    QCOMPARE(res->stringList(), QStringList() << "A" << "B" << "C" << "D");
    QCOMPARE(res->objectList().size(), 2);
    QCOMPARE(res->objectList()[0]->someString(), QSL("SOME STRING"));
    QCOMPARE(res->objectList()[1]->someString(), QSL("SOME STRING 2"));
}

void LQObjectSerializerTest::test_case2()
{
    QFile jsonFile(":/json_1.json");
    QVERIFY(jsonFile.open(QIODevice::ReadOnly));

    QObject bag;
    new GlossaryRoot(&bag);
    new Glossary(&bag);

    QByteArray jsonString = jsonFile.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(jsonString);
    QJsonObject json = doc.object();
    QVERIFY(!json.isEmpty());

    LDeserializer<GlossaryRoot> deserializer;
    QScopedPointer<GlossaryRoot> g(deserializer.deserialize(json));
    QVERIFY(g->glossary() != nullptr);
    QVERIFY(g->glossary()->GlossDiv() != nullptr);
    QVERIFY(g->glossary()->GlossDiv()->GlossList() != nullptr);
    QVERIFY(g->glossary()->GlossDiv()->GlossList()->GlossEntry() != nullptr);

    GlossEntryObj* entry = g->glossary()->GlossDiv()->GlossList()->GlossEntry();
    QCOMPARE(g->glossary()->title(), QSL("example glossary"));
    QCOMPARE(g->glossary()->GlossDiv()->title(), QSL("S"));
    QCOMPARE(entry->ID(), QSL("SGML"));
    QCOMPARE(entry->SortAs(), QSL("SGML"));
    QCOMPARE(entry->GlossTerm(), QSL("Standard Generalized Markup Language"));
    QCOMPARE(entry->Acronym(), QSL("SGML"));
    QCOMPARE(entry->Abbrev(), QSL("ISO 8879:1986"));
    QCOMPARE(entry->GlossDef()->para(), QSL("A meta-markup language, used to create markup languages such as DocBook."));
    QCOMPARE(entry->GlossDef()->GlossSeeAlso().size(), 2);
    QCOMPARE(entry->GlossDef()->GlossSeeAlso()[0], QSL("GML"));
    QCOMPARE(entry->GlossDef()->GlossSeeAlso()[1], QSL("XML"));
}

void LQObjectSerializerTest::test_case3()
{
    QFile jsonFile(":/json_2.json");
    QVERIFY(jsonFile.open(QIODevice::ReadOnly));

    QByteArray jsonString = jsonFile.readAll();

    LDeserializer<MenuRoot> deserializer;
    QScopedPointer<MenuRoot> g(deserializer.deserialize(jsonString));
    QVERIFY(g->menu() != nullptr);
    QCOMPARE(g->menu()->items().size(), 22);
    QCOMPARE(g->menu()->items().at(0)->id(), QSL("Open"));
    QCOMPARE(g->menu()->items().at(0)->label(), QString());
    QCOMPARE(g->menu()->items().at(1)->id(), QSL("OpenNew"));
    QCOMPARE(g->menu()->items().at(1)->label(), QSL("Open New"));
    QCOMPARE(g->menu()->items().at(2), nullptr);
    QVERIFY(g->menu()->items().at(0)->label().isNull());

    LSerializer serializer;
    QJsonObject obj = serializer.serialize<MenuRoot>(g.data());
    QJsonDocument doc = QJsonDocument::fromJson(jsonString);
    QJsonObject json = doc.object();
    QCOMPARE(obj, json);
}

void LQObjectSerializerTest::test_case4()
{
    QFile jsonFile(":/json_3.json");
    QVERIFY(jsonFile.open(QIODevice::ReadOnly));

    QByteArray jsonString = jsonFile.readAll();

    LDeserializer<FPersonInfo> deserializer;
    QScopedPointer<FPersonInfo> g(deserializer.deserialize(jsonString));
    QVERIFY(g);
    QCOMPARE(g->age(), 33);
    QCOMPARE(g->name(), "Andrew");
    QCOMPARE(g->identifiers().size(), 4);
    QCOMPARE(g->identifiers(), QList<int>() << 32 << 45 << 67 << 78);
    QVERIFY(g->more());
    QVERIFY(g->more()->valid());
    QCOMPARE(g->more()->gps(), QSL("44.9064', W073° 59.0735'"));
}

void LQObjectSerializerTest::test_case5()
{
    MonitorSize size;
    size.setW(1117);
    size.setH(644);

    MonitorSize res;
    res.setW(3840);
    res.setH(2160);

    Monitor monitor;
    monitor.setManufacturer(QSL("Samsung"));
    monitor.setModel(QSL("UE50TU7090U"));
    monitor.setSize(&size);
    monitor.setResolution(&res);

    QJsonObject json = LSerializer().serialize<Monitor>(&monitor);
    QVERIFY(json.contains(QSL("manufacturer")));
    QVERIFY(json.contains(QSL("model")));
    QVERIFY(json.contains(QSL("resolution")));
    QVERIFY(json.contains(QSL("size")));
    QVERIFY(json[QSL("resolution")].toObject().contains(QSL("w")));
    QVERIFY(json[QSL("resolution")].toObject().contains(QSL("h")));
    QVERIFY(json[QSL("size")].toObject().contains(QSL("w")));
    QVERIFY(json[QSL("size")].toObject().contains(QSL("h")));
    QCOMPARE(json[QSL("resolution")].toObject()[QSL("w")], 3840);
    QCOMPARE(json[QSL("resolution")].toObject()[QSL("h")], 2160);
    QCOMPARE(json[QSL("size")].toObject()[QSL("w")], 1117);
    QCOMPARE(json[QSL("size")].toObject()[QSL("h")], 644);

    QFile jsonFile(":/json_4.json");
    QVERIFY(jsonFile.open(QIODevice::ReadOnly));

    QByteArray jsonString = jsonFile.readAll();

    LDeserializer<Monitor> deserializer;
    QScopedPointer<Monitor> m(deserializer.deserialize(jsonString));

    QVERIFY(m);
    QCOMPARE(m->manufacturer(), QSL("Samsung"));
    QCOMPARE(m->model(), QSL("Some real model"));
    QVERIFY(m->size());
    QCOMPARE(m->size()->w(), 1920);

    delete m->size();
    delete m->resolution();
}

void LQObjectSerializerTest::test_case6()
{
    QFile jsonFile(":/json_5.json");
    QVERIFY(jsonFile.open(QIODevice::ReadOnly));

    QByteArray jsonString = jsonFile.readAll();

    LDeserializer<KodiResponse> deserializer;
    QScopedPointer<KodiResponse> m(deserializer.deserialize(jsonString));

    QVERIFY(m);
    QCOMPARE(m->id(), 12345);
    QCOMPARE(m->result()->item()->type(), QSL("channel"));
}

void LQObjectSerializerTest::test_case7()
{
    QFile jsonFile(":/json_5.json");
    QVERIFY(jsonFile.open(QIODevice::ReadOnly));

    QByteArray jsonString = jsonFile.readAll();

    LDeserializer<KodiResponseVariant> deserializer;
    QScopedPointer<KodiResponseVariant> m(deserializer.deserialize(jsonString));

    QVERIFY(m);
    QCOMPARE(m->id(), 12345);
    QCOMPARE(m->result()["item"].toHash()["type"].toString(), QSL("channel"));
}

void LQObjectSerializerTest::test_case8()
{
    KodiResponseVariant response;
    response.setId(0);
    response.setResult(QVariantHash {
        { QSL("author"), QSL("Luca Carlon") },
        { QSL("dates"), QVariantHash {
            { QSL("today"), QSL("2023.12.06") },
            { QSL("yesterday"), QSL("2023.12.05") },
        }},
        { QSL("numbers"), QVariantList { 1, 2, 3 } }
    });

    LSerializer serializer;
    QJsonObject json = serializer.serialize<KodiResponseVariant>(&response);
    QCOMPARE(json["result"].toObject()["author"].toString(), QSL("Luca Carlon"));
    QCOMPARE(json["result"].toObject()["dates"].toObject()["today"].toString(), QSL("2023.12.06"));
    QCOMPARE(json["result"].toObject()["numbers"].toArray()[2].toInt(), 3);
}

void LQObjectSerializerTest::test_case9()
{
    CustomTypes customTypes;

    LSerializer serializer;
    QJsonObject json = serializer.serialize<CustomTypes>(&customTypes);

    QCOMPARE(json["myRect"].toString(), QSL("0.1,0.2,0.3,0.4"));
}

void LQObjectSerializerTest::test_case10()
{
    HashTest hashTest;
    hashTest.set_test1({ { QSL("test1"), 1 }, { QSL("test2"), 2 } });
    hashTest.set_test2({ { QSL("test1"), 1 }, { QSL("test2"), 2 } });

    LSerializer serializer;
    QJsonObject json = serializer.serialize<HashTest>(&hashTest);

    QCOMPARE(json["test1"].toObject()["test1"].toInt(), 1);
    QCOMPARE(json["test1"].toObject()["test2"].toInt(), 2);
    QCOMPARE(json["test2"].toObject()["test1"].toInt(), 1);
    QCOMPARE(json["test2"].toObject()["test2"].toInt(), 2);
}

void LQObjectSerializerTest::test_case11()
{
    QScopedPointer<QObject> qo1(new QObject);
    qo1->setObjectName(QSL("Name1"));
    QScopedPointer<QObject> qo2(new QObject);
    qo2->setObjectName(QSL("Name2"));

    HashTest hashTest;
    hashTest.set_test3({ { QSL("test1"), qo1.data() }, { QSL("test2"), qo2.data() } });
    hashTest.set_test4({ qo1.data(), qo2.data() });

    LSerializer serializer;
    QJsonObject json = serializer.serialize<HashTest>(&hashTest);

    QCOMPARE(json["test4"].toArray()[0].toObject()["objectName"], "Name1");
    QCOMPARE(json["test3"].toObject()["test1"].toObject()["objectName"], "Name1");
}

void LQObjectSerializerTest::test_case12()
{
    QList<int> list;
    for (int i = 0; i < 100; i++)
        list.append(i);

    QJsonArray json = LSerializer().serialize(list);

    QCOMPARE(json.size(), list.size());
    for (int i = 0; i < 100; i++)
        QCOMPARE(json[i].toInt(), i);

    QList<double> _list = LDeserializer<int>().deserializeNumberArray(json);

    for (int i = 0; i < _list.size(); i++)
        QVERIFY(_list[i] - i < 1E-6);
}

QTEST_GUILESS_MAIN(LQObjectSerializerTest)

#include "tst_lqobjectserializertest.moc"
