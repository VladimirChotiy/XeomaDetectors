#ifndef TREEQUERYITEM_H
#define TREEQUERYITEM_H

#include <QVariant>
#include <QVector>
#include <QSqlRecord>

class TreeQueryItem
{

public:
    enum TreeItemType {
        Header,
        Object,
        Detector
    };


    explicit TreeQueryItem(const QSqlRecord data, TreeItemType itemType, TreeQueryItem *parent = nullptr);
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
    const QSqlRecord dataRecord;
    TreeQueryItem *m_parentItem;;
    TreeItemType iType;
};

#endif // TREEQUERYITEM_H
