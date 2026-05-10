#ifndef USERMGR_H
#define USERMGR_H

#include <QObject>
#include <QVariant>

class UserMgr:public QObject
{
    Q_OBJECT
public:
    explicit UserMgr(QObject *parent=nullptr);

    void setUserInfo(int uid,const QString& name,const QString& nick,const QString& icon,int sex);

    //加 const 修饰，保证 成员函数 不会修改 类里的成员变量
    Q_INVOKABLE int getUid() const;
    Q_INVOKABLE QString getName() const;
    QString getNick() const;
    Q_INVOKABLE QString getIcon() const;
    int getSex() const;

    Q_INVOKABLE void addFriendApply(int uid,const QString& name,int sex,const QString& icon,int status);        //添加好友申请列表
    Q_PROPERTY(QVariantList applyList READ applyList WRITE setApplyList NOTIFY applyListChanged FINAL)

    Q_INVOKABLE void addFriend(int uid,const QString& name,int sex,const QString& icon);        //添加好友列表
    Q_PROPERTY(QVariantList friendList READ friendList WRITE setFriendList NOTIFY friendListChanged FINAL)

    Q_INVOKABLE void addChatMsg(int uid,const QString& msg,bool isMe,const QString& icon);
    Q_INVOKABLE QVariantList getChatList(int uid);
    //查询头像
    QString getIconByUid(int uid) const;

    QVariantList applyList() const;
    void setApplyList(const QVariantList &newApplyList);

    QVariantList friendList() const;
    void setFriendList(const QVariantList &newFriendList);

signals:
    void applyListChanged();

    void friendListChanged();

    void chatListChanged(int uid);

private:
    int _uid;
    QString _name;
    QString _nick;
    QString _icon;
    int _sex;

    QVariantList _applyList;
    QVariantList _friendList;

    //聊天
    QMap<int,QVariantList> _chatMap;
    //头像
    QHash<int, QString> _iconMap;
};

#endif // USERMGR_H
