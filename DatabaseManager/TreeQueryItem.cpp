#include "TreeQueryItem.h"
#include <QPixmap>

TreeQueryItem::TreeQueryItem(const QSqlRecord data, TreeItemType itemType, TreeQueryItem *parent) :
    dataRecord(data), m_parentItem(parent), iType(itemType)
{
}

TreeQueryItem::~TreeQueryItem()
{
    qDeleteAll(m_childItems);
}

void TreeQueryItem::appendChild(TreeQueryItem *child)
{
    m_childItems.append(child);
}

TreeQueryItem *TreeQueryItem::child(int row)
{
    if (row < 0 || row > m_childItems.size()) {
        return nullptr;
    }
    return m_childItems.at(row);
}

int TreeQueryItem::childCount() const
{
    return m_childItems.count();
}

int TreeQueryItem::columnCount() const
{
    return dataRecord.count();
}

QVariant TreeQueryItem::data(int column) const
{
    switch (iType) {
    case TreeItemType::Object: {
        return dataRecord.value(column);
        break;
    }
    case TreeItemType::Detector: {
        if (column == 0) {
            if (dataRecord.value(0).toBool()) {
                return QPixmap(":/Icons/icons/export.ico");
            }else {
                return QPixmap(":/Icons/icons/import.ico");
            }
        }
        return dataRecord.value(column);
        break;
    }
    default: return QVariant();
    }
}

int TreeQueryItem::row() const
{
    if (m_parentItem) {
        return m_parentItem->m_childItems.indexOf(const_cast<TreeQueryItem*>(this));
    }
    return 0;
}

TreeQueryItem *TreeQueryItem::parentItem()
{
    return m_parentItem;
}
