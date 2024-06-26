#include "RasterMaskExtraction.h"
#include "ImageLabel.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QDebug>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <vector>

RasterMaskExtraction::RasterMaskExtraction(QWidget* parent)
    : QWidget(parent) {
}

void RasterMaskExtraction::openAndProcessImages() {
    QDialog bandSelectionDialog(this);
    bandSelectionDialog.setWindowTitle("Select Raster and Mask");
    QFormLayout formLayout(&bandSelectionDialog);

    QLineEdit rasterFileLineEdit;
    QPushButton rasterFileButton("Select Raster File", &bandSelectionDialog);
    formLayout.addRow("Raster File:", &rasterFileLineEdit);
    formLayout.addWidget(&rasterFileButton);

    QComboBox rasterBandComboBox;
    formLayout.addRow("Select Band:", &rasterBandComboBox);

    QLineEdit maskFileLineEdit;
    QPushButton maskFileButton("Select Mask File", &bandSelectionDialog);
    formLayout.addRow("Mask File:", &maskFileLineEdit);
    formLayout.addWidget(&maskFileButton);

    connect(&rasterFileButton, &QPushButton::clicked, [&]() {
        QString fileName = QFileDialog::getOpenFileName(this, "Select Raster Data", "", "Raster files (*.tif *.tiff *.img *.bmp *.png *.jpg)");
        if (!fileName.isEmpty()) {
            rasterFileLineEdit.setText(fileName);
            GDALDataset* poDataset = (GDALDataset*)GDALOpen(fileName.toStdString().c_str(), GA_ReadOnly);
            if (poDataset) {
                rasterBandComboBox.clear();
                for (int j = 1; j <= poDataset->GetRasterCount(); ++j) {
                    rasterBandComboBox.addItem(QString("Band %1").arg(j));
                }
                GDALClose(poDataset);
            }
        }
        });

    connect(&maskFileButton, &QPushButton::clicked, [&]() {
        QString fileName = QFileDialog::getOpenFileName(this, "Select Mask Data", "", "Raster files (*.tif *.tiff *.img *.bmp *.png *.jpg)");
        if (!fileName.isEmpty()) {
            maskFileLineEdit.setText(fileName);
        }
        });

    QPushButton okButton("OK", &bandSelectionDialog);
    formLayout.addWidget(&okButton);
    connect(&okButton, &QPushButton::clicked, [&]() {
        if (rasterFileLineEdit.text().isEmpty() || maskFileLineEdit.text().isEmpty()) {
            QMessageBox::warning(this, "Warning", "You must select both raster and mask files.");
        }
        else {
            bandSelectionDialog.accept();
        }
        });

    bandSelectionDialog.setLayout(&formLayout);

    if (bandSelectionDialog.exec() != QDialog::Accepted) {
        QMessageBox::information(this, "Info", "Raster and Mask selection cancelled.");
        return;
    }

    GDALAllRegister();

    QString rasterFileName = rasterFileLineEdit.text();
    int rasterBandIndex = rasterBandComboBox.currentIndex() + 1;
    QString maskFileName = maskFileLineEdit.text();

    GDALDataset* rasterDataset = (GDALDataset*)GDALOpen(rasterFileName.toStdString().c_str(), GA_ReadOnly);
    if (rasterDataset == nullptr) {
        qDebug() << "Failed to open raster data file:" << CPLGetLastErrorMsg();
        QMessageBox::critical(this, "Error", "Failed to open raster data file");
        return;
    }

    GDALDataset* maskDataset = (GDALDataset*)GDALOpen(maskFileName.toStdString().c_str(), GA_ReadOnly);
    if (maskDataset == nullptr) {
        qDebug() << "Failed to open mask data file:" << CPLGetLastErrorMsg();
        QMessageBox::critical(this, "Error", "Failed to open mask data file");
        GDALClose(rasterDataset);
        return;
    }

    GDALRasterBand* rasterBand = rasterDataset->GetRasterBand(rasterBandIndex);
    GDALRasterBand* maskBand = maskDataset->GetRasterBand(1);

    int nXSize = rasterBand->GetXSize();
    int nYSize = rasterBand->GetYSize();
    GDALDataType rasterDataType = rasterBand->GetRasterDataType();
    QImage::Format imageFormat = determineQImageFormat(rasterDataType);
    QImage rasterImage(nXSize, nYSize, imageFormat);
    QImage maskImage(nXSize, nYSize, imageFormat);

    if (rasterDataType == GDT_Byte) {
        std::vector<GByte> rasterBuffer(nXSize * nYSize);
        std::vector<GByte> maskBuffer(nXSize * nYSize);
        rasterBand->RasterIO(GF_Read, 0, 0, nXSize, nYSize, rasterBuffer.data(), nXSize, nYSize, GDT_Byte, 0, 0);
        maskBand->RasterIO(GF_Read, 0, 0, nXSize, nYSize, maskBuffer.data(), nXSize, nYSize, GDT_Byte, 0, 0);
        for (int y = 0; y < nYSize; ++y) {
            for (int x = 0; x < nXSize; ++x) {
                GByte value = rasterBuffer[y * nXSize + x];
                GByte maskValue = maskBuffer[y * nXSize + x];
                rasterImage.setPixel(x, y, qRgb(value, value, value));
                maskImage.setPixel(x, y, qRgb(maskValue, maskValue, maskValue));
            }
        }
    }
    else if (rasterDataType == GDT_UInt16) {
        std::vector<GUInt16> rasterBuffer(nXSize * nYSize);
        std::vector<GUInt16> maskBuffer(nXSize * nYSize);
        rasterBand->RasterIO(GF_Read, 0, 0, nXSize, nYSize, rasterBuffer.data(), nXSize, nYSize, GDT_UInt16, 0, 0);
        maskBand->RasterIO(GF_Read, 0, 0, nXSize, nYSize, maskBuffer.data(), nXSize, nYSize, GDT_UInt16, 0, 0);
        for (int y = 0; y < nYSize; ++y) {
            for (int x = 0; x < nXSize; ++x) {
                GUInt16 value = rasterBuffer[y * nXSize + x];
                GUInt16 maskValue = maskBuffer[y * nXSize + x];
                rasterImage.setPixel(x, y, qRgb(value >> 8, value >> 8, value >> 8));
                maskImage.setPixel(x, y, qRgb(maskValue >> 8, maskValue >> 8, maskValue >> 8));
            }
        }
    }
    else if (rasterDataType == GDT_Int16) {
        std::vector<GInt16> rasterBuffer(nXSize * nYSize);
        std::vector<GInt16> maskBuffer(nXSize * nYSize);
        rasterBand->RasterIO(GF_Read, 0, 0, nXSize, nYSize, rasterBuffer.data(), nXSize, nYSize, GDT_Int16, 0, 0);
        maskBand->RasterIO(GF_Read, 0, 0, nXSize, nYSize, maskBuffer.data(), nXSize, nYSize, GDT_Int16, 0, 0);
        for (int y = 0; y < nYSize; ++y) {
            for (int x = 0; x < nXSize; ++x) {
                GInt16 value = rasterBuffer[y * nXSize + x];
                GInt16 maskValue = maskBuffer[y * nXSize + x];
                rasterImage.setPixel(x, y, qRgb(value >> 8, value >> 8, value >> 8));
                maskImage.setPixel(x, y, qRgb(maskValue >> 8, maskValue >> 8, maskValue >> 8));
            }
        }
    }
    else {
        rasterBand->RasterIO(GF_Read, 0, 0, nXSize, nYSize, rasterImage.bits(), nXSize, nYSize, rasterDataType, 0, 0);
        maskBand->RasterIO(GF_Read, 0, 0, nXSize, nYSize, maskImage.bits(), nXSize, nYSize, rasterDataType, 0, 0);
    }

    QImage resultImage = applyMask(rasterImage, maskImage);
    displayImage(resultImage);

    GDALClose(rasterDataset);
    GDALClose(maskDataset);
}

void RasterMaskExtraction::displayImage(const QImage& image) {
    ImageLabel* imageLabel = new ImageLabel(this);
    imageLabel->setPixmap(QPixmap::fromImage(image));
    imageLabel->setScaledContents(true);

    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidget(imageLabel);
    scrollArea->setAlignment(Qt::AlignCenter);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(scrollArea);
    setLayout(layout);

    showMaximized();
}

QImage RasterMaskExtraction::applyMask(const QImage& image, const QImage& mask) {
    QImage result(image.size(), image.format());

    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            QRgb pixelValue = image.pixel(x, y);
            QRgb maskValue = mask.pixel(x, y);

            if (qGray(maskValue) > 0) { // 假设非零值表示保留
                result.setPixel(x, y, pixelValue);
            }
            else {
                result.setPixel(x, y, qRgb(0, 0, 0)); // 否则设置为黑色
            }
        }
    }

    return result;
}

QImage::Format RasterMaskExtraction::determineQImageFormat(GDALDataType dataType) {
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
