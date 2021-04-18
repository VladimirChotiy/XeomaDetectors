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
class QSqlQueryModel;
class clReportGenerator;
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
    QSqlQueryModel *structureRaportModel;
    QByteArray protocolTableState;
    QByteArray structureTreeState;
    QPixmap labelPhoto;
    QString prHostname = "xeoma.sos-dn.net";
    QString prUsername = "report_gen";
    QString prPassword;
    QString prDetDatabase = "udb_detectors";
    QString prPicDatabase = "udb_pictures";
    int prPort = 3306;
    clReportGenerator *m_repGenerator = nullptr;

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
    void on_actionSelectAll_triggered();
    void on_lb_Photo_customContextMenuRequested(const QPoint &pos);
    void on_tw_Structure_customContextMenuRequested(const QPoint &pos);
    void connectToDatabase(QVariantList param);
    void getSqlRequest(int type, const QSqlQuery *sqlQuery);
    void getPicRequest(int type, const QSqlQuery *picQuery);
    void showStatusbarMessage(const QString& message);
    void selectNewTreeItem(const QModelIndex &newIndex, const QModelIndex &oldIndex);
    void refreshStructure(bool result);
    void refreshPixmap(const QModelIndex &current, const QModelIndex &previous);
    void showFullPhoto();
    QString generateRequestFilter(const QString &firstSql);
    void prepareReport(int id, const QByteArray &repTemplate, const QString &sqlString, bool filter = false, bool isDesigner = false);
    //void saveTemplate(int id, const QByteArray &rpTemplate);
    //void savePhoto(QList<QPixmap> photoList);




    void on_tbl_ProtocolTable_customContextMenuRequested(const QPoint &pos);

    void on_actionRaport_triggered();

signals:
    void connectionClosed();
    void sendSqlRequest(int type, const QString& sqlRequest);
    void sendPicRequest(int type, const QString &picRequest);
};
#endif // MAINWINDOW_H
