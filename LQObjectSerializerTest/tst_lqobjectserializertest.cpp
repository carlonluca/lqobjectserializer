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
#include "deps/lqtutils/lqtutils.h"

class SomeQObjectChild2 : public QObject
{
    Q_OBJECT
    L_RW_PROP(QString, someString, setSomeString, QString())
public:
        Q_INVOKABLE SomeQObjectChild2(QObject* parent = nullptr) : QObject(parent) {}
    ~SomeQObjectChild2() { qDebug() << Q_FUNC_INFO; }
};

class SomeQObjectChild : public QObject
{
    Q_OBJECT
    L_RW_PROP(QString, someString, setSomeString, QString())
    public:
        Q_INVOKABLE SomeQObjectChild(QObject* parent = nullptr) : QObject(parent) {}
    ~SomeQObjectChild() { qDebug() << Q_FUNC_INFO; }
};

class SomeQObject : public QObject
{
    Q_OBJECT
    L_RW_PROP(int, someInt, setSomeInt, 0)
    L_RW_PROP(qint64, someLong, setSomeLong, 0)
    L_RW_PROP(bool, someBool, setSomeBool, false)
    L_RW_PROP(double, someDouble, setSomeDouble, 0)
    L_RW_PROP(QString, someString, setSomeString, QString())
    L_RW_PROP(SomeQObjectChild*, child1, setChild1, nullptr)
    L_RW_PROP(SomeQObjectChild2*, child2, setChild2, nullptr)
    L_RW_PROP(QList<int>, intList, setIntList, QList<int>())
    L_RW_PROP(QList<QString>, stringList, setStringList, QList<QString>())
    L_RW_PROP(QList<SomeQObjectChild*>, objectList, setObjectList, QList<SomeQObjectChild*>())
public:
    Q_INVOKABLE SomeQObject(QObject* parent = nullptr) :
      QObject(parent), m_someInt(0), m_child1(nullptr), m_child2(nullptr) {}
    ~SomeQObject() { qDebug() << Q_FUNC_INFO; }

public slots:
    void add_objectList(QObject* child) { m_objectList.append(static_cast<SomeQObjectChild*>(child)); }
};

class GlossDefObj : public QObject
{
    Q_OBJECT
    L_RW_PROP(QString, para, setPara, QString())
    L_RW_PROP(QList<QString>, GlossSeeAlso, setGlossSeeAlso)
public:
    Q_INVOKABLE GlossDefObj(QObject* parent = nullptr) : QObject(parent) {}
};

class GlossEntryObj : public QObject
{
    Q_OBJECT
    L_RW_PROP(QString, ID, setID)
    L_RW_PROP(QString, SortAs, setSortAs)
    L_RW_PROP(QString, GlossTerm, setGlossTerm)
    L_RW_PROP(QString, Acronym, setAcronym)
    L_RW_PROP(QString, Abbrev, setAbbrev)
    L_RW_PROP(QString, GlossSee, setGlossSee)
    L_RW_PROP(GlossDefObj*, GlossDef, setGlossDef)
public:
    Q_INVOKABLE GlossEntryObj(QObject* parent = nullptr) : QObject(parent) {}
};

class GlossListObj : public QObject
{
    Q_OBJECT
    L_RW_PROP(GlossEntryObj*, GlossEntry, setGlossEntry, nullptr)
public:
    Q_INVOKABLE GlossListObj(QObject* parent = nullptr) : QObject(parent) {}
};

class GlossDivObj : public QObject
{
    Q_OBJECT
    L_RW_PROP(QString, title, setTitle)
    L_RW_PROP(GlossListObj*, GlossList, setGlossList, nullptr)
public:
    Q_INVOKABLE GlossDivObj(QObject* parent = nullptr) : QObject(parent) {}
};

class Glossary : public QObject
{
    Q_OBJECT
    L_RW_PROP(QString, title, setTitle)
    L_RW_PROP(GlossDivObj*, GlossDiv, setGlossDiv, nullptr)
public:
    Q_INVOKABLE Glossary(QObject* parent = nullptr) : QObject(parent) {}
};

class GlossaryRoot : public QObject
{
    Q_OBJECT
    L_RW_PROP(Glossary*, glossary, setGlossary, nullptr)
public:
    Q_INVOKABLE GlossaryRoot(QObject* parent = nullptr) : QObject(parent) {}
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
    void test_case3();
};

class Item : public QObject
{
    Q_OBJECT
    L_RW_PROP(QString, id, setId, QString())
    L_RW_PROP(QString, label, setLabel, QString())
public:
    Q_INVOKABLE Item(QObject* parent = nullptr) : QObject(parent) {}
};

class Menu : public QObject
{
    Q_OBJECT
    L_RW_PROP(QString, header, setHeader)
    L_RW_PROP(QList<Item*>, items, setItems)
public:
    Q_INVOKABLE Menu(QObject* parent = nullptr) : QObject(parent) {}
    Q_INVOKABLE void add_items(QObject* obj) { m_items.append(static_cast<Item*>(obj)); }
};

class MenuRoot : public QObject
{
    Q_OBJECT
    L_RW_PROP(Menu*, menu, setMenu, nullptr)
public:
    Q_INVOKABLE MenuRoot(QObject* parent = nullptr) : QObject(parent) {}
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

    QByteArray jsonString = jsonFile.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(jsonString);
    QJsonObject json = doc.object();
    QVERIFY(!json.isEmpty());

    QHash<QString, QMetaObject> factory {
        { QSL("Glossary*"), Glossary::staticMetaObject },
        { QSL("GlossDivObj*"), GlossDivObj::staticMetaObject },
        { QSL("GlossListObj*"), GlossListObj::staticMetaObject },
        { QSL("GlossEntryObj*"), GlossEntryObj::staticMetaObject },
        { QSL("GlossDefObj*"), GlossDefObj::staticMetaObject }
    };

    LDeserializer<GlossaryRoot> deserializer(factory);
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
    QJsonDocument doc = QJsonDocument::fromJson(jsonString);
    QJsonObject json = doc.object();
    QVERIFY(!json.isEmpty());

    QHash<QString, QMetaObject> factory {
        { QSL("Item*"), Item::staticMetaObject },
        { QSL("Menu*"), Menu::staticMetaObject }
    };

    LDeserializer<MenuRoot> deserializer(factory);
    QScopedPointer<MenuRoot> g(deserializer.deserialize(json));
    QVERIFY(g->menu() != nullptr);
    QCOMPARE(g->menu()->items().size(), 22);
    QCOMPARE(g->menu()->items().at(0)->id(), QSL("Open"));
    QCOMPARE(g->menu()->items().at(0)->label(), QString());
    QCOMPARE(g->menu()->items().at(1)->id(), QSL("OpenNew"));
    QCOMPARE(g->menu()->items().at(1)->label(), QSL("Open New"));
    QCOMPARE(g->menu()->items().at(2), nullptr);
}

QTEST_APPLESS_MAIN(LQObjectSerializerTest)

#include "tst_lqobjectserializertest.moc"
