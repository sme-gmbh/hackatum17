#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QFileDialog"
#include "QImage"
#include "QMessageBox"
#include "QPixmap"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_trainingNewStation_clicked()
{
    if (ui->lineEdit_trainingStationName->text().isEmpty()) {
        QMessageBox::warning(this, "New Station", "You did not enter a station name, abort.");
        return;
    }
    QTreeWidgetItem* item = new QTreeWidgetItem(QStringList() << ui->lineEdit_trainingStationName->text());
    item->setData(0, 1001, QVariant(QString("Station")));
    ui->treeWidget_trainingImages->addTopLevelItem(item);
}

void MainWindow::on_treeWidget_trainingImages_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    if (current->data(0, 1001).toString() == "Station")
        ui->pushButton_traingImageNew->setEnabled(true);
    else
        ui->pushButton_traingImageNew->setEnabled(false);
}

void MainWindow::on_pushButton_traingImageNew_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Add training image (station logo)", "", "Images (*.bmp *.gif *.jpg *.jpeg *.png *.pbm *.pgm *.ppm *.xbm *.xpm)");
    if (filename.isEmpty())
        return;

    QImage image;
    bool ok = image.load(filename);
    if (!ok) {
        QMessageBox::warning(this, "Failure", "Image could not be loaded!");
        return;
    }

    QTreeWidgetItem* item = new QTreeWidgetItem(QStringList() << ui->lineEdit_trainingImageName->text());
    ui->treeWidget_trainingImages->currentItem()->addChild(item);

    ui->label_trainingImage->setPixmap(QPixmap::fromImage(image));
}
