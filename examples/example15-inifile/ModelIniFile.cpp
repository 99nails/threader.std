#include "ModelIniFile.h"

static const QString lineTypeNames[LineType::Section + 1] =
    { "UnknownLineType", "Comment", "Parameter", "Section"};

ModelIniFile::ModelIniFile(QObject *parent)
    : QAbstractTableModel(parent)
{}

void ModelIniFile::setIniFile(const IniFile::Ptr &iniFile)
{
    _iniFile = iniFile;

    notifyModelUpdated();
}

QVariant ModelIniFile::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QVariant();
}

QModelIndex ModelIniFile::parent(const QModelIndex &index) const
{
    return QModelIndex();
}

int ModelIniFile::rowCount(const QModelIndex &parent) const
{
    return _lines.size();
}

int ModelIniFile::columnCount(const QModelIndex &parent) const
{
    return 3;
}

QVariant ModelIniFile::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (!_iniFile)
            return QVariant();

        const int row = index.row();
        const int column = index.column();

        auto line = _lines[row];
        switch (column)
        {
        case 0:
            return lineTypeNames[line->lineType()];
        case 1:
            return (line->section() != nullptr) ? QString::fromStdString(line->section()->toString())
                                                : QVariant();
        case 2:
            return QString::fromStdString(line->toString());
        }

        // switch (line->lineType()) {
        // case UnknownLineType:
        //     if (column == 0)
        //         return c;
        //     else
        //         return QVariant();
        // case Comment:
        // case Parameter:
        // case Section:
        //     break;
        // }

        return QVariant();
    }
    return QVariant();
}

void ModelIniFile::notifyModelUpdated()
{
    _lines = _iniFile->lines();
    emit layoutChanged();
}

IniLineBase::Ptr ModelIniFile::line(const int index)
{
    if (index >= _lines.size())
        return nullptr;

    return _lines[index];
}
