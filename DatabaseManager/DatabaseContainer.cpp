#include "DatabaseContainer.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QThread>
#include <QVariant>

DatabaseContainer::DatabaseContainer(const QString pHostname, const QString pDatabaseName, const QString pUserName, const QString pPassword, int pPort, QObject *parent) :
    QObject(parent), hostname(pHostname), dbName(pDatabaseName), userName(pUserName), password(pPassword), port(pPort)
{
    QObject::connect(this, &DatabaseContainer::connectionClose, this, &DatabaseContainer::deleteLater);
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
        emit this->databaseResult(false);
        emit this->statusMessage("Ошибка подключения к БД.");
        qDebug() << "Error opening DB: " << dbName << Qt::endl << mainDB.lastError().text();
    }else {
        emit this->databaseResult(true);
        emit this->statusMessage("Подключение к БД выполнено");
    }

    resultQuery = new QSqlQuery(mainDB);
}

void DatabaseContainer::stopConnection()
{
    QSqlDatabase::removeDatabase(dbName);
    emit this->databaseResult(false);
    emit this->connectionClose();
}

void DatabaseContainer::queryRequest(const QString &sqlRequest)
{
    qDebug() << "Conteiner: " << sqlRequest << QThread::currentThread();
    if (resultQuery->exec(sqlRequest)){
        emit this->resultQueryReady(resultQuery);
        emit this->statusMessage("Запрос к БД успешно выполнен. ");
    }else {
        emit this->statusMessage("Ошибка выполнения запроса к БД.");
    }
}
