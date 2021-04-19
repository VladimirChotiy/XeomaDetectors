#ifndef CLEXCELEXPORT_H
#define CLEXCELEXPORT_H


#include <QObject>

QT_BEGIN_NAMESPACE
class QAxObject;
QT_END_NAMESPACE

class clExcelExport
{
public:
    clExcelExport(const clExcelExport& other) = delete;
    clExcelExport& operator=(const clExcelExport& other) = delete;
    clExcelExport(bool closeExcelOnExit = false);
    ~clExcelExport();

    void SetCellValue(int lineIndex, int columnIndex, const QVariant value);

private:
    QAxObject* m_excelApplication;
    QAxObject* m_workbooks;
    QAxObject* m_workbook;
    QAxObject* m_sheets;
    QAxObject* m_sheet;
    bool m_closeExcelOnExit;
};


#endif // CLEXCELEXPORT_H
