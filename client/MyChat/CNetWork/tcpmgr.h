#ifndef TCPMGR_H
#define TCPMGR_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>
#include "usermgr.h"

class TcpMgr:public QObject
{
    Q_OBJECT
public:
    explicit TcpMgr(QObject *parent=nullptr);
    Q_INVOKABLE void connectToChatServer(const QString& host,int port,int uid,const QString& token);

    Q_INVOKABLE void searchUserSend(const QString& uid_name);

    Q_INVOKABLE void addFriendSend(int uid,int touid,const QString& applyname);

    Q_INVOKABLE void authFriendApply(int fromuid,int touid,const QString& back_name);

    Q_INVOKABLE void sendChatMsg(int fromuid,int touid,const QString& content);

    void setUserMgr(UserMgr* userInfo);

signals:
    void searchUserRes(bool success,int uid,const QString& name,int sex,const QString& icon);
    void notifyAddFriend(int uid,const QString& name,int sex,const QString& icon);
    void loadFriendApply(int uid,const QString& name,int sex);
    void notifyAuthFriend(int fromuid,int touid,const QString& name,int sex,const QString& icon);
    void recvChatMsg(int fromuid,const QString& content);

private:
    void loginSend(int uid,const QString& token);
    QTcpSocket* socket;

    UserMgr* _userInfo=nullptr;
};

#endif // TCPMGR_H
