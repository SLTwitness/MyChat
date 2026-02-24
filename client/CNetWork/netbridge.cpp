#include "netbridge.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

NetBridge::NetBridge(QObject *parent)
    : QObject{parent}
{
    manager=new QNetworkAccessManager(this);
}

void NetBridge::emailRequest(const QString &email)
{
    QNetworkRequest req(QUrl("http://127.0.0.1:8080/get_verifycode"));
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");

    QJsonObject obj;
    obj["email"]=email;
    QJsonDocument doc(obj);

    auto reply=manager->post(req,doc.toJson());

    connect(reply,&QNetworkReply::finished,this,[=](){      //值捕获拷贝reply，引用捕获的话在后面delete时，捕获野指针会导致程序崩溃
        if(reply->error()!=QNetworkReply::NoError){
            emit verifyCode("error : "+reply->errorString());
        }
        else{
            /*测试用：读取验证码cur_VerifyCode*/
            auto data=reply->readAll();
            QJsonObject obj=QJsonDocument::fromJson(data).object();
            curVerifyCode=obj["code"].toString();
            qDebug()<<curVerifyCode;
            /*测试用：读取验证码cur_VerifyCode*/

            emit verifyCode(data);
        }
        reply->deleteLater();
    });
}

QString NetBridge::getCode()
{
    return curVerifyCode;
}

