﻿/**
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
#include <QNetworkAccessManager>

#include "../LQObjectSerializer/lserializer.h"
#include "../deps/lqtutils/lqtutils_qsl.h"

L_BEGIN_GADGET(LGHOwner)
L_RW_GPROP(QString, login, setLogin, "login")
L_RW_GPROP(int, id, setId, 8)
L_RW_GPROP(QString, node_id, setNode_id, "node_id")
L_RW_GPROP(QString, avatar_url, setAvatar_url, "avatar")
L_END_GADGET
Q_DECLARE_METATYPE(LGHOwner*)

L_BEGIN_GADGET(LGHRepo)
L_RW_GPROP(int, id, setId)
L_RW_GPROP(QString, node_id, setNode_id)
L_RW_GPROP(QString, name, setName)
L_RW_GPROP(QString, full_name, setFull_name)
L_RW_GPROP(LGHOwner*, owner, setOwner)
L_END_GADGET
Q_DECLARE_METATYPE(LGHRepo*)

class LGithubTestCase : public QObject
{
    Q_OBJECT
public:
    LGithubTestCase();

private slots:
    void test_case1();
};

LGithubTestCase::LGithubTestCase()
{
    qRegisterMetaType<void*>();
    qRegisterMetaType<LGHOwner*>();
    qRegisterMetaType<LGHRepo*>();
    qRegisterMetaType<LGHOwner>();
    qRegisterMetaType<LGHRepo>();
}

void LGithubTestCase::test_case1()
{
    QEventLoop loop;
    QNetworkAccessManager manager;
    QString body;
    connect(&manager, &QNetworkAccessManager::finished, qApp, [&loop, &body] (QNetworkReply* reply) {
        body.append(reply->readAll());
        loop.quit();
    });
    manager.get(QNetworkRequest(QUrl(QSL("https://api.github.com/users/carlonluca/repos?per_page=100"))));
    loop.exec();

    QVERIFY(!body.isEmpty());

    QJsonArray jsonArray = QJsonDocument::fromJson(body.toUtf8()).array();
    lqo::Deserializer<LGHRepo> deserializer;
    QList<LGHRepo*> repos = deserializer.deserializeObjectArray(jsonArray);
    QVERIFY(repos.size() > 0);
    QVERIFY(std::find_if(repos.begin(), repos.end(), [] (LGHRepo* repo) -> bool {
        return repo->name() == QSL("lqobjectserializer");
    }) != std::end(repos));
#if 1
    QVERIFY((*std::find_if(repos.begin(), repos.end(), [] (LGHRepo* repo) -> bool {
        return repo->name() == QSL("lqobjectserializer");
    }))->owner()->login() == QSL("carlonluca"));
#endif

    // Free.
    foreach (LGHRepo* repo, repos) {
        delete repo->owner();
        delete repo;
    }
}

QTEST_GUILESS_MAIN(LGithubTestCase)

#include "tst_githubtestcase.moc"
