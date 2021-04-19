#include "ProtocolQueryModel.h"
#include <QFont>
#include <QSize>
#include <QIcon>
#include <QBrush>
#include <QDebug>

ProtocolQueryModel::ProtocolQueryModel(QObject *parent) :
    QSqlQueryModel(parent)
{

}

QVariant ProtocolQueryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    switch (role) {
    case Qt::DisplayRole: {
        if (index.column() == 2) {
            if (QSqlQueryModel::data(index, Qt::DisplayRole).toInt() == 0) {
                return QString("Въезд");
            }else {
                return QString("Выезд");
            }
        }else {
            return QSqlQueryModel::data(index, role);
        }

    case Qt::DecorationRole: {
            if (index.column() == 2) {
                if (QSqlQueryModel::data(index, Qt::DisplayRole).toInt() == 0) {
                    return QIcon(":/Icons/icons/import.ico");
                }else {
                    return QIcon(":/Icons/icons/export.ico");
                }
            }else {
                return QSqlQueryModel::data(index, role);
            }
        }
    case Qt::TextAlignmentRole: return this->getDataTextAlign(index.column());
    case Qt::FontRole: return QFont("MS Shell Dlg 2", 9);;
    }
    default: return QSqlQueryModel::data(index, role);
    }
}

QVariant ProtocolQueryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal) {
        return QSqlQueryModel::headerData(section, orientation, role);
    }

    switch (role) {
    case Qt::DisplayRole: return this->getHeaderNames(section);
    case Qt::TextAlignmentRole: return QVariant(Qt::AlignCenter | (Qt::Alignment)Qt::TextWordWrap);
    case Qt::FontRole: return QFont("MS Shell Dlg 2", 10, QFont::Bold);
    case Qt::SizeHintRole: return QSize(50, 60);
    //case Qt::InitialSortOrderRole: return QVariant(Qt::AscendingOrder);
    default: return QSqlQueryModel::headerData(section, orientation, role);
    }

}

QVariant ProtocolQueryModel::getHeaderNames(int column) const
{
    switch (column) {
    case 0: return "№ п/п";
    case 1: return "Название детектора";
    case 2: return "Направление";
    case 3: return "Время события";
    case 4: return "Название объекта";
    case 5: return "Адрес объекта";
    default: return QSqlQueryModel::headerData(column, Qt::Horizontal, Qt::DisplayRole);
    }
}

QVariant ProtocolQueryModel::getDataTextAlign(int column) const
{
    if (column == 0 || column == 2 || column == 3) {
        return QVariant(Qt::AlignCenter);
    }else {
        return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
    }
}
