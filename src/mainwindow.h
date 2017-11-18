#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>

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

    void addReferenceImage(QString filename, QString stationName);

private slots:
    void on_pushButton_trainingNewStation_clicked();

    void on_treeWidget_trainingImages_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_pushButton_traingImageNew_clicked();

    void on_pushButton_testLoadImage_clicked();

    void on_spinBox_matchX_valueChanged(int arg1);

    void on_spinBox_matchY_valueChanged(int arg1);

    void on_doubleSpinBox_compareScale_valueChanged(double arg1);

private:
    Ui::MainWindow *ui;
    QImage referenceImage;
    QImage testImageHPFclean;   // Test image, high pass filtered, clean (without any modification)

    void loadReferenceImages();
    void paintReferenceInTest();
    QPoint findImage(QImage big, QImage small);
};

#endif // MAINWINDOW_H
