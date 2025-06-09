#include "mainwindow.h"
#include <commdlg.h>
#include <commctrl.h>
#include <sstream>
#include <iomanip>
#include "resource.h"

#define ID_OPEN_BUTTON 1001
#define ID_WEBCAM_BUTTON 1002
#define ID_MODEL_COMBO 1003
#define ID_CONFIDENCE_EDIT 1004
#define ID_IOU_EDIT 1005
#define ID_RESULTS_EDIT 1006
#define ID_STATUS_STATIC 1007
#define ID_PROGRESS_BAR 1008
#define ID_IMAGE_STATIC 1009
#define ID_FPS_EDIT 1010

MainWindow::MainWindow(HINSTANCE hInstance)
    : m_hInstance(hInstance)
    , m_hwnd(NULL)
    , m_hImageStatic(NULL)
    , m_hOpenButton(NULL)
    , m_hWebcamButton(NULL)
    , m_hModelCombo(NULL)
    , m_hConfidenceEdit(NULL)
    , m_hIouEdit(NULL)
    , m_hResultsEdit(NULL)
    , m_hStatusStatic(NULL)
    , m_hProgressBar(NULL)
    , m_hFpsEdit(NULL)
    , m_detectionClient(nullptr)
    , m_imageProcessor(nullptr)
    , m_webcamCapture(nullptr)
    , m_hCurrentBitmap(NULL)
    , m_isProcessing(false)
    , m_isWebcamActive(false)
    , m_confidenceThreshold(0.5)
    , m_iouThreshold(0.45)
    , m_selectedModel("yolov5s")
    , m_webcamFps(5)
{
    m_detectionClient = new DetectionClient();
    m_imageProcessor = new ImageProcessor();
    m_webcamCapture = new WebcamCapture();
}

MainWindow::~MainWindow()
{
    if (m_hCurrentBitmap) {
        DeleteObject(m_hCurrentBitmap);
    }
    delete m_detectionClient;
    delete m_imageProcessor;
    delete m_webcamCapture;
}

bool MainWindow::Create()
{
    const wchar_t CLASS_NAME[] = L"YOLODetectionWindow";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = m_hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);

    RegisterClass(&wc);

    m_hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"YOLO Real-time Object Detection - Windows Native",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1200, 800,
        NULL, NULL, m_hInstance, this
    );

    return m_hwnd != NULL;
}

void MainWindow::Show(int nCmdShow)
{
    ShowWindow(m_hwnd, nCmdShow);
    UpdateWindow(m_hwnd);
}

LRESULT CALLBACK MainWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    MainWindow* pThis = NULL;

    if (uMsg == WM_NCCREATE) {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        pThis = (MainWindow*)pCreate->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
        pThis->m_hwnd = hwnd;
    } else {
        pThis = (MainWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }

    if (pThis) {
        return pThis->HandleMessage(uMsg, wParam, lParam);
    } else {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
    case WM_CREATE:
        CreateControls();
        return 0;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_OPEN_BUTTON:
            OnOpenImage();
            break;
        case ID_WEBCAM_BUTTON:
            if (m_isWebcamActive) {
                OnStopWebcam();
            } else {
                OnStartWebcam();
            }
            break;
        }
        return 0;

    case WM_SIZE:
        ResizeControls();
        return 0;

    case WM_DESTROY:
        if (m_isWebcamActive) {
            OnStopWebcam();
        }
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(m_hwnd, &ps);
            EndPaint(m_hwnd, &ps);
        }
        return 0;
    }

    return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}

