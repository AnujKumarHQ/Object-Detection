# YOLO Object Detection - C++ UI with Python Backend

A cross-platform desktop application that combines a modern C++ Qt interface with a powerful Python YOLO detection backend.

## Features

- **Modern Qt6 Interface**: Clean, responsive UI with real-time detection results
- **Multiple Input Sources**: Support for images, videos, and webcam input
- **Configurable Detection**: Adjustable confidence and IoU thresholds
- **Multiple YOLO Models**: Support for YOLOv5s, YOLOv5m, YOLOv5l, and YOLOv5x
- **Visual Results**: Bounding boxes with class labels and confidence scores
- **Cross-Platform**: Works on Windows, macOS, and Linux

## Architecture

The application uses a hybrid architecture:
- **C++ Frontend**: Qt6-based GUI for user interaction and image display
- **Python Backend**: YOLO detection engine using PyTorch and Ultralytics
- **IPC Communication**: JSON-based communication between C++ and Python processes

## Prerequisites

### System Requirements
- CMake 3.16 or higher
- Qt6 (Core, Widgets modules)
- C++17 compatible compiler
- Python 3.7 or higher

### Python Dependencies
```bash
pip install -r python/requirements.txt
```

Required Python packages:
- torch>=1.9.0
- torchvision>=0.10.0
- ultralytics
- opencv-python>=4.5.0
- numpy>=1.21.0
- Pillow>=8.3.0

## Building

### Linux/macOS
```bash
chmod +x build.sh
./build.sh
```

### Windows
```cmd
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

### Manual Build
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

## Usage

1. **Launch the Application**
   ```bash
   ./build/YOLODetectionApp
   ```

2. **Load an Image**
   - Click "Open Image" to select an image file
   - Supported formats: PNG, JPG, JPEG, BMP, TIFF

3. **Configure Detection Settings**
   - **Model**: Choose from YOLOv5s/m/l/x variants
   - **Confidence**: Set minimum confidence threshold (0.0-1.0)
   - **IoU Threshold**: Set IoU threshold for NMS (0.0-1.0)

4. **View Results**
   - Detected objects are highlighted with bounding boxes
   - Detection details appear in the results panel
   - Processing time and object count are displayed

## Project Structure

```
├── src/                    # C++ source files
│   ├── main.cpp           # Application entry point
│   ├── mainwindow.h/cpp   # Main UI window
│   ├── detection_client.h/cpp  # Python process manager
│   └── image_processor.h/cpp   # Image annotation utilities
├── python/                # Python backend
│   ├── detection_server.py    # YOLO detection server
│   └── requirements.txt       # Python dependencies
├── CMakeLists.txt         # CMake configuration
├── build.sh              # Build script
└── README.md             # This file
```

## Key Components

### DetectionClient
Manages communication with the Python detection backend:
- Spawns Python processes for detection requests
- Handles JSON serialization/deserialization
- Provides timeout and error handling

### ImageProcessor
Handles image annotation and display:
- Draws bounding boxes with class-specific colors
- Renders labels and confidence scores
- Manages image scaling and display

### MainWindow
Main application interface:
- File operations (open image/video)
- Detection parameter controls
- Results visualization
- Progress indication

## Configuration

### Model Selection
The application supports multiple YOLO models:
- **YOLOv5s**: Fastest, smallest model
- **YOLOv5m**: Balanced speed/accuracy
- **YOLOv5l**: Higher accuracy
- **YOLOv5x**: Best accuracy, slowest

### Detection Parameters
- **Confidence Threshold**: Minimum confidence for detections (default: 0.5)
- **IoU Threshold**: IoU threshold for Non-Maximum Suppression (default: 0.45)

## Troubleshooting

### Common Issues

1. **Python Process Fails to Start**
   - Ensure Python is in your PATH
   - Verify all Python dependencies are installed
   - Check that the detection_server.py script is accessible

2. **Qt6 Not Found**
   - Install Qt6 development packages
   - Set CMAKE_PREFIX_PATH to Qt6 installation directory

3. **Model Loading Errors**
   - Ensure internet connection for first-time model download
   - Check available disk space (models are ~14-166MB)
   - Verify PyTorch installation

### Performance Tips

- Use GPU acceleration by installing CUDA-compatible PyTorch
- Choose appropriate model size for your use case
- Adjust confidence threshold to filter unwanted detections

## Future Enhancements

- [ ] Video processing support
- [ ] Real-time webcam detection
- [ ] Custom model training integration
- [ ] Batch processing capabilities
- [ ] Export detection results to various formats
- [ ] Plugin system for custom post-processing

## License

This project is open source. Please check individual component licenses:
- Qt6: LGPL/Commercial
- PyTorch: BSD-style license
- Ultralytics YOLOv5: GPL-3.0

## Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues for bugs and feature requests.