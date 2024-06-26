#include "GISLayer.h"

GISLayer::GISLayer(const QString& layerName, QGraphicsScene* parentScene)
    : name(layerName), scene(parentScene), dataset(nullptr), isVisible(true), pen(Qt::blue), brush(Qt::yellow) {}

bool GISLayer::loadShapefile(const QString& path) {
    dataset = (GDALDataset*)GDALOpenEx(path.toStdString().c_str(), GDAL_OF_VECTOR, nullptr, nullptr, nullptr);
    if (!dataset) {
        QMessageBox::warning(nullptr, "错误", "无法打开文件: " + path);
        return false;
    }

    OGRLayer* layer = dataset->GetLayer(0);
    if (!layer) {
        QMessageBox::warning(nullptr, "错误", "无法加载层");
        return false;
    }

    OGRSpatialReference* srcSRS = layer->GetSpatialRef();
    OGRSpatialReference dstSRS;
    dstSRS.SetWellKnownGeogCS("WGS84");

    OGRCoordinateTransformation* coordTransform = OGRCreateCoordinateTransformation(srcSRS, &dstSRS);
    if (!coordTransform) {
        QMessageBox::warning(nullptr, "错误", "坐标转换失败");
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
    case wkbPolygon: {//面的处理逻辑待修正，会有bug
        OGRPolygon* poPolygon = (OGRPolygon*)geometry;
        OGRLinearRing* poRing = poPolygon->getExteriorRing();
        QPolygonF polygon;
        for (int j = 0; j < poRing->getNumPoints(); ++j) {
            QPointF point(poRing->getX(j), poRing->getY(j));
            polygon << point;
        }
        QGraphicsPolygonItem* polygonItem = new QGraphicsPolygonItem(polygon);
        scene->addItem(polygonItem);
        setupGraphicsItemsStyle(polygonItem);
        geometryItemMap.insert(geometry, polygonItem);

        // 处理内部环
        for (int ringIdx = 0; ringIdx < poPolygon->getNumInteriorRings(); ++ringIdx) {
            poRing = poPolygon->getInteriorRing(ringIdx);
            polygon.clear();
            for (int j = 0; j < poRing->getNumPoints(); ++j) {
                QPointF point(poRing->getX(j), poRing->getY(j));
                polygon << point;
            }
            QGraphicsPolygonItem* interiorPolygonItem = new QGraphicsPolygonItem(polygon);
            scene->addItem(interiorPolygonItem);
            setupGraphicsItemsStyle(interiorPolygonItem);
            geometryItemMap.insert(geometry, interiorPolygonItem);
        }
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
