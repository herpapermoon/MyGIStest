//#include "trueColorDisplay.h"
//#include <QApplication>
//#include <QMainWindow>
//#include <QImage>
//#include <QFileDialog>
//#include <QLabel>
//#include <QMessageBox>
//#include <QMenuBar>
//#include <QAction>
//#include <QScrollArea>
//#include <QVBoxLayout>
//#include <gdal_priv.h>
//#include <vector>
//#include <QString>
//#include <QWidget>
//#include <QList>
//#include <QDebug>
//#include <QPlainTextEdit>
//#include <QVBoxLayout>
//#include <QMutex>
//#include <QMutexLocker>
//#include <QScreen>
//#include <QWheelEvent>
//#include <QProgressDialog>
//
//ImageLabel::ImageLabel(QWidget* parent) : QLabel(parent), scaleFactor(1.0), mousePressed(false) {
//    setAlignment(Qt::AlignCenter);
//    setScaledContents(true); // Enable automatic scaling
//}
//
//void ImageLabel::wheelEvent(QWheelEvent* event) {
//    if (event->angleDelta().y() > 0) {
//        scaleFactor *= 1.1;
//    }
//    else {
//        scaleFactor /= 1.1;
//    }
//    resize(scaleFactor * pixmap()->size());
//}
//
//void ImageLabel::mousePressEvent(QMouseEvent* event) {
//    if (event->button() == Qt::LeftButton) {
//        lastMousePosition = event->pos();
//        mousePressed = true;
//    }
//}
//
//void ImageLabel::mouseMoveEvent(QMouseEvent* event) {
//    if (mousePressed) {
//        int dx = event->pos().x() - lastMousePosition.x();
//        int dy = event->pos().y() - lastMousePosition.y();
//        parentWidget()->parentWidget()->scroll(dx, dy);
//        lastMousePosition = event->pos();
//    }
//}
//
//void ImageLabel::mouseReleaseEvent(QMouseEvent* event) {
//    if (event->button() == Qt::LeftButton) {
//        mousePressed = false;
//    }
//}
//
//QImage::Format determineQImageFormat(GDALDataType dataType) {
//    switch (dataType) {
//    case GDT_UInt16:
//    case GDT_Int16:
//        return QImage::Format_Grayscale16;
//    case GDT_UInt32:
//    case GDT_Int32:
//    case GDT_Float32:
//    case GDT_Float64:
//        return QImage::Format_RGB32;
//    case GDT_Byte:
//    default:
//        return QImage::Format_Grayscale8;
//    }
//}
//
//QImage applyColorTable(const QImage& grayImage, const GDALColorTable* colorTable) {
//    QImage colorImage(grayImage.size(), QImage::Format_RGB32);
//    for (int y = 0; y < grayImage.height(); ++y) {
//        for (int x = 0; x < grayImage.width(); ++x) {
//            int pixelValue = qGray(grayImage.pixel(x, y));
//            if (colorTable && pixelValue < colorTable->GetColorEntryCount()) {
//                const GDALColorEntry* entry = colorTable->GetColorEntry(pixelValue);
//                colorImage.setPixel(x, y, qRgb(entry->c1, entry->c2, entry->c3));
//            }
//            else {
//                colorImage.setPixel(x, y, qRgb(pixelValue, pixelValue, pixelValue));
//            }
//        }
//    }
//    return colorImage;
//}
//
//TrueColorDisplay::TrueColorDisplay(QWidget* parent)
//    : parent(parent), progressBar(new QProgressBar(parent)) {
//    progressBar->setRange(0, 100);
//    progressBar->setValue(0);
//    progressBar->setVisible(false);
//}
//
//void TrueColorDisplay::importRasterData() {
//    QString fileName = QFileDialog::getOpenFileName(parent, "Open Raster Data", "", "Raster files (*.tif *.tiff *.img *.bmp *.png *.jpg)");
//    if (fileName.isEmpty()) {
//        QMessageBox::information(parent, "Info", "No file selected");
//        return;
//    }
//
//    QProgressDialog progressDialog("Opening raster data...", "Cancel", 0, 100, parent);
//    progressDialog.setWindowModality(Qt::WindowModal);
//    progressDialog.show();
//
//    GDALAllRegister();
//    GDALDataset* poDataset = (GDALDataset*)GDALOpen(fileName.toStdString().c_str(), GA_ReadOnly);
//
//    if (poDataset == nullptr) {
//        qDebug() << "Failed to open raster data file:" << CPLGetLastErrorMsg();
//        QMessageBox::critical(parent, "Error", "Failed to open raster data file");
//        return;
//    }
//
//    int nBands = poDataset->GetRasterCount();
//    if (nBands < 1) {
//        QMessageBox::critical(parent, "Error", "Raster data file contains no bands");
//        GDALClose(poDataset);
//        return;
//    }
//
//    QList<QImage> bandImages;
//    for (int i = 1; i <= nBands; ++i) {
//        GDALRasterBand* poBand = poDataset->GetRasterBand(i);
//        int nXSize = poBand->GetXSize();
//        int nYSize = poBand->GetYSize();
//        GDALDataType dataType = poBand->GetRasterDataType();
//        QImage::Format imageFormat = determineQImageFormat(dataType);
//        QImage image(nXSize, nYSize, imageFormat);
//
//        std::vector<GByte> buffer(nXSize * nYSize * GDALGetDataTypeSizeBytes(dataType));
//        poBand->RasterIO(GF_Read, 0, 0, nXSize, nYSize, buffer.data(), nXSize, nYSize, dataType, 0, 0);
//
//        if (dataType == GDT_Byte) {
//            for (int y = 0; y < nYSize; ++y) {
//                for (int x = 0; x < nXSize; ++x) {
//                    GByte value = buffer[y * nXSize + x];
//                    image.setPixel(x, y, qRgb(value, value, value));
//                }
//            }
//        }
//        else if (dataType == GDT_UInt16 || dataType == GDT_Int16) {
//            for (int y = 0; y < nYSize; ++y) {
//                for (int x = 0; x < nXSize; ++x) {
//                    int value = buffer[(y * nXSize + x) * 2];
//                    image.setPixel(x, y, qRgb(value >> 8, value >> 8, value >> 8));
//                }
//            }
//        }
//        else {
//            poBand->RasterIO(GF_Read, 0, 0, nXSize, nYSize, image.bits(), nXSize, nYSize, dataType, 0, 0);
//        }
//
//        bandImages.append(image);
//
//        // 更新进度条
//        int progress = static_cast<int>((static_cast<double>(i) / nBands) * 100);
//        progressDialog.setValue(progress);
//        if (progressDialog.wasCanceled()) {
//            GDALClose(poDataset);
//            return;
//        }
//    }
//
//    if (nBands == 1) {
//        GDALRasterBand* poBand = poDataset->GetRasterBand(1);
//        const GDALColorTable* colorTable = nullptr;
//        try {
//            colorTable = poBand->GetColorTable();
//        }
//        catch (const std::exception& e) {
//            qDebug() << "Error retrieving color table:" << e.what();
//        }
//        displaySingleBandImage(bandImages[0], colorTable);
//    }
//    else if (nBands >= 3) {
//        displayColorComposite(bandImages, true);
//    }
//    else {
//        QMessageBox::critical(parent, "Error", "Raster data file contains less than 3 bands");
//    }
//
//    GDALClose(poDataset);
//    progressDialog.setValue(100);
//}
//
//void TrueColorDisplay::displaySingleBandImage(const QImage& bandImage, const GDALColorTable* colorTable) {
//    QImage colorImage = colorTable ? applyColorTable(bandImage, colorTable) : bandImage;
//    ImageLabel* imageLabel = new ImageLabel;
//    imageLabel->setPixmap(QPixmap::fromImage(colorImage));
//
//    QScrollArea* scrollArea = new QScrollArea;
//    scrollArea->setWidget(imageLabel);
//    scrollArea->setAlignment(Qt::AlignCenter);
//
//    QVBoxLayout* layout = new QVBoxLayout;
//    layout->addWidget(scrollArea);
//    QWidget* container = new QWidget;
//    container->setLayout(layout);
//    container->show();
//
//    QMainWindow* mainWindow = new QMainWindow();
//    mainWindow->setCentralWidget(container);
//    mainWindow->showMaximized();
//}
//
//void TrueColorDisplay::displayColorComposite(const QList<QImage>& bandImages, bool isTrueColor) {
//    if (bandImages.size() < 3) {
//        QMessageBox::information(parent, "Warning", "Not enough band images to display");
//        return;
//    }
//
//    int width = bandImages[0].width();
//    int height = bandImages[0].height();
//    QImage colorImage(width, height, QImage::Format_RGB32);
//
//    int redIndex = isTrueColor ? 0 : 2;
//    int greenIndex = 1;
//    int blueIndex = isTrueColor ? 2 : 0;
//
//    for (int y = 0; y < height; ++y) {
//        for (int x = 0; x < width; ++x) {
//            QRgb pixelValue = qRgb(
//                qRed(bandImages[redIndex].pixel(x, y)),
//                qGreen(bandImages[greenIndex].pixel(x, y)),
//                qBlue(bandImages[blueIndex].pixel(x, y))
//            );
//            colorImage.setPixel(x, y, pixelValue);
//        }
//    }
//
//    ImageLabel* imageLabel = new ImageLabel;
//    imageLabel->setPixmap(QPixmap::fromImage(colorImage));
//
//    QScrollArea* scrollArea = new QScrollArea;
//    scrollArea->setWidget(imageLabel);
//    scrollArea->setAlignment(Qt::AlignCenter);
//
//    QVBoxLayout* layout = new QVBoxLayout;
//    layout->addWidget(scrollArea);
//    QWidget* container = new QWidget;
//    container->setLayout(layout);
//    container->show();
//
//    QMainWindow* mainWindow = new QMainWindow();
//    mainWindow->setCentralWidget(container);
//    mainWindow->showMaximized();
//}

