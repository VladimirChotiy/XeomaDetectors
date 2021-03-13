#ifndef UISTRUCTDETECTORDIALOG_H
#define UISTRUCTDETECTORDIALOG_H

#include <QDialog>

namespace Ui {
class uiStructDetectorDialog;
}

class uiStructDetectorDialog : public QDialog
{
    Q_OBJECT

public:

    explicit uiStructDetectorDialog(int index, QVector<std::pair<QString, int>> objects, QVariantList data, QWidget *parent = nullptr);
    ~uiStructDetectorDialog();

signals:
    void sendSqlRequest(int type, const QString &sqlRequest);

private slots:
    void on_uiStructDetectorDialog_accepted();

private:
    Ui::uiStructDetectorDialog *ui;
    bool isEditMode = false;
    int recIndex;
    void loadSettings();
    void saveSettings();
};

#endif // UISTRUCTDETECTORDIALOG_H
