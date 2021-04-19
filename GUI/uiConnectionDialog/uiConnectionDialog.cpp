#include "uiConnectionDialog.h"
#include "ui_uiConnectionDialog.h"
#include "StoreSettings.h"
#include <QAbstractButton>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QByteArray>
#include <QDataStream>
#include <QIODevice>
#include <QDebug>

uiConnectionDialog::uiConnectionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::uiConnectionDialog)
{
    ui->setupUi(this);
    this->loadSettings();
}

uiConnectionDialog::~uiConnectionDialog()
{
    delete ui;
}

void uiConnectionDialog::saveSettings()
{
    StoreSettings libStore(this);
    libStore.saveFormPosition(this->pos());
    libStore.saveFormGeometry(this->size());

    QVariantList settingsList;
    settingsList << ui->ed_Hostname->text()
                 << ui->ed_User->text()
                 << ui->ed_Port->value()
                 << ui->ed_MainDatabse->text()
                 << ui->ed_PictureDatabse->text();
    QByteArray dbByteArray;
    QDataStream dsDB(&dbByteArray, QIODevice::WriteOnly);
    dsDB << settingsList;
    libStore.setByteArray("Connection", dbByteArray);

    libStore.setPassword(ui->ed_Password->text());
}

void uiConnectionDialog::restoreDefaults()
{
    ui->ed_Hostname->setText(defHostname);
    ui->ed_User->setText(defUsername);
    ui->ed_Password->setText(defPassword);
    ui->ed_Port->setValue(defPort);
    ui->ed_MainDatabse->setText(defMainDatabase);
    ui->ed_PictureDatabse->setText(defPictureDatabase);
}

void uiConnectionDialog::on_uiConnectionDialog_accepted()
{
    this->saveSettings();
    QVariantList send{};
    send << ui->ed_Hostname->text()
         << ui->ed_User->text()
         << ui->ed_Password->text()
         << ui->ed_Port->value()
         << ui->ed_MainDatabse->text()
         << ui->ed_PictureDatabse->text();
    emit this->sendConnectionParam(send);
}

void uiConnectionDialog::loadSettings()
{
    StoreSettings libStore(this);
    this->move(libStore.getFormPosition());
    this->resize(libStore.getFormGeometry());

    QVariantList settingsList;
    QByteArray byDB {libStore.getByteArray("Connection")};
    QDataStream dsDB(&byDB, QIODevice::ReadOnly);
    dsDB >> settingsList;
    if (settingsList.isEmpty()) {
        this->restoreDefaults();
    }

    ui->ed_Hostname->setText(settingsList[0].toString());
    ui->ed_User->setText(settingsList[1].toString());
    ui->ed_Port->setValue(settingsList[2].toInt());
    ui->ed_MainDatabse->setText(settingsList[3].toString());
    ui->ed_PictureDatabse->setText(settingsList[4].toString());
    ui->ed_Password->setText(libStore.getPassword());
}

void uiConnectionDialog::on_bb_Dialog_clicked(QAbstractButton *button)
{
    if (ui->bb_Dialog->standardButton(button) == QDialogButtonBox::RestoreDefaults) {
        this->restoreDefaults();
    }
}
