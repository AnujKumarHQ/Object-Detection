#include "image_processor.h"
#include <map>

ImageProcessor::ImageProcessor()
    : m_colorIndex(0)
{
}

ImageProcessor::~ImageProcessor()
{
}

HBITMAP ImageProcessor::drawBoundingBoxes(const std::string& imagePath, 
                                        const DetectionResult& result,
                                        bool showLabels,
                                        bool showConfidence)
{
    // For simplicity, this is a placeholder implementation
    // In a full implementation, you would load the image and draw bounding boxes
    return NULL;
}

HBITMAP ImageProcessor::drawBoundingBoxes(HBITMAP originalBitmap,
                                        const DetectionResult& result,
                                        bool showLabels,
                                        bool showConfidence)
{
    // For simplicity, this is a placeholder implementation
    // In a full implementation, you would draw bounding boxes on the bitmap
    return NULL;
}

COLORREF ImageProcessor::getClassColor(const std::string& className)
{
    static std::map<std::string, COLORREF> classColors;
    
    if (classColors.find(className) != classColors.end()) {
        return classColors[className];
    }
    
    // Generate a unique color for each class
    COLORREF predefinedColors[] = {
        RGB(255, 0, 0),     // Red
        RGB(0, 255, 0),     // Green
        RGB(0, 0, 255),     // Blue
        RGB(255, 255, 0),   // Yellow
        RGB(255, 0, 255),   // Magenta
        RGB(0, 255, 255),   // Cyan
        RGB(255, 128, 0),   // Orange
        RGB(128, 0, 255),   // Purple
    };
    
    int numColors = sizeof(predefinedColors) / sizeof(predefinedColors[0]);
    COLORREF color = predefinedColors[m_colorIndex % numColors];
    
    classColors[className] = color;
    m_colorIndex++;
    
    return color;
}

void ImageProcessor::drawBoundingBox(HDC hdc, const Detection& detection,
                                   bool showLabels, bool showConfidence)
{
    // Implementation for drawing bounding boxes
    COLORREF boxColor = getClassColor(detection.className);
    
    HPEN hPen = CreatePen(PS_SOLID, 3, boxColor);
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    
    // Draw rectangle
    Rectangle(hdc, detection.bbox.x, detection.bbox.y, 
              detection.bbox.x + detection.bbox.width, 
              detection.bbox.y + detection.bbox.height);
    
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}