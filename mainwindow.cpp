#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "StoreSettings.h"
#include "DatabaseManager/DatabaseContainer.h"
#include "GUI/uiConnectionDialog/uiConnectionDialog.h"
#include <QMessageBox>
#include <QCloseEvent>
#include <QThread>
#include <QSqlDatabase>
#include <QLabel>
#include <QPixmap>
#include <QDateTime>
#include <QSqlQuery>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    loadSettings();
    this->advancedGUIInit();
    testModel = new StructureTreeModel(this);
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
    ui->actionAutoconnect->setChecked(libStore.paramIsEnabled("Autoconnect"));
}

void MainWindow::saveSettings()
{
    StoreSettings libStore(this);
    libStore.saveFormPosition(this->pos());
    libStore.saveFormGeometry(this->size());
    libStore.setByteArray("TestGeometry", ui->splitter->saveState());
    libStore.setParamEnabled("Autoconnect", ui->actionAutoconnect->isChecked());
}

void MainWindow::advancedGUIInit()
{
    sbl_ConnectionStatus = new QLabel();
    QPixmap statusPixmap(QPixmap(":/Icons/icons/server.ico").scaledToHeight(ui->sb_MainStatusbar->height()*0.6));
    sbl_ConnectionStatus->setPixmap(statusPixmap);
    sbl_ConnectionStatus->setEnabled(false);

    sbl_StorageStatus = new QLabel();
    QPixmap storagePixmap(QPixmap(":/Icons/icons/storage.ico").scaledToHeight(ui->sb_MainStatusbar->height()*0.6));
    sbl_StorageStatus->setPixmap(storagePixmap);
    sbl_StorageStatus->setEnabled(false);

    ui->sb_MainStatusbar->addPermanentWidget(sbl_ConnectionStatus);
    ui->sb_MainStatusbar->addPermanentWidget(sbl_StorageStatus);
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
    emit this->sendSqlRequest("SELECT tbl_detectors.id, tbl_detectors.name, tbl_detectors.direct, tbl_detectors.count, tbl_detectors.obj_id, tbl_objects.name, tbl_objects.address FROM tbl_detectors LEFT JOIN tbl_objects ON tbl_detectors.obj_id = tbl_objects.id ORDER BY tbl_detectors.obj_id, tbl_detectors.id");

}

void MainWindow::connectToDatabase(QVariantList param)
{
    emit connectionClosed();

    qRegisterMetaType<DatabaseContainer*>("DatabaseContainer");
    //qRegisterMetaType<QSqlQuery*>("sendQuery");

    DatabaseContainer *mainConnection = new DatabaseContainer(param.at(0).toString(), param.at(4).toString(), param.at(1).toString(), param.at(2).toString(), param.at(3).toInt());
    QThread *mainConThread = new QThread();
    mainConnection->moveToThread(mainConThread);
    QObject::connect(mainConThread, &QThread::started, mainConnection, &DatabaseContainer::runConnection);
    QObject::connect(this, &MainWindow::connectionClosed, mainConnection, &DatabaseContainer::stopConnection, Qt::QueuedConnection);
    QObject::connect(mainConnection, &DatabaseContainer::connectionClose, mainConThread, &QThread::quit);
    QObject::connect(mainConThread, &QThread::finished, mainConThread, &QThread::deleteLater);
    QObject::connect(mainConnection, &DatabaseContainer::databaseResult, sbl_ConnectionStatus, &QLabel::setEnabled);
    QObject::connect(mainConnection, &DatabaseContainer::statusMessage, this, &MainWindow::showStatusbarMessage);
    QObject::connect(this, &MainWindow::sendSqlRequest, mainConnection, &DatabaseContainer::queryRequest, Qt::QueuedConnection);
    QObject::connect(mainConnection, &DatabaseContainer::resultQueryReady, this, &MainWindow::getSqlRequest, Qt::QueuedConnection);
    mainConThread->start();

    DatabaseContainer *storageConnection = new DatabaseContainer(param.at(0).toString(), param.at(5).toString(), param.at(1).toString(), param.at(2).toString(), param.at(3).toInt());
    QThread *storageConThread = new QThread();
    storageConnection->moveToThread(storageConThread);
    QObject::connect(storageConThread, &QThread::started, storageConnection, &DatabaseContainer::runConnection);
    QObject::connect(this, &MainWindow::connectionClosed, storageConnection, &DatabaseContainer::stopConnection, Qt::QueuedConnection);
    QObject::connect(storageConnection, &DatabaseContainer::connectionClose, storageConThread, &QThread::quit);
    QObject::connect(storageConThread, &QThread::finished, storageConThread, &QThread::deleteLater);
    QObject::connect(storageConnection, &DatabaseContainer::databaseResult, sbl_StorageStatus, &QLabel::setEnabled);
    QObject::connect(storageConnection, &DatabaseContainer::statusMessage, this, &MainWindow::showStatusbarMessage);
    storageConThread->start();


}

void MainWindow::showStatusbarMessage(const QString &message)
{
    QString resultMessage;
    resultMessage = "[" + QDateTime::currentDateTime().time().toString() + "] " + message;
    ui->sb_MainStatusbar->showMessage(resultMessage, 10000);
}

void MainWindow::getSqlRequest(const QSqlQuery *sqlQuery)
{
    testModel->setQuery(sqlQuery);
    ui->treeView->setModel(testModel);
}

void MainWindow::on_actionConvertToExcel_triggered()
{
    qDebug() << QSqlDatabase::connectionNames();
}
