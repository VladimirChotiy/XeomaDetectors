#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "StoreSettings.h"
#include "DatabaseManager/DatabaseContainer.h"
#include "DatabaseManager/StructureTreeModel.h"
#include "DatabaseManager/ProtocolQueryModel.h"
#include "GUI/uiConnectionDialog/uiConnectionDialog.h"
#include "GUI/uiStructureDialog/uiStructObjectDialog.h"
#include "GUI/uiStructureDialog/uiStructDetectorDialog.h"
#include "GUI/uiPhotoWidget/uiPhotoWidget.h"
#include <QMessageBox>
#include <QCloseEvent>
#include <QThread>
#include <QLabel>
#include <QPixmap>
#include <QDateTime>
#include <QSqlQuery>
#include <QSortFilterProxyModel>
#include <QModelIndexList>
#include <QMessageBox>
#include <QMenu>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    protocolModel = new ProtocolQueryModel(this);
    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(protocolModel);
    ui->tbl_ProtocolTable->setSortingEnabled(true);
    ui->tbl_ProtocolTable->horizontalHeader()->setSortIndicatorShown(true);
    ui->tbl_ProtocolTable->horizontalHeader()->setSectionsMovable(true);
    ui->tbl_ProtocolTable->setModel(proxyModel);
    ui->tbl_ProtocolTable->sortByColumn(0, Qt::DescendingOrder);
    QObject::connect(ui->tbl_ProtocolTable->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &MainWindow::refreshPixmap);
    QObject::connect(ui->lb_Photo, &uiPhotolabel::clicked, this, &MainWindow::showFullPhoto);
    QObject::connect(ui->lb_Photo, &uiPhotolabel::customContextMenuRequested, this, &MainWindow::showPhotoContextmenu);
    this->loadSettings();
    this->advancedGUIInit();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadSettings()
{
    StoreSettings libStore(this);
    if (libStore.paramIsEnabled("Maximized")) {
        this->showMaximized();
    }else {
        this->move(libStore.getFormPosition());
        this->resize(libStore.getFormGeometry());
    }
    ui->splitter->restoreState(libStore.getByteArray("SpliterGeometry"));
    ui->actionAutoconnect->setChecked(libStore.paramIsEnabled("Autoconnect"));

    protocolTableState = libStore.getByteArray("ProtocolState");
    structureTreeState = libStore.getByteArray("StructureState");

    ui->cb_showPhoto->setChecked(libStore.paramIsEnabled("ShowPhoto"));
}

void MainWindow::saveSettings()
{
    StoreSettings libStore(this);
    libStore.setByteArray("SpliterGeometry", ui->splitter->saveState());
    libStore.setParamEnabled("Autoconnect", ui->actionAutoconnect->isChecked());
    libStore.setParamEnabled("Maximized", this->isMaximized());
    if (!this->isMaximized()) {
        libStore.saveFormPosition(this->pos());
        libStore.saveFormGeometry(this->size());
    }

    libStore.setByteArray("ProtocolState", ui->tbl_ProtocolTable->horizontalHeader()->saveState());
    libStore.setByteArray("StructureState", ui->tw_Structure->header()->saveState());

    libStore.setParamEnabled("ShowPhoto", ui->cb_showPhoto->isChecked());
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

    QObject::connect(ui->tw_Structure, &QTreeView::customContextMenuRequested, this, &MainWindow::showTreeViewContextMenu);
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

    ui->cb_showPhoto->setEnabled(false);

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
    QObject::connect(this, &MainWindow::sendPicRequest, storageConnection, &DatabaseContainer::queryRequest, Qt::QueuedConnection);
    QObject::connect(storageConnection, &DatabaseContainer::resultQueryReady, this, &MainWindow::getPicRequest, Qt::QueuedConnection);
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
    bool isRoot = structureModel->parentIsRoot(newIndex);
    ui->actionAddObject->setEnabled(true);
    ui->actionEditObject->setEnabled(isRoot);
    ui->actionDeleteObject->setEnabled(isRoot);
    ui->actionAddDetector->setEnabled(true);
    ui->actionEditDetector->setEnabled(!isRoot);
    ui->actionDeleteDetector->setEnabled(!isRoot);
}

