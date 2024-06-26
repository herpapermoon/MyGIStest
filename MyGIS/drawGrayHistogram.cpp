#include "drawGrayHistogram.h"
#include <QFileDialog>
#include <QVBoxLayout>
#include <QPainter>
#include <vector>
#include <algorithm>

drawGrayHistogram::drawGrayHistogram(QWidget* parent) : QMainWindow(parent) {
    // 创建一个QWidget对象作为中心部件
    QWidget* centralWidget = new QWidget(this);
    // 设置中心部件
    setCentralWidget(centralWidget);

    // 创建一个QVBoxLayout对象，用于管理中心部件的布局
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);

    // 创建一个QPushButton对象，用于导入Landsat图像
    importButton = new QPushButton("Import Landsat Image", this);
    // 将导入按钮的clicked信号与drawGrayHistogram类的onImportButtonClicked()槽函数连接
    connect(importButton, &QPushButton::clicked, this, &drawGrayHistogram::onImportButtonClicked);
    // 将导入按钮添加到布局中
    layout->addWidget(importButton);

    // 创建一个QTextEdit对象，用于显示统计信息
    textEdit = new QTextEdit(this);
    // 设置QTextEdit为只读
    textEdit->setReadOnly(true);
    // 将QTextEdit添加到布局中
    layout->addWidget(textEdit);

    // 创建一个QLabel对象，用于显示灰度直方图
    histogramLabel = new QLabel(this);
    // 将QLabel设置为可伸缩的
    histogramLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    // 将QLabel添加到布局中
    layout->addWidget(histogramLabel);

    // 注册GDAL驱动
    GDALAllRegister();
}

drawGrayHistogram::~drawGrayHistogram() {
    // 如果dataset不为空，关闭dataset
    if (dataset != nullptr) {
        GDALClose(dataset);
    }
}

void drawGrayHistogram::onImportButtonClicked() {
    // 打开文件对话框，获取选择的文件名
    QString fileName = QFileDialog::getOpenFileName(this, "Open Landsat Image", "", "TIF files (*.tif)");
    // 如果文件名不为空
    if (!fileName.isEmpty()) {
        // 打开文件
        dataset = (GDALDataset*)GDALOpen(fileName.toStdString().c_str(), GA_ReadOnly);
        // 如果文件打开成功
        if (dataset != nullptr) {
            // 计算统计信息并绘制直方图
            calculateStatistics();
        }
        // 如果文件打开失败
        else {
            // 在QTextEdit中显示失败信息
            textEdit->setText("Failed to open file.");
        }
    }
}

void drawGrayHistogram::calculateStatistics() {
    // 如果dataset为空，直接返回
    if (dataset == nullptr) return;

    // 清空QTextEdit中的内容
    textEdit->clear();
    // 遍历每个波段
    for (int i = 1; i <= dataset->GetRasterCount(); i++) {
        // 获取波段
        GDALRasterBand* poBand = dataset->GetRasterBand(i);
        // 获取波段块大小
        int nBlockXSize, nBlockYSize;
        poBand->GetBlockSize(&nBlockXSize, &nBlockYSize);

        // 计算统计信息
        double min, max, mean, stdDev;
        poBand->ComputeStatistics(FALSE, &min, &max, &mean, &stdDev, nullptr, nullptr);

        // 在QTextEdit中显示统计信息
        textEdit->append(QString("Band %1 statistics:").arg(i));
        textEdit->append(QString("Min: %1").arg(min));
        textEdit->append(QString("Max: %1").arg(max));
        textEdit->append(QString("Mean: %1").arg(mean));
        textEdit->append(QString("Standard Deviation: %1").arg(stdDev));
        textEdit->append("");

        // 绘制灰度直方图
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

