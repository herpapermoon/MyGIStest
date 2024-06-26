//#include "fakeColorDisplay.h"
//#include <QFileDialog>
//#include <QMessageBox>
//#include <QScrollArea>
//#include <QVBoxLayout>
//#include <QDebug>
//#include <QPushButton>
//#include <QDialog>
//
//// FakeColorDisplay implementation
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
//FakeColorDisplay::FakeColorDisplay(QWidget* parent)
//    : QObject(parent), parent(parent), formLayout(nullptr) {}
//
//void FakeColorDisplay::importRasterData() {
//    bandSelectionDialog = new QDialog(parent);
//    bandSelectionDialog->setWindowTitle("Select Bands");
//    formLayout = new QFormLayout(bandSelectionDialog);
//
//    for (int i = 0; i < 3; ++i) {
//        QLineEdit* fileLineEdit = new QLineEdit(bandSelectionDialog);
//        QPushButton* fileButton = new QPushButton("Select File", bandSelectionDialog);
//        formLayout->addRow(QString("File for Band %1:").arg(i + 1), fileLineEdit);
//        formLayout->addWidget(fileButton);
//        fileLineEdits.append(fileLineEdit);
//        fileButtons.append(fileButton);
//
//        QComboBox* bandComboBox = new QComboBox(bandSelectionDialog);
//        formLayout->addRow(QString("Select Band for Band %1:").arg(i + 1), bandComboBox);
//        bandComboBoxes.append(bandComboBox);
//
//        // Connect button to slot for file selection
//        connect(fileButton, SIGNAL(clicked()), this, SLOT(selectFile()));
//    }
//
//    QPushButton* okButton = new QPushButton("OK", bandSelectionDialog);
//    formLayout->addWidget(okButton);
//    connect(okButton, SIGNAL(clicked()), bandSelectionDialog, SLOT(accept()));
//
//    bandSelectionDialog->setLayout(formLayout);
//    bandSelectionDialog->exec();
//
//    if (bandSelectionDialog->result() != QDialog::Accepted) {
//        QMessageBox::information(parent, "Info", "Band selection cancelled.");
//        return;
//    }
//
//    GDALAllRegister();
//
//    QList<QImage> bandImages;
//    for (int i = 0; i < 3; ++i) {
//        QString fileName = fileLineEdits[i]->text();
//        int bandIndex = bandComboBoxes[i]->currentIndex() + 1;
//
//        qDebug() << "Opening file:" << fileName << "Band index:" << bandIndex;
//
//        GDALDataset* poDataset = (GDALDataset*)GDALOpen(fileName.toStdString().c_str(), GA_ReadOnly);
//        if (poDataset == nullptr) {
//            qDebug() << "Failed to open raster data file:" << CPLGetLastErrorMsg();
//            QMessageBox::critical(parent, "Error", "Failed to open raster data file");
//            return;
//        }
//
//        GDALRasterBand* poBand = poDataset->GetRasterBand(bandIndex);
//        if (poBand == nullptr) {
//            qDebug() << "Failed to get band:" << bandIndex;
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
//        GDALClose(poDataset);
//    }
//
//    displayColorComposite(bandImages, false);
//}
//
//void FakeColorDisplay::selectFile() {
//    QPushButton* button = qobject_cast<QPushButton*>(sender());
//    if (!button) return;
//
//    int index = fileButtons.indexOf(button);
//    if (index == -1) return;
//
//    QString fileName = QFileDialog::getOpenFileName(parent, "Select Raster Data", "", "Raster files (*.tif *.tiff *.img *.bmp *.png *.jpg)");
//    if (!fileName.isEmpty()) {
//        fileLineEdits[index]->setText(fileName);
//        GDALDataset* poDataset = (GDALDataset*)GDALOpen(fileName.toStdString().c_str(), GA_ReadOnly);
//        if (poDataset) {
//            bandComboBoxes[index]->clear();
//            for (int j = 1; j <= poDataset->GetRasterCount(); ++j) {
//                bandComboBoxes[index]->addItem(QString("Band %1").arg(j));
//            }
//            GDALClose(poDataset);
//        }
//        else {
//            qDebug() << "Failed to open raster data file for combo box:" << CPLGetLastErrorMsg();
//        }
//    }
//}
//
//void FakeColorDisplay::displayColorComposite(const QList<QImage>& bandImages, bool isTrueColor) {
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