void MainWindow::refreshStructure(bool result)
{
    ui->actionRefresh->setEnabled(result);
    if (result){
        emit this->sendSqlRequest(0, "SELECT tbl_detectors.id, tbl_detectors.name, tbl_detectors.direct, tbl_detectors.count, tbl_detectors.obj_id, tbl_objects.name, tbl_objects.address, tbl_objects.id FROM tbl_detectors RIGHT JOIN tbl_objects ON tbl_detectors.obj_id = tbl_objects.id ORDER BY tbl_objects.id, tbl_detectors.id");
    }
}

void MainWindow::showTreeViewContextMenu(const QPoint &point)
{
    QModelIndex index = ui->tw_Structure->indexAt(point);
    if (index.isValid()) {
        QMenu *contextMenu = new QMenu(this);
        QActionGroup *contextGroup = new QActionGroup(this);
        contextMenu->addAction(ui->actionRefresh);
        contextMenu->addSection("Объекты");
        contextGroup->addAction(ui->actionAddObject);
        contextGroup->addAction(ui->actionEditObject);
        contextGroup->addAction(ui->actionDeleteObject);
        contextMenu->addAction(ui->actionAddObject);
        contextMenu->addAction(ui->actionEditObject);
        contextMenu->addAction(ui->actionDeleteObject);
        contextMenu->addSection("Детекторы");
        contextGroup->addAction(ui->actionAddDetector);
        contextGroup->addAction(ui->actionEditDetector);
        contextGroup->addAction(ui->actionDeleteDetector);
        contextMenu->addAction(ui->actionAddDetector);
        contextMenu->addAction(ui->actionEditDetector);
        contextMenu->addAction(ui->actionDeleteDetector);
        contextMenu->popup(ui->tw_Structure->viewport()->mapToGlobal(point));
    }
}

void MainWindow::showPhotoContextmenu(const QPoint &point)
{
    QMenu *photoContextMenu = new QMenu(this);
    photoContextMenu->addAction(ui->actionSave);
    photoContextMenu->popup(ui->lb_Photo->mapToGlobal(point));
}

void MainWindow::refreshPixmap(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous)
    if (ui->cb_showPhoto->isChecked()) {
        int id = protocolModel->data(protocolModel->index(current.row(), 6), Qt::DisplayRole).toInt();
        int detectorID = protocolModel->data(protocolModel->index(current.row(), 6), Qt::DisplayRole).toInt();
        QDateTime dt = protocolModel->data(protocolModel->index(current.row(), 3), Qt::DisplayRole).toDateTime();
        ui->lb_Photo->setText("Загрузка изображения...");
        QString test = QString::number(detectorID) + "_" + dt.date().toString("yyyyMMdd") + "_" + dt.time().toString("hhmmss") + ".jpg";
        emit this->sendPicRequest(0, QString("SELECT tbl_pictures.pic_data, (SELECT '%2') AS pic_date FROM tbl_pictures WHERE tbl_pictures.id = %1").arg(id).arg(test));
    }
}

void MainWindow::showFullPhoto()
{
    uiPhotoWidget *fullPhotoWidget = new uiPhotoWidget(labelPhoto, this);
    fullPhotoWidget->setWindowTitle(ui->lb_Photo->property("picName").toString());
    fullPhotoWidget->setAttribute(Qt::WA_DeleteOnClose);
    fullPhotoWidget->show();
}

void MainWindow::getSqlRequest(int type, const QSqlQuery *sqlQuery)
{
    switch (type) {
    case 0: {
            if (structureModel != nullptr) {
                delete structureModel;
            }
            structureModel = new StructureTreeModel(*sqlQuery, this);
            ui->tw_Structure->setModel(structureModel);
            ui->tw_Structure->header()->restoreState(structureTreeState);
            QObject::connect(ui->tw_Structure->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &MainWindow::selectNewTreeItem);
            break;
        }
    case 1: {
            protocolModel->setQuery(*sqlQuery);
            ui->tbl_ProtocolTable->horizontalHeader()->restoreState(protocolTableState);
            ui->tbl_ProtocolTable->setColumnHidden(6, true);
            ui->tbl_ProtocolTable->setColumnHidden(7, true);
            ui->cb_showPhoto->setEnabled(true);
            break;
        }

    case 2: {
            this->refreshStructure(true);
        }
    default: break;
    }
}

