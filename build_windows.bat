@echo off
echo Building YOLO Detection Application for Windows...

REM Check if Qt6 path is set
if "%CMAKE_PREFIX_PATH%"=="" (
    echo ERROR: CMAKE_PREFIX_PATH not set!
    echo Please set it to your Qt6 installation path, for example:
    echo set CMAKE_PREFIX_PATH=C:\Qt\6.5.0\msvc2022_64
    echo.
    echo Common Qt6 installation paths:
    echo - C:\Qt\6.5.0\msvc2022_64
    echo - C:\Qt\6.6.0\msvc2022_64
    echo - C:\Program Files\Qt\6.5.0\msvc2022_64
    pause
    exit /b 1
)

REM Create build directory
if not exist build mkdir build
cd build

REM Configure with CMake
echo Configuring with CMake...
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="%CMAKE_PREFIX_PATH%"

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