#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "CNetWork/netbridge.h"
#include "CNetWork/tcpmgr.h"
#include "CNetWork/usermgr.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/MyChat/main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);

    qmlRegisterSingletonType<NetBridge>("Net",1,0,"NetBridge",[](QQmlEngine*,QJSEngine*){
        return new NetBridge();
    });

    TcpMgr* tcpMgr = new TcpMgr();
    qmlRegisterSingletonInstance("Tcp",1,0,"TcpMgr", tcpMgr);

    UserMgr* userMgr = new UserMgr();
    tcpMgr->setUserMgr(userMgr);
    qmlRegisterSingletonInstance("User",1,0,"UserMgr",userMgr);

    engine.load(url);

    return app.exec();
}