void MainWindow::CreateControls()
{
    // Open Image Button
    m_hOpenButton = CreateWindow(
        L"BUTTON", L"Open Image",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, 10, 120, 30,
        m_hwnd, (HMENU)ID_OPEN_BUTTON, m_hInstance, NULL
    );

    // Webcam Button
    m_hWebcamButton = CreateWindow(
        L"BUTTON", L"Start Webcam",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        140, 10, 120, 30,
        m_hwnd, (HMENU)ID_WEBCAM_BUTTON, m_hInstance, NULL
    );

    // Model Selection
    CreateWindow(L"STATIC", L"Model:",
        WS_VISIBLE | WS_CHILD,
        10, 50, 60, 20,
        m_hwnd, NULL, m_hInstance, NULL
    );

    m_hModelCombo = CreateWindow(
        WC_COMBOBOX, L"",
        CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_VISIBLE,
        80, 50, 100, 200,
        m_hwnd, (HMENU)ID_MODEL_COMBO, m_hInstance, NULL
    );

    // Add model options
    SendMessage(m_hModelCombo, CB_ADDSTRING, 0, (LPARAM)L"yolov5s");
    SendMessage(m_hModelCombo, CB_ADDSTRING, 0, (LPARAM)L"yolov5m");
    SendMessage(m_hModelCombo, CB_ADDSTRING, 0, (LPARAM)L"yolov5l");
    SendMessage(m_hModelCombo, CB_ADDSTRING, 0, (LPARAM)L"yolov5x");
    SendMessage(m_hModelCombo, CB_SETCURSEL, 0, 0);

    // Confidence Threshold
    CreateWindow(L"STATIC", L"Confidence:",
        WS_VISIBLE | WS_CHILD,
        10, 80, 80, 20,
        m_hwnd, NULL, m_hInstance, NULL
    );

    m_hConfidenceEdit = CreateWindow(
        L"EDIT", L"0.5",
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
        100, 80, 60, 20,
        m_hwnd, (HMENU)ID_CONFIDENCE_EDIT, m_hInstance, NULL
    );

    // IoU Threshold
    CreateWindow(L"STATIC", L"IoU:",
        WS_VISIBLE | WS_CHILD,
        10, 110, 40, 20,
        m_hwnd, NULL, m_hInstance, NULL
    );

    m_hIouEdit = CreateWindow(
        L"EDIT", L"0.45",
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
        60, 110, 60, 20,
        m_hwnd, (HMENU)ID_IOU_EDIT, m_hInstance, NULL
    );

    // FPS Setting
    CreateWindow(L"STATIC", L"FPS:",
        WS_VISIBLE | WS_CHILD,
        10, 140, 40, 20,
        m_hwnd, NULL, m_hInstance, NULL
    );

    m_hFpsEdit = CreateWindow(
        L"EDIT", L"5",
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
        60, 140, 60, 20,
        m_hwnd, (HMENU)ID_FPS_EDIT, m_hInstance, NULL
    );

    // Image Display Area
    m_hImageStatic = CreateWindow(
        L"STATIC", L"No image loaded\nClick 'Open Image' for static detection\nor 'Start Webcam' for real-time detection",
        WS_CHILD | WS_VISIBLE | SS_CENTER | SS_SUNKEN,
        280, 10, 600, 400,
        m_hwnd, (HMENU)ID_IMAGE_STATIC, m_hInstance, NULL
    );

    // Progress Bar
    m_hProgressBar = CreateWindow(
        PROGRESS_CLASS, NULL,
        WS_CHILD | PBS_MARQUEE,
        280, 420, 600, 20,
        m_hwnd, (HMENU)ID_PROGRESS_BAR, m_hInstance, NULL
    );

    // Status Label
    m_hStatusStatic = CreateWindow(
        L"STATIC", L"Ready - Choose image or webcam detection",
        WS_CHILD | WS_VISIBLE,
        280, 450, 600, 20,
        m_hwnd, (HMENU)ID_STATUS_STATIC, m_hInstance, NULL
    );

    // Results Text Area
    CreateWindow(L"STATIC", L"Detection Results:",
        WS_VISIBLE | WS_CHILD,
        280, 480, 150, 20,
        m_hwnd, NULL, m_hInstance, NULL
    );

    m_hResultsEdit = CreateWindow(
        L"EDIT", L"Detection results will appear here...\n\nFor real-time detection:\n1. Click 'Start Webcam'\n2. Adjust FPS (1-10 recommended)\n3. Watch live detection results",
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_MULTILINE | ES_READONLY,
        280, 500, 600, 200,
        m_hwnd, (HMENU)ID_RESULTS_EDIT, m_hInstance, NULL
    );
}

