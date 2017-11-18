#include "imagefilter.h"
#include "imagetransform.h"

// Copyright Peter Diener

ImageFilter::ImageFilter()
{

}

// Result is the sum of the numeric distance of two images.
// The larger the value the larger the distance, meaning more different images.
quint32 ImageFilter::imageCompare(QImage *referenceImage, QImage *sampleImage)
{
    quint64 sum = 0;

    for (qint32 y = 1; y < referenceImage->height() - 1; y++)
        for (qint32 x = 1; x < referenceImage->width() - 1; x++)
        {
            QRgb ref_pixel = referenceImage->pixel(x, y);
            QRgb sample_pixel = sampleImage->pixel(x, y);

            sum += ImageTransform::colorDistance(QColor(ref_pixel), QColor(sample_pixel));
        }

    return (sum);
}

