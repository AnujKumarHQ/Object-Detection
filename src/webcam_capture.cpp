#include "webcam_capture.h"
#include <iostream>
#include <sstream>
#include <filesystem>
#include <chrono>
#include <thread>

WebcamCapture::WebcamCapture()
    : m_isCapturing(false)
    , m_deviceId(0)
    , m_targetFps(5)  // 5 FPS for real-time detection
    , m_frameCounter(0)
{
    // Create temp directory for frames
    char tempPath[MAX_PATH];
    GetTempPathA(MAX_PATH, tempPath);
    m_tempDir = std::string(tempPath) + "yolo_frames\\";
    CreateDirectoryA(m_tempDir.c_str(), NULL);
}

WebcamCapture::~WebcamCapture()
{
    stopCapture();
}

bool WebcamCapture::initialize(int deviceId)
{
    m_deviceId = deviceId;
    
    // Test if camera is available using Python OpenCV
    std::string testScript = getPythonScriptPath() + "\\..\\test_camera.py";
    std::string command = "python \"" + testScript + "\" " + std::to_string(deviceId);
    
    int result = system(command.c_str());
    return result == 0;
}

void WebcamCapture::startCapture(FrameCallback onFrame, ErrorCallback onError)
{
    if (m_isCapturing) {
        return;
    }

    m_frameCallback = onFrame;
    m_errorCallback = onError;
    m_isCapturing = true;
    m_frameCounter = 0;

    m_captureThread = std::thread(&WebcamCapture::captureLoop, this);
}

void WebcamCapture::stopCapture()
{
    m_isCapturing = false;
    if (m_captureThread.joinable()) {
        m_captureThread.join();
    }
}

void WebcamCapture::captureLoop()
{
    auto frameInterval = std::chrono::milliseconds(1000 / m_targetFps);
    auto lastFrameTime = std::chrono::steady_clock::now();

    while (m_isCapturing) {
        auto currentTime = std::chrono::steady_clock::now();
        
        if (currentTime - lastFrameTime >= frameInterval) {
            try {
                std::string framePath = saveFrame();
                if (!framePath.empty() && m_frameCallback) {
                    m_frameCallback(framePath);
                }
                lastFrameTime = currentTime;
            } catch (const std::exception& e) {
                if (m_errorCallback) {
                    m_errorCallback("Frame capture error: " + std::string(e.what()));
                }
                break;
            }
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

std::string WebcamCapture::saveFrame()
{
    // Use Python script to capture frame
    std::string captureScript = getPythonScriptPath() + "\\..\\capture_frame.py";
    std::string framePath = m_tempDir + "frame_" + std::to_string(m_frameCounter++) + ".jpg";
    
    std::string command = "python \"" + captureScript + "\" " + 
                         std::to_string(m_deviceId) + " \"" + framePath + "\"";
    
    int result = system(command.c_str());
    if (result == 0 && std::filesystem::exists(framePath)) {
        return framePath;
    }
    
    return "";
}

std::string WebcamCapture::getPythonScriptPath()
{
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string exePath(buffer);
    
    size_t lastSlash = exePath.find_last_of("\\/");
    std::string exeDir = exePath.substr(0, lastSlash);
    
    return exeDir + "\\..\\python";
}