//#include "trueColorDisplay.h"
//#include <QFileDialog>
//#include <QMessageBox>
//#include <QScrollArea>
//#include <QVBoxLayout>
//#include <QDebug>
//
//namespace {
//    // 定义 determineQImageFormat 函数
//    QImage::Format determineQImageFormat(GDALDataType dataType) {
//        switch (dataType) {
//        case GDT_UInt16:
//        case GDT_Int16:
//            return QImage::Format_Grayscale16;
//        case GDT_UInt32:
//        case GDT_Int32:
//        case GDT_Float32:
//        case GDT_Float64:
//            return QImage::Format_RGB32;
//        case GDT_Byte:
//        default:
//            return QImage::Format_Grayscale8;
//        }
//    }
//}
//
//TrueColorDisplay::TrueColorDisplay(QWidget* parent)
//    : QObject(parent), parent(parent) {}
//
//void TrueColorDisplay::importRasterData() {
//    QString fileName = QFileDialog::getOpenFileName(parent, "Open Raster Data", "", "Raster files (*.tif *.tiff *.img *.bmp *.png *.jpg)");
//    if (fileName.isEmpty()) {
//        QMessageBox::information(parent, "Info", "No file selected");
//        return;
//    }
//
//    GDALAllRegister();
//    GDALDataset* poDataset = (GDALDataset*)GDALOpen(fileName.toStdString().c_str(), GA_ReadOnly);
//    if (poDataset == nullptr) {
//        qDebug() << "Failed to open raster data file:" << CPLGetLastErrorMsg();
//        QMessageBox::critical(parent, "Error", "Failed to open raster data file");
//        return;
//    }
//
//    int nBands = poDataset->GetRasterCount();
//    if (nBands < 3) {
//        QMessageBox::critical(parent, "Error", "Raster data file contains less than 3 bands");
//        GDALClose(poDataset);
//        return;
//    }
//
//    QList<QImage> bandImages;
//    for (int i = 1; i <= 3; ++i) {
//        GDALRasterBand* poBand = poDataset->GetRasterBand(i);
//        if (poBand == nullptr) {
//            qDebug() << "Failed to get band:" << i;
//            QMessageBox::critical(parent, "Error", "Failed to get band");
//            GDALClose(poDataset);
//            return;
//        }
//
//        int nXSize = poBand->GetXSize();
//        int nYSize = poBand->GetYSize();
//        GDALDataType dataType = poBand->GetRasterDataType();
//        QImage::Format imageFormat = determineQImageFormat(dataType);
//        QImage image(nXSize, nYSize, imageFormat);
//
//        std::vector<GByte> buffer(nXSize * nYSize * GDALGetDataTypeSizeBytes(dataType));
//        poBand->RasterIO(GF_Read, 0, 0, nXSize, nYSize, buffer.data(), nXSize, nYSize, dataType, 0, 0);
//
//        if (dataType == GDT_Byte) {
//            for (int y = 0; y < nYSize; ++y) {
//                for (int x = 0; x < nXSize; ++x) {
//                    GByte value = buffer[y * nXSize + x];
//                    image.setPixel(x, y, qRgb(value, value, value));
//                }
//            }
//        }
//        else if (dataType == GDT_UInt16 || dataType == GDT_Int16) {
//            for (int y = 0; y < nYSize; ++y) {
//                for (int x = 0; x < nXSize; ++x) {
//                    int value = buffer[(y * nXSize + x) * 2];
//                    image.setPixel(x, y, qRgb(value >> 8, value >> 8, value >> 8));
//                }
//            }
//        }
//        else {
//            poBand->RasterIO(GF_Read, 0, 0, nXSize, nYSize, image.bits(), nXSize, nYSize, dataType, 0, 0);
//        }
//
//        bandImages.append(image);
//    }
//
//    displayColorComposite(bandImages, true);
//    GDALClose(poDataset);
//}
//
//void TrueColorDisplay::displayColorComposite(const QList<QImage>& bandImages, bool isTrueColor) {
//    int width = bandImages[0].width();
//    int height = bandImages[0].height();
//    QImage colorImage(width, height, QImage::Format_RGB32);
//
//    int redIndex = isTrueColor ? 0 : 2;
//    int greenIndex = 1;
//    int blueIndex = isTrueColor ? 2 : 0;
//
//    for (int y = 0; y < height; ++y) {
//        for (int x = 0; x < width; ++x) {
//            QRgb pixelValue = qRgb(
//                qRed(bandImages[redIndex].pixel(x, y)),
//                qGreen(bandImages[greenIndex].pixel(x, y)),
//                qBlue(bandImages[blueIndex].pixel(x, y))
//            );
//            colorImage.setPixel(x, y, pixelValue);
//        }
//    }
//
//    ImageLabel* imageLabel = new ImageLabel(parent);
//    imageLabel->setPixmap(QPixmap::fromImage(colorImage));
//
//    QScrollArea* scrollArea = new QScrollArea(parent);
//    scrollArea->setWidget(imageLabel);
//    scrollArea->setAlignment(Qt::AlignCenter);
//
//    QVBoxLayout* layout = new QVBoxLayout;
//    layout->addWidget(scrollArea);
//    QWidget* container = new QWidget;
//    container->setLayout(layout);
//    container->show();
//
//    QMainWindow* mainWindow = new QMainWindow();
//    mainWindow->setCentralWidget(container);
//    mainWindow->showMaximized();
//}

