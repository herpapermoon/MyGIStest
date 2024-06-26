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
//    // 连接菜单项的信号与槽
//    connect(ui.actionTrue_color_display, SIGNAL(triggered()), trueColorDisplay, SLOT(importRasterData()));
//    connect(ui.actionFake_color_display, SIGNAL(triggered()), fakeColorDisplay, SLOT(importRasterData()));
//    connect(ui.actionDomain_statistics, SIGNAL(triggered()), countStatistics, SLOT(show())); // 显示统计窗口
//    connect(ui.actionDraw_gray_histogram, SIGNAL(triggered()), grayHistogram, SLOT(show())); // 显示灰度直方图窗口
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
    QVBoxLayout* viewlayout = new QVBoxLayout(ui.showview);//view的布局模式
    viewlayout->addWidget(view);
    ui.showview->setLayout(viewlayout);                     //



    setlayerManagelayout();                                 //设置图层管理相关的布局

    trueColorDisplay = new TrueColorDisplay(this);
    fakeColorDisplay = new FakeColorDisplay(this);
    countStatistics = new count(this);
    grayHistogram = new drawGrayHistogram(this);
    histogramEqualizationDisplay = new HistogramEqualizationDisplay(this);
    rasterMaskExtraction = new RasterMaskExtraction(this);

    // 连接菜单项的信号与槽
    connect(ui.actionTrue_color_display, SIGNAL(triggered()), trueColorDisplay, SLOT(importRasterData()));
    connect(ui.actionFake_color_display, SIGNAL(triggered()), fakeColorDisplay, SLOT(importRasterData()));
    connect(ui.actionDomain_statistics, SIGNAL(triggered()), countStatistics, SLOT(show())); // 显示统计窗口
    connect(ui.actionDraw_gray_histogram, SIGNAL(triggered()), grayHistogram, SLOT(show())); // 显示灰度直方图窗口
    connect(ui.actionHistogram_equalization_enhanced_display, SIGNAL(triggered()), histogramEqualizationDisplay, SLOT(openAndProcessImage())); // 显示直方图均衡化增强显示窗口
    connect(ui.actionGrid, SIGNAL(triggered()), rasterMaskExtraction, SLOT(openAndProcessImages())); // 显示栅格掩膜提取窗口
          
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


