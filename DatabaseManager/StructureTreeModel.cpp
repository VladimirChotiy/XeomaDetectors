#include "StructureTreeModel.h"
#include "TreeQueryItem.h"
#include <QFont>
#include <QIcon>
#include <QBrush>
#include <QDebug>

StructureTreeModel::StructureTreeModel(const QSqlQuery &data, QObject *parent) :
    QAbstractItemModel(parent), dataQuery(data)
{
    rootItem = new TreeQueryItem({"ID", "Объект", "Адрес", "Счетчик"});
    this->setQuery();
}

StructureTreeModel::~StructureTreeModel()
{
    delete rootItem;
}

void StructureTreeModel::setQuery()
{
    int objId = -1;
    TreeQueryItem *parentItem = rootItem;
    while (dataQuery.next()) {
        if (dataQuery.value(7).toInt() > objId) {
            QVector<QVariant> objData;
            objData.push_back(dataQuery.value(7).toInt());
            objData.push_back(dataQuery.value(5).toString());
            objData.push_back(dataQuery.value(6).toString());
            objData.push_back("");
            TreeQueryItem *objChild = new TreeQueryItem(objData, rootItem);
            objChild->parentItem()->appendChild(objChild);
            objId = dataQuery.value(7).toInt();

            parentItem = objChild;

            if (dataQuery.value(4).toInt() == objId){
                QVector<QVariant> decData;
                decData.push_back(dataQuery.value(0).toInt());
                decData.push_back(dataQuery.value(1).toString());
                decData.push_back(dataQuery.value(2).toInt());
                decData.push_back(dataQuery.value(3).toInt());
                TreeQueryItem *detChild = new TreeQueryItem(decData, parentItem);
                detChild->parentItem()->appendChild(detChild);
            }

        } else if (dataQuery.value(7) == objId) {
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

bool StructureTreeModel::parentIsRoot(const QModelIndex &index) const
{
    TreeQueryItem *checkItem;
    checkItem = static_cast<TreeQueryItem*>(index.internalPointer());
    TreeQueryItem *parentItem = checkItem->parentItem();
    if (parentItem == rootItem) {
        return true;
    }else {
        return false;
    }
}

QVector<std::pair<QString, int> > StructureTreeModel::getObjectList()
{
    QVector<std::pair<QString, int>> resultVector;
    for (int idx = 0; idx < rootItem->childCount(); idx++) {
        TreeQueryItem *chItem = rootItem->child(idx);
        resultVector.push_back(std::make_pair(chItem->data(1).toString(), chItem->data(0).toInt()));
    }
    return resultVector;
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
        int test = static_cast<TreeQueryItem*>(parent.internalPointer())->columnCount();
        return test;
    }
    return rootItem->columnCount();
}

QVariant StructureTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    TreeQueryItem *item = static_cast<TreeQueryItem*>(index.internalPointer());
    switch (role) {
    case Qt::DisplayRole: {
        if (item->parentItem() != rootItem && index.column() == 2) {
            if (item->data(2) == 1) {
                return QString("Выезд");
            }else {
                return QString("Въезд");
            }
        }
        return item->data(index.column());
        break;
    }
    case Qt::DecorationRole: {
        if (item->parentItem() != rootItem && index.column() == 0) {
            if (item->data(2).toBool()) {
                return QIcon(":/Icons/icons/export.ico");
            }else {
                return QIcon(":/Icons/icons/import.ico");
            }
        }
        return QVariant();
        break;
    }
    case Qt::BackgroundRole: {
        if (item->parentItem() == rootItem) {
            return QBrush(Qt::lightGray, Qt::SolidPattern);
        }
        return QVariant();
    }
    case Qt::FontRole: {
        if (item->parentItem() == rootItem) {
            return QFont("Arial", 9, QFont::Bold);
        }
        return QVariant();
    }
    case Qt::TextAlignmentRole: {
        if (item->parentItem() != rootItem && index.column() == 2) {
            return Qt::AlignCenter;
        }
        return QVariant(Qt::AlignVCenter | Qt::AlignLeft);
    }
    default: return QVariant();
    }
    return item->data(index.column());
}


QVariant StructureTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal) {
        switch (role) {
        case Qt::DisplayRole: {
            return rootItem->data(section);
            break;
        }
        case Qt::TextAlignmentRole: {
            return Qt::AlignCenter;
            break;
        }
        case Qt::FontRole: {
            return QFont("Arial", 10, QFont::Bold);
        }
        default: QAbstractItemModel::headerData(section, orientation, role);
        }
    }

    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        return rootItem->data(section);
    }
    return QVariant();
}

Qt::ItemFlags StructureTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }
    return QAbstractItemModel::flags(index);
}
