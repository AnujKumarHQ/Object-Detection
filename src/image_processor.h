#ifndef IMAGE_PROCESSOR_H
#define IMAGE_PROCESSOR_H

#include <windows.h>
#include <string>
#include "detection_client.h"

class ImageProcessor {
public:
    ImageProcessor();
    ~ImageProcessor();

    HBITMAP drawBoundingBoxes(const std::string& imagePath, 
                             const DetectionResult& result,
                             bool showLabels = true,
                             bool showConfidence = true);

    HBITMAP drawBoundingBoxes(HBITMAP originalBitmap,
                             const DetectionResult& result,
                             bool showLabels = true,
                             bool showConfidence = true);

private:
    COLORREF getClassColor(const std::string& className);
    void drawBoundingBox(HDC hdc, const Detection& detection,
                        bool showLabels, bool showConfidence);

    int m_colorIndex;
};

#endif // IMAGE_PROCESSOR_H