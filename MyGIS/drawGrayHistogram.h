#ifndef DRAWGRAYHISTOGRAM_H
#define DRAWGRAYHISTOGRAM_H

#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include <gdal_priv.h>

class drawGrayHistogram : public QMainWindow {
    Q_OBJECT

public:
    drawGrayHistogram(QWidget* parent = nullptr);
    ~drawGrayHistogram();

private slots:
    void onImportButtonClicked();

private:
    void calculateStatistics();
    void drawHistogram(GDALRasterBand* poBand);

    QPushButton* importButton;
    QTextEdit* textEdit;
    QLabel* histogramLabel;
    GDALDataset* dataset = nullptr;
};

#endif // DRAWGRAYHISTOGRAM_H


