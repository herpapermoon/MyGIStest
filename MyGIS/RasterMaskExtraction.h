#ifndef RASTERMASKEXTRACTION_H
#define RASTERMASKEXTRACTION_H

#include <QWidget>
#include <QImage>
#include <gdal_priv.h>

class RasterMaskExtraction : public QWidget {
    Q_OBJECT

public:
    explicit RasterMaskExtraction(QWidget* parent = nullptr);

public slots:
    void openAndProcessImages();

private:
    void displayImage(const QImage& image);
    QImage applyMask(const QImage& image, const QImage& mask);
    QImage::Format determineQImageFormat(GDALDataType dataType);
};

#endif // RASTERMASKEXTRACTION_H

