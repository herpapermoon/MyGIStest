#include "StyleDialog.h"

StyleDialog::StyleDialog(QWidget* parent) : QDialog(parent), lineColor(Qt::black), fillColor(Qt::yellow), lineWidth(1) {
    QFormLayout* formLayout = new QFormLayout;

    QPushButton* lineColorButton = new QPushButton("Select line color");
    connect(lineColorButton, &QPushButton::clicked, this, &StyleDialog::selectLineColor);

    QPushButton* fillColorButton = new QPushButton("Select fill color");
    connect(fillColorButton, &QPushButton::clicked, this, &StyleDialog::selectFillColor);

    QSpinBox* lineWidthSpinBox = new QSpinBox;
    lineWidthSpinBox->setRange(1, 10);
    lineWidthSpinBox->setValue(lineWidth); // 初始化值
    connect(lineWidthSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &StyleDialog::setLineWidth);

    formLayout->addRow("line color:", lineColorButton);
    formLayout->addRow("fill color:", fillColorButton);
    formLayout->addRow("line width:", lineWidthSpinBox);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addLayout(formLayout);

    QPushButton* okButton = new QPushButton("OK");
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    layout->addWidget(okButton);

    setLayout(layout);
    setWindowTitle("set style");
}

void StyleDialog::selectLineColor() {
    lineColor = QColorDialog::getColor(lineColor, this, "Select line color");
}

void StyleDialog::selectFillColor() {
    fillColor = QColorDialog::getColor(fillColor, this, "Select fill color");
}

void StyleDialog::setLineWidth(int width) {
    lineWidth = width > 0 ? width : 1; // 确保宽度值为正
}
