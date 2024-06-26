#include "MyGIS.h"
#include <QVBoxLayout>
#include <QDebug>



void MyGIS::openFile()          //��ʸ���ļ�������ͼ��
{
    QString fileName = QFileDialog::getOpenFileName(this, "open shapefile", "", "Shapefiles (*.shp)");
    if (!fileName.isEmpty()) {
        QString layerName = QInputDialog::getText(this, "layer name", "Please set layer name:");
        if (!layerName.isEmpty()) {
            GISLayer* layer = new GISLayer(layerName, this->scene);
            if (layer->loadShapefile(fileName)) {
                layers[layerName] = layer;
                addLayerControl(layerName);
            }
            else {
                delete layer;
            }
        }
    }
}
void MyGIS::addLayerControl(const QString& layerName){ //��ӿ���ͼ��Ŀؼ�
    QWidget* layerWidget = new QWidget;
    QHBoxLayout* layerLayout = new QHBoxLayout;

    QCheckBox* visibilityCheckBox = new QCheckBox(layerName);
    visibilityCheckBox->setChecked(true);

    connect(visibilityCheckBox, &QCheckBox::toggled, this, [this, layerName](bool checked) {
        toggleLayerVisibility(layerName);
        });

    QPushButton* deleteButton = new QPushButton("delete");
    connect(deleteButton, &QPushButton::clicked, this, [this, layerName]() {
        removeLayer(layerName);
        });

    QPushButton* styleButton = new QPushButton("set style");
    connect(styleButton, &QPushButton::clicked, this, [this, layerName]() {
        setLayerStyle(layerName);
        });

    layerLayout->addWidget(visibilityCheckBox);
    layerLayout->addWidget(styleButton);
    layerLayout->addWidget(deleteButton);
    layerWidget->setLayout(layerLayout);


    layerControlLayout->addWidget(layerWidget);
}


void MyGIS::toggleLayerVisibility(QString layerName) { //�л�ͼ��ɼ���
    if (layers.find(layerName) != layers.end()) {
        GISLayer* layer = layers[layerName];
        layer->setVisible(!layer->isVisible);
    }
}

void MyGIS::removeLayer(QString layerName) {        //ɾ��layer
    if (layers.find(layerName) != layers.end()) {
        GISLayer* layer = layers[layerName];
        layer->removeLayer();
        layers.erase(layerName);
        updateLayerControls();
    }
}

void MyGIS::setLayerStyle(QString layerName) {      //����ͼ����ʽ
    if (layers.find(layerName) != layers.end()) {
        GISLayer* layer = layers[layerName];
        StyleDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted) {
            QPen pen(dialog.lineColor);
            pen.setWidthF(qMax(0.1, dialog.lineWidth / 10.0)); // ʹ�ø����������߿���СֵΪ0.1
            QBrush brush(dialog.fillColor);
            layer->setStyle(pen, brush);
        }
    }
}



void MyGIS::updateLayerControls() {        //���¿���ͼ��Ŀؼ�
    QLayoutItem* item;
    while ((item = layerControlLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    for (const auto& layerPair : layers) {
        addLayerControl(layerPair.first);
    }
}

void MyGIS::zoomIn() {                      //�Ŵ���
    view->scale(1.2, 1.2);
    updateLineWidths();
}

void MyGIS::zoomOut() {                     //��С����
    view->scale(1.0 / 1.2, 1.0 / 1.2);
    updateLineWidths();
}

void MyGIS::updateLineWidths() {
    qreal scale = view->transform().m11(); // ��ȡ��ǰ����������
    for (auto& layerPair : layers) {
        GISLayer* layer = layerPair.second;
        QPen pen = layer->pen;
        pen.setWidthF(qMax(0.1, 1.0 / scale)); // �����������ӵ����߿���СֵΪ0.1
        layer->setStyle(pen, layer->brush);
    }
}

void MyGIS::setlayerManagelayout() {
       layerManagelayout = new QVBoxLayout(ui.layermanage);
// ����������ͼ����Ʋ���
    layerControlLayout = new QVBoxLayout(ui.layermanage);

    // �������ſ��Ʋ���
    zoomControlLayout = new QHBoxLayout;

    // �����Ŵ����С��ť
    zoomInButton = new QPushButton("in", ui.layermanage);
    connect(zoomInButton, &QPushButton::clicked, this, &MyGIS::zoomIn);
    zoomOutButton = new QPushButton("out", ui.layermanage);
    connect(zoomOutButton, &QPushButton::clicked, this, &MyGIS::zoomOut);

    // ����ť��ӵ����ſ��Ʋ���
    zoomControlLayout->addWidget(zoomInButton);
    zoomControlLayout->addWidget(zoomOutButton);

    // �����ſ��Ʋ��ֺ�ͼ����Ʋ�����ӵ�ͼ�������
    layerManagelayout->addLayout(zoomControlLayout);
    layerManagelayout->addLayout(layerControlLayout);




}