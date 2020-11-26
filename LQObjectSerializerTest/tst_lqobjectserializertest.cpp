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
 * furnished to do 9so, subject to the following conditions:
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
L_RW_PROP(GlossDefObj*, GlossDef, setGlossDef)
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
L_RW_PROP(bool, valid, setValid)
L_END_CLASS

L_BEGIN_CLASS(FPersonInfo)
L_RW_PROP(QString, name, setName)
L_RW_PROP(int, age, setAge)
L_RW_PROP(QList<int>, identifiers, setIdentifiers)
L_RW_PROP(FMoreInfo*, more, setMore)
L_END_CLASS

L_BEGIN_GADGET(MonitorSize)
L_RW_GPROP(int, w, setW)
L_RW_GPROP(int, h, setH)
L_END_GADGET

L_BEGIN_GADGET(Monitor)
L_RW_GPROP(QString, manufacturer, setManufacturer)
L_RW_GPROP(QString, model, setModel)
L_RW_GPROP(MonitorSize*, size, setSize)
L_RW_GPROP(MonitorSize*, resolution, setResolution)
L_END_GADGET

class LQObjectSerializerTest : public QObject
{
    Q_OBJECT
public:
    LQObjectSerializerTest();
    ~LQObjectSerializerTest();

private slots:
    void test_case1();
    void test_case2();
    void test_case3();
    void test_case4();
    void test_case5();
};

LQObjectSerializerTest::LQObjectSerializerTest()
{
    qRegisterMetaType<void*>();
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
}

LQObjectSerializerTest::~LQObjectSerializerTest()
{}

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

    LSerializer serializer;
    QJsonObject json = serializer.serialize<SomeQObject>(&someObj);
    QJsonDocument doc(json);

    qDebug().noquote() << QString(doc.toJson(QJsonDocument::Indented));

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

    new GlossaryRoot(qApp);
    new Glossary(qApp);

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

    LSerializer serializer;
    QJsonObject obj = serializer.serialize<MenuRoot>(g.data());
    QJsonDocument doc = QJsonDocument::fromJson(jsonString);
    QJsonObject json = doc.object();
    qDebug().noquote() << json;
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
#if 0
    QFile jsonFile(":/json_4.json");
    QVERIFY(jsonFile.open(QIODevice::ReadOnly));

    QByteArray jsonString = jsonFile.readAll();
#endif

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
}

QTEST_APPLESS_MAIN(LQObjectSerializerTest)

#include "tst_lqobjectserializertest.moc"
