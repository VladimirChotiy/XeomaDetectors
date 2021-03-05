#include "StructureTreeModel.h"
#include "TreeQueryItem.h"
#include <QDebug>

StructureTreeModel::StructureTreeModel(QObject *parent) :
    QAbstractItemModel(parent)
{
    rootItem = new TreeQueryItem({"ID", "Объект", "Адрес"});
}

StructureTreeModel::~StructureTreeModel()
{
    delete rootItem;
}

void StructureTreeModel::setQuery(const QSqlQuery *query)
{
    QSqlQuery dataQuery = *query;
    int objId = -1;
    TreeQueryItem *parentItem = rootItem;
    while (dataQuery.next()) {
        if (dataQuery.value(4).toInt() > objId) {
            QVector<QVariant> objData;
            objData.push_back(dataQuery.value(4).toInt());
            objData.push_back(dataQuery.value(5).toString());
            objData.push_back(dataQuery.value(6).toString());
            TreeQueryItem *objChild = new TreeQueryItem(objData, rootItem);
            objChild->parentItem()->appendChild(objChild);
            objId = dataQuery.value(4).toInt();

            parentItem = objChild;

            QVector<QVariant> decData;
            decData.push_back(dataQuery.value(0).toInt());
            decData.push_back(dataQuery.value(1).toString());
            decData.push_back(dataQuery.value(2).toInt());
            decData.push_back(dataQuery.value(3).toInt());
            TreeQueryItem *detChild = new TreeQueryItem(decData, parentItem);
            detChild->parentItem()->appendChild(detChild);

        } else if (dataQuery.value(4) == objId) {
            QVector<QVariant> decData;
            decData.push_back(dataQuery.value(0).toInt());
            decData.push_back(dataQuery.value(1).toString());
            decData.push_back(dataQuery.value(2).toInt());
            decData.push_back(dataQuery.value(3).toInt());
            TreeQueryItem *detChild = new TreeQueryItem(decData, parentItem);
            detChild->parentItem()->appendChild(detChild);
        }
    }
}

QModelIndex StructureTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    TreeQueryItem *parentItem;
    if (!parent.isValid()) {
        parentItem = rootItem;
    }else {
        parentItem = static_cast<TreeQueryItem*>(parent.internalPointer());
    }

    TreeQueryItem *childItem = parentItem->child(row);
    if (childItem) {
        return createIndex(row, column, childItem);
    }
    return QModelIndex();
}

QModelIndex StructureTreeModel::parent(const QModelIndex &child) const
{
    if (!child.isValid()) {
        return QModelIndex();
    }
    TreeQueryItem *childItem = static_cast<TreeQueryItem*>(child.internalPointer());
    TreeQueryItem *parentItem = childItem->parentItem();

    if (parentItem == rootItem) {
        return QModelIndex();
    }
    return createIndex(parentItem->row(), 0, parentItem);
}

int StructureTreeModel::rowCount(const QModelIndex &parent) const
{
    TreeQueryItem *parentItem;
    if (parent.column() > 0) {
        return 0;
    }
    if(!parent.isValid()) {
        parentItem = rootItem;
    }else {
        parentItem = static_cast<TreeQueryItem*>(parent.internalPointer());
    }
    return parentItem->childCount();
}

int StructureTreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return static_cast<TreeQueryItem*>(parent.internalPointer())->columnCount();
    }
    return rootItem->columnCount();
}

QVariant StructureTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    TreeQueryItem *item = static_cast<TreeQueryItem*>(index.internalPointer());
    return item->data(index.column());
}
