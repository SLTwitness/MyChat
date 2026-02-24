#ifndef NETBRIDGE_H
#define NETBRIDGE_H

#include <QObject>
#include <QNetworkAccessManager>

class NetBridge : public QObject
{
    Q_OBJECT
public:
    explicit NetBridge(QObject *parent = nullptr);


    Q_INVOKABLE void emailRequest(const QString& email);

    /*测试用*/
    Q_INVOKABLE QString getCode();
    /*测试用*/

signals:
    void verifyCode(const QString& json);

private:
    QNetworkAccessManager* manager;

    /*测试用：验证码*/
    QString curVerifyCode;
    /*测试用：验证码*/
};

#endif // NETBRIDGE_H
