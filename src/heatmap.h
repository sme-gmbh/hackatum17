#ifndef HEATMAP_H
#define HEATMAP_H

#include <QWidget>
#include <QLabel>
#include <QSize>
#include <QPainter>
#include <QPixmap>

class HeatMap : public QLabel
{
    Q_OBJECT
public:
    explicit HeatMap(QWidget *parent = 0);

    void initialize(QSize size, int pointSize);
    void paint(QPoint pos, double intensity);
    void finish();

private:

    QSize pointSize;
    QImage *image;
    QPainter painter;
    QPen pen;
    QBrush brush;

signals:

public slots:
};

#endif // HEATMAP_H
