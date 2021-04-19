#include "clReportGenerator.h"
#include <QProgressDialog>
#include <LimeReport>
#include <LRCallbackDS>
#include <QApplication>
#include <QSqlRecord>
#include <QWidget>
#include <QPrinter>

clReportGenerator::clReportGenerator(int id, const QByteArray &reportTemplate, QWidget *parent) :
    QWidget(parent),
    loadTemplate(reportTemplate),
    templateID(id)
{
    m_LimeReport = new LimeReport::ReportEngine(this);
    m_LimeReport->loadFromByteArray(&loadTemplate);
    LimeReport::ICallbackDatasource *m_dsProtocol = m_LimeReport->dataManager()->createCallbackDatasource("dsProtocol");

    QObject::connect(m_LimeReport, SIGNAL(renderStarted()), this, SLOT(renderStarted()));
    QObject::connect(m_LimeReport, SIGNAL(renderPageFinished(int)), this, SLOT(renderPageFinished(int)));
    QObject::connect(m_LimeReport, SIGNAL(renderFinished()), this, SLOT(renderFinished()));
    QObject::connect(m_LimeReport, SIGNAL(onLoad(bool&)), this, SLOT(slotLoad(bool&)));
    QObject::connect(m_LimeReport, SIGNAL(onSave(bool&)), this, SLOT(slotSave(bool&)));
#ifdef QT_DEBUG
#else
    QObject::connect(m_LimeReport, SIGNAL(onSaveAs(bool&)), this, SLOT(slotSave(bool&)));
#endif
    QObject::connect(m_dsProtocol, SIGNAL(getCallbackData(const LimeReport::CallbackInfo&, QVariant&)), this, SLOT(getProtocolCallbackData(const LimeReport::CallbackInfo&, QVariant&)));
    QObject::connect(m_dsProtocol, SIGNAL(changePos(const LimeReport::CallbackInfo::ChangePosType&, bool&)), this, SLOT(protocolChangePos(const LimeReport::CallbackInfo::ChangePosType&, bool&)));
}

clReportGenerator::~clReportGenerator()
{

}

void clReportGenerator::renderStarted()
{
    if (m_LimeReport->isShowProgressDialog()) {
        m_currentPage = 0;
        m_ProgressDialog = new QProgressDialog("Создание отчета...", "Отмена", 0, 0, this);
        QObject::connect(m_ProgressDialog, &QProgressDialog::canceled, m_LimeReport, &LimeReport::ReportEngine::cancelRender);
        QApplication::processEvents();
        m_ProgressDialog->show();
    }
}

void clReportGenerator::renderPageFinished(int renderedPageCount)
{
    if (m_ProgressDialog) {
        m_ProgressDialog->setLabelText(QString::number(renderedPageCount) + " страниц готово");
        m_ProgressDialog->setValue(renderedPageCount);
    }
}

void clReportGenerator::renderFinished()
{
    if (m_ProgressDialog) {
        m_ProgressDialog->close();
        delete m_ProgressDialog;
    }

    m_ProgressDialog = nullptr;
}

void clReportGenerator::getProtocolCallbackData(const LimeReport::CallbackInfo &info, QVariant &data)
{
    switch (info.dataType) {
        case LimeReport::CallbackInfo::RowCount: {
            data = protocolDataQuery.size();
            break;
        }
        case LimeReport::CallbackInfo::ColumnCount: {
            data = protocolDataQuery.record().count();
            break;
        }
        case LimeReport::CallbackInfo::ColumnHeaderData: {
            if (info.index < protocolDataQuery.record().count()) {
                data = protocolDataQuery.record().fieldName(info.index);
            }
            break;
        }
        case LimeReport::CallbackInfo::ColumnData: {
            data = protocolDataQuery.value(info.columnName);
            break;
        }
        case LimeReport::CallbackInfo::IsEmpty: {
            data = !protocolDataQuery.first();
            break;
        }
        case LimeReport::CallbackInfo::HasNext: {
            data = protocolDataQuery.next();
            if (data.toBool()) {
                protocolDataQuery.previous();
            }
            break;
        }
    default: break;
    }
}

void clReportGenerator::protocolChangePos(const LimeReport::CallbackInfo::ChangePosType &type, bool &result)
{
    if (type == LimeReport::CallbackInfo::First) {
        result = protocolDataQuery.first();
    }else {
        result = protocolDataQuery.next();
    }
}

void clReportGenerator::slotLoad(bool &result)
{
    m_LimeReport->loadFromByteArray(&loadTemplate);
    result = true;
}

void clReportGenerator::slotSave(bool &result)
{
    QByteArray sendTemplate;
    sendTemplate = m_LimeReport->saveToByteArray();
    //emit this->saveTemplate(templateID, sendTemplate);
    result = true;
}



void clReportGenerator::runReportDesigner()
{
    m_LimeReport->setShowProgressDialog(false);
    m_LimeReport->designReport();
}

void clReportGenerator::runReport()
{
    m_LimeReport->setShowProgressDialog(true);
    m_LimeReport->previewReport();
}

void clReportGenerator::setProtocolQuery(const QSqlQuery &protocolQuery)
{
    protocolDataQuery = protocolQuery;
}

void clReportGenerator::setUserVariables(const QString beginString, const QString endString)
{
    m_LimeReport->dataManager()->setReportVariable("beginDate", beginString);
    m_LimeReport->dataManager()->setReportVariable("endDate", endString);
}
