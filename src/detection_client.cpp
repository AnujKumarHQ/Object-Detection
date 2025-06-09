#include "detection_client.h"
#include <windows.h>
#include <iostream>
#include <sstream>
#include <thread>
#include <filesystem>

DetectionClient::DetectionClient()
    : m_isProcessing(false)
{
    m_pythonExecutable = findPythonExecutable();
    m_pythonScriptPath = getPythonScriptPath();
}

DetectionClient::~DetectionClient()
{
}

void DetectionClient::detectObjects(const DetectionRequest& request, 
                                   CompletionCallback onComplete,
                                   ErrorCallback onError)
{
    if (m_isProcessing) {
        onError("Detection already in progress");
        return;
    }

    m_isProcessing = true;

    // Run detection in a separate thread
    std::thread([this, request, onComplete, onError]() {
        try {
            std::string jsonRequest = createRequestJson(request);
            
            // Escape quotes in JSON for command line
            std::string escapedJson = jsonRequest;
            size_t pos = 0;
            while ((pos = escapedJson.find("\"", pos)) != std::string::npos) {
                escapedJson.replace(pos, 1, "\\\"");
                pos += 2;
            }

            // Build command
            std::string command = m_pythonExecutable + " \"" + m_pythonScriptPath + "\" \"" + escapedJson + "\"";

            // Execute Python script
            SECURITY_ATTRIBUTES sa;
            sa.nLength = sizeof(SECURITY_ATTRIBUTES);
            sa.lpSecurityDescriptor = NULL;
            sa.bInheritHandle = TRUE;

            HANDLE hReadPipe, hWritePipe;
            if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0)) {
                m_isProcessing = false;
                onError("Failed to create pipe");
                return;
            }

            STARTUPINFOA si;
            PROCESS_INFORMATION pi;
            ZeroMemory(&si, sizeof(si));
            si.cb = sizeof(si);
            si.hStdOutput = hWritePipe;
            si.hStdError = hWritePipe;
            si.dwFlags |= STARTF_USESTDHANDLES;

            ZeroMemory(&pi, sizeof(pi));

            if (!CreateProcessA(NULL, const_cast<char*>(command.c_str()), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
                CloseHandle(hReadPipe);
                CloseHandle(hWritePipe);
                m_isProcessing = false;
                onError("Failed to start Python process");
                return;
            }

            CloseHandle(hWritePipe);

            // Read output
            std::string output;
            char buffer[4096];
            DWORD bytesRead;
            while (ReadFile(hReadPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
                buffer[bytesRead] = '\0';
                output += buffer;
            }

            CloseHandle(hReadPipe);

            // Wait for process to complete
            WaitForSingleObject(pi.hProcess, INFINITE);

            DWORD exitCode;
            GetExitCodeProcess(pi.hProcess, &exitCode);

            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);

            m_isProcessing = false;

            if (exitCode != 0) {
                onError("Python process failed with exit code " + std::to_string(exitCode) + ": " + output);
                return;
            }

            // Parse response
            DetectionResult result = parseResponse(output);
            if (result.success) {
                onComplete(result);
            } else {
                onError(result.errorMessage);
            }

        } catch (const std::exception& e) {
            m_isProcessing = false;
            onError("Exception: " + std::string(e.what()));
        }
    }).detach();
}

std::string DetectionClient::createRequestJson(const DetectionRequest& request)
{
    std::ostringstream json;
    json << "{";
    json << "\"image_path\":\"" << request.imagePath << "\",";
    json << "\"confidence_threshold\":" << request.confidenceThreshold << ",";
    json << "\"iou_threshold\":" << request.iouThreshold << ",";
    json << "\"model_name\":\"" << request.modelName << "\",";
    json << "\"save_annotated\":" << (request.saveAnnotated ? "true" : "false");
    json << "}";
    return json.str();
}

DetectionResult DetectionClient::parseResponse(const std::string& response)
{
    DetectionResult result;
    result.success = false;
    result.processingTime = 0;

    // Simple JSON parsing (for a full implementation, use a JSON library)
    if (response.find("\"success\":true") != std::string::npos) {
        result.success = true;
        
        // Extract processing time
        size_t timePos = response.find("\"processing_time\":");
        if (timePos != std::string::npos) {
            timePos += 18; // Length of "processing_time":
            size_t endPos = response.find(",", timePos);
            if (endPos == std::string::npos) endPos = response.find("}", timePos);
            if (endPos != std::string::npos) {
                std::string timeStr = response.substr(timePos, endPos - timePos);
                result.processingTime = std::stoi(timeStr);
            }
        }

        // Extract detections (simplified parsing)
        size_t detectionsPos = response.find("\"detections\":[");
        if (detectionsPos != std::string::npos) {
            // For simplicity, just count the number of detections
            size_t count = 0;
            size_t pos = detectionsPos;
            while ((pos = response.find("{\"class\":", pos + 1)) != std::string::npos) {
                count++;
                
                // Extract class name
                size_t classStart = response.find("\"class\":\"", pos) + 9;
                size_t classEnd = response.find("\"", classStart);
                std::string className = response.substr(classStart, classEnd - classStart);
                
                // Extract confidence
                size_t confStart = response.find("\"confidence\":", pos) + 13;
                size_t confEnd = response.find(",", confStart);
                double confidence = std::stod(response.substr(confStart, confEnd - confStart));
                
                Detection detection;
                detection.className = className;
                detection.confidence = confidence;
                detection.bbox = {0, 0, 0, 0}; // Simplified for this example
                
                result.detections.push_back(detection);
            }
        }
    } else {
        // Extract error message
        size_t errorPos = response.find("\"error\":\"");
        if (errorPos != std::string::npos) {
            errorPos += 9;
            size_t endPos = response.find("\"", errorPos);
            if (endPos != std::string::npos) {
                result.errorMessage = response.substr(errorPos, endPos - errorPos);
            }
        } else {
            result.errorMessage = "Unknown error occurred";
        }
    }

    return result;
}

std::string DetectionClient::findPythonExecutable()
{
    std::vector<std::string> candidates = {"python", "python3", "py"};
    
    for (const std::string& candidate : candidates) {
        std::string command = candidate + " --version >nul 2>&1";
        if (system(command.c_str()) == 0) {
            return candidate;
        }
    }
    
    return "python"; // Fallback
}

std::string DetectionClient::getPythonScriptPath()
{
    // Get executable directory
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string exePath(buffer);
    
    // Get directory
    size_t lastSlash = exePath.find_last_of("\\/");
    std::string exeDir = exePath.substr(0, lastSlash);
    
    // Construct script path
    return exeDir + "\\..\\python\\detection_server.py";
}