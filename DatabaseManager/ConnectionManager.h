#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <QObject>

class ConnectionManager : public QObject
{
    Q_OBJECT

public:
    explicit ConnectionManager(QObject *parent = nullptr);
    ~ConnectionManager();
    void setMainHost(const QString pHostname, const QString pUserName, const QString pPassword, int pPort = 3306);
    void setMainDatabase(const QString pDatabaseName);
    void setPictureStorage(const QString pStorageName);
    void runConnections();

private:
    QString hostname;
    QString userName;
    QString password;
    int port;
    QString mainDatabase;
    QString pictureStorage;
};

#endif // CONNECTIONMANAGER_H
