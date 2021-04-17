#include "uiReportSelector.h"
#include "ui_uiReportSelector.h"
#include <QDateTime>

uiReportSelector::uiReportSelector(const QSqlQuery &dataQuery, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::uiReportSelector)
{
    ui->setupUi(this);

    repModel = new QSqlQueryModel(this);
    repModel->setQuery(dataQuery);
    ui->ed_ReportSelector->setModel(repModel);
    ui->ed_ReportSelector->setModelColumn(1);
    ui->ed_ReportSelector->setCurrentIndex(-1);
    QObject::connect(ui->ed_ReportSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(refreshData(int)));
    ui->ed_ReportSelector->setCurrentIndex(0);
}

uiReportSelector::~uiReportSelector()
{
    delete ui;
}

void uiReportSelector::refreshData(int index)
{
    QString resultText;
    QDateTime templateDate;
    resultText = repModel->data(repModel->index(index, 2)).toString();
    resultText += "\nШаблон обновлен: ";
    templateDate = repModel->data(repModel->index(index, 4)).toDateTime();
    resultText += templateDate.toString(Qt::LocaleDate);
    ui->txt_ReportDecription->setText(resultText);
}

void uiReportSelector::on_uiReportSelector_accepted()
{
    int id = ui->ed_ReportSelector->currentIndex();
    QByteArray reportTemplate;
    QString sqlString;
    bool needFilter;

    reportTemplate = repModel->data(repModel->index(id, 3)).toByteArray();
    sqlString = repModel->data(repModel->index(id, 5)).toString();
    needFilter = repModel->data(repModel->index(id, 6)).toBool();

    emit this->generateRaport(id, reportTemplate, sqlString, needFilter);
}
