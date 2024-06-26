#include "count.h"
#include <QFileDialog>
#include <QVBoxLayout>

count::count(QWidget* parent) : QMainWindow(parent), dataset(nullptr) {
    // ����һ��QWidget������Ϊ���Ĳ���
    QWidget* centralWidget = new QWidget(this);
    // �������Ĳ���
    setCentralWidget(centralWidget);

    // ����һ��QVBoxLayout�������ڹ������Ĳ����Ĳ���
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);

    // ����һ��QPushButton�������ڵ���Landsatͼ��
    importButton = new QPushButton("Import Landsat Image", this);
    // �����밴ť��clicked�ź���count���onImportButtonClicked()�ۺ�������
    connect(importButton, &QPushButton::clicked, this, &count::onImportButtonClicked);
    // �����밴ť��ӵ�������
    layout->addWidget(importButton);

    // ����һ��QTextEdit����������ʾͳ����Ϣ
    textEdit = new QTextEdit(this);
    // ����QTextEditΪֻ��
    textEdit->setReadOnly(true);
    // ��QTextEdit��ӵ�������
    layout->addWidget(textEdit);

    // ע��GDAL����
    GDALAllRegister();

    //// ���ô������
    //showMaximized();
}

count::~count() {
    // ���dataset��Ϊ�գ��ر�dataset
    if (dataset != nullptr) {
        GDALClose(dataset);
    }
}

void count::onImportButtonClicked() {
    // ���ļ��Ի��򣬻�ȡѡ����ļ���
    QString fileName = QFileDialog::getOpenFileName(this, "Open Landsat Image", "", "TIF files (*.tif)");
    // ����ļ�����Ϊ��
    if (!fileName.isEmpty()) {
        // ���ļ�
        dataset = (GDALDataset*)GDALOpen(fileName.toStdString().c_str(), GA_ReadOnly);
        // ����ļ��򿪳ɹ�
        if (dataset != nullptr) {
            // ����ͳ����Ϣ
            calculateStatistics();
        }
        // ����ļ���ʧ��
        else {
            // ��QTextEdit����ʾʧ����Ϣ
            textEdit->setText("Failed to open file.");
        }
    }
}

void count::calculateStatistics() {
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
    }
}
