#ifndef FAKECOLORDISPLAY_H
#define FAKECOLORDISPLAY_H

#include <QMainWindow>
#include <QImage>
#include <gdal_priv.h>
#include <vector>
#include <QString>
#include <QList>
#include <QObject>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QDialog>
#include "ImageLabel.h"

class FakeColorDisplay : public QObject {
    Q_OBJECT

public:
    FakeColorDisplay(QWidget* parent = nullptr);

public slots:
    void importRasterData();

private:
    void displayColorComposite(const QList<QImage>& bandImages, bool isTrueColor = false);

    QWidget* parent;
    QDialog* bandSelectionDialog;
    QFormLayout* formLayout;
    QList<QLineEdit*> fileLineEdits;
    QList<QPushButton*> fileButtons;
    QList<QComboBox*> bandComboBoxes;
};

#endif // FAKECOLORDISPLAY_H


