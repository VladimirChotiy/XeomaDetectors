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
#include "GUI/uiReportSelector/uiReportSelector.h"
#include "Report/clExcelExport.h"
#include "Report/clReportGenerator.h"
#include <QMessageBox>
#include <QCloseEvent>
#include <QThread>
#include <QLabel>
#include <QPixmap>
#include <QDateTime>
#include <QSqlQuery>
#include <QSqlQueryModel>
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
    //ui->tbl_ProtocolTable->sortByColumn(0, Qt::DescendingOrder);

    QObject::connect(ui->tbl_ProtocolTable->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &MainWindow::refreshPixmap);
    QObject::connect(ui->lb_Photo, &uiPhotolabel::clicked, this, &MainWindow::showFullPhoto);

    structureRaportModel = new QSqlQueryModel(this);

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

    ui->tb_MainToolbar->setHidden(libStore.paramIsEnabled(ui->tb_MainToolbar->objectName()));
    ui->tb_Object->setHidden(libStore.paramIsEnabled(ui->tb_Object->objectName()));
    ui->tb_Detector->setHidden(libStore.paramIsEnabled(ui->tb_Detector->objectName()));
    ui->tb_Report->setHidden(libStore.paramIsEnabled(ui->tb_Report->objectName()));
    ui->tb_Exit->setHidden(libStore.paramIsEnabled(ui->tb_Exit->objectName()));
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

    libStore.setParamEnabled(ui->tb_MainToolbar->objectName(), ui->tb_MainToolbar->isHidden());
    libStore.setParamEnabled(ui->tb_Object->objectName(), ui->tb_Object->isHidden());
    libStore.setParamEnabled(ui->tb_Detector->objectName(), ui->tb_Detector->isHidden());
    libStore.setParamEnabled(ui->tb_Report->objectName(), ui->tb_Report->isHidden());
    libStore.setParamEnabled(ui->tb_Exit->objectName(), ui->tb_Exit->isHidden());
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

    ui->cb_Direction->addItem("Все", "tbl_detectors.direct BETWEEN 0 AND 1");
    ui->cb_Direction->addItem("Въезд", "tbl_detectors.direct = 0");
    ui->cb_Direction->addItem("Выезд", "tbl_detectors.direct = 1");
    ui->cb_Direction->setCurrentText(0);

    ui->dt_Begin->setDate(QDate::fromString("01.01.2021", "dd.MM.yyyy"));
    ui->dt_Begin->setTime(QTime::fromString("00:00"));
    ui->dt_End->setDate(QDate::currentDate());
    ui->dt_End->setTime(QTime::fromString("23:59"));

    ui->tbl_ProtocolTable->addAction(ui->actionSave);
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
    const QString sndRequest = "SELECT * FROM tbl_templates ORDER BY tbl_templates.id";
    emit this->sendSqlRequest(3, sndRequest);
}

void MainWindow::connectToDatabase(QVariantList param)
{
    emit connectionClosed();

    ui->cb_showPhoto->setEnabled(false);

    qRegisterMetaType<DatabaseContainer*>("DatabaseContainer");

    prHostname = param.at(0).toString();
    prDetDatabase = param.at(4).toString();
    prPicDatabase = param.at(5).toString();
    prUsername = param.at(1).toString();
    prPassword = param.at(2).toString();
    prPort = param.at(3).toInt();

    DatabaseContainer *mainConnection = new DatabaseContainer(prHostname, prDetDatabase, prUsername, prPassword, prPort);
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

    DatabaseContainer *storageConnection = new DatabaseContainer(prHostname, prPicDatabase, prUsername, prPassword, prPort);
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
    ui->actionRaportDesigner->setEnabled(result);
    ui->actionRaport->setEnabled(result);
    if (result){
        emit this->sendSqlRequest(0, "SELECT tbl_detectors.id, tbl_detectors.name, tbl_detectors.direct, tbl_detectors.count, tbl_detectors.obj_id, tbl_objects.name, tbl_objects.address, tbl_objects.id FROM tbl_detectors RIGHT JOIN tbl_objects ON tbl_detectors.obj_id = tbl_objects.id ORDER BY tbl_objects.id, tbl_detectors.id");
    }
}

