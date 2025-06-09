#ifndef IMAGE_PROCESSOR_H
#define IMAGE_PROCESSOR_H

#include <QObject>
#include <QPixmap>
#include <QPainter>
#include <QColor>
#include <QFont>
#include "detection_client.h"

class ImageProcessor : public QObject
{
    Q_OBJECT

public:
    explicit ImageProcessor(QObject *parent = nullptr);

    QPixmap drawBoundingBoxes(const QString& imagePath, 
                             const DetectionResult& result,
                             bool showLabels = true,
                             bool showConfidence = true);

    QPixmap drawBoundingBoxes(const QPixmap& originalPixmap,
                             const DetectionResult& result,
                             bool showLabels = true,
                             bool showConfidence = true);

private:
    QColor getClassColor(const QString& className);
    void drawBoundingBox(QPainter& painter, 
                        const Detection& detection,
                        bool showLabels,
                        bool showConfidence);

    QMap<QString, QColor> m_classColors;
    QFont m_labelFont;
    int m_colorIndex;
};

#endif // IMAGE_PROCESSOR_H