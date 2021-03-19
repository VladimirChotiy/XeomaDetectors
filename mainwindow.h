#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVariantList>
#include <QVector>
#include "GUI/uiPhotoLabel/uiPhotolabel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
class QLabel;
class QSqlQuery;
class QSortFilterProxyModel;
class ProtocolQueryModel;
class StructureTreeModel;
class QPixmap;
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
    StructureTreeModel *structureModel = nullptr;
    ProtocolQueryModel *protocolModel;
    QSortFilterProxyModel *proxyModel;
    QByteArray protocolTableState;
    QByteArray structureTreeState;
    QPixmap labelPhoto;

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
    void on_actionRefresh_triggered();
    void on_actionDeleteObject_triggered();
    void on_actionDeleteDetector_triggered();
    void on_actionAddObject_triggered();
    void on_actionEditObject_triggered();
    void on_actionAddDetector_triggered();
    void on_actionEditDetector_triggered();
    void on_cb_showPhoto_stateChanged(int arg1);
    void on_actionSave_triggered();
    void connectToDatabase(QVariantList param);
    void getSqlRequest(int type, const QSqlQuery *sqlQuery);
    void getPicRequest(int type, const QSqlQuery *picQuery);
    void showStatusbarMessage(const QString& message);
    void selectNewTreeItem(const QModelIndex &newIndex, const QModelIndex &oldIndex);
    void refreshStructure(bool result);
    void showTreeViewContextMenu(const QPoint &point);
    void showPhotoContextmenu(const QPoint &point);
    void refreshPixmap(const QModelIndex &current, const QModelIndex &previous);
    void showFullPhoto();
    //void savePhoto(QList<QPixmap> photoList);

signals:
    void connectionClosed();
    void sendSqlRequest(int type, const QString& sqlRequest);
    void sendPicRequest(int type, const QString &picRequest);
};
#endif // MAINWINDOW_H
