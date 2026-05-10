#include "usermgr.h"
#include "QDebug"

UserMgr::UserMgr(QObject *parent):QObject(parent) {

}

void UserMgr::setUserInfo(int uid, const QString &name, const QString &nick, const QString &icon, int sex)
{
    _uid=uid;
    _name=name;
    _nick=nick;
    _icon=icon;
    _sex=sex;

    _iconMap[uid] = icon;

    qDebug() << "save current user:";
    qDebug() << "_uid =" << _uid;
    qDebug() << "_name =" << _name;
    qDebug() << "_nick =" << _nick;
    qDebug() << "_sex =" << _sex;
}

int UserMgr::getUid() const
{
    return _uid;
}

QString UserMgr::getName() const
{
    return _name;
}

QString UserMgr::getNick() const
{
    return _nick;
}

QString UserMgr::getIcon() const
{
    return _icon;
}

int UserMgr::getSex() const
{
    return _sex;
}

void UserMgr::addFriendApply(int uid, const QString &name, int sex, const QString &icon, int status)
{
    QVariantMap item;
    item["uid"]=uid;
    item["name"]=name;
    item["sex"]=sex;
    item["img"]=icon;
    item["msg"]=status;

    _applyList.append(item);

    emit applyListChanged();
}

void UserMgr::addFriend(int uid, const QString &name, int sex, const QString &icon)
{
    QVariantMap item;
    item["uid"]=uid;
    item["name"]=name;
    item["sex"]=sex;
    item["img"]=icon;

    _iconMap[uid] = icon;

    _friendList.append(item);
    emit friendListChanged();
}

void UserMgr::addChatMsg(int uid, const QString &msg, bool isMe, const QString &icon)
{
    QVariantMap item;
    item["uid"]=uid;
    item["msg"]=msg;
    item["isMe"]=isMe;
    item["img"]=icon;

    _chatMap[uid].append(item);
    emit chatListChanged(uid);
}

QVariantList UserMgr::getChatList(int uid)
{
    return _chatMap.value(uid);
}

QString UserMgr::getIconByUid(int uid) const
{
     return _iconMap.value(uid);
}

QVariantList UserMgr::applyList() const
{
    return _applyList;
}

void UserMgr::setApplyList(const QVariantList &newApplyList)
{
    if (_applyList == newApplyList)
        return;
    _applyList = newApplyList;
    emit applyListChanged();
}

QVariantList UserMgr::friendList() const
{
    return _friendList;
}

void UserMgr::setFriendList(const QVariantList &newFriendList)
{
    if (_friendList == newFriendList)
        return;
    _friendList = newFriendList;
    emit friendListChanged();
}
