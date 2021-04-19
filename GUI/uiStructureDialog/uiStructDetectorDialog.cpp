#include "uiStructDetectorDialog.h"
#include "ui_uiStructDetectorDialog.h"
#include "StoreSettings.h"
#include <QDebug>

uiStructDetectorDialog::uiStructDetectorDialog(int index, QVector<std::pair<QString, int>> objects, QVariantList data, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::uiStructDetectorDialog)
{
    ui->setupUi(this);
    this->loadSettings();
    if (data.isEmpty()) {
        isEditMode = false;
        this->setWindowIcon(QIcon(":/Icons/icons/add.ico"));
        this->setWindowTitle("Добавление нового детектора");
        ui->ed_Name->setText("");

        ui->ed_Direction->setCurrentIndex(0);
        ui->ed_Name->setPlaceholderText("Введите имя детектора");
    }else {
        isEditMode = true;
        recIndex = data.value(0).toInt();
        this->setWindowIcon(QIcon(":/Icons/icons/edit.ico"));
        this->setWindowTitle(QString("Редактирование детектора №%1 - %2").arg(data.value(0).toInt()).arg(data.value(1).toString()));
        ui->ed_Name->setText(data.value(1).toString());
        ui->ed_Direction->setCurrentIndex(data.value(2).toInt());
    }

    int resultIndex = -1;

    for (auto &objItem : objects) {
        QString objName;
        int objID;
        std::tie(objName, objID) = objItem;
        ui->ed_Object->addItem(objName, objID);
        if (objID == index) {
            resultIndex = ui->ed_Object->count() - 1;
        }
    }
    ui->ed_Object->setCurrentIndex(resultIndex);
}

uiStructDetectorDialog::~uiStructDetectorDialog()
{
    delete ui;
}

void uiStructDetectorDialog::on_uiStructDetectorDialog_accepted()
{
    QString resStr;
    if (!isEditMode) {
        resStr = QString("INSERT INTO tbl_detectors(tbl_detectors.name, tbl_detectors.obj_id, tbl_detectors.direct) VALUES('%1', %2, %3)")
                .arg(ui->ed_Name->text())
                .arg(ui->ed_Object->currentData().toInt())
                .arg(ui->ed_Direction->currentIndex());
    }else {
        resStr = QString("UPDATE tbl_detectors SET tbl_detectors.name = '%2', tbl_detectors.obj_id = %3, tbl_detectors.direct = %4 WHERE tbl_detectors.id = %1")
                .arg(recIndex)
                .arg(ui->ed_Name->text())
                .arg(ui->ed_Object->currentData().toInt())
                .arg(ui->ed_Direction->currentIndex());
    }
    //qDebug() << resStr;
    this->saveSettings();
    emit this->sendSqlRequest(2, resStr);
}

void uiStructDetectorDialog::loadSettings()
{
    StoreSettings libStore(this);
    this->move(libStore.getFormPosition());
    this->resize(libStore.getFormGeometry());
}

void uiStructDetectorDialog::saveSettings()
{
    StoreSettings libStore(this);
    libStore.saveFormPosition(this->pos());
    libStore.saveFormGeometry(this->size());
}
