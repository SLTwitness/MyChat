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

    Q_INVOKABLE void registerRequest(const QString& user,const QString& email,const QString& passwd,const QString& verifycode);

    Q_INVOKABLE void loginRequst(const QString& email,const QString& passwd);

signals:
    void verifySend(bool success,const QString& message);
    void registerSend(bool success,const QString& message);
    void loginSend(bool success,const QString& message,const QString& host,int port,int uid,const QString& token);

private:
    QNetworkAccessManager* manager;
};

#endif // NETBRIDGE_H
