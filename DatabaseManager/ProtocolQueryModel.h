#ifndef PROTOCOLQUERYMODEL_H
#define PROTOCOLQUERYMODEL_H

#include <QSqlQueryModel>
#include <QObject>

class ProtocolQueryModel : public QSqlQueryModel
{
public:
    ProtocolQueryModel(QObject *parent = nullptr);

    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
    QVariant getHeaderNames(int column) const;
    QVariant getDataTextAlign(int column) const;
};

#endif // PROTOCOLQUERYMODEL_H
