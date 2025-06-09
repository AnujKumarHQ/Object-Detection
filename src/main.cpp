#include <windows.h>
#include <commctrl.h>
#include "mainwindow.h"
#include "resource.h"

#pragma comment(lib, "comctl32.lib")

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // Initialize common controls
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_WIN95_CLASSES | ICC_PROGRESS_CLASS | ICC_UPDOWN_CLASS;
    InitCommonControlsEx(&icex);

    // Create and show main window
    MainWindow mainWindow(hInstance);
    if (!mainWindow.Create()) {
        MessageBox(NULL, L"Failed to create main window", L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    mainWindow.Show(nCmdShow);

    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}