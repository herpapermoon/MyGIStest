#ifndef GISLAYER_H
#define GISLAYER_H

#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QPen>
#include <QBrush>
#include <gdal_priv.h>
#include <ogrsf_frmts.h>
#include <vector>
#include <map>
#include <QMessageBox>
#include <QDebug>

class GISLayer {
public:
    QString name;
    QGraphicsScene* scene;
    GDALDataset* dataset;
    std::vector<OGRFeature*> features;
    QMap<OGRGeometry*, QGraphicsItem*> geometryItemMap;
    bool isVisible;
    QPen pen;
    QBrush brush;

    GISLayer(const QString& layerName, QGraphicsScene* parentScene);

    bool loadShapefile(const QString& path);
    void addGeometryToScene(OGRGeometry* geometry);
    void setupGraphicsItemsStyle(QGraphicsItem* item);
    void setVisible(bool visible);
    void removeLayer();
    void setStyle(const QPen& newPen, const QBrush& newBrush);
};

#endif // GISLAYER_H
