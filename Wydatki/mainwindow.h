#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Manager.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    Manager manager;

    void updateItems();

    void updateStatusBar();

    void updateTitle();

public slots:
    void changeFile();
    void saveAs();

    void loadPKOFile();
    void loadBSFile();

    void addFilter();
    void removeFilter();

    void filterEdited(int row, int column);

    void updateCategories();
};

#endif // MAINWINDOW_H
