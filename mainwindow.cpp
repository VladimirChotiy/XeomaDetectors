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
#include <QSqlQueryModel>
#include <QModelIndexList>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    loadSettings();
    this->advancedGUIInit();
    structureModel = new StructureTreeModel(this);
    protocolModel = new QSqlQueryModel(this);
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
    if (libStore.paramIsEnabled("Maximized")) {
        this->showMaximized();
    }
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
    libStore.setParamEnabled("Maximized", this->isMaximized());
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

}

void MainWindow::connectToDatabase(QVariantList param)
{
    emit connectionClosed();

    qRegisterMetaType<DatabaseContainer*>("DatabaseContainer");

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
    QObject::connect(mainConnection, &DatabaseContainer::databaseResult, this, &MainWindow::refreshStructure, Qt::QueuedConnection);
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

void MainWindow::selectNewTreeItem(const QModelIndex &newIndex, const QModelIndex &oldIndex)
{
    Q_UNUSED(oldIndex)
    qDebug() << "click!";
    qDebug() << newIndex.data(Qt::DisplayRole);
}

void MainWindow::refreshStructure(bool result)
{
    if (result){
        emit this->sendSqlRequest(0, "SELECT tbl_detectors.id, tbl_detectors.name, tbl_detectors.direct, tbl_detectors.count, tbl_detectors.obj_id, tbl_objects.name, tbl_objects.address FROM tbl_detectors LEFT JOIN tbl_objects ON tbl_detectors.obj_id = tbl_objects.id ORDER BY tbl_detectors.obj_id, tbl_detectors.id");
    }
}

void MainWindow::getSqlRequest(int type, const QSqlQuery *sqlQuery)
{
    switch (type) {
    case 0: {
            structureModel->setQuery(sqlQuery);
            ui->tw_Structure->setModel(structureModel);
            break;
        }
    case 1: {
            protocolModel->setQuery(*sqlQuery);
            ui->tbl_ProtocolTable->setModel(protocolModel);
            break;
        }
    }
}

void MainWindow::on_actionConvertToExcel_triggered()
{
    qDebug() << QSqlDatabase::connectionNames();
}

void MainWindow::on_actionRefresh_triggered()
{
    QStringList sqlFilter;
    QModelIndexList testIndex = ui->tw_Structure->selectionModel()->selectedRows();
    for (QModelIndex item : testIndex) {
        if (structureModel->parentIsRoot(item)) {
            sqlFilter.append(QString("tbl_detectors.obj_id = %1").arg(item.data().toInt()));
        }else {
            sqlFilter.append(QString("tbl_detectors.id = %1").arg(item.data().toInt()));
        }
    }
    emit this->sendSqlRequest(1, "SELECT tbl_protocol.id AS protID, tbl_detectors.name AS detNAME, tbl_detectors.direct, tbl_protocol.event_time, tbl_objects.name AS objNAME, tbl_objects.address FROM tbl_protocol LEFT JOIN tbl_detectors ON tbl_protocol.det_id = tbl_detectors.id LEFT JOIN tbl_objects ON tbl_detectors.obj_id = tbl_objects.id WHERE " + sqlFilter.join(" OR "));
}
