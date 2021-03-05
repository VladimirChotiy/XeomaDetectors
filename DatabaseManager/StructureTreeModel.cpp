#include "StructureTreeModel.h"
#include "TreeQueryItem.h"

StructureTreeModel::StructureTreeModel(QSqlQuery &objQuery, QSqlQuery &detQuery, QObject *parent) :
    QAbstractItemModel(parent)
{
    objectsQuery = new QSqlQuery(objQuery);
    detectorsQuery = new QSqlQuery(detQuery);
    rootItem = new TreeQueryItem(QSqlRecord(), TreeQueryItem::TreeItemType::Header);
    this->setupModelData(rootItem);
}

StructureTreeModel::~StructureTreeModel()
{
    delete rootItem;
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

void StructureTreeModel::setupModelData(TreeQueryItem *parent)
{
    while (objectsQuery->next()) {
        TreeQueryItem *newChild = new TreeQueryItem(objectsQuery->record(), TreeQueryItem::TreeItemType::Object);
        parent->appendChild(newChild);
    }
}
