#ifndef STRUCTURETREEMODEL_H
#define STRUCTURETREEMODEL_H

#include <QAbstractItemModel>
#include <QObject>
#include <QModelIndex>
#include <QVariant>
#include <QSqlRecord>
#include <QSqlQuery>

class TreeQueryItem;

class StructureTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit StructureTreeModel(QObject *parent = nullptr);
    ~StructureTreeModel();
    void setQuery(const QSqlQuery *query);

    virtual QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    virtual QModelIndex parent(const QModelIndex &child) const override;
    virtual int rowCount(const QModelIndex &parent) const override;
    virtual int columnCount(const QModelIndex &parent) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;

private:
    void setupModelData(TreeQueryItem *parent);
    TreeQueryItem *rootItem;
};

#endif // STRUCTURETREEMODEL_H
