#!/bin/bash

# Build script for YOLO Detection App

echo "Building YOLO Detection Application..."

# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the application
make -j$(nproc)

echo "Build completed!"
echo "Executable location: build/YOLODetectionApp"
echo ""
echo "Before running the application, make sure to:"
echo "1. Install Python dependencies: pip install -r python/requirements.txt"
echo "2. Ensure Qt6 is installed on your system"
echo "3. Run the application: ./build/YOLODetectionApp"