#ifndef DETECTION_CLIENT_H
#define DETECTION_CLIENT_H

#include <string>
#include <vector>
#include <functional>

struct Detection {
    std::string className;
    double confidence;
    struct {
        int x, y, width, height;
    } bbox;
};

struct DetectionResult {
    std::vector<Detection> detections;
    int processingTime;
    bool success;
    std::string errorMessage;
};

struct DetectionRequest {
    std::string imagePath;
    double confidenceThreshold;
    double iouThreshold;
    std::string modelName;
    bool saveAnnotated;
};

class DetectionClient {
public:
    DetectionClient();
    ~DetectionClient();

    using CompletionCallback = std::function<void(const DetectionResult&)>;
    using ErrorCallback = std::function<void(const std::string&)>;

    void detectObjects(const DetectionRequest& request, 
                      CompletionCallback onComplete,
                      ErrorCallback onError);

    bool isProcessing() const { return m_isProcessing; }

private:
    std::string createRequestJson(const DetectionRequest& request);
    DetectionResult parseResponse(const std::string& response);
    std::string findPythonExecutable();
    std::string getPythonScriptPath();

    bool m_isProcessing;
    std::string m_pythonExecutable;
    std::string m_pythonScriptPath;
};

#endif // DETECTION_CLIENT_H