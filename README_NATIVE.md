# YOLO Real-time Object Detection - Windows Native App

A lightweight Windows desktop application that combines a native Win32 interface with a powerful Python YOLO detection backend. **No Qt required!** Now supports **real-time webcam detection**.

## Features

- **Native Windows Interface**: Clean, responsive UI using Win32 API
- **Lightweight**: No large dependencies like Qt
- **Real-time Webcam Detection**: Live object detection from webcam feed
- **Static Image Detection**: Support for single image analysis
- **Configurable Detection**: Adjustable confidence and IoU thresholds
- **Multiple YOLO Models**: Support for YOLOv5s, YOLOv5m, YOLOv5l, and YOLOv5x
- **Adjustable Frame Rate**: Control detection FPS (1-10 FPS recommended)
- **Visual Results**: Detection results with class labels and confidence scores

## Prerequisites

### System Requirements
- Windows 10 or later
- Visual Studio 2019/2022 with C++ development tools
- CMake 3.16 or higher
- Python 3.7 or higher
- **Webcam/Camera** (for real-time detection)

### Python Dependencies
```cmd
cd python
pip install -r requirements.txt
```

Required Python packages:
- torch>=1.9.0
- torchvision>=0.10.0
- ultralytics
- opencv-python>=4.5.0
- numpy>=1.21.0
- Pillow>=8.3.0

## Building

### Quick Build (Recommended)
```cmd
build_windows_native.bat
```

### Manual Build
```cmd
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

## Running

After successful build:
```cmd
build\Release\YOLODetectionApp.exe
```

## Usage

### Real-time Webcam Detection

1. **Launch the Application**
   - Run `YOLODetectionApp.exe`

2. **Start Webcam Detection**
   - Click "Start Webcam" to begin real-time detection
   - The application will automatically detect your default camera (device 0)

3. **Configure Real-time Settings**
   - **FPS**: Set detection frame rate (1-10 recommended)
     - Lower FPS = better performance, less CPU usage
     - Higher FPS = more responsive, higher CPU usage
   - **Model**: Choose detection model (YOLOv5s recommended for real-time)
   - **Confidence**: Set minimum confidence threshold
   - **IoU Threshold**: Set IoU threshold for NMS

4. **Monitor Results**
   - Live detection results appear in the results panel
   - Processing time and object count are displayed in real-time
   - Status bar shows current FPS and detection statistics

5. **Stop Detection**
   - Click "Stop Webcam" to end real-time detection

### Static Image Detection

1. **Load an Image**
   - Click "Open Image" to select an image file
   - Supported formats: PNG, JPG, JPEG, BMP, TIFF

2. **Configure Detection Settings**
   - Same settings as real-time detection
   - Higher accuracy models (YOLOv5l, YOLOv5x) work well for static images

3. **View Results**
   - Detection details appear in the results panel
   - Processing time and object count are displayed

## Performance Tips

### For Real-time Detection
- **Use YOLOv5s** for best real-time performance
- **Set FPS to 3-5** for smooth detection without overwhelming the system
- **Lower confidence threshold** (0.3-0.4) to catch more objects
- **Close other camera applications** to avoid conflicts

### For Best Accuracy
- **Use YOLOv5l or YOLOv5x** for static images
- **Higher confidence threshold** (0.6-0.8) for fewer false positives
- **GPU acceleration**: Install CUDA-compatible PyTorch for faster processing

### System Optimization
- **Ensure good lighting** for webcam detection
- **Position camera properly** for best detection results
- **Monitor CPU usage** and adjust FPS accordingly

## Project Structure

```
├── src/                    # C++ source files
│   ├── main.cpp           # Application entry point
│   ├── mainwindow.h/cpp   # Main UI window (Win32)
│   ├── detection_client.h/cpp  # Python process manager
│   ├── image_processor.h/cpp   # Image processing utilities
│   ├── webcam_capture.h/cpp    # Webcam capture manager
│   ├── resource.h         # Resource definitions
│   └── app.rc            # Windows resources
├── python/                # Python backend
│   ├── detection_server.py    # YOLO detection server
│   ├── capture_frame.py       # Webcam frame capture
│   ├── test_camera.py         # Camera availability test
│   └── requirements.txt       # Python dependencies
├── CMakeLists.txt         # CMake configuration
├── build_windows_native.bat   # Build script
└── README_NATIVE.md       # This file
```

## Troubleshooting

### Webcam Issues
1. **Camera not detected**: 
   - Check if camera is connected and working
   - Close other applications using the camera
   - Try different USB ports

2. **Poor detection performance**:
   - Reduce FPS setting
   - Use YOLOv5s model
   - Ensure good lighting conditions

3. **Frame capture errors**:
   - Verify OpenCV installation: `python -c "import cv2; print(cv2.__version__)"`
   - Check camera permissions in Windows settings

### Build Issues
1. **Visual Studio not found**: Install Visual Studio 2019/2022 with C++ development tools
2. **CMake errors**: Ensure CMake 3.16+ is installed and in PATH

### Runtime Issues
1. **Python not found**: Ensure Python is in PATH
2. **Missing dependencies**: Run `pip install -r python/requirements.txt`
3. **Detection script not found**: Ensure all Python scripts exist in `python/` directory

## Advanced Configuration

### Camera Selection
- Default camera (device 0) is used automatically
- To use different camera, modify `m_deviceId` in `webcam_capture.cpp`

### Frame Rate Optimization
- **1-2 FPS**: Very low CPU usage, good for monitoring
- **3-5 FPS**: Balanced performance and responsiveness
- **6-10 FPS**: High responsiveness, higher CPU usage

### Model Selection Guide
- **YOLOv5s**: Fastest, good for real-time (13.7MB)
- **YOLOv5m**: Balanced speed/accuracy (25.1MB)
- **YOLOv5l**: Higher accuracy, slower (89.3MB)
- **YOLOv5x**: Best accuracy, slowest (166.8MB)

## Future Enhancements

- [ ] Multiple camera support
- [ ] Recording detection sessions
- [ ] Custom detection zones
- [ ] Alert system for specific objects
- [ ] Batch video processing
- [ ] Export detection logs

## License

This project is open source. Please check individual component licenses:
- Win32 API: Microsoft Windows SDK
- PyTorch: BSD-style license
- Ultralytics YOLOv5: GPL-3.0
- OpenCV: Apache 2.0 license