#include <QtTest>

class LQObjectSerializerTest : public QObject
{
	Q_OBJECT
public:
	LQObjectSerializerTest();
	~LQObjectSerializerTest();

private slots:
	void test_case1();
};

LQObjectSerializerTest::LQObjectSerializerTest()
{

}

LQObjectSerializerTest::~LQObjectSerializerTest()
{

}

void LQObjectSerializerTest::test_case1()
{
	QVERIFY("HELLO" == "HELLO");
}

QTEST_APPLESS_MAIN(LQObjectSerializerTest)

#include "tst_lqobjectserializertest.moc"
