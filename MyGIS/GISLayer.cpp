#include "GISLayer.h"

GISLayer::GISLayer(const QString& layerName, QGraphicsScene* parentScene)
    : name(layerName), scene(parentScene), dataset(nullptr), isVisible(true), pen(Qt::blue), brush(Qt::yellow) {}

bool GISLayer::loadShapefile(const QString& path) {
    dataset = (GDALDataset*)GDALOpenEx(path.toStdString().c_str(), GDAL_OF_VECTOR, nullptr, nullptr, nullptr);
    if (!dataset) {
        QMessageBox::warning(nullptr, "error", "cannot open file: " + path);
        return false;
    }

    OGRLayer* layer = dataset->GetLayer(0);
    if (!layer) {
        QMessageBox::warning(nullptr, "error", "cannot load layer");
        return false;
    }

    OGRSpatialReference* srcSRS = layer->GetSpatialRef();
    OGRSpatialReference dstSRS;
    dstSRS.SetWellKnownGeogCS("WGS84");

    OGRCoordinateTransformation* coordTransform = OGRCreateCoordinateTransformation(srcSRS, &dstSRS);
    if (!coordTransform) {
        QMessageBox::warning(nullptr, "error", "transform failed");
        return false;
    }

    OGRFeature* feature;
    layer->ResetReading();
    while ((feature = layer->GetNextFeature()) != nullptr) {
        OGRGeometry* geometry = feature->GetGeometryRef();
        if (geometry != nullptr && geometry->transform(coordTransform) == OGRERR_NONE) {
            addGeometryToScene(geometry);
        }
        features.push_back(feature);
    }

    OCTDestroyCoordinateTransformation(coordTransform);
    return true;
}

void GISLayer::addGeometryToScene(OGRGeometry* geometry) {
    OGRwkbGeometryType geometryType = wkbFlatten(geometry->getGeometryType());
    switch (geometryType) {
    case wkbPoint: {
        OGRPoint* poPoint = (OGRPoint*)geometry;
        QPointF point(poPoint->getX(), poPoint->getY());
        double fixedRadius = 1;
        QGraphicsEllipseItem* pointItem = new QGraphicsEllipseItem(point.x() - fixedRadius, point.y() - fixedRadius, 2 * fixedRadius, 2 * fixedRadius);
        scene->addItem(pointItem);
        setupGraphicsItemsStyle(pointItem);
        geometryItemMap.insert(geometry, pointItem);
        break;
    }
    case wkbLineString: {
        OGRLineString* poLine = (OGRLineString*)geometry;
        QPainterPath path;
        for (int j = 0; j < poLine->getNumPoints(); ++j) {
            QPointF point(poLine->getX(j), poLine->getY(j));
            if (j == 0) {
                path.moveTo(point);
            }
            else {
                path.lineTo(point);
            }
        }
        QGraphicsPathItem* lineItem = new QGraphicsPathItem(path);
        scene->addItem(lineItem);
        setupGraphicsItemsStyle(lineItem);
        geometryItemMap.insert(geometry, lineItem);
        break;
    }
    case wkbPolygon: {
        OGRPolygon* poPolygon = (OGRPolygon*)geometry;
        QPainterPath path;

        // 设置填充规则为非零填充规则
        path.setFillRule(Qt::WindingFill);

        // 处理外环
        OGRLinearRing* poRing = poPolygon->getExteriorRing();
        if (poRing != nullptr) {
            for (int j = 0; j < poRing->getNumPoints(); ++j) {
                QPointF point(poRing->getX(j), poRing->getY(j));
                if (j == 0) {
                    path.moveTo(point);
                }
                else {
                    path.lineTo(point);
                }
            }
            path.closeSubpath();
        }

        // 处理内部环
        for (int ringIdx = 0; ringIdx < poPolygon->getNumInteriorRings(); ++ringIdx) {
            poRing = poPolygon->getInteriorRing(ringIdx);
            QPainterPath innerPath;
            if (poRing != nullptr) {
                for (int j = 0; j < poRing->getNumPoints(); ++j) {
                    QPointF point(poRing->getX(j), poRing->getY(j));
                    if (j == 0) {
                        innerPath.moveTo(point);
                    }
                    else {
                        innerPath.lineTo(point);
                    }
                }
                innerPath.closeSubpath();
                path.addPath(innerPath);
            }
        }

        QGraphicsPathItem* polygonItem = new QGraphicsPathItem(path);
        scene->addItem(polygonItem);
        setupGraphicsItemsStyle(polygonItem);
        geometryItemMap.insert(geometry, polygonItem);
        break;
    }
    default:
        qDebug() << "Unsupported geometry type: " << geometry->getGeometryType();
        break;
    }
}



void GISLayer::setupGraphicsItemsStyle(QGraphicsItem* item) {
    QPen modifiedPen = pen;
    modifiedPen.setWidthF(qMax(0.1, pen.widthF())); // 使用浮点数设置线宽，最小值为0.1
    if (QGraphicsEllipseItem* ellipseItem = dynamic_cast<QGraphicsEllipseItem*>(item)) {
        ellipseItem->setPen(modifiedPen);
        ellipseItem->setBrush(brush);
    }
    else if (QGraphicsPathItem* pathItem = dynamic_cast<QGraphicsPathItem*>(item)) {
        pathItem->setPen(modifiedPen);
        pathItem->setBrush(brush); // 确保路径项也可以填充
    }
    else if (QGraphicsPolygonItem* polygonItem = dynamic_cast<QGraphicsPolygonItem*>(item)) {
        polygonItem->setPen(modifiedPen);
        polygonItem->setBrush(brush);
    }
}


void GISLayer::setVisible(bool visible) {
    isVisible = visible;
    for (auto& item : geometryItemMap) {
        item->setVisible(visible);
    }
}

void GISLayer::removeLayer() {
    for (auto& item : geometryItemMap) {
        scene->removeItem(item);
        delete item;
    }
    geometryItemMap.clear();
}

void GISLayer::setStyle(const QPen& newPen, const QBrush& newBrush) {
    pen = newPen;
    brush = newBrush;
    for (auto& item : geometryItemMap) {
        setupGraphicsItemsStyle(item);
    }
}
