#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>
#include <QFile>
#include <QDir>
#include <QDirIterator>
#include <QtDebug>
#include <QFileInfo>
#include <QPainter>
#include <QPen>
#include <QBrush>

#include "imagetransform.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->loadReferenceImages();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addReferenceImage(QString filename, QString stationName)
{
    for (int i=0; i< ui->treeWidget_trainingImages->topLevelItemCount(); i++) {
        if (ui->treeWidget_trainingImages->topLevelItem(i)->text(0) == stationName) {
            QTreeWidgetItem* item = new QTreeWidgetItem(QStringList() << "" << filename);

            QImage image;
            bool ok = image.load(filename);
            if (!ok) {
                QMessageBox::warning(this, "Failure", "Image could not be loaded: " + filename);
            }

            item->setIcon(0, QIcon(QPixmap::fromImage(image.scaledToHeight(64, Qt::SmoothTransformation))));

            ui->treeWidget_trainingImages->topLevelItem(i)->addChild(item);
            return;
        }
    }

    // If program flow reaches this point, we do not have that station yet, so lets create it

    QTreeWidgetItem* topLevelItem = new QTreeWidgetItem(QStringList() << stationName);
    topLevelItem->setData(0, 1001, QVariant(QString("Station")));
    ui->treeWidget_trainingImages->addTopLevelItem(topLevelItem);
    QTreeWidgetItem* item = new QTreeWidgetItem(QStringList() << "" << filename);

    QImage image;
    bool ok = image.load(filename);
    if (!ok) {
        QMessageBox::warning(this, "Failure", "Image could not be loaded: " + filename);
    }

    item->setIcon(0, QIcon(QPixmap::fromImage(image.scaledToHeight(64, Qt::SmoothTransformation))));
    topLevelItem->addChild(item);

    ui->treeWidget_trainingImages->resizeColumnToContents(0);
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
    else {
        ui->pushButton_traingImageNew->setEnabled(false);
        referenceImage.load(current->text(1));
        ui->label_trainingImage->setPixmap(QPixmap::fromImage(referenceImage));
    }
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

void MainWindow::on_pushButton_testLoadImage_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Add sample image to find logo", "", "Images (*.bmp *.gif *.jpg *.jpeg *.png *.pbm *.pgm *.ppm *.xbm *.xpm)");
    if (filename.isEmpty())
        return;

    QImage image;
    bool ok = image.load(filename);
    if (!ok) {
        QMessageBox::warning(this, "Failure", "Image could not be loaded!");
        return;
    }

    ui->label_testImage->setPixmap(QPixmap::fromImage(image));

    QImage filteredImage = ImageTransform::highPassFilter(image);
    testImageHPFclean = filteredImage;


    ui->label_filterImage->setPixmap(QPixmap::fromImage(filteredImage));


    double s = ui->doubleSpinBox_compareScale->value();
    QImage logoHPF = ImageTransform::highPassFilter(referenceImage.transformed(QTransform().scale(s, s), Qt::SmoothTransformation));
    findImage(testImageHPFclean, logoHPF);
}

void MainWindow::loadReferenceImages()
{
    QDir dir;
    dir.setCurrent("etc/references");

    QStringList dirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    foreach(QString dirPath, dirs) {


        QDir innerDir = QDir(dirPath);


        if (!innerDir.exists()) {
            QMessageBox::warning(this, "Warning", "Dir does not exist: " + innerDir.absolutePath());
            continue;
        }


        QStringList filePaths = innerDir.entryList(QDir::Files);

        foreach (QString filePath, filePaths) {
            filePath.prepend(dirPath + "/");
            addReferenceImage(QFileInfo(filePath).absoluteFilePath(), QDir(dirPath).dirName());
        }
    }
}

void MainWindow::paintReferenceInTest()
{
    QImage image = testImageHPFclean;

    QPainter painter(&image);

    double s = ui->doubleSpinBox_compareScale->value();
    QImage logoHPF = ImageTransform::highPassFilter(referenceImage.transformed(QTransform().scale(s, s), Qt::SmoothTransformation));

    painter.drawImage(ui->spinBox_matchX->value(), ui->spinBox_matchY->value(), logoHPF);

    painter.end();

    ui->label_filterImage->setPixmap(QPixmap::fromImage(image));
}

void MainWindow::on_spinBox_matchX_valueChanged(int arg1)
{
    paintReferenceInTest();
}

void MainWindow::on_spinBox_matchY_valueChanged(int arg1)
{
    paintReferenceInTest();
}

void MainWindow::on_doubleSpinBox_compareScale_valueChanged(double arg1)
{
    paintReferenceInTest();
}

QPoint MainWindow::findImage(QImage big, QImage small)
{
    QPoint bestPosition;
    double bestBonus = 0;
    double currentBonus = 0.0;

    int bigSizeX = big.size().width();
    int bigSizeY = big.size().height();
    int smallSizeX = small.size().width();
    int smallSizeY = small.size().height();

    int offsetXmax = bigSizeX - smallSizeX;
    int offsetYmax = bigSizeY - smallSizeY;

    for (int offsetY = 0; offsetY <= offsetYmax; offsetY++) {
        for (int offsetX = 0; offsetX < offsetXmax; offsetX++) {
            currentBonus = 0.0;
            QImage cutout = big.copy(offsetX, offsetY, smallSizeX, smallSizeY);

            for (int x = 0; x < smallSizeX; x++) {
                for (int y = 0; y < smallSizeY; y++) {
                    currentBonus += cutout.pixelColor(x, y).redF() * small.pixelColor(x, y).redF();
                }
            }

            if (currentBonus > bestBonus) {
                bestBonus = currentBonus;
                bestPosition = QPoint(offsetX, offsetY);
            }

        }
    }
    ui->label_compareResult->setNum(bestBonus);
    ui->spinBox_matchX->setValue(bestPosition.x());
    ui->spinBox_matchY->setValue(bestPosition.y());
    return bestPosition;
}
