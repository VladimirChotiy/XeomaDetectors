#ifndef UISTRUCTOBJECTDIALOG_H
#define UISTRUCTOBJECTDIALOG_H

#include <QDialog>

namespace Ui {
class uiStructObjectDialog;
}

class uiStructObjectDialog : public QDialog
{
    Q_OBJECT
public:
    enum ObjDialogType {
        New,
        Edit
    };

    explicit uiStructObjectDialog(ObjDialogType dlgType, QVariantList data, QWidget *parent = nullptr);
    ~uiStructObjectDialog();

signals:
    void sendResult(int type, const QString &sqlRequest);

private slots:
    void on_uiStructObjectDialog_accepted();
    void on_ed_Name_textChanged(const QString &arg1);

private:
    Ui::uiStructObjectDialog *ui;
    ObjDialogType dType;
    QVariantList dData;
    void loadSettings();
    void saveSettings();
};

#endif // UISTRUCTOBJECTDIALOG_H
