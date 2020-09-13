#include "TableModel.h"
#include <QFont>
#include <QBrush>
#include <iostream>

TableModel::TableModel(QObject *parent, Manager *manager)
    : QAbstractTableModel(parent), man(manager)
{

}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    int row = index.row(), column = index.column();
    switch(role)
    {
    case Qt::DisplayRole:
    {
        auto item = man->getItem(row);
        switch(column)
        {
        case 0: return QString(item.date.c_str());
        case 1: return tr("%1").arg(item.value);
        case 2: return QString(item.assignedCategory.c_str());
        case 3: return QString(item.description.c_str());
        }
        break;
    }
    case Qt::ForegroundRole:
        if(column == 2 && man->hasItemCategory(row))
            return QBrush(Qt::gray);
        break;
    case Qt::FontRole:
        if(column == 2 && !man->getItem(row).category.empty())
        {
            QFont bold;
            bold.setBold(true);
            return bold;
        }
        break;
    }

    return QVariant();
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        QString headers[] = { "Data", "Wartość", "Kategoria", "Opis" };
        return headers[section];
    }
    return QVariant();
}

void TableModel::updateAll()
{
    auto index1 = createIndex(0, 0), index2 = createIndex(man->getItemsCount() - 1, 3);
    emit dataChanged(index1, index2);
    emit layoutChanged();
}

bool TableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    int row = index.row();
    if(role == Qt::EditRole) {
        man->setItemCategory(row, value.toString().toStdString());
        man->save();
    }
    emit categoryChanged();
    return true;
}

Qt::ItemFlags TableModel::flags(const QModelIndex &index) const
{
    if(index.column() == 2) return Qt::ItemIsEditable | Qt::ItemIsEnabled;
    else return Qt::ItemIsEnabled;
}
