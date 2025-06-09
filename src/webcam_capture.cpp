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
    std::string pythonDir = getPythonScriptPath();
    std::string testScript = pythonDir + "\\test_camera.py";
    
    // Check if test script exists
    if (!std::filesystem::exists(testScript)) {
        std::cerr << "Test script not found: " << testScript << std::endl;
        return false;
    }
    
    // Build command with proper path handling
    std::string command = "python \"" + testScript + "\" " + std::to_string(deviceId) + " 2>nul";
    
    std::cout << "Testing camera with command: " << command << std::endl;
    
    int result = system(command.c_str());
    
    if (result != 0) {
        std::cerr << "Camera test failed with exit code: " << result << std::endl;
        
        // Try alternative Python commands
        std::vector<std::string> pythonCmds = {"python3", "py", "python.exe"};
        for (const auto& pythonCmd : pythonCmds) {
            std::string altCommand = pythonCmd + " \"" + testScript + "\" " + std::to_string(deviceId) + " 2>nul";
            std::cout << "Trying alternative: " << altCommand << std::endl;
            
            result = system(altCommand.c_str());
            if (result == 0) {
                std::cout << "Camera test successful with: " << pythonCmd << std::endl;
                return true;
            }
        }
        
        return false;
    }
    
    std::cout << "Camera test successful" << std::endl;
    return true;
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
    std::string pythonDir = getPythonScriptPath();
    std::string captureScript = pythonDir + "\\capture_frame.py";
    std::string framePath = m_tempDir + "frame_" + std::to_string(m_frameCounter++) + ".jpg";
    
    // Check if capture script exists
    if (!std::filesystem::exists(captureScript)) {
        std::cerr << "Capture script not found: " << captureScript << std::endl;
        return "";
    }
    
    // Build command with proper error handling
    std::string command = "python \"" + captureScript + "\" " + 
                         std::to_string(m_deviceId) + " \"" + framePath + "\" 2>nul";
    
    int result = system(command.c_str());
    if (result == 0 && std::filesystem::exists(framePath)) {
        return framePath;
    }
    
    // Try alternative Python commands if default fails
    std::vector<std::string> pythonCmds = {"python3", "py", "python.exe"};
    for (const auto& pythonCmd : pythonCmds) {
        std::string altCommand = pythonCmd + " \"" + captureScript + "\" " + 
                               std::to_string(m_deviceId) + " \"" + framePath + "\" 2>nul";
        
        result = system(altCommand.c_str());
        if (result == 0 && std::filesystem::exists(framePath)) {
            return framePath;
        }
    }
    
    std::cerr << "Frame capture failed with all Python commands" << std::endl;
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