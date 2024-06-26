#include "drawGrayHistogram.h"
#include <QFileDialog>
#include <QVBoxLayout>
#include <QPainter>
#include <vector>
#include <algorithm>

drawGrayHistogram::drawGrayHistogram(QWidget* parent) : QMainWindow(parent) {
    // ����һ��QWidget������Ϊ���Ĳ���
    QWidget* centralWidget = new QWidget(this);
    // �������Ĳ���
    setCentralWidget(centralWidget);

    // ����һ��QVBoxLayout�������ڹ������Ĳ����Ĳ���
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);

    // ����һ��QPushButton�������ڵ���Landsatͼ��
    importButton = new QPushButton("Import Landsat Image", this);
    // �����밴ť��clicked�ź���drawGrayHistogram���onImportButtonClicked()�ۺ�������
    connect(importButton, &QPushButton::clicked, this, &drawGrayHistogram::onImportButtonClicked);
    // �����밴ť��ӵ�������
    layout->addWidget(importButton);

    // ����һ��QTextEdit����������ʾͳ����Ϣ
    textEdit = new QTextEdit(this);
    // ����QTextEditΪֻ��
    textEdit->setReadOnly(true);
    // ��QTextEdit��ӵ�������
    layout->addWidget(textEdit);

    // ����һ��QLabel����������ʾ�Ҷ�ֱ��ͼ
    histogramLabel = new QLabel(this);
    // ��QLabel����Ϊ��������
    histogramLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    // ��QLabel��ӵ�������
    layout->addWidget(histogramLabel);

    // ע��GDAL����
    GDALAllRegister();
}

drawGrayHistogram::~drawGrayHistogram() {
    // ���dataset��Ϊ�գ��ر�dataset
    if (dataset != nullptr) {
        GDALClose(dataset);
    }
}

void drawGrayHistogram::onImportButtonClicked() {
    // ���ļ��Ի��򣬻�ȡѡ����ļ���
    QString fileName = QFileDialog::getOpenFileName(this, "Open Landsat Image", "", "TIF files (*.tif)");
    // ����ļ�����Ϊ��
    if (!fileName.isEmpty()) {
        // ���ļ�
        dataset = (GDALDataset*)GDALOpen(fileName.toStdString().c_str(), GA_ReadOnly);
        // ����ļ��򿪳ɹ�
        if (dataset != nullptr) {
            // ����ͳ����Ϣ������ֱ��ͼ
            calculateStatistics();
        }
        // ����ļ���ʧ��
        else {
            // ��QTextEdit����ʾʧ����Ϣ
            textEdit->setText("Failed to open file.");
        }
    }
}

void drawGrayHistogram::calculateStatistics() {
    // ���datasetΪ�գ�ֱ�ӷ���
    if (dataset == nullptr) return;

    // ���QTextEdit�е�����
    textEdit->clear();
    // ����ÿ������
    for (int i = 1; i <= dataset->GetRasterCount(); i++) {
        // ��ȡ����
        GDALRasterBand* poBand = dataset->GetRasterBand(i);
        // ��ȡ���ο��С
        int nBlockXSize, nBlockYSize;
        poBand->GetBlockSize(&nBlockXSize, &nBlockYSize);

        // ����ͳ����Ϣ
        double min, max, mean, stdDev;
        poBand->ComputeStatistics(FALSE, &min, &max, &mean, &stdDev, nullptr, nullptr);

        // ��QTextEdit����ʾͳ����Ϣ
        textEdit->append(QString("Band %1 statistics:").arg(i));
        textEdit->append(QString("Min: %1").arg(min));
        textEdit->append(QString("Max: %1").arg(max));
        textEdit->append(QString("Mean: %1").arg(mean));
        textEdit->append(QString("Standard Deviation: %1").arg(stdDev));
        textEdit->append("");

        // ���ƻҶ�ֱ��ͼ
        drawHistogram(poBand);
    }
}

void drawGrayHistogram::drawHistogram(GDALRasterBand* poBand) {
    int nXSize = poBand->GetXSize();
    int nYSize = poBand->GetYSize();
    int nBins = 256;
    std::vector<int> histogram(nBins, 0);

    std::vector<uchar> buffer(nXSize);
    for (int y = 0; y < nYSize; ++y) {
        poBand->RasterIO(GF_Read, 0, y, nXSize, 1, buffer.data(), nXSize, 1, GDT_Byte, 0, 0);
        for (int x = 0; x < nXSize; ++x) {
            int value = buffer[x];
            histogram[value]++;
        }
    }

    int maxCount = *std::max_element(histogram.begin(), histogram.end());

    QPixmap pixmap(histogramLabel->size());
    pixmap.fill(Qt::white);
    QPainter painter(&pixmap);
    painter.setPen(Qt::black);

    double xScale = pixmap.width() / 256.0;
    double yScale = pixmap.height() / static_cast<double>(maxCount);

    for (int i = 0; i < 256; ++i) {
        int barHeight = static_cast<int>(histogram[i] * yScale);
        QRect barRect(i * xScale, pixmap.height() - barHeight, xScale, barHeight);
        painter.fillRect(barRect, Qt::black);
    }

    histogramLabel->setPixmap(pixmap);
}