void MainWindow::getPicRequest(int type, const QSqlQuery *picQuery)
{
    Q_UNUSED(type)
    QSqlQuery resultQuery = *picQuery;
    if (resultQuery.next()) {
        QPixmap screen;
        screen.loadFromData(picQuery->value(0).toByteArray());
        labelPhoto.loadFromData(picQuery->value(0).toByteArray());
        ui->lb_Photo->setProperty("picName", resultQuery.value(1).toString());
        ui->lb_Photo->setPixmap(screen.scaled(ui->lb_Photo->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        ui->lb_Photo->setText("");
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
    if (!sqlFilter.isEmpty()) {
        this->showStatusbarMessage("Запрос к БД... подождите.");
        emit this->sendSqlRequest(1, "SELECT tbl_protocol.id AS protID, tbl_detectors.name AS detNAME, tbl_detectors.direct, tbl_protocol.event_time, tbl_objects.name AS objNAME, tbl_objects.address, tbl_protocol.pic_id, tbl_detectors.id AS detecID FROM tbl_protocol LEFT JOIN tbl_detectors ON tbl_protocol.det_id = tbl_detectors.id LEFT JOIN tbl_objects ON tbl_detectors.obj_id = tbl_objects.id WHERE " + sqlFilter.join(" OR "));
    }
}

void MainWindow::on_actionDeleteObject_triggered()
{
    int id = ui->tw_Structure->model()->data(ui->tw_Structure->model()->index(ui->tw_Structure->currentIndex().row(), 0), Qt::DisplayRole).toInt();
    QString name = ui->tw_Structure->model()->data(ui->tw_Structure->model()->index(ui->tw_Structure->currentIndex().row(), 1), Qt::DisplayRole).toString();

    QMessageBox::StandardButton result;
    result = QMessageBox::warning(this, "Удаление объекта", QString("Вы действительно хотите удалить объект №%1 - ").arg(id) + name + "?", QMessageBox::StandardButton::Ok | QMessageBox::StandardButton::Cancel);
    if (result == QMessageBox::StandardButton::Ok) {
        emit this->sendSqlRequest(2, QString("DELETE FROM tbl_objects WHERE tbl_objects.id = %1").arg(id));
    }
}

void MainWindow::on_actionDeleteDetector_triggered()
{
    int id = structureModel->data(structureModel->index(ui->tw_Structure->currentIndex().row(), 0, structureModel->parent(ui->tw_Structure->currentIndex())), Qt::DisplayRole).toInt();
    QString name = structureModel->data(structureModel->index(ui->tw_Structure->currentIndex().row(), 1, structureModel->parent(ui->tw_Structure->currentIndex())), Qt::DisplayRole).toString();

    QMessageBox::StandardButton result;
    result = QMessageBox::warning(this, "Удаление детектора", QString("Вы действительно хотите удалить детектор №%1 - ").arg(id) + name + "?", QMessageBox::StandardButton::Ok | QMessageBox::StandardButton::Cancel);
    if (result == QMessageBox::StandardButton::Ok) {
        emit this->sendSqlRequest(2, QString("DELETE FROM tbl_detectors WHERE tbl_detectors.id = %1").arg(id));
    }
}

void MainWindow::on_actionAddObject_triggered()
{
    uiStructObjectDialog *addObject = new uiStructObjectDialog(uiStructObjectDialog::ObjDialogType::New, QVariantList(), this);
    addObject->setAttribute(Qt::WA_DeleteOnClose);
    QObject::connect(addObject, &uiStructObjectDialog::sendResult, this, &MainWindow::sendSqlRequest);
    addObject->open();
}

void MainWindow::on_actionEditObject_triggered()
{
    QVariantList sendParams;
    sendParams.append(ui->tw_Structure->model()->data(ui->tw_Structure->model()->index(ui->tw_Structure->currentIndex().row(), 0), Qt::DisplayRole).toInt());
    sendParams.append(ui->tw_Structure->model()->data(ui->tw_Structure->model()->index(ui->tw_Structure->currentIndex().row(), 1), Qt::DisplayRole).toString());
    sendParams.append(ui->tw_Structure->model()->data(ui->tw_Structure->model()->index(ui->tw_Structure->currentIndex().row(), 2), Qt::DisplayRole).toString());

    uiStructObjectDialog *editObject = new uiStructObjectDialog(uiStructObjectDialog::ObjDialogType::Edit, sendParams, this);
    editObject->setAttribute(Qt::WA_DeleteOnClose);
    QObject::connect(editObject, &uiStructObjectDialog::sendResult, this, &MainWindow::sendSqlRequest);
    editObject->open();
}

void MainWindow::on_actionAddDetector_triggered()
{
    int parentIndex;
    QModelIndex curIndex = ui->tw_Structure->currentIndex();
    if (structureModel->parentIsRoot(curIndex)) {
        parentIndex = ui->tw_Structure->model()->data(ui->tw_Structure->model()->index(curIndex.row(), 0), Qt::DisplayRole).toInt();
    }else {
        QModelIndex prIndex = structureModel->parent(curIndex);
        parentIndex = ui->tw_Structure->model()->data(ui->tw_Structure->model()->index(prIndex.row(), 0), Qt::DisplayRole).toInt();
    }

    uiStructDetectorDialog *addDetector = new uiStructDetectorDialog(parentIndex, structureModel->getObjectList(), QVariantList(), this);
    addDetector->setAttribute(Qt::WA_DeleteOnClose);
    QObject::connect(addDetector, &uiStructDetectorDialog::sendSqlRequest, this, &MainWindow::sendSqlRequest);
    addDetector->open();
}

void MainWindow::on_actionEditDetector_triggered()
{
    int parentIndex;
    QModelIndex curIndex = ui->tw_Structure->currentIndex();
    QVariantList sendParam;
    QModelIndex prIndex = structureModel->parent(curIndex);
    parentIndex = ui->tw_Structure->model()->data(ui->tw_Structure->model()->index(prIndex.row(), 0), Qt::DisplayRole).toInt();
    sendParam << structureModel->data(structureModel->index(ui->tw_Structure->currentIndex().row(), 0, structureModel->parent(ui->tw_Structure->currentIndex())), Qt::DisplayRole).toInt();
    sendParam << structureModel->data(structureModel->index(ui->tw_Structure->currentIndex().row(), 1, structureModel->parent(ui->tw_Structure->currentIndex())), Qt::DisplayRole).toString();
    QString dirParam = structureModel->data(structureModel->index(ui->tw_Structure->currentIndex().row(), 2, structureModel->parent(ui->tw_Structure->currentIndex())), Qt::DisplayRole).toString();
    if (dirParam == "Въезд") {
        sendParam << 0;
    }else {
        sendParam << 1;
    }
    uiStructDetectorDialog *editDetector = new uiStructDetectorDialog(parentIndex, structureModel->getObjectList(), sendParam, this);
    editDetector->setAttribute(Qt::WA_DeleteOnClose);
    QObject::connect(editDetector, &uiStructDetectorDialog::sendSqlRequest, this, &MainWindow::sendSqlRequest);
    editDetector->open();
}

void MainWindow::on_cb_showPhoto_stateChanged(int arg1)
{
    if (arg1 == 0) {
        ui->lb_Photo->setPixmap(QPixmap());
        ui->lb_Photo->setText("Изображения отключены");
    }else {
        this->refreshPixmap(ui->tbl_ProtocolTable->currentIndex(), QModelIndex());
    }
}

void MainWindow::on_actionSave_triggered()
{
    StoreSettings libStore(this);
    QString pathName = libStore.getStringParam("imagePath");
    QString fileName = QFileDialog::getSaveFileName(this, "Сохранить изображение...", pathName + "/" + ui->lb_Photo->property("picName").toString(), "JPEG images (*.jpg)");
    if (fileName.isEmpty()) {
        return;
    }else {
        QFile saveFile(fileName);
        QFileInfo saveFileInfo (saveFile);
        if (!saveFile.open(QIODevice::WriteOnly)) {
            QMessageBox::information(this, "Не удается сохранить файл", saveFile.errorString());
            this->showStatusbarMessage("Ошибка. Не удалось сохранить файл.");
            return;
        }
        if (this->labelPhoto.save(&saveFile)) {
            this->showStatusbarMessage("Файл" + saveFileInfo.fileName() + " успешно сохранен.");
            libStore.setStringParam("imagePath", saveFileInfo.path());
        }else {
            this->showStatusbarMessage("Ошибка. Не удалось сохранить файл.");
        }
    }
}
