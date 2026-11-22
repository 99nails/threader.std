#include "MainWindowIniFile.h"
#include "ui_MainWindowIniFile.h"

#include <QFile>
#include <QFileDialog>

MainWindowIniFile::MainWindowIniFile(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindowIniFile)
    , _currentLine(nullptr)
{
    ui->setupUi(this);

    ui->tableView->setModel(&_model);

    connect(ui->actionFileNew, &QAction::triggered, this, &MainWindowIniFile::slotActionFileNew);
    connect(ui->actionFileOpen, &QAction::triggered, this, &MainWindowIniFile::slotActionFileOpen);
    connect(ui->actionFileSave, &QAction::triggered, this, &MainWindowIniFile::slotActionFileSave);
    connect(ui->actionFileSaveAs, &QAction::triggered, this, &MainWindowIniFile::slotActionFileSaveAs);
    connect(ui->actionFileExit, &QAction::triggered, this, &MainWindowIniFile::slotActionFileExit);

    connect(ui->tableView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &MainWindowIniFile::slotCurrentChanged);

    connect(ui->pushButtonApply, &QPushButton::clicked,
            this, &MainWindowIniFile::slotPushButtonApplyClicked);
    connect(ui->pushButtonDeleteParameter, &QPushButton::clicked,
            this, &MainWindowIniFile::slotPushButtonDeleteParameterClicked);
    connect(ui->pushButtonDeleteSection, &QPushButton::clicked,
            this, &MainWindowIniFile::slotPushButtonDeleteSectionClicked);

    _iniFile = std::make_shared<IniFile>();
    _model.setIniFile(_iniFile);
    ui->tableView->resizeColumnsToContents();

    showMaximized();
}

MainWindowIniFile::~MainWindowIniFile()
{
    delete ui;

    _iniFile = nullptr;
    _currentLine = nullptr;
}

void MainWindowIniFile::slotActionFileNew()
{
    _iniFile = std::make_shared<IniFile>();
    _model.setIniFile(nullptr);
    _currentLine = nullptr;
}

void MainWindowIniFile::slotActionFileOpen()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("Ini files (*.ini)"));
    dialog.setViewMode(QFileDialog::Detail);


    if (dialog.exec())
    {
        QStringList selectedFiles = dialog.selectedFiles();
        if (!selectedFiles.empty())
        {
            _iniFile = std::make_shared<IniFile>();
            _iniFile->fileOpen(selectedFiles.first().toStdString());
            _model.setIniFile(_iniFile);
            ui->tableView->resizeColumnsToContents();
            _currentLine = nullptr;
        }
    }
}

void MainWindowIniFile::slotActionFileSave()
{
    _iniFile->fileSave();
}

void MainWindowIniFile::slotActionFileSaveAs()
{
    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setWindowTitle(tr("Save file"));
    dialog.setDirectory(QDir::homePath());
    dialog.selectFile(QString::fromStdString(_iniFile->fileName()));
    dialog.setNameFilter(tr("Ini files (*.ini);;All files (*)"));
    dialog.setDefaultSuffix("txt");
    dialog.setViewMode(QFileDialog::Detail);
    dialog.setOption(QFileDialog::DontResolveSymlinks);

    if (dialog.exec() != QDialog::Accepted)
        return;

    QStringList selectedFiles = dialog.selectedFiles();
    if (!selectedFiles.empty())
    {
        _iniFile->fileSave(selectedFiles.first().toStdString());
    }
}

void MainWindowIniFile::slotActionFileExit()
{
    close();
}

void MainWindowIniFile::slotCurrentChanged(const QModelIndex &current,
                                           const QModelIndex &previous)
{
    const auto row = current.row();
    _currentLine = _model.line(row);

    switch (_currentLine->lineType())
    {
    case UnknownLineType:
    case Comment:
        ui->lineEditParameterName->setText(QString());
        ui->lineEditParameterValue->setText(QString::fromStdString(_currentLine->toString()));
        break;
    case Parameter:
    {
        auto parameter = std::dynamic_pointer_cast<IniLineParameter>(_currentLine);
        if (!parameter)
            break;

        ui->lineEditParameterName->setText(QString::fromStdString(parameter->name()));
        ui->lineEditParameterValue->setText(QString::fromStdString(parameter->value()));

        break;
    }
    case Section:
        ui->lineEditParameterName->setText(QString());
        ui->lineEditParameterValue->setText(QString());
        break;
    }

    if (_currentLine->section())
        ui->lineEditSection->setText(QString::fromStdString(_currentLine->section()->name()));
    else
        ui->lineEditSection->setText({});
}

void MainWindowIniFile::slotPushButtonApplyClicked()
{
    _iniFile->setValue(ui->lineEditSection->text().toStdString(),
                       ui->lineEditParameterName->text().toStdString(),
                       ui->lineEditParameterValue->text().toStdString());

    _model.notifyModelUpdated();
}

void MainWindowIniFile::slotPushButtonDeleteParameterClicked()
{
    _iniFile->sectionParameterDelete(ui->lineEditSection->text().toStdString(),
                                     ui->lineEditParameterName->text().toStdString());
    _model.notifyModelUpdated();
}

void MainWindowIniFile::slotPushButtonDeleteSectionClicked()
{
    _iniFile->sectionDelete(ui->lineEditSection->text().toStdString());
    _model.notifyModelUpdated();
}
