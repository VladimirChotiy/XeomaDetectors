#include "TreeQueryItem.h"
#include <QPixmap>

TreeQueryItem::TreeQueryItem(const QVector<QVariant> &data, TreeQueryItem *parent) :
    m_itemData(data), m_parentItem(parent)
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
    if (row < 0 || row >= m_childItems.size()) {
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
    return m_itemData.count();
}

QVariant TreeQueryItem::data(int column) const
{
    if (column < 0 || column > m_itemData.size()) {
        return QVariant();
    }
    return m_itemData.at(column);
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
