#ifndef IMAGEFILTER_H
#define IMAGEFILTER_H

// Copyright Peter Diener

#include "QImage"

class ImageFilter
{
public:
    ImageFilter();
    quint32 imageCompare(QImage *referenceImage, QImage *sampleImage);
};

#endif // IMAGEFILTER_H