#include "trueColorDisplay.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QDebug>

namespace {
    // 定义 determineQImageFormat 函数
    QImage::Format determineQImageFormat(GDALDataType dataType) {
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

    // 应用配色表到单波段灰度图像
    QImage applyColorTable(const QImage& grayImage, const GDALColorTable* colorTable) {
        QImage colorImage(grayImage.size(), QImage::Format_RGB32);
        for (int y = 0; y < grayImage.height(); ++y) {
            for (int x = 0; x < grayImage.width(); ++x) {
                int pixelValue = qGray(grayImage.pixel(x, y));
                if (colorTable && pixelValue < colorTable->GetColorEntryCount()) {
                    const GDALColorEntry* entry = colorTable->GetColorEntry(pixelValue);
                    colorImage.setPixel(x, y, qRgb(entry->c1, entry->c2, entry->c3));
                }
                else {
                    colorImage.setPixel(x, y, qRgb(pixelValue, pixelValue, pixelValue));
                }
            }
        }
        return colorImage;
    }
}

TrueColorDisplay::TrueColorDisplay(QWidget* parent)
    : QObject(parent), parent(parent) {}

void TrueColorDisplay::importRasterData() {
    QString fileName = QFileDialog::getOpenFileName(parent, "Open Raster Data", "", "Raster files (*.tif *.tiff *.img *.bmp *.png *.jpg)");
    if (fileName.isEmpty()) {
        QMessageBox::information(parent, "Info", "No file selected");
        return;
    }

    GDALAllRegister();
    GDALDataset* poDataset = (GDALDataset*)GDALOpen(fileName.toStdString().c_str(), GA_ReadOnly);
    if (poDataset == nullptr) {
        qDebug() << "Failed to open raster data file:" << CPLGetLastErrorMsg();
        QMessageBox::critical(parent, "Error", "Failed to open raster data file");
        return;
    }

    int nBands = poDataset->GetRasterCount();
    if (nBands < 1) {
        QMessageBox::critical(parent, "Error", "Raster data file contains no bands");
        GDALClose(poDataset);
        return;
    }

    QList<QImage> bandImages;
    for (int i = 1; i <= nBands; ++i) {
        GDALRasterBand* poBand = poDataset->GetRasterBand(i);
        if (poBand == nullptr) {
            qDebug() << "Failed to get band:" << i;
            QMessageBox::critical(parent, "Error", "Failed to get band");
            GDALClose(poDataset);
            return;
        }

        int nXSize = poBand->GetXSize();
        int nYSize = poBand->GetYSize();
        GDALDataType dataType = poBand->GetRasterDataType();
        QImage::Format imageFormat = determineQImageFormat(dataType);
        QImage image(nXSize, nYSize, imageFormat);

        std::vector<GByte> buffer(nXSize * nYSize * GDALGetDataTypeSizeBytes(dataType));
        poBand->RasterIO(GF_Read, 0, 0, nXSize, nYSize, buffer.data(), nXSize, nYSize, dataType, 0, 0);

        if (dataType == GDT_Byte) {
            for (int y = 0; y < nYSize; ++y) {
                for (int x = 0; x < nXSize; ++x) {
                    GByte value = buffer[y * nXSize + x];
                    image.setPixel(x, y, qRgb(value, value, value));
                }
            }
        }
        else if (dataType == GDT_UInt16 || dataType == GDT_Int16) {
            for (int y = 0; y < nYSize; ++y) {
                for (int x = 0; x < nXSize; ++x) {
                    int value = buffer[(y * nXSize + x) * 2];
                    image.setPixel(x, y, qRgb(value >> 8, value >> 8, value >> 8));
                }
            }
        }
        else {
            poBand->RasterIO(GF_Read, 0, 0, nXSize, nYSize, image.bits(), nXSize, nYSize, dataType, 0, 0);
        }

        bandImages.append(image);
    }

    if (nBands == 1) {
        // 获取第一个波段
        GDALRasterBand* poBand = poDataset->GetRasterBand(1);
        const GDALColorTable* colorTable = nullptr;
        try {
            colorTable = poBand->GetColorTable();
        }
        catch (const std::exception& e) {
            qDebug() << "Error retrieving color table:" << e.what();
        }

        displaySingleBandImage(bandImages[0], colorTable);
    }
    else if (nBands >= 3) {
        displayColorComposite(bandImages, true);
        displayColorComposite(bandImages, false);
    }
    else {
        QMessageBox::critical(parent, "Error", "Raster data file contains less than 3 bands");
    }

    GDALClose(poDataset);
}

void TrueColorDisplay::displaySingleBandImage(const QImage& bandImage, const GDALColorTable* colorTable) {
    QImage colorImage;
    if (colorTable) {
        colorImage = applyColorTable(bandImage, colorTable);
    }
    else {
        colorImage = bandImage;
    }

    ImageLabel* imageLabel = new ImageLabel(parent);
    imageLabel->setPixmap(QPixmap::fromImage(colorImage));
    imageLabel->setScaledContents(true);

    QScrollArea* scrollArea = new QScrollArea(parent);
    scrollArea->setWidget(imageLabel);
    scrollArea->setAlignment(Qt::AlignCenter);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(scrollArea);
    QWidget* container = new QWidget;
    container->setLayout(layout);
    container->show();

    QMainWindow* mainWindow = new QMainWindow();
    mainWindow->setCentralWidget(container);
    mainWindow->showMaximized();
}

void TrueColorDisplay::displayColorComposite(const QList<QImage>& bandImages, bool isTrueColor) {
    int width = bandImages[0].width();
    int height = bandImages[0].height();
    QImage colorImage(width, height, QImage::Format_RGB32);

    int redIndex = isTrueColor ? 0 : 2;
    int greenIndex = 1;
    int blueIndex = isTrueColor ? 2 : 0;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            QRgb pixelValue = qRgb(
                qRed(bandImages[redIndex].pixel(x, y)),
                qGreen(bandImages[greenIndex].pixel(x, y)),
                qBlue(bandImages[blueIndex].pixel(x, y))
            );
            colorImage.setPixel(x, y, pixelValue);
        }
    }

    ImageLabel* imageLabel = new ImageLabel(parent);
    imageLabel->setPixmap(QPixmap::fromImage(colorImage));

    QScrollArea* scrollArea = new QScrollArea(parent);
    scrollArea->setWidget(imageLabel);
    scrollArea->setAlignment(Qt::AlignCenter);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(scrollArea);
    QWidget* container = new QWidget;
    container->setLayout(layout);
    container->show();

    QMainWindow* mainWindow = new QMainWindow();
    mainWindow->setCentralWidget(container);
    mainWindow->showMaximized();
}

