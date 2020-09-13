#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QAbstractTableModel>
#include <Manager.h>

class TableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    TableModel(QObject *parent, Manager *manager);

    int rowCount(const QModelIndex &parent = QModelIndex()) const
    {
        return man->getItemsCount();
    }

    int columnCount(const QModelIndex &parent = QModelIndex()) const
    {
        return 4;
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex & index) const;

    void updateAll();

private:
    Manager *man;

signals:
    void categoryChanged();
};

#endif // TABLEMODEL_H
