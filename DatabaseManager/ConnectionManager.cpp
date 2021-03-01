#include "ConnectionManager.h"

#include "DatabaseContainer.h"
#include <QThread>
#include <QSqlDatabase>

ConnectionManager::ConnectionManager(QObject *parent) :
    QObject(parent)
{

}

ConnectionManager::~ConnectionManager()
{

}

void ConnectionManager::setMainHost(const QString pHostname, const QString pUserName, const QString pPassword, int pPort)
{
    hostname = pHostname;
    userName = pUserName;
    password = pPassword;
    port = pPort;
}

void ConnectionManager::setMainDatabase(const QString pDatabaseName)
{
    mainDatabase = pDatabaseName;
}

void ConnectionManager::setPictureStorage(const QString pStorageName)
{
    pictureStorage = pStorageName;
}

void ConnectionManager::runConnections()
{
    DatabaseContainer *mainConnection = new DatabaseContainer(hostname, mainDatabase, userName, password, port);
    QThread *mainConThread = new QThread();
    mainConnection->moveToThread(mainConThread);
    QObject::connect(mainConThread, &QThread::started, mainConnection, &DatabaseContainer::runConnection);
    QObject::connect(this, &ConnectionManager::destroyed, mainConThread, &QThread::quit);
    QObject::connect(this, &ConnectionManager::destroyed, mainConnection, &DatabaseContainer::deleteLater);
    QObject::connect(mainConThread, &QThread::finished, mainConThread, &QThread::deleteLater);
    mainConThread->start();
}
