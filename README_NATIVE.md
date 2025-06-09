# YOLO Object Detection - Windows Native App

A lightweight Windows desktop application that combines a native Win32 interface with a powerful Python YOLO detection backend. **No Qt required!**

## Features

- **Native Windows Interface**: Clean, responsive UI using Win32 API
- **Lightweight**: No large dependencies like Qt
- **Multiple Input Sources**: Support for images (video and webcam coming soon)
- **Configurable Detection**: Adjustable confidence and IoU thresholds
- **Multiple YOLO Models**: Support for YOLOv5s, YOLOv5m, YOLOv5l, and YOLOv5x
- **Visual Results**: Detection results with class labels and confidence scores

## Prerequisites

### System Requirements
- Windows 10 or later
- Visual Studio 2019/2022 with C++ development tools
- CMake 3.16 or higher
- Python 3.7 or higher

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

1. **Launch the Application**
   - Run `YOLODetectionApp.exe`

2. **Load an Image**
   - Click "Open Image" to select an image file
   - Supported formats: PNG, JPG, JPEG, BMP, TIFF

3. **Configure Detection Settings**
   - **Model**: Choose from YOLOv5s/m/l/x variants
   - **Confidence**: Set minimum confidence threshold (0.0-1.0)
   - **IoU Threshold**: Set IoU threshold for NMS (0.0-1.0)

4. **View Results**
   - Detection details appear in the results panel
   - Processing time and object count are displayed

## Project Structure

```
├── src/                    # C++ source files
│   ├── main.cpp           # Application entry point
│   ├── mainwindow.h/cpp   # Main UI window (Win32)
│   ├── detection_client.h/cpp  # Python process manager
│   ├── image_processor.h/cpp   # Image processing utilities
│   ├── resource.h         # Resource definitions
│   └── app.rc            # Windows resources
├── python/                # Python backend
│   ├── detection_server.py    # YOLO detection server
│   └── requirements.txt       # Python dependencies
├── CMakeLists.txt         # CMake configuration
├── build_windows_native.bat   # Build script
└── README_NATIVE.md       # This file
```

## Advantages over Qt Version

- **Smaller Size**: No Qt libraries required (~100MB+ savings)
- **Faster Startup**: Native Windows controls load instantly
- **Better Integration**: Uses Windows native look and feel
- **Simpler Deployment**: Only requires Visual C++ Redistributable

## Troubleshooting

### Build Issues
1. **Visual Studio not found**: Install Visual Studio 2019/2022 with C++ development tools
2. **CMake errors**: Ensure CMake 3.16+ is installed and in PATH

### Runtime Issues
1. **Python not found**: Ensure Python is in PATH
2. **Missing dependencies**: Run `pip install -r python/requirements.txt`
3. **Detection script not found**: Ensure `python/detection_server.py` exists

### Performance Tips
- Use GPU acceleration by installing CUDA-compatible PyTorch
- Choose appropriate model size (YOLOv5s for speed, YOLOv5x for accuracy)
- Adjust confidence threshold to filter unwanted detections

## Future Enhancements

- [ ] Image display with bounding box visualization
- [ ] Video processing support
- [ ] Real-time webcam detection
- [ ] Batch processing capabilities
- [ ] Export detection results

## License

This project is open source. Please check individual component licenses:
- Win32 API: Microsoft Windows SDK
- PyTorch: BSD-style license
- Ultralytics YOLOv5: GPL-3.0