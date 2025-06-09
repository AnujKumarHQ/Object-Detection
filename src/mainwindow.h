#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <windows.h>
#include <string>
#include <vector>
#include "detection_client.h"
#include "image_processor.h"
#include "webcam_capture.h"

class MainWindow {
public:
    MainWindow(HINSTANCE hInstance);
    ~MainWindow();

    bool Create();
    void Show(int nCmdShow);
    HWND GetHandle() const { return m_hwnd; }

private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

    void CreateControls();
    void OnOpenImage();
    void OnStartWebcam();
    void OnStopWebcam();
    void OnDetectionComplete(const DetectionResult& result);
    void OnDetectionError(const std::wstring& error);
    void OnWebcamFrame(const std::string& framePath);
    void OnWebcamError(const std::string& error);
    void UpdateImageDisplay();
    void UpdateResultsText(const DetectionResult& result);
    void ResizeControls();

    HINSTANCE m_hInstance;
    HWND m_hwnd;
    
    // Control handles
    HWND m_hImageStatic;
    HWND m_hOpenButton;
    HWND m_hWebcamButton;
    HWND m_hModelCombo;
    HWND m_hConfidenceEdit;
    HWND m_hIouEdit;
    HWND m_hResultsEdit;
    HWND m_hStatusStatic;
    HWND m_hProgressBar;
    HWND m_hFpsEdit;
    
    // Backend components
    DetectionClient* m_detectionClient;
    ImageProcessor* m_imageProcessor;
    WebcamCapture* m_webcamCapture;
    
    // State
    std::wstring m_currentImagePath;
    HBITMAP m_hCurrentBitmap;
    bool m_isProcessing;
    bool m_isWebcamActive;
    
    // Settings
    double m_confidenceThreshold;
    double m_iouThreshold;
    std::string m_selectedModel;
    int m_webcamFps;
};

#endif // MAINWINDOW_H