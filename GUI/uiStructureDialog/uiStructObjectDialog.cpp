#include "uiStructObjectDialog.h"
#include "ui_uiStructObjectDialog.h"
#include "StoreSettings.h"
#include <QDialogButtonBox>
#include <QStringList>
#include <QIcon>
#include <QPushButton>
#include <QDebug>

uiStructObjectDialog::uiStructObjectDialog(ObjDialogType dlgType, QVariantList data, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::uiStructObjectDialog),
    dType(dlgType), dData(data)
{
    ui->setupUi(this);
    this->loadSettings();
    if (dType == ObjDialogType::New) {
        this->setWindowIcon(QIcon(":/Icons/icons/add.ico"));
        this->setWindowTitle("Добавление нового объекта");
        ui->ed_Name->setText("");
        ui->ed_Address->setText("");
        ui->ed_Name->setPlaceholderText("Введите имя объекта");
        ui->ed_Address->setPlaceholderText("Введите адрес объекта");
    } else {
        this->setWindowIcon(QIcon(":/Icons/icons/edit.ico"));
        if (!dData.isEmpty()) {
            this->setWindowTitle(QString("Редактирование объекта №%1").arg(data.value(0).toInt()));
            ui->ed_Name->setText(dData.value(1).toString());
            ui->ed_Address->setText(dData.value(2).toString());
            ui->buttonBox->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        }else {
            this->setWindowTitle("Ошибка при открытии диалога!");
            ui->buttonBox->setStandardButtons(QDialogButtonBox::Cancel);
            return;
        }
    }
    this->on_ed_Name_textChanged("");
}

uiStructObjectDialog::~uiStructObjectDialog()
{
    delete ui;
}

void uiStructObjectDialog::on_uiStructObjectDialog_accepted()
{
    QString resStr;
    if (dType == ObjDialogType::New) {
        resStr = QString("INSERT INTO tbl_objects(tbl_objects.name, tbl_objects.address) VALUES('%1', '%2')").arg(ui->ed_Name->text()).arg(ui->ed_Address->text());
    }else {
        resStr = QString("UPDATE tbl_objects SET tbl_objects.name = '%2', tbl_objects.address = '%3' WHERE tbl_objects.id = %1").arg(dData.value(0).toInt()).arg(ui->ed_Name->text()).arg(ui->ed_Address->text());
    }
    this->saveSettings();
    emit this->sendResult(2, resStr);
}

void uiStructObjectDialog::on_ed_Name_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1)
    QPushButton *okButton = ui->buttonBox->button(QDialogButtonBox::Ok);
    if (ui->ed_Name->text() == "") {
        okButton->setEnabled(false);
    }else {
        okButton->setEnabled(true);
    }
}

void uiStructObjectDialog::loadSettings()
{
    StoreSettings libStore(this);
    this->move(libStore.getFormPosition());
    this->resize(libStore.getFormGeometry());
}

void uiStructObjectDialog::saveSettings()
{
    StoreSettings libStore(this);
    libStore.saveFormPosition(this->pos());
    libStore.saveFormGeometry(this->size());
}
