#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "StoreSettings.h"
#include "DatabaseManager/DatabaseContainer.h"
#include "GUI/uiConnectionDialog/uiConnectionDialog.h"
#include <QMessageBox>
#include <QCloseEvent>
#include <QThread>
#include <QSqlDatabase>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    loadSettings();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadSettings()
{
    StoreSettings libStore(this);
    this->move(libStore.getFormPosition());
    this->resize(libStore.getFormGeometry());
    ui->splitter->restoreState(libStore.getByteArray("TestGeometry"));
}

void MainWindow::saveSettings()
{
    StoreSettings libStore(this);
    libStore.saveFormPosition(this->pos());
    libStore.saveFormGeometry(this->size());
    libStore.setByteArray("TestGeometry", ui->splitter->saveState());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveSettings();
    emit this->connectionClosed();
    event->accept();
}


void MainWindow::on_actionAbout_QT_triggered()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::on_actionAboutProgramm_triggered()
{
    QMessageBox::about(this, "Информация о программе", "Программа для работы с базой данных детекторов Xeoma \nСалон Охранных Систем \n" + QApplication::organizationDomain() + "\nООО КОМПЛЕКС БЕЗОПАСНОСТИ КВАДРО-Т \nВерсия " + QApplication::applicationVersion());
}

void MainWindow::on_actionClose_triggered()
{
    this->close();
}

void MainWindow::on_actionConnectToDatabase_triggered()
{
    uiConnectionDialog *ui_ConnectionDialog = new uiConnectionDialog(this);
    QObject::connect(ui_ConnectionDialog, &uiConnectionDialog::sendConnectionParam, this, &MainWindow::connectToDatabase);
    ui_ConnectionDialog->setAttribute(Qt::WA_DeleteOnClose);
    ui_ConnectionDialog->open();
}

void MainWindow::on_actionRaportDesigner_triggered()
{
    emit connectionClosed();

}

void MainWindow::connectToDatabase(QVariantList param)
{
    emit connectionClosed();

    DatabaseContainer *mainConnection = new DatabaseContainer(param[0].toString(), param[4].toString(), param[1].toString(), param[2].toString(), param[3].toInt());
    QThread *mainConThread = new QThread();
    mainConnection->moveToThread(mainConThread);
    QObject::connect(mainConThread, &QThread::started, mainConnection, &DatabaseContainer::runConnection);
    QObject::connect(this, &MainWindow::connectionClosed, mainConnection, &DatabaseContainer::stopConnection, Qt::DirectConnection);
    QObject::connect(this, &MainWindow::connectionClosed, mainConThread, &QThread::quit);
    QObject::connect(mainConnection, &DatabaseContainer::connectionClose, mainConnection, &DatabaseContainer::deleteLater);
    QObject::connect(mainConThread, &QThread::finished, mainConThread, &QThread::deleteLater);
    mainConThread->start();

    DatabaseContainer *storageConnection = new DatabaseContainer(param[0].toString(), param[5].toString(), param[1].toString(), param[2].toString(), param[3].toInt());
    QThread *storageConThread = new QThread();
    storageConnection->moveToThread(storageConThread);
    QObject::connect(storageConThread, &QThread::started, storageConnection, &DatabaseContainer::runConnection);
    QObject::connect(this, &MainWindow::connectionClosed, storageConThread, &QThread::quit);
    QObject::connect(this, &MainWindow::connectionClosed, storageConnection, &DatabaseContainer::stopConnection, Qt::DirectConnection);
    QObject::connect(storageConnection, &DatabaseContainer::connectionClose, storageConnection, &DatabaseContainer::deleteLater);
    QObject::connect(storageConThread, &QThread::finished, storageConThread, &QThread::deleteLater);
    storageConThread->start();
}

void MainWindow::on_actionConvertToExcel_triggered()
{
    qDebug() << QSqlDatabase::connectionNames();
}
