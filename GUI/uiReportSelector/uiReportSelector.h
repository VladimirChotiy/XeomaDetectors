#ifndef UIREPORTSELECTOR_H
#define UIREPORTSELECTOR_H

#include <QDialog>
#include <QSqlQueryModel>

QT_BEGIN_NAMESPACE
QT_END_NAMESPACE

namespace Ui {
class uiReportSelector;
}

class uiReportSelector : public QDialog
{
    Q_OBJECT

public:
    explicit uiReportSelector(const QSqlQuery &dataQuery, bool isDesigner = false, QWidget *parent = nullptr);
    ~uiReportSelector();

private:
    Ui::uiReportSelector *ui;
    QSqlQueryModel *repModel;
    bool typeIsDesigner;

private slots:
    void refreshData(int index);
    void on_uiReportSelector_accepted();

signals:
    void generateRaport(int id, const QByteArray &repTemplate, const QString &sqlString, bool filter, bool isDesigner);

};

#endif // UIREPORTSELECTOR_H
