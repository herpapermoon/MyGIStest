#ifndef HISTOGRAMEQUALIZATIONDISPLAY_H
#define HISTOGRAMEQUALIZATIONDISPLAY_H

#include <QWidget>
#include <QImage>
#include <gdal_priv.h>

class HistogramEqualizationDisplay : public QWidget {
    Q_OBJECT

public:
    explicit HistogramEqualizationDisplay(QWidget* parent = nullptr);

public slots:
    void openAndProcessImage();

private:
    void displaySingleBandImage(const QImage& bandImage);
    QImage histogramEqualization(const QImage& image);
    QImage::Format determineQImageFormat(GDALDataType dataType);
};

#endif // HISTOGRAMEQUALIZATIONDISPLAY_H


