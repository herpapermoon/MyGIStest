#include "ImageLabel.h"

ImageLabel::ImageLabel(QWidget* parent) : QLabel(parent), scaleFactor(1.0), mousePressed(false) {
    setAlignment(Qt::AlignCenter);
    setScaledContents(true); // Enable automatic scaling
}

void ImageLabel::wheelEvent(QWheelEvent* event) {
    if (event->angleDelta().y() > 0) {
        scaleFactor *= 1.1;
    }
    else {
        scaleFactor /= 1.1;
    }
    resize(scaleFactor * pixmap()->size());
}

void ImageLabel::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        lastMousePosition = event->pos();
        mousePressed = true;
    }
}

void ImageLabel::mouseMoveEvent(QMouseEvent* event) {
    if (mousePressed) {
        int dx = event->pos().x() - lastMousePosition.x();
        int dy = event->pos().y() - lastMousePosition.y();
        parentWidget()->parentWidget()->scroll(dx, dy);
        lastMousePosition = event->pos();
    }
}

void ImageLabel::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        mousePressed = false;
    }
}
