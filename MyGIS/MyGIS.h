#ifndef MYGIS_H
#define MYGIS_H

#include <QtWidgets/QMainWindow>
#include "ui_MyGIS.h"
#include "trueColorDisplay.h"
#include "fakeColorDisplay.h"
#include "count.h"
#include "drawGrayHistogram.h"
#include "HistogramEqualizationDisplay.h"
#include "RasterMaskExtraction.h"
#include <QMainWindow>
#include <QGraphicsView>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QInputDialog>
#include <QCheckBox>
#include <QHBoxLayout>
#include "StyleDialog.h"
#include "GISLayer.h"

class MyGIS : public QMainWindow {
    Q_OBJECT

public:
    MyGIS(QWidget* parent = nullptr);
    ~MyGIS();

    QGraphicsScene* scene;
    QGraphicsView* view;
    QString currentPath;
    std::map<QString, GISLayer*> layers;
    QVBoxLayout* layerControlLayout;
    QHBoxLayout* zoomControlLayout;
    QVBoxLayout* layerManagelayout;

    QPushButton* zoomInButton;
    QPushButton* zoomOutButton;

public slots:
    void openFile();
    void toggleLayerVisibility(QString layerName);
    void removeLayer(QString layerName);
    void setLayerStyle(QString layerName);
    void addLayerControl(const QString& layerName);
    void updateLayerControls();
    void zoomIn();
    void zoomOut();
    void updateLineWidths();

    void setlayerManagelayout();



private:
    Ui::MyGISClass ui;
    TrueColorDisplay* trueColorDisplay; // 用于真彩色显示的类
    FakeColorDisplay* fakeColorDisplay; // 用于假彩色显示的类
    count* countStatistics; // 用于统计的类
    drawGrayHistogram* grayHistogram; // 用于绘制灰度直方图的类
    HistogramEqualizationDisplay* histogramEqualizationDisplay; // 用于直方图均衡化显示的类
    RasterMaskExtraction* rasterMaskExtraction; // 用于栅格掩膜提取的类
};

#endif // MYGIS_H
