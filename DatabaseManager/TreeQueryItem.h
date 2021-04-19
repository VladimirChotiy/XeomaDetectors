#ifndef TREEQUERYITEM_H
#define TREEQUERYITEM_H

#include <QVariant>
#include <QVector>
#include <QStringList>

class TreeQueryItem
{

public:
    explicit TreeQueryItem(const QVector<QVariant> &data, TreeQueryItem *parent = nullptr);
    ~TreeQueryItem();

    void appendChild(TreeQueryItem *child);

    TreeQueryItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    TreeQueryItem *parentItem();

private:
    QVector<TreeQueryItem*> m_childItems;
    QVector<QVariant> m_itemData;
    TreeQueryItem *m_parentItem;;
};

#endif // TREEQUERYITEM_H
