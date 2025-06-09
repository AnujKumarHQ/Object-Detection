# Windows Setup Guide for YOLO Detection App

## Prerequisites Installation

### 1. Install Qt6 for Windows

**Option A: Qt Online Installer (Recommended)**
1. Download Qt Online Installer from: https://www.qt.io/download-qt-installer
2. Run the installer and create a Qt account (free)
3. Select Qt 6.5 or later with MSVC 2022 64-bit
4. Install to default location (usually `C:\Qt`)

**Option B: vcpkg (Alternative)**
```cmd
# Install vcpkg if you don't have it
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg integrate install

# Install Qt6
.\vcpkg install qt6-base:x64-windows qt6-tools:x64-windows
```

### 2. Install Python Dependencies
```cmd
cd python
pip install -r requirements.txt
```

## Building the Application

### Method 1: Using Qt6 Installation

```cmd
# Set Qt6 path (adjust path to your Qt installation)
set CMAKE_PREFIX_PATH=C:\Qt\6.5.0\msvc2022_64

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=C:\Qt\6.5.0\msvc2022_64

# Build
cmake --build . --config Release
```

### Method 2: Using vcpkg

```cmd
# Create build directory
mkdir build
cd build

# Configure with vcpkg toolchain
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:\path\to\vcpkg\scripts\buildsystems\vcpkg.cmake -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build . --config Release
```

## Running the Application

After successful build:

```cmd
# From the build directory
Release\YOLODetectionApp.exe

# Or from project root
build\Release\YOLODetectionApp.exe
```

## Troubleshooting

### Qt6 Path Issues
If CMake can't find Qt6, try:

1. **Find your Qt installation:**
   - Default: `C:\Qt\6.x.x\msvc2022_64`
   - Check Program Files: `C:\Program Files\Qt`

2. **Set environment variable permanently:**
   ```cmd
   setx CMAKE_PREFIX_PATH "C:\Qt\6.5.0\msvc2022_64"
   ```

3. **Or specify in CMake command:**
   ```cmd
   cmake .. -DCMAKE_PREFIX_PATH="C:\Qt\6.5.0\msvc2022_64"
   ```

### Python Issues
1. **Ensure Python is in PATH:**
   ```cmd
   python --version
   ```

2. **Install dependencies:**
   ```cmd
   pip install torch torchvision ultralytics opencv-python numpy pillow
   ```

### Visual Studio Issues
Make sure you have Visual Studio 2022 with C++ development tools installed.

## Quick Start Commands

```cmd
# Complete setup (adjust Qt path)
set CMAKE_PREFIX_PATH=C:\Qt\6.5.0\msvc2022_64
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
cd ..
build\Release\YOLODetectionApp.exe
```