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
    TrueColorDisplay* trueColorDisplay; // �������ɫ��ʾ����
    FakeColorDisplay* fakeColorDisplay; // ���ڼٲ�ɫ��ʾ����
    count* countStatistics; // ����ͳ�Ƶ���
    drawGrayHistogram* grayHistogram; // ���ڻ��ƻҶ�ֱ��ͼ����
    HistogramEqualizationDisplay* histogramEqualizationDisplay; // ����ֱ��ͼ���⻯��ʾ����
    RasterMaskExtraction* rasterMaskExtraction; // ����դ����Ĥ��ȡ����
};

#endif // MYGIS_H
