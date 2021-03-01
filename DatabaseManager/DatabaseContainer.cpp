#include "DatabaseContainer.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QThread>

DatabaseContainer::DatabaseContainer(const QString pHostname, const QString pDatabaseName, const QString pUserName, const QString pPassword, int pPort, QObject *parent) :
    QObject(parent), hostname(pHostname), dbName(pDatabaseName), userName(pUserName), password(pPassword), port(pPort)
{

}

void DatabaseContainer::runConnection()
{
    QSqlDatabase mainDB = QSqlDatabase::addDatabase("QMYSQL", dbName);
    mainDB.setHostName(hostname);
    mainDB.setPort(port);
    mainDB.setDatabaseName(dbName);
    mainDB.setUserName(userName);
    mainDB.setPassword(password);

    if (!mainDB.open()) {
        qDebug() << "Error opening DB: " << dbName << Qt::endl << mainDB.lastError().text();
    }

    resultQuery = new QSqlQuery(mainDB);
    qDebug() << QThread::currentThread();
}
