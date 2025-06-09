@echo off
echo Building YOLO Detection Application for Windows (Native)...

REM Create build directory
if not exist build mkdir build
cd build

REM Configure with CMake
echo Configuring with CMake...
cmake .. -DCMAKE_BUILD_TYPE=Release

if %ERRORLEVEL% neq 0 (
    echo CMake configuration failed!
    pause
    exit /b 1
)

REM Build the application
echo Building application...
cmake --build . --config Release

if %ERRORLEVEL% neq 0 (
    echo Build failed!
    pause
    exit /b 1
)

echo.
echo Build completed successfully!
echo Executable location: build\Release\YOLODetectionApp.exe
echo.
echo Before running the application, make sure to:
echo 1. Install Python dependencies: pip install -r python\requirements.txt
echo 2. Run the application: build\Release\YOLODetectionApp.exe
echo.
pause