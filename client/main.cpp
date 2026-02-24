#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "CNetWork/netbridge.h"

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

    engine.load(url);

    return app.exec();
}
