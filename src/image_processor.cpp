#include "image_processor.h"
#include <QDebug>
#include <QFontMetrics>

ImageProcessor::ImageProcessor(QObject *parent)
    : QObject(parent)
    , m_colorIndex(0)
{
    m_labelFont.setFamily("Arial");
    m_labelFont.setPointSize(12);
    m_labelFont.setBold(true);
}

QPixmap ImageProcessor::drawBoundingBoxes(const QString& imagePath, 
                                        const DetectionResult& result,
                                        bool showLabels,
                                        bool showConfidence)
{
    QPixmap originalPixmap(imagePath);
    if (originalPixmap.isNull()) {
        qWarning() << "Failed to load image:" << imagePath;
        return QPixmap();
    }
    
    return drawBoundingBoxes(originalPixmap, result, showLabels, showConfidence);
}

QPixmap ImageProcessor::drawBoundingBoxes(const QPixmap& originalPixmap,
                                        const DetectionResult& result,
                                        bool showLabels,
                                        bool showConfidence)
{
    if (originalPixmap.isNull()) {
        return QPixmap();
    }
    
    QPixmap annotatedPixmap = originalPixmap.copy();
    QPainter painter(&annotatedPixmap);
    
    // Enable antialiasing for smoother drawing
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setFont(m_labelFont);
    
    // Draw each detection
    for (const Detection& detection : result.detections) {
        drawBoundingBox(painter, detection, showLabels, showConfidence);
    }
    
    return annotatedPixmap;
}

QColor ImageProcessor::getClassColor(const QString& className)
{
    if (m_classColors.contains(className)) {
        return m_classColors[className];
    }
    
    // Generate a unique color for each class
    QList<QColor> predefinedColors = {
        QColor(255, 0, 0),     // Red
        QColor(0, 255, 0),     // Green
        QColor(0, 0, 255),     // Blue
        QColor(255, 255, 0),   // Yellow
        QColor(255, 0, 255),   // Magenta
        QColor(0, 255, 255),   // Cyan
        QColor(255, 128, 0),   // Orange
        QColor(128, 0, 255),   // Purple
        QColor(255, 192, 203), // Pink
        QColor(0, 128, 0),     // Dark Green
        QColor(128, 128, 0),   // Olive
        QColor(0, 128, 128),   // Teal
    };
    
    QColor color;
    if (m_colorIndex < predefinedColors.size()) {
        color = predefinedColors[m_colorIndex];
    } else {
        // Generate random color if we run out of predefined ones
        color = QColor::fromHsv((m_colorIndex * 137) % 360, 255, 255);
    }
    
    m_classColors[className] = color;
    m_colorIndex++;
    
    return color;
}

void ImageProcessor::drawBoundingBox(QPainter& painter, 
                                   const Detection& detection,
                                   bool showLabels,
                                   bool showConfidence)
{
    QColor boxColor = getClassColor(detection.className);
    
    // Draw bounding box
    QPen boxPen(boxColor, 3);
    painter.setPen(boxPen);
    painter.drawRect(detection.bbox);
    
    if (showLabels || showConfidence) {
        // Prepare label text
        QString labelText;
        if (showLabels) {
            labelText = detection.className;
        }
        if (showConfidence) {
            if (!labelText.isEmpty()) {
                labelText += " ";
            }
            labelText += QString("%.1f%%").arg(detection.confidence * 100);
        }
        
        if (!labelText.isEmpty()) {
            // Calculate label background size
            QFontMetrics fontMetrics(m_labelFont);
            QRect textRect = fontMetrics.boundingRect(labelText);
            int padding = 4;
            QRect labelRect(
                detection.bbox.x(),
                detection.bbox.y() - textRect.height() - padding * 2,
                textRect.width() + padding * 2,
                textRect.height() + padding * 2
            );
            
            // Ensure label stays within image bounds
            if (labelRect.y() < 0) {
                labelRect.moveTop(detection.bbox.y());
            }
            
            // Draw label background
            painter.fillRect(labelRect, boxColor);
            
            // Draw label text
            painter.setPen(QPen(Qt::white));
            painter.drawText(
                labelRect.x() + padding,
                labelRect.y() + padding + fontMetrics.ascent(),
                labelText
            );
        }
    }
}