void MainWindow::refreshPixmap(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous)
    if (ui->cb_showPhoto->isChecked()) {
        int id = protocolModel->data(protocolModel->index(current.row(), 6), Qt::DisplayRole).toInt();
        int detectorID = protocolModel->data(protocolModel->index(current.row(), 6), Qt::DisplayRole).toInt();
        QDateTime dt = protocolModel->data(protocolModel->index(current.row(), 3), Qt::DisplayRole).toDateTime();
        //ui->lb_Photo->setText("Загрузка изображения...");
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

QString MainWindow::generateRequestFilter(const QString &firstSql)
{
    QString resultRequest;
    QStringList sqlFilter;
    QStringList protocolFilter;
    QModelIndexList testIndex = ui->tw_Structure->selectionModel()->selectedRows();

    for (QModelIndex item : testIndex) {
        if (structureModel->parentIsRoot(item)) {
            sqlFilter.append(QString("tbl_detectors.obj_id = %1").arg(item.data().toInt()));
        }else {
            sqlFilter.append(QString("tbl_detectors.id = %1").arg(item.data().toInt()));
        }
    }

    if (!sqlFilter.isEmpty()) {
        protocolFilter << "(" + sqlFilter.join(" OR ") + ")";
    }

    protocolFilter << ui->cb_Direction->currentData(Qt::UserRole).toString();
    protocolFilter << "(tbl_protocol.event_time BETWEEN (SELECT '" + ui->dt_Begin->dateTime().toString("yyyy-MM-dd hh:mm:ss") + "') AND (SELECT '" + ui->dt_End->dateTime().toString("yyyy-MM-dd hh:mm:ss") +"'))";

    if (!protocolFilter.isEmpty()) {
        resultRequest = firstSql + protocolFilter.join(" AND ");
        return resultRequest;
    }
    return QString();
}

QString MainWindow::generateProtocolRequest()
{
    const QString mainRequest = "SELECT tbl_protocol.id AS protID, tbl_detectors.name AS detNAME, tbl_detectors.direct, tbl_protocol.event_time, tbl_objects.name AS objNAME, tbl_objects.address, tbl_protocol.pic_id, tbl_detectors.id AS detecID FROM tbl_protocol LEFT JOIN tbl_detectors ON tbl_protocol.det_id = tbl_detectors.id LEFT JOIN tbl_objects ON tbl_detectors.obj_id = tbl_objects.id WHERE ";
    const QString orderString = " ORDER BY tbl_objects.name, tbl_detectors.direct, tbl_protocol.event_time";

    return this->generateRequestFilter(mainRequest) + orderString;
}

void MainWindow::prepareReport(int id, const QByteArray &repTemplate, const QString &sqlString, bool filter)
{
    QString resultRequest;
    if (filter) {
        resultRequest = this->generateRequestFilter(sqlString);
    }else {
        resultRequest = sqlString;
    }
    m_repGenerator = new clReportGenerator(id, repTemplate, this);
    //QObject::connect(m_repGenerator, &clReportGenerator::saveTemplate, this, &MainWindow::saveTemplate);
    emit this->sendSqlRequest(4, resultRequest);
}

//void MainWindow::saveTemplate(int id, const QByteArray &rpTemplate)
//{
//    QString sqlRequest;
//    sqlRequest = QString("UPDATE tbl_templates SET tbl_templates.template = %2 WHERE tbl_templates.id = %1").arg(id).arg(QString(rpTemplate.toBase64()));
//    emit this->sendSqlRequest(5, sqlRequest);
//}

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
                structureRaportModel->setQuery(*sqlQuery);
                break;
            }
        case 1: {
                protocolModel->setQuery(*sqlQuery);
                ui->tbl_ProtocolTable->horizontalHeader()->restoreState(protocolTableState);
                ui->tbl_ProtocolTable->setColumnHidden(6, true);
                ui->tbl_ProtocolTable->setColumnHidden(7, true);
                ui->cb_showPhoto->setEnabled(true);
                ui->actionConvertToExcel->setEnabled(true);
                ui->actionRaportDesigner->setEnabled(true);
                ui->actionSave->setEnabled(true);
                break;
            }

        case 2: {
                this->refreshStructure(true);
                break;
            }

        case 3: {
                uiReportSelector *repSelectorDialog = new uiReportSelector(*sqlQuery, this);
                repSelectorDialog->setAttribute(Qt::WA_DeleteOnClose);
                QObject::connect(repSelectorDialog, &uiReportSelector::generateRaport, this, &MainWindow::prepareReport);
                repSelectorDialog->open();
                break;
            }
        case 4: {
                m_repGenerator->setUserVariables(ui->dt_Begin->dateTime().toString("dd MMMM yyyyг. hh:mm:ss"), ui->dt_End->dateTime().toString("dd MMMM yyyyг. hh:mm:ss"));
                m_repGenerator->setProtocolQuery(*sqlQuery);
                this->showStatusbarMessage("Запуск генератора отчетов");
                m_repGenerator->runReportDesigner();
                ui->actionRaportDesigner->setEnabled(true);
                delete m_repGenerator;
                m_repGenerator = nullptr;
                break;
            }
