#include "heatmap.h"

HeatMap::HeatMap(QWidget *parent) : QLabel(parent)
{

}

void HeatMap::initialize(QSize size, int pointSize)
{
    image = new QImage(size.width(), size.height(), QImage::Format_RGBA8888);
    image->fill(Qt::white);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.begin(image);
    pen.setCapStyle(Qt::RoundCap);
    pen.setWidth(pointSize);
}

void HeatMap::paint(QPoint pos, double intensity)
{
    if (intensity >= 1.0)
        intensity = 1.0;
    if (intensity < 0.0)
        intensity = 0.0;

    pen.setColor(QColor(0, 0, 0, intensity * 30));
    painter.setPen(pen);
    painter.drawPoint(pos);
}

void HeatMap::finish()
{
    painter.end();

    this->setPixmap(QPixmap::fromImage(*image));
    delete image;
}