void MainWindow::OnOpenImage()
{
    if (m_isProcessing) {
        MessageBox(m_hwnd, L"Detection already in progress", L"Info", MB_OK | MB_ICONINFORMATION);
        return;
    }

    if (m_isWebcamActive) {
        OnStopWebcam();
    }

    OPENFILENAME ofn;
    wchar_t szFile[260] = { 0 };

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = m_hwnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"Image Files\0*.png;*.jpg;*.jpeg;*.bmp;*.tiff\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn)) {
        m_currentImagePath = szFile;
        
        // Show progress
        ShowWindow(m_hProgressBar, SW_SHOW);
        SendMessage(m_hProgressBar, PBM_SETMARQUEE, TRUE, 50);
        SetWindowText(m_hStatusStatic, L"Processing image...");
        m_isProcessing = true;

        // Load and display image
        UpdateImageDisplay();

        // Start detection
        DetectionRequest request;
        
        // Convert wide string to string
        int size = WideCharToMultiByte(CP_UTF8, 0, m_currentImagePath.c_str(), -1, NULL, 0, NULL, NULL);
        std::string imagePath(size, 0);
        WideCharToMultiByte(CP_UTF8, 0, m_currentImagePath.c_str(), -1, &imagePath[0], size, NULL, NULL);
        imagePath.pop_back(); // Remove null terminator
        
        request.imagePath = imagePath;
        request.confidenceThreshold = m_confidenceThreshold;
        request.iouThreshold = m_iouThreshold;
        request.modelName = m_selectedModel;
        request.saveAnnotated = false;

        // Start detection
        m_detectionClient->detectObjects(request, 
            [this](const DetectionResult& result) { OnDetectionComplete(result); },
            [this](const std::string& error) { 
                std::wstring werror(error.begin(), error.end());
                OnDetectionError(werror); 
            }
        );
    }
}

void MainWindow::OnStartWebcam()
{
    if (m_isWebcamActive) {
        return;
    }

    // Get FPS setting
    wchar_t fpsText[10];
    GetWindowText(m_hFpsEdit, fpsText, 10);
    m_webcamFps = _wtoi(fpsText);
    if (m_webcamFps < 1) m_webcamFps = 1;
    if (m_webcamFps > 10) m_webcamFps = 10;

    // Initialize webcam
    if (!m_webcamCapture->initialize(0)) {
        MessageBox(m_hwnd, L"Failed to initialize webcam. Please check if camera is connected and not in use by another application.", 
                   L"Webcam Error", MB_OK | MB_ICONERROR);
        return;
    }

    m_webcamCapture->setFrameRate(m_webcamFps);
    
    // Start webcam capture
    m_webcamCapture->startCapture(
        [this](const std::string& framePath) { OnWebcamFrame(framePath); },
        [this](const std::string& error) { OnWebcamError(error); }
    );

    m_isWebcamActive = true;
    SetWindowText(m_hWebcamButton, L"Stop Webcam");
    SetWindowText(m_hStatusStatic, L"Webcam active - Real-time detection running");
    SetWindowText(m_hImageStatic, L"Webcam feed active\nReal-time detection in progress...");
    
    // Disable image button while webcam is active
    EnableWindow(m_hOpenButton, FALSE);
}

void MainWindow::OnStopWebcam()
{
    if (!m_isWebcamActive) {
        return;
    }

    m_webcamCapture->stopCapture();
    m_isWebcamActive = false;
    
    SetWindowText(m_hWebcamButton, L"Start Webcam");
    SetWindowText(m_hStatusStatic, L"Webcam stopped");
    SetWindowText(m_hImageStatic, L"Webcam stopped\nClick 'Start Webcam' to resume real-time detection");
    
    // Re-enable image button
    EnableWindow(m_hOpenButton, TRUE);
    
    ShowWindow(m_hProgressBar, SW_HIDE);
}

void MainWindow::OnWebcamFrame(const std::string& framePath)
{
    if (!m_isWebcamActive) {
        return;
    }

    // Skip if already processing (to maintain frame rate)
    if (m_isProcessing) {
        return;
    }

    m_isProcessing = true;

    // Create detection request
    DetectionRequest request;
    request.imagePath = framePath;
    request.confidenceThreshold = m_confidenceThreshold;
    request.iouThreshold = m_iouThreshold;
    request.modelName = m_selectedModel;
    request.saveAnnotated = false;

    // Start detection
    m_detectionClient->detectObjects(request, 
        [this](const DetectionResult& result) { 
            OnDetectionComplete(result);
            m_isProcessing = false;
        },
        [this](const std::string& error) { 
            m_isProcessing = false;
            // Don't show error dialog for webcam frames, just log
            std::wstring status = L"Frame detection error (continuing...)";
            SetWindowText(m_hStatusStatic, status.c_str());
        }
    );
}

