#include "HistogramEqualizationDisplay.h"
#include "ImageLabel.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QDebug>
#include <vector>
#include <QMainWindow>
#include <numeric>

HistogramEqualizationDisplay::HistogramEqualizationDisplay(QWidget* parent)
    : QWidget(parent) {
}

void HistogramEqualizationDisplay::openAndProcessImage() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open Raster Data", "", "Raster files (*.tif *.tiff *.img *.bmp *.png *.jpg)");
    qDebug() << "Selected file:" << fileName;

    if (fileName.isEmpty()) {
        QMessageBox::information(this, "Info", "No file selected");
        return;
    }

    GDALDataset* poDataset = (GDALDataset*)GDALOpen(fileName.toStdString().c_str(), GA_ReadOnly);

    if (poDataset == nullptr) {
        qDebug() << "Failed to open raster data file:" << CPLGetLastErrorMsg();
        QMessageBox::critical(this, "Error", "Failed to open raster data file");
        return;
    }

    int nBands = poDataset->GetRasterCount();
    qDebug() << "Number of bands:" << nBands;

    if (nBands < 1) {
        QMessageBox::critical(this, "Error", "Raster data file contains no bands");
        GDALClose(poDataset);
        return;
    }

    int rasterXSize = poDataset->GetRasterXSize();
    int rasterYSize = poDataset->GetRasterYSize();
    const char* projectionRef = poDataset->GetProjectionRef();
    qDebug() << "Raster Size: " << rasterXSize << " x " << rasterYSize;
    qDebug() << "Projection: " << projectionRef;

    QList<QImage> bandImages;
    for (int i = 1; i <= nBands; ++i) {
        GDALRasterBand* poBand = poDataset->GetRasterBand(i);
        int nXSize = poBand->GetXSize();
        int nYSize = poBand->GetYSize();
        GDALDataType dataType = poBand->GetRasterDataType();
        QImage::Format imageFormat = determineQImageFormat(dataType);
        QImage image(nXSize, nYSize, imageFormat);

        if (dataType == GDT_Byte) {
            std::vector<GByte> buffer(nXSize * nYSize);
            poBand->RasterIO(GF_Read, 0, 0, nXSize, nYSize, buffer.data(), nXSize, nYSize, GDT_Byte, 0, 0);
            for (int y = 0; y < nYSize; ++y) {
                for (int x = 0; x < nXSize; ++x) {
                    GByte value = buffer[y * nXSize + x];
                    image.setPixel(x, y, qRgb(value, value, value));
                }
            }
        }
        else if (dataType == GDT_UInt16) {
            std::vector<GUInt16> buffer(nXSize * nYSize);
            poBand->RasterIO(GF_Read, 0, 0, nXSize, nYSize, buffer.data(), nXSize, nYSize, GDT_UInt16, 0, 0);
            for (int y = 0; y < nYSize; ++y) {
                for (int x = 0; x < nXSize; ++x) {
                    image.setPixel(x, y, qRgb(buffer[y * nXSize + x] >> 8, buffer[y * nXSize + x] >> 8, buffer[y * nXSize + x] >> 8));
                }
            }
        }
        else if (dataType == GDT_Int16) {
            std::vector<GInt16> buffer(nXSize * nYSize);
            poBand->RasterIO(GF_Read, 0, 0, nXSize, nYSize, buffer.data(), nXSize, nYSize, GDT_Int16, 0, 0);
            for (int y = 0; y < nYSize; ++y) {
                for (int x = 0; x < nXSize; ++x) {
                    image.setPixel(x, y, qRgb(buffer[y * nXSize + x] >> 8, buffer[y * nXSize + x] >> 8, buffer[y * nXSize + x] >> 8));
                }
            }
        }
        else {
            poBand->RasterIO(GF_Read, 0, 0, nXSize, nYSize, image.bits(), nXSize, nYSize, dataType, 0, 0);
        }

        bandImages.append(image);
    }

    if (nBands == 1) {
        displaySingleBandImage(bandImages[0]);
    }
    else {
        QMessageBox::critical(this, "Error", "Raster data file contains more than 1 band");
    }

    GDALClose(poDataset);
}

void HistogramEqualizationDisplay::displaySingleBandImage(const QImage& bandImage) {
    QImage equalizedImage = histogramEqualization(bandImage);

    ImageLabel* imageLabel = new ImageLabel(this);
    imageLabel->setPixmap(QPixmap::fromImage(equalizedImage));
    imageLabel->setScaledContents(true);

    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidget(imageLabel);
    scrollArea->setAlignment(Qt::AlignCenter);

    QWidget* container = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(container);
    layout->addWidget(scrollArea);
    container->setLayout(layout);
    container->show();

    QMainWindow* mainWindow = new QMainWindow();
    mainWindow->setCentralWidget(container);
    mainWindow->showMaximized();
}

QImage HistogramEqualizationDisplay::histogramEqualization(const QImage& image) {
    QImage equalizedImage = image;
    int width = image.width();
    int height = image.height();

    // 计算直方图
    std::vector<int> histogram(256, 0);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int gray = qGray(image.pixel(x, y));
            histogram[gray]++;
        }
    }

    // 计算累积分布函数（CDF）
    std::vector<int> cdf(256, 0);
    std::partial_sum(histogram.begin(), histogram.end(), cdf.begin());

    // 归一化 CDF
    int totalPixels = width * height;
    for (int i = 0; i < 256; ++i) {
        cdf[i] = static_cast<int>(255.0 * cdf[i] / totalPixels);
    }

    // 应用均衡化
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int gray = qGray(image.pixel(x, y));
            int equalizedGray = cdf[gray];
            equalizedImage.setPixel(x, y, qRgb(equalizedGray, equalizedGray, equalizedGray));
        }
    }

    return equalizedImage;
}

QImage::Format HistogramEqualizationDisplay::determineQImageFormat(GDALDataType dataType) {
    switch (dataType) {
    case GDT_UInt16:
    case GDT_Int16:
        return QImage::Format_Grayscale16;
    case GDT_UInt32:
    case GDT_Int32:
    case GDT_Float32:
    case GDT_Float64:
        return QImage::Format_RGB32;
    case GDT_Byte:
    default:
        return QImage::Format_Grayscale8;
    }
}
