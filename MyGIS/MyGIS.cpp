//#include "MyGIS.h"
//#include <QVBoxLayout>
//#include <QDebug>
//
//MyGIS::MyGIS(QWidget* parent)
//    : QMainWindow(parent), trueColorDisplay(nullptr), fakeColorDisplay(nullptr), countStatistics(nullptr), grayHistogram(nullptr) {
//    ui.setupUi(this);
//
//    trueColorDisplay = new TrueColorDisplay(this);
//    fakeColorDisplay = new FakeColorDisplay(this);
//    countStatistics = new count(this);
//    grayHistogram = new drawGrayHistogram(this);
//
//    // ���Ӳ˵�����ź����
//    connect(ui.actionTrue_color_display, SIGNAL(triggered()), trueColorDisplay, SLOT(importRasterData()));
//    connect(ui.actionFake_color_display, SIGNAL(triggered()), fakeColorDisplay, SLOT(importRasterData()));
//    connect(ui.actionDomain_statistics, SIGNAL(triggered()), countStatistics, SLOT(show())); // ��ʾͳ�ƴ���
//    connect(ui.actionDraw_gray_histogram, SIGNAL(triggered()), grayHistogram, SLOT(show())); // ��ʾ�Ҷ�ֱ��ͼ����
//}
//
//MyGIS::~MyGIS() {
//    delete trueColorDisplay;
//    delete fakeColorDisplay;
//    delete countStatistics;
//    delete grayHistogram;
//}

#include "MyGIS.h"
#include <QVBoxLayout>
#include <QDebug>

MyGIS::MyGIS(QWidget* parent)
    : QMainWindow(parent), trueColorDisplay(nullptr), fakeColorDisplay(nullptr), countStatistics(nullptr), grayHistogram(nullptr), histogramEqualizationDisplay(nullptr), rasterMaskExtraction(nullptr) {
    ui.setupUi(this);

    scene = new QGraphicsScene(this);
    view = new QGraphicsView(scene,ui.showview);
    view->setSceneRect(-500, -500, 1000, 1000);
    view->setDragMode(QGraphicsView::ScrollHandDrag);
    view->scale(1, -1); // Flip the Y axis
    QVBoxLayout* viewlayout = new QVBoxLayout(ui.showview);//view�Ĳ���ģʽ
    viewlayout->addWidget(view);
    ui.showview->setLayout(viewlayout);                     //



    setlayerManagelayout();                                 //����ͼ�������صĲ���

    trueColorDisplay = new TrueColorDisplay(this);
    fakeColorDisplay = new FakeColorDisplay(this);
    countStatistics = new count(this);
    grayHistogram = new drawGrayHistogram(this);
    histogramEqualizationDisplay = new HistogramEqualizationDisplay(this);
    rasterMaskExtraction = new RasterMaskExtraction(this);

    // ���Ӳ˵�����ź����
    connect(ui.actionTrue_color_display, SIGNAL(triggered()), trueColorDisplay, SLOT(importRasterData()));
    connect(ui.actionFake_color_display, SIGNAL(triggered()), fakeColorDisplay, SLOT(importRasterData()));
    connect(ui.actionDomain_statistics, SIGNAL(triggered()), countStatistics, SLOT(show())); // ��ʾͳ�ƴ���
    connect(ui.actionDraw_gray_histogram, SIGNAL(triggered()), grayHistogram, SLOT(show())); // ��ʾ�Ҷ�ֱ��ͼ����
    connect(ui.actionHistogram_equalization_enhanced_display, SIGNAL(triggered()), histogramEqualizationDisplay, SLOT(openAndProcessImage())); // ��ʾֱ��ͼ���⻯��ǿ��ʾ����
    connect(ui.actionGrid, SIGNAL(triggered()), rasterMaskExtraction, SLOT(openAndProcessImages())); // ��ʾդ����Ĥ��ȡ����
          
    connect(ui.actionVector_Data, SIGNAL(triggered()), this, SLOT(openFile()));



}

MyGIS::~MyGIS() {
    delete trueColorDisplay;
    delete fakeColorDisplay;
    delete countStatistics;
    delete grayHistogram;
    delete histogramEqualizationDisplay;
    delete rasterMaskExtraction;
}