#include "fakeColorDisplay.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QDebug>
#include <QPushButton>
#include <QDialog>

// FakeColorDisplay implementation
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
}

FakeColorDisplay::FakeColorDisplay(QWidget* parent)
    : QObject(parent), parent(parent), formLayout(nullptr) {}

void FakeColorDisplay::importRasterData() {
    bandSelectionDialog = new QDialog(parent);
    bandSelectionDialog->setWindowTitle("Select Bands");
    formLayout = new QFormLayout(bandSelectionDialog);

    for (int i = 0; i < 3; ++i) {
        QLineEdit* fileLineEdit = new QLineEdit(bandSelectionDialog);
        QPushButton* fileButton = new QPushButton("Select File", bandSelectionDialog);
        formLayout->addRow(QString("File for Band %1:").arg(i + 1), fileLineEdit);
        formLayout->addWidget(fileButton);
        fileLineEdits.append(fileLineEdit);
        fileButtons.append(fileButton);

        QComboBox* bandComboBox = new QComboBox(bandSelectionDialog);
        formLayout->addRow(QString("Select Band for Band %1:").arg(i + 1), bandComboBox);
        bandComboBoxes.append(bandComboBox);

        // Connect button to slot for file selection
        connect(fileButton, &QPushButton::clicked, this, [this, fileLineEdit, bandComboBox]() {
            QString fileName = QFileDialog::getOpenFileName(parent, "Select Raster Data", "", "Raster files (*.tif *.tiff *.img *.bmp *.png *.jpg)");
            if (!fileName.isEmpty()) {
                fileLineEdit->setText(fileName);
                GDALAllRegister();
                GDALDataset* poDataset = (GDALDataset*)GDALOpen(fileName.toStdString().c_str(), GA_ReadOnly);
                if (poDataset) {
                    bandComboBox->clear();
                    for (int j = 1; j <= poDataset->GetRasterCount(); ++j) {
                        bandComboBox->addItem(QString("Band %1").arg(j));
                    }
                    GDALClose(poDataset);
                }
            }
            });
    }

    QPushButton* okButton = new QPushButton("OK", bandSelectionDialog);
    formLayout->addWidget(okButton);
    connect(okButton, &QPushButton::clicked, bandSelectionDialog, &QDialog::accept);

    bandSelectionDialog->setLayout(formLayout);
    bandSelectionDialog->exec();

    if (bandSelectionDialog->result() != QDialog::Accepted) {
        QMessageBox::information(parent, "Info", "Band selection cancelled.");
        return;
    }

    GDALAllRegister();

    QList<QImage> bandImages;
    for (int i = 0; i < 3; ++i) {
        QString fileName = fileLineEdits[i]->text();
        int bandIndex = bandComboBoxes[i]->currentIndex() + 1;

        qDebug() << "Opening file:" << fileName << "Band index:" << bandIndex;

        GDALDataset* poDataset = (GDALDataset*)GDALOpen(fileName.toStdString().c_str(), GA_ReadOnly);
        if (poDataset == nullptr) {
            qDebug() << "Failed to open raster data file:" << CPLGetLastErrorMsg();
            QMessageBox::critical(parent, "Error", "Failed to open raster data file");
            return;
        }

        GDALRasterBand* poBand = poDataset->GetRasterBand(bandIndex);
        if (poBand == nullptr) {
            qDebug() << "Failed to get band:" << bandIndex;
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
        GDALClose(poDataset);
    }

    displayColorComposite(bandImages, false);
}

void FakeColorDisplay::displayColorComposite(const QList<QImage>& bandImages, bool isTrueColor) {
    if (bandImages.size() < 3) {
        QMessageBox::information(parent, "Warning", "Not enough band images to display");
        return;
    }

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
