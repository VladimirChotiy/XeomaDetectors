#ifndef UICONNECTIONDIALOG_H
#define UICONNECTIONDIALOG_H

#include <QDialog>
#include <QVariantList>

QT_BEGIN_NAMESPACE
class QAbstractButton;
QT_END_NAMESPACE

namespace Ui {
class uiConnectionDialog;
}

class uiConnectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit uiConnectionDialog(QWidget *parent = nullptr);
    ~uiConnectionDialog();

private slots:
    void on_uiConnectionDialog_accepted();
    void on_bb_Dialog_clicked(QAbstractButton *button);

private:
    Ui::uiConnectionDialog *ui;
    const QString defHostname {"xeoma.sos-dn.net"};
    const QString defUsername {"user"};
    const QString defPassword {""};
    const QString defMainDatabase {"udb_detectors"};
    const QString defPictureDatabase {"udb_pictures"};
    const int defPort {3306};
    bool isAutoconnect;
    void loadSettings();
    void saveSettings();
    void restoreDefaults();

signals:
    void sendConnectionParam(QVariantList param) const;
};

#endif // UICONNECTIONDIALOG_H
