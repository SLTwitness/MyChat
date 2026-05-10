#include "tcpmgr.h"
#include <QDataStream>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "const.h"

TcpMgr::TcpMgr(QObject *parent):QObject(parent)
{
    qDebug() << "TcpMgr constructed this=" << this;
    socket=new QTcpSocket(this);
    qDebug() << "socket created=" << socket;

    connect(socket,&QTcpSocket::connected,this,[=](){
        qDebug()<<"connected to ChatServer success";
    });

    connect(socket,&QTcpSocket::readyRead,this,[=](){
        QByteArray data=socket->readAll();

        QDataStream stream(data);
        stream.setByteOrder(QDataStream::BigEndian);

        short msg_id;
        short msg_len;
        stream>>msg_id;
        stream>>msg_len;

        QByteArray body=data.mid(4,msg_len);
        qDebug()<<"recv msg_id="<<msg_id;
        qDebug()<<"recv body="<<body;

        QJsonParseError err;
        QJsonDocument doc=QJsonDocument::fromJson(body,&err);

        if(err.error!=QJsonParseError::NoError){
            qDebug()<<"json parse error";
            return;
        }

        QJsonObject obj=doc.object();

        //登录回包
        if(msg_id == MSG_CHAT_LOGIN_RSP){
            int err = obj["error"].toInt();
            if(err == 0){
                //初始化用户信息
                _userInfo->setUserInfo(obj["uid"].toInt(),obj["name"].toString(),
                                    obj["nick"].toString(),obj["icon"].toString(),obj["sex"].toInt());

                //加载好友申请列表
                QJsonArray applyList=obj["apply_list"].toArray();
                for(auto apply:applyList){
                    QJsonObject applyObj=apply.toObject();
                    qDebug()<<"正在加载好友申请列表...";
                    _userInfo->addFriendApply(applyObj["uid"].toInt(),applyObj["name"].toString(),applyObj["sex"].toInt(),applyObj["icon"].toString(),applyObj["status"].toInt());
                    qDebug()<<"读取到："<<applyObj["uid"].toInt()<<",name="<<applyObj["name"].toString()<<",sex="<<applyObj["sex"].toInt()<<",icon:"<<applyObj["icon"].toString()<<",status:"<<applyObj["status"].toInt();
                }

                //加载好友列表
                QJsonArray friendList=obj["friend_list"].toArray();
                for(auto _friend:friendList){
                    QJsonObject friendObj=_friend.toObject();
                    qDebug()<<"正在加载好友列表...";
                    _userInfo->addFriend(friendObj["uid"].toInt(),friendObj["name"].toString(),friendObj["sex"].toInt(),friendObj["icon"].toString());
                    qDebug()<<"读取到："<<friendObj["uid"].toInt()<<",name="<<friendObj["name"].toString()<<",sex="<<friendObj["sex"].toInt()<<",icon:"<<friendObj["icon"].toString();
                }
            }
            return;
        }

        //搜索用户回包
        if(msg_id==ID_SEARCH_USER_RSP){
            int err=obj["error"].toInt();
            if(err!=0){
                emit searchUserRes(false,-1,"",1,"");
            }
            else{
                emit searchUserRes(true,obj["uid"].toInt(),obj["name"].toString(),obj["sex"].toInt(),obj["icon"].toString());
            }
            return;
        }

        //接收好友申请
        if(msg_id==ID_NOTIFY_ADD_FRIEND_REQ){
            int err=obj["error"].toInt();
            if(err!=0){
                qDebug() << "WRONG";
            }
            else{
                int applyuid = obj["applyuid"].toInt();
                QString name = obj["name"].toString();
                QString desc = obj["desc"].toString();
                QString icon = obj["icon"].toString();
                int sex = obj["sex"].toInt();
                QString nick = obj["nick"].toString();

                qDebug() << "[Notify Add Friend]";
                qDebug() << "applyuid =" << applyuid;
                qDebug() << "name =" << name;
                qDebug() << "icon =" << icon;
                qDebug() << "desc =" << desc;
                qDebug() << "sex =" << sex;
                qDebug() << "nick =" << nick;
                _userInfo->addFriendApply(applyuid,name,sex,icon,0);

                emit notifyAddFriend(applyuid,name,sex,icon);
            }
            return;
        }

        //申请方接收好友认证通过
        if(msg_id==ID_NOTIFY_AUTH_FRIEND_REQ){
            int err=obj["error"].toInt();
            if(err!=0){
                qDebug() << "WRONG";
            }
            else{
                int fromuid = obj["fromuid"].toInt();
                int touid = obj["touid"].toInt();
                QString name=obj["name"].toString();
                QString icon=obj["icon"].toString();
                int sex=obj["sex"].toInt();


                qDebug() << "[Auth Friend Notify]";
                qDebug() << fromuid<<touid<<name<<icon<<sex;
                _userInfo->addFriend(fromuid,name,sex,icon);

                emit notifyAuthFriend(fromuid,touid,name,sex,icon);
            }
            return;
        }

        //接收好友的发送消息
        if(msg_id==ID_TEXT_CHAT_MSG_REQ){
            int err=obj["error"].toInt();
            if(err!=0){
                qDebug() << "WRONG";
            }
            else{
                int fromuid = obj["fromuid"].toInt();
                int touid = obj["touid"].toInt();

                QJsonArray arr=obj["text_array"].toArray();

                for(auto msg_val:arr){
                    QJsonObject msgObj=msg_val.toObject();
                    QString content=msgObj["content"].toString();
                    QString msgid=msgObj["msgid"].toString();

                    qDebug() << "[Recv Chat]";
                    qDebug() << fromuid<<"->"<<touid<<content<<msgid;

                    //临时：获取头像
                    QString senderIcon = _userInfo->getIconByUid(fromuid);
                    if (senderIcon.isEmpty()) {
                        senderIcon = "qrc:/MyChat/Icon/userImg_test.jpg";
                    }
                    _userInfo->addChatMsg(fromuid,content,false,senderIcon);

                    emit recvChatMsg(fromuid,content);
                }
            }
            return;
        }
    });

    connect(socket,&QTcpSocket::errorOccurred,this,[=](QAbstractSocket::SocketError err){
        Q_UNUSED(err);
        qDebug()<<"socket error: "<<socket->errorString();
    });
}

