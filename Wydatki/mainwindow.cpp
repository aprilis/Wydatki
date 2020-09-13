#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "TableModel.h"

#include <QIcon>
#include <QMenu>
#include <QToolButton>
#include <QFileDialog>
#include <QFileInfo>

#include <fstream>
#include <cereal/archives/xml.hpp>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    auto importMenu = new QMenu;
    importMenu->addAction(ui->actionLoadPKO);
    importMenu->addAction(ui->actionLoadBS);

    auto importButton = new QToolButton;
    importButton->setIcon(QIcon::fromTheme("document-import"));
    importButton->setText(tr("Importuj"));
    importButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    importButton->setToolTip(tr("Importuj plik z banku z danymi"));
    importButton->setMenu(importMenu);
    importButton->setPopupMode(QToolButton::InstantPopup);
    ui->mainToolBar->addWidget(importButton);

    connect(ui->actionChangeFile, SIGNAL(triggered()), this, SLOT(changeFile()));
    connect(ui->actionSaveAs, SIGNAL(triggered()), this, SLOT(saveAs()));
    connect(ui->actionLoadPKO, SIGNAL(triggered()), this, SLOT(loadPKOFile()));
    connect(ui->actionLoadBS, SIGNAL(triggered()), this, SLOT(loadBSFile()));
    connect(ui->toolButtonAdd, SIGNAL(clicked()), this, SLOT(addFilter()));
    connect(ui->toolButtonRemove, SIGNAL(clicked()), this, SLOT(removeFilter()));
    connect(ui->filtersTable, SIGNAL(cellChanged(int,int)), this, SLOT(filterEdited(int,int)));

    auto model = new TableModel(this, &manager);
    ui->tableView->setModel(model);
    connect(model, SIGNAL(categoryChanged()), this, SLOT(updateCategories()));

    updateItems();

    int n = manager.getFiltersCount();
    ui->filtersTable->setRowCount(n);
    for(int i = 0; i < n; i++)
    {
        auto filter = manager.getFilter(i);
        auto item1 = new QTableWidgetItem(filter.first.c_str()),
                item2 = new QTableWidgetItem(filter.second.c_str());
        ui->filtersTable->setItem(i, 0, item1);
        ui->filtersTable->setItem(i, 1, item2);
    }

    ui->summaryTable->sortByColumn(1, Qt::AscendingOrder);
    ui->summaryTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);

    updateStatusBar();
    updateTitle();
}

void MainWindow::updateTitle() {
    auto path = manager.getPath();
    QFileInfo fileInfo(QString::fromStdString(path));
    auto title = fileInfo.fileName() + tr(" - Moje Wydatki");
    setWindowTitle(title);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeFile()
{
    auto file = QFileDialog::getSaveFileName(this, "Wybierz plik z danymi", QString(), tr("Pliki xml (*.xml)"),
                                             nullptr, QFileDialog::DontConfirmOverwrite);
    if(file.isNull()) return;
    QFileInfo fileInfo(file);
    if(fileInfo.suffix().isNull()) {
        file += ".xml";
    }

    manager.changeFile(file.toStdString());
    updateItems();
    updateStatusBar();
    updateTitle();
}

void MainWindow::saveAs()
{
    auto file = QFileDialog::getSaveFileName(this, "Wybierz plik z danymi", QString(), tr("Pliki xml (*.xml)"));
    if(file.isNull()) return;
    QFileInfo fileInfo(file);
    if(fileInfo.suffix().isNull()) {
        file += ".xml";
    }

    manager.saveAs(file.toStdString());
    updateTitle();
}

void MainWindow::loadPKOFile()
{
    auto file = QFileDialog::getOpenFileName(this, "Wybierz plik z danymi", QString(), tr("Pliki xml (*.xml)"));
    if(!file.isNull()) manager.importPKO(file.toStdString());

    updateItems();
    updateStatusBar();
}

void MainWindow::loadBSFile()
{
    auto file = QFileDialog::getOpenFileName(this, "Wybierz plik z danymi", QString(), tr("Pliki csv (*.csv)"));
    if(!file.isNull()) manager.importBS(file.toStdString());

    updateItems();
    updateStatusBar();
}

void MainWindow::updateItems()
{
    manager.updateAll();
    auto model = (TableModel*)ui->tableView->model();
    model->updateAll();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();

    updateCategories();
}

void MainWindow::addFilter()
{
    ui->filtersTable->insertRow(ui->filtersTable->rowCount());
    manager.addFilter();
}

void MainWindow::removeFilter()
{
    int row = ui->filtersTable->currentRow();
    if(row >= 0 && row < ui->filtersTable->rowCount())
    {
        ui->filtersTable->removeRow(ui->filtersTable->currentRow());
        manager.removeFilter(row);
    }
    updateItems();
}

void MainWindow::filterEdited(int row, int column)
{
    auto filter = manager.getFilter(row);
    auto item = ui->filtersTable->item(row, column);
    if(column == 0) filter.first = item->text().toStdString();
    else filter.second = item->text().toStdString();
    manager.setFilter(row, filter);
    updateItems();
}

void MainWindow::updateCategories()
{
    auto cat = manager.getCategories();
    ui->summaryTable->setSortingEnabled(false);
    ui->summaryTable->setRowCount(cat.size());
    int i = 0;
    for(auto &x: cat)
    {
        auto item1 = new QTableWidgetItem(x.first.c_str()),
             item2 = new QTableWidgetItem,
             item3 = new QTableWidgetItem,
             item4 = new QTableWidgetItem;
        item2->setData(Qt::EditRole, x.second.first.first - x.second.first.second);
        item3->setData(Qt::EditRole, x.second.first.first);
        item4->setData(Qt::EditRole, x.second.first.second);
        ui->summaryTable->setItem(i, 0, item1);
        ui->summaryTable->setItem(i, 1, item2);
        ui->summaryTable->setItem(i, 2, item3);
        ui->summaryTable->setItem(i, 3, item4);
        i++;
    }
    ui->summaryTable->setSortingEnabled(true);
}

void MainWindow::updateStatusBar()
{
    auto p = manager.getTotal();
    ui->statusBar->showMessage(tr("Wydatki: %1\tPrzychody: %2\tBilans: %3").arg(p.first).arg(p.second).arg(p.second - p.first));
}