//        case 5: {
//                qDebug() << "RRRR";
//                break;
//            }
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
        ui->lb_Photo->setPhoto(screen.scaled(ui->lb_Photo->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        ui->lb_Photo->setText("");
    }
}

void MainWindow::on_actionConvertToExcel_triggered()
{
    try {
        clExcelExport exportToExcel;
        int rows = ui->tbl_ProtocolTable->verticalHeader()->count();
        int columns = ui->tbl_ProtocolTable->horizontalHeader()->count();
        int tblColumn = 1;

        for (int col = 0; col < columns; col++) {
            if (!ui->tbl_ProtocolTable->isColumnHidden(col)) {
                QString headerName = ui->tbl_ProtocolTable->model()->headerData(col, Qt::Horizontal).toString();
                exportToExcel.SetCellValue(1, tblColumn, headerName);
                tblColumn++;
            }
        }

        for (int row = 0; row < rows; row++) {
            tblColumn = 1;
            for (int col = 0; col < columns; col++) {
                if (!ui->tbl_ProtocolTable->isColumnHidden(col)) {
                    QVariant cellData = ui->tbl_ProtocolTable->model()->data(ui->tbl_ProtocolTable->model()->index(row, col));
                    exportToExcel.SetCellValue(row + 2, tblColumn, cellData);
                    tblColumn++;
                }
            }
        }
    }  catch (const std::exception &e) {
        QMessageBox::critical(this, "Ошибка экспорта в MS Excel", e.what());
    }
}

void MainWindow::on_actionRefresh_triggered()
{
    QString sndRequest;
    sndRequest = this->generateProtocolRequest();
    if (!sndRequest.isEmpty()) {
        ui->actionConvertToExcel->setEnabled(false);
        ui->actionRaportDesigner->setEnabled(false);
        ui->actionSave->setEnabled(false);
        this->showStatusbarMessage("Запрос к БД... подождите.");
        emit this->sendSqlRequest(1, sndRequest);
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
    ui->lb_Photo->viewImage(arg1);
    if (arg1 != 0) {
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

void MainWindow::on_actionSelectAll_triggered()
{
    ui->tw_Structure->selectAll();
}

void MainWindow::on_lb_Photo_customContextMenuRequested(const QPoint &pos)
{
    if (ui->lb_Photo->pixmap() == nullptr) return;
    QMenu *photoContextMenu = new QMenu(this);
    photoContextMenu->addAction(ui->actionSave);
    photoContextMenu->popup(ui->lb_Photo->mapToGlobal(pos));
}

void MainWindow::on_tw_Structure_customContextMenuRequested(const QPoint &pos)
{
    QModelIndex index = ui->tw_Structure->indexAt(pos);
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
        contextMenu->popup(ui->tw_Structure->viewport()->mapToGlobal(pos));
        contextMenu->addSection("Выбрать все");
        contextMenu->addAction(ui->actionSelectAll);
    }
}

void MainWindow::on_tbl_ProtocolTable_customContextMenuRequested(const QPoint &pos)
{
    QMenu *protocolContextMenu = new QMenu(this);
    protocolContextMenu->addAction(ui->actionSave);
    protocolContextMenu->popup(ui->tbl_ProtocolTable->mapToGlobal(pos));
}
