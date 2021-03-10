#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVariantList>
#include "DatabaseManager/StructureTreeModel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
class QLabel;
class QSqlQueryModel;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    void loadSettings();
    void saveSettings();
    void advancedGUIInit();
    QLabel *sbl_ConnectionStatus;
    QLabel *sbl_StorageStatus;
    StructureTreeModel *structureModel;
    QSqlQueryModel *protocolModel;

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void on_actionAbout_QT_triggered();
    void on_actionAboutProgramm_triggered();
    void on_actionClose_triggered();
    void on_actionConnectToDatabase_triggered();
    void on_actionRaportDesigner_triggered();
    void on_actionConvertToExcel_triggered();
    void connectToDatabase(QVariantList param);
    void on_actionRefresh_triggered();
    void getSqlRequest(int type, const QSqlQuery *sqlQuery);
    void showStatusbarMessage(const QString& message);
    void selectNewTreeItem(const QModelIndex &newIndex, const QModelIndex &oldIndex);
    void refreshStructure(bool result);

signals:
    void connectionClosed();
    void sendSqlRequest(int type, const QString& sqlRequest);
};
#endif // MAINWINDOW_H
