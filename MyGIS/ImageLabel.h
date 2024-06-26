#ifndef IMAGELABEL_H
#define IMAGELABEL_H

#include <QLabel>
#include <QWheelEvent>
#include <QMouseEvent>

class ImageLabel : public QLabel {
    Q_OBJECT
public:
    ImageLabel(QWidget* parent = nullptr);

protected:
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    double scaleFactor;
    QPoint lastMousePosition;
    bool mousePressed;
};

#endif // IMAGELABEL_H

