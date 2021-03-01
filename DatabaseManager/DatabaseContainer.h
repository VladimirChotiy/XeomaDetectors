#ifndef DATABASECONTAINER_H
#define DATABASECONTAINER_H

#include <QObject>

class QSqlQuery;

class DatabaseContainer : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseContainer(const QString pHostname, const QString pDatabaseName, const QString pUserName, const QString pPassword, int pPort, QObject *parent = nullptr);

private:
    QString hostname;
    QString dbName;
    QString userName;
    QString password;
    int port;
    QSqlQuery *resultQuery;

public slots:
    void runConnection();

signals:

};

#endif // DATABASECONTAINER_H
