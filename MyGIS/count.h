#ifndef COUNT_H
#define COUNT_H

#include <QtWidgets/QMainWindow>
#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <gdal_priv.h>

class count : public QMainWindow {
    Q_OBJECT

public:
    count(QWidget* parent = nullptr);
    ~count();

private slots:
    void onImportButtonClicked();

private:
    QPushButton* importButton;
    QTextEdit* textEdit;
    GDALDataset* dataset;

    void calculateStatistics();
};

#endif // COUNT_H

