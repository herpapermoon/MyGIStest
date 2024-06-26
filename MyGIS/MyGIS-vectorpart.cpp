#include "MyGIS.h"
#include <QVBoxLayout>
#include <QDebug>



void MyGIS::openFile()          //打开矢量文件并创建图层
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
void MyGIS::addLayerControl(const QString& layerName){ //添加控制图层的控件
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


void MyGIS::toggleLayerVisibility(QString layerName) { //切换图层可见性
    if (layers.find(layerName) != layers.end()) {
        GISLayer* layer = layers[layerName];
        layer->setVisible(!layer->isVisible);
    }
}

void MyGIS::removeLayer(QString layerName) {        //删除layer
    if (layers.find(layerName) != layers.end()) {
        GISLayer* layer = layers[layerName];
        layer->removeLayer();
        layers.erase(layerName);
        updateLayerControls();
    }
}

void MyGIS::setLayerStyle(QString layerName) {      //设置图层样式
    if (layers.find(layerName) != layers.end()) {
        GISLayer* layer = layers[layerName];
        StyleDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted) {
            QPen pen(dialog.lineColor);
            pen.setWidthF(qMax(0.1, dialog.lineWidth / 10.0)); // 使用浮点数设置线宽，最小值为0.1
            QBrush brush(dialog.fillColor);
            layer->setStyle(pen, brush);
        }
    }
}



void MyGIS::updateLayerControls() {        //更新控制图层的控件
    QLayoutItem* item;
    while ((item = layerControlLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    for (const auto& layerPair : layers) {
        addLayerControl(layerPair.first);
    }
}

void MyGIS::zoomIn() {                      //放大函数
    view->scale(1.2, 1.2);
    updateLineWidths();
}

void MyGIS::zoomOut() {                     //缩小函数
    view->scale(1.0 / 1.2, 1.0 / 1.2);
    updateLineWidths();
}

void MyGIS::updateLineWidths() {
    qreal scale = view->transform().m11(); // 获取当前的缩放因子
    for (auto& layerPair : layers) {
        GISLayer* layer = layerPair.second;
        QPen pen = layer->pen;
        pen.setWidthF(qMax(0.1, 1.0 / scale)); // 根据缩放因子调整线宽，最小值为0.1
        layer->setStyle(pen, layer->brush);
    }
}

void MyGIS::setlayerManagelayout() {
       layerManagelayout = new QVBoxLayout(ui.layermanage);
// 创建并设置图层控制布局
    layerControlLayout = new QVBoxLayout(ui.layermanage);

    // 创建缩放控制布局
    zoomControlLayout = new QHBoxLayout;

    // 创建放大和缩小按钮
    zoomInButton = new QPushButton("in", ui.layermanage);
    connect(zoomInButton, &QPushButton::clicked, this, &MyGIS::zoomIn);
    zoomOutButton = new QPushButton("out", ui.layermanage);
    connect(zoomOutButton, &QPushButton::clicked, this, &MyGIS::zoomOut);

    // 将按钮添加到缩放控制布局
    zoomControlLayout->addWidget(zoomInButton);
    zoomControlLayout->addWidget(zoomOutButton);

    // 将缩放控制布局和图层控制布局添加到图层管理布局
    layerManagelayout->addLayout(zoomControlLayout);
    layerManagelayout->addLayout(layerControlLayout);




}