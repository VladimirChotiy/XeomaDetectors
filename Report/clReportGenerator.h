#ifndef CLREPORTGENERATOR_H
#define CLREPORTGENERATOR_H

#include <QObject>
#include "lrreportengine.h"
#include <QSqlQuery>

QT_BEGIN_NAMESPACE
class QProgressDialog;
QT_END_NAMESPACE

class clReportGenerator : public QWidget
{
    Q_OBJECT
public:
    explicit clReportGenerator(int id, const QByteArray &reportTemplate, QWidget *parent = nullptr);
    ~clReportGenerator();

protected:
    int m_currentPage;
    LimeReport::ReportEngine *m_LimeReport;
    QProgressDialog *m_ProgressDialog = nullptr;
    QSqlQuery protocolDataQuery;
    QByteArray loadTemplate;
    int templateID;

private slots:
    void renderStarted();
    void renderPageFinished(int renderedPageCount);
    void renderFinished();
    void getProtocolCallbackData(const LimeReport::CallbackInfo &info, QVariant &data);
    void protocolChangePos(const LimeReport::CallbackInfo::ChangePosType &type, bool &result);
    void slotLoad(bool &result);
    void slotSave(bool &result);

public slots:
    void runReportDesigner();
    void setProtocolQuery(const QSqlQuery &protocolQuery);
    void setUserVariables(const QString beginString, const QString endString);

signals:
    void saveTemplate(int id, const QByteArray &rpTemplate);

};

#endif // CLREPORTGENERATOR_H
