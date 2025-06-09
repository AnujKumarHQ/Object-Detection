@echo off
echo Testing Webcam Setup...
echo.

echo 1. Testing Python availability...
python --version
if %ERRORLEVEL% neq 0 (
    echo Python not found! Please install Python and add it to PATH.
    pause
    exit /b 1
)

echo.
echo 2. Testing OpenCV installation...
python -c "import cv2; print('OpenCV version:', cv2.__version__)"
if %ERRORLEVEL% neq 0 (
    echo OpenCV not found! Installing...
    pip install opencv-python
)

echo.
echo 3. Running camera debug script...
python python\debug_camera.py

echo.
echo 4. Testing camera capture...
python python\test_camera.py 0

echo.
echo If all tests pass, your webcam should work with the application.
pause