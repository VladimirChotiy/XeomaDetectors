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
    void setQuery(const QSqlQuery &query);
    bool parentIsRoot(const QModelIndex &index) const;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    void setupModelData(TreeQueryItem *parent);
    TreeQueryItem *rootItem;
};

#endif // STRUCTURETREEMODEL_H
