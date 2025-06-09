#ifndef WEBCAM_CAPTURE_H
#define WEBCAM_CAPTURE_H

#include <windows.h>
#include <string>
#include <functional>
#include <thread>
#include <atomic>

class WebcamCapture {
public:
    WebcamCapture();
    ~WebcamCapture();

    using FrameCallback = std::function<void(const std::string& framePath)>;
    using ErrorCallback = std::function<void(const std::string& error)>;

    bool initialize(int deviceId = 0);
    void startCapture(FrameCallback onFrame, ErrorCallback onError);
    void stopCapture();
    bool isCapturing() const { return m_isCapturing; }
    
    void setFrameRate(int fps) { m_targetFps = fps; }
    int getFrameRate() const { return m_targetFps; }

private:
    void captureLoop();
    std::string saveFrame();
    std::string getPythonScriptPath();

    std::atomic<bool> m_isCapturing;
    std::thread m_captureThread;
    FrameCallback m_frameCallback;
    ErrorCallback m_errorCallback;
    
    int m_deviceId;
    int m_targetFps;
    std::string m_tempDir;
    int m_frameCounter;
};

#endif // WEBCAM_CAPTURE_H