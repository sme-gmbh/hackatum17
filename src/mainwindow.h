#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "QTreeWidgetItem"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_trainingNewStation_clicked();

    void on_treeWidget_trainingImages_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_pushButton_traingImageNew_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
