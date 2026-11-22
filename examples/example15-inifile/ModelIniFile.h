#pragma once

#include "IniFile.h"

#include <QAbstractItemModel>

class ModelIniFile : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit ModelIniFile(QObject *parent = nullptr);

    void setIniFile(const IniFile::Ptr &iniFile);

    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const override;

    void notifyModelUpdated();

    IniLineBase::Ptr line(const int index);

private:
    IniFile::Ptr _iniFile;
    IniLineVector _lines;
};