void TcpMgr::connectToChatServer(const QString &host, int port, int uid, const QString &token)
{
    qDebug()<<"prepare connect ChatServer: ";
    qDebug()<<"host="<<host;
    qDebug()<<"port="<<port;
    qDebug()<<"uid="<<uid;
    qDebug()<<"token="<<token;

    socket->connectToHost(host,port);
    connect(socket,&QTcpSocket::connected,this,[=](){
       loginSend(uid,token);
    });
}

void TcpMgr::loginSend(int uid, const QString &token)
{
    QJsonObject obj;
    obj["uid"]=uid;
    obj["token"]=token;

    QJsonDocument doc(obj);
    QByteArray body=doc.toJson(QJsonDocument::Compact);

    short msg_id=MSG_CHAT_LOGIN;
    short msg_len=body.size();

    QByteArray packet;
    QDataStream stream(&packet,QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);

    stream<<msg_id;
    stream<<msg_len;

    packet.append(body);
    socket->write(packet);
    socket->flush();

    qDebug()<<"msg_id="<<msg_id;
    qDebug()<<"msg_len="<<msg_len;
    qDebug()<<"body="<<body;
    qDebug()<<"packet hex="<<packet.toHex();
}

void TcpMgr::searchUserSend(const QString &uid_name)
{
    QJsonObject obj;

    obj["uid"]=uid_name;

    QJsonDocument doc(obj);
    QByteArray body=doc.toJson(QJsonDocument::Compact);

    short msg_id=ID_SEARCH_USER_REQ;
    short msg_len=body.size();

    QByteArray packet;
    QDataStream stream(&packet,QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);

    stream<<msg_id;
    stream<<msg_len;

    packet.append(body);
    socket->write(packet);
    socket->flush();

    qDebug() << "[Search Send]";
    qDebug() << "msg_id=" << msg_id;
    qDebug() << "body=" << body;
}

void TcpMgr::addFriendSend(int uid, int touid, const QString &applyname)
{
    QJsonObject obj;
    obj["uid"]=uid;
    obj["touid"]=touid;
    obj["applyname"]=applyname;

    QJsonDocument doc(obj);
    QByteArray body=doc.toJson(QJsonDocument::Compact);

    short msg_id=ID_ADD_FRIEND_REQ;
    short msg_len=body.size();

    QByteArray packet;
    QDataStream stream(&packet,QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);

    stream<<msg_id;
    stream<<msg_len;

    packet.append(body);
    socket->write(packet);
    socket->flush();

    qDebug() << "[Send Friend Apply]";
    qDebug() << "msg_id=" << msg_id;
    qDebug() << "body=" << body;
}

void TcpMgr::authFriendApply(int fromuid, int touid, const QString &back_name)
{
    QJsonObject obj;
    obj["fromuid"]=fromuid;
    obj["touid"]=touid;
    obj["back_name"]=back_name;

    QJsonDocument doc(obj);
    QByteArray body=doc.toJson(QJsonDocument::Compact);

    short msg_id=ID_AUTH_FRIEDN_REQ;
    short msg_len=body.size();

    QByteArray packet;
    QDataStream stream(&packet,QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);

    stream<<msg_id;
    stream<<msg_len;

    packet.append(body);
    socket->write(packet);
    socket->flush();

    qDebug() << "[Send Friend Auth]"<<packet;
}

void TcpMgr::sendChatMsg(int fromuid, int touid, const QString &content)
{
    QJsonObject root;
    root["fromuid"]=fromuid;
    root["touid"]=touid;

    QJsonArray arr;
    QJsonObject msgObj;
    msgObj["content"]=content;

    QString msgid = QString::number(QDateTime::currentMSecsSinceEpoch());
    msgObj["msgid"]=msgid;

    arr.append(msgObj);
    root["text_array"]=arr;

    QJsonDocument doc(root);
    QByteArray body=doc.toJson(QJsonDocument::Compact);

    QByteArray packet;
    QDataStream stream(&packet,QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);

    short msg_id=ID_TEXT_CHAT_MSG_REQ;
    short msg_len=body.size();

    stream<<msg_id;
    stream<<msg_len;

    packet.append(body);
    socket->write(packet);
}

void TcpMgr::setUserMgr(UserMgr *userInfo)
{
    _userInfo=userInfo;
}
