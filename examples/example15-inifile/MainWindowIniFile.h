#pragma once

#include "IniFile.h"
#include "ModelIniFile.h"

#include <QMainWindow>

using namespace Threader::Utils;

namespace Ui {
class MainWindowIniFile;
}

class MainWindowIniFile : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindowIniFile(QWidget *parent = nullptr);
    ~MainWindowIniFile();

private slots:
    void slotActionFileNew();
    void slotActionFileOpen();
    void slotActionFileSave();
    void slotActionFileSaveAs();
    void slotActionFileExit();

    void slotCurrentChanged(const QModelIndex& current, const QModelIndex& previous);

    void slotPushButtonApplyClicked();
    void slotPushButtonDeleteParameterClicked();
    void slotPushButtonDeleteSectionClicked();

private:
    Ui::MainWindowIniFile *ui;

    IniFile::Ptr _iniFile;
    ModelIniFile _model;
    IniLineBase::Ptr _currentLine;
};
