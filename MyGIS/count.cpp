#include "count.h"
#include <QFileDialog>
#include <QVBoxLayout>

count::count(QWidget* parent) : QMainWindow(parent), dataset(nullptr) {
    // 创建一个QWidget对象作为中心部件
    QWidget* centralWidget = new QWidget(this);
    // 设置中心部件
    setCentralWidget(centralWidget);

    // 创建一个QVBoxLayout对象，用于管理中心部件的布局
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);

    // 创建一个QPushButton对象，用于导入Landsat图像
    importButton = new QPushButton("Import Landsat Image", this);
    // 将导入按钮的clicked信号与count类的onImportButtonClicked()槽函数连接
    connect(importButton, &QPushButton::clicked, this, &count::onImportButtonClicked);
    // 将导入按钮添加到布局中
    layout->addWidget(importButton);

    // 创建一个QTextEdit对象，用于显示统计信息
    textEdit = new QTextEdit(this);
    // 设置QTextEdit为只读
    textEdit->setReadOnly(true);
    // 将QTextEdit添加到布局中
    layout->addWidget(textEdit);

    // 注册GDAL驱动
    GDALAllRegister();

    //// 设置窗口最大化
    //showMaximized();
}

count::~count() {
    // 如果dataset不为空，关闭dataset
    if (dataset != nullptr) {
        GDALClose(dataset);
    }
}

void count::onImportButtonClicked() {
    // 打开文件对话框，获取选择的文件名
    QString fileName = QFileDialog::getOpenFileName(this, "Open Landsat Image", "", "TIF files (*.tif)");
    // 如果文件名不为空
    if (!fileName.isEmpty()) {
        // 打开文件
        dataset = (GDALDataset*)GDALOpen(fileName.toStdString().c_str(), GA_ReadOnly);
        // 如果文件打开成功
        if (dataset != nullptr) {
            // 计算统计信息
            calculateStatistics();
        }
        // 如果文件打开失败
        else {
            // 在QTextEdit中显示失败信息
            textEdit->setText("Failed to open file.");
        }
    }
}

void count::calculateStatistics() {
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
    }
}
