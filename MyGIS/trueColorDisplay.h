//#ifndef TRUECOLORDISPLAY_H
//#define TRUECOLORDISPLAY_H
//
//#include <QMainWindow>
//#include <QImage>
//#include <gdal_priv.h>
//#include <vector>
//#include <QString>
//#include <QList>
//#include <QObject>
//#include <QLabel>
//#include <QWheelEvent>
//#include <QMouseEvent>
//#include <QProgressBar>
//
//class ImageLabel : public QLabel {
//    Q_OBJECT
//public:
//    ImageLabel(QWidget* parent = nullptr);
//
//protected:
//    void wheelEvent(QWheelEvent* event) override;
//    void mousePressEvent(QMouseEvent* event) override;
//    void mouseMoveEvent(QMouseEvent* event) override;
//    void mouseReleaseEvent(QMouseEvent* event) override;
//
//private:
//    double scaleFactor;
//    QPoint lastMousePosition;
//    bool mousePressed;
//};
//
//class TrueColorDisplay : public QObject {
//    Q_OBJECT
//
//public:
//    TrueColorDisplay(QWidget *parent = nullptr);
//
//public slots:
//    void importRasterData();
//
//private:
//    void displaySingleBandImage(const QImage& bandImage, const GDALColorTable* colorTable);
//    void displayColorComposite(const QList<QImage>& bandImages, bool isTrueColor = true);
//
//    QWidget* parent;
//    QProgressBar* progressBar;
//};
//
//#endif // TRUECOLORDISPLAY_H

#ifndef TRUECOLORDISPLAY_H
#define TRUECOLORDISPLAY_H

#include <QMainWindow>
#include <QImage>
#include <gdal_priv.h>
#include <vector>
#include <QString>
#include <QList>
#include <QObject>
#include "ImageLabel.h"

class TrueColorDisplay : public QObject {
    Q_OBJECT

public:
    TrueColorDisplay(QWidget* parent = nullptr);

public slots:
    void importRasterData();

private:
    void displaySingleBandImage(const QImage& bandImage, const GDALColorTable* colorTable);
    void displayColorComposite(const QList<QImage>& bandImages, bool isTrueColor = true);

    QWidget* parent;
};

#endif // TRUECOLORDISPLAY_H




