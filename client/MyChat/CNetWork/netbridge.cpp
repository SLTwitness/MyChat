#include "netbridge.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include "ErrorCode.h"
#include "tcpmgr.h"

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
            emit verifySend(false,"error : "+reply->errorString());
            reply->deleteLater();
            return;
        }
        else{
            QByteArray data=reply->readAll();
            QJsonParseError errJson;
            QJsonDocument doc=QJsonDocument::fromJson(data,&errJson);
            if(errJson.error!=QJsonParseError::NoError){
                emit verifySend(false,"json解析错误");
                reply->deleteLater();
                return;
            }
            QJsonObject obj=doc.object();
            int err=obj["error"].toInt();
            if(err==errorCode::Success){
                emit verifySend(true,"邮件发送成功");
            }
            else emit verifySend(false,"邮件发送失败");
        }
        reply->deleteLater();
    });
    return;
}

void NetBridge::registerRequest(const QString &user, const QString &email, const QString &passwd, const QString &verifycode)
{
    QNetworkRequest req(QUrl("http://127.0.0.1:8080/user_register"));
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");

    QJsonObject obj;
    obj["user"]=user;
    obj["email"]=email;
    obj["passwd"]=passwd;
    obj["verifycode"]=verifycode;
    QJsonDocument doc(obj);

    auto reply=manager->post(req,doc.toJson());

    connect(reply,&QNetworkReply::finished,this,[=](){
        if(reply->error()!=QNetworkReply::NoError){
            emit registerSend(false,"error : "+reply->errorString());
            reply->deleteLater();
            return;
        }
        else{
            QByteArray data=reply->readAll();
            QJsonParseError errJson;
            QJsonDocument doc=QJsonDocument::fromJson(data,&errJson);
            if(errJson.error!=QJsonParseError::NoError){
                emit registerSend(false,"json解析错误");
                reply->deleteLater();
                return;
            }
            QJsonObject obj=doc.object();
            int err=obj["error"].toInt();
            if(err==errorCode::Success){
                emit registerSend(true,"注册成功");
            }
            else if(err==errorCode::VerifyExpired){
                emit registerSend(false,"验证码已过期");
            }
            else if(err==errorCode::VerifyCodeErr){
                emit registerSend(false,"验证码错误");
            }
            else if(err==errorCode::UserExist){
                emit registerSend(false,"用户已存在");
            }
            else emit registerSend(false,"未知错误");
        }
        reply->deleteLater();
    });
    return;
}

void NetBridge::loginRequst(const QString &email, const QString &passwd)
{
    QNetworkRequest req(QUrl("http://127.0.0.1:8080/user_login"));
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");

    QJsonObject obj;
    obj["email"]=email;
    obj["passwd"]=passwd;
    QJsonDocument doc(obj);

    auto reply=manager->post(req,doc.toJson());

    connect(reply,&QNetworkReply::finished,this,[=](){
        if(reply->error()!=QNetworkReply::NoError){
            emit loginSend(false,"error : "+reply->errorString(),"",0,0,"");
            reply->deleteLater();
            return;
        }
        else{
            QByteArray data=reply->readAll();
            QJsonParseError errJson;
            QJsonDocument doc=QJsonDocument::fromJson(data,&errJson);
            if(errJson.error!=QJsonParseError::NoError){
                emit loginSend(false,"json解析错误","",0,0,"");
                reply->deleteLater();
                return;
            }
            QJsonObject obj=doc.object();
            int err=obj["error"].toInt();
            if(err==errorCode::Success){
                qDebug() << "login success, start connect chatserver";
                QString host=obj["host"].toString();
                QString port_str=obj["port"].toString();
                int port=port_str.toInt();
                int uid=obj["uid"].toInt();
                QString token=obj["token"].toString();

                emit loginSend(true,"登入成功",host,port,uid,token);
            }
            else if(err==errorCode::PasswdInvalid){
                emit loginSend(false,"密码不正确","",0,0,"");
            }
            else if(err==errorCode::RPCFailed){
                emit loginSend(false,"连接错误","",0,0,"");
            }
        }
        reply->deleteLater();
    });
    return;
}