void MainWindow::OnWebcamError(const std::string& error)
{
    std::wstring werror(error.begin(), error.end());
    MessageBox(m_hwnd, werror.c_str(), L"Webcam Error", MB_OK | MB_ICONERROR);
    OnStopWebcam();
}

void MainWindow::OnDetectionComplete(const DetectionResult& result)
{
    if (!m_isWebcamActive) {
        m_isProcessing = false;
        ShowWindow(m_hProgressBar, SW_HIDE);
    }
    
    UpdateResultsText(result);
    
    std::wstringstream status;
    if (m_isWebcamActive) {
        status << L"Live: " << result.detections.size() << L" objects (" << result.processingTime << L"ms) - FPS: " << m_webcamFps;
    } else {
        status << L"Detected " << result.detections.size() << L" objects in " << result.processingTime << L"ms";
    }
    SetWindowText(m_hStatusStatic, status.str().c_str());
}

void MainWindow::OnDetectionError(const std::wstring& error)
{
    m_isProcessing = false;
    ShowWindow(m_hProgressBar, SW_HIDE);
    
    MessageBox(m_hwnd, error.c_str(), L"Detection Error", MB_OK | MB_ICONERROR);
    SetWindowText(m_hStatusStatic, L"Error occurred");
    SetWindowText(m_hResultsEdit, (L"Detection failed: " + error).c_str());
}

void MainWindow::UpdateImageDisplay()
{
    if (m_currentImagePath.empty()) return;

    // For simplicity, just show the filename
    std::wstring filename = m_currentImagePath.substr(m_currentImagePath.find_last_of(L"\\") + 1);
    SetWindowText(m_hImageStatic, (L"Image loaded: " + filename).c_str());
}

void MainWindow::UpdateResultsText(const DetectionResult& result)
{
    std::wstringstream resultsText;
    
    if (m_isWebcamActive) {
        resultsText << L"REAL-TIME DETECTION (Frame processed in " << result.processingTime << L"ms)\r\n";
    } else {
        resultsText << L"Detection completed in " << result.processingTime << L"ms\r\n";
    }
    
    resultsText << L"Objects detected: " << result.detections.size() << L"\r\n\r\n";

    for (size_t i = 0; i < result.detections.size(); ++i) {
        const Detection& det = result.detections[i];
        resultsText << L"Object " << (i + 1) << L":\r\n";
        
        // Convert class name to wide string
        std::wstring className(det.className.begin(), det.className.end());
        resultsText << L"  Class: " << className << L"\r\n";
        resultsText << L"  Confidence: " << std::fixed << std::setprecision(1) << (det.confidence * 100) << L"%\r\n";
        resultsText << L"  Box: (" << det.bbox.x << L", " << det.bbox.y << L", " 
                   << det.bbox.width << L", " << det.bbox.height << L")\r\n\r\n";
    }

    if (m_isWebcamActive) {
        resultsText << L"\r\n--- LIVE FEED ACTIVE ---\r\n";
        resultsText << L"FPS: " << m_webcamFps << L" | Model: " << std::wstring(m_selectedModel.begin(), m_selectedModel.end()) << L"\r\n";
    }

    SetWindowText(m_hResultsEdit, resultsText.str().c_str());
}

void MainWindow::ResizeControls()
{
    RECT rect;
    GetClientRect(m_hwnd, &rect);
    
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    
    // Resize image display area
    SetWindowPos(m_hImageStatic, NULL, 280, 10, width - 300, height - 320, SWP_NOZORDER);
    
    // Resize progress bar
    SetWindowPos(m_hProgressBar, NULL, 280, height - 300, width - 300, 20, SWP_NOZORDER);
    
    // Resize status label
    SetWindowPos(m_hStatusStatic, NULL, 280, height - 270, width - 300, 20, SWP_NOZORDER);
    
    // Resize results text area
    SetWindowPos(m_hResultsEdit, NULL, 280, height - 240, width - 300, 200, SWP_NOZORDER);
}