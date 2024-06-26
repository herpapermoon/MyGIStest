#ifndef STYLEDIALOG_H
#define STYLEDIALOG_H

#include <QDialog>
#include <QColorDialog>
#include <QFormLayout>
#include <QSpinBox>
#include <QPushButton>
#include <QVBoxLayout>

class StyleDialog : public QDialog {
    Q_OBJECT
public:
    QColor lineColor;
    QColor fillColor;
    int lineWidth;

    StyleDialog(QWidget* parent = nullptr);

public slots:
    void selectLineColor();
    void selectFillColor();
    void setLineWidth(int width);
};

#endif // STYLEDIALOG_H
