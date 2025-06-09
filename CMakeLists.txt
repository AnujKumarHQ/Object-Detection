cmake_minimum_required(VERSION 3.16)
project(YOLODetectionApp VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Enable automatic MOC, UIC, and RCC processing
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Add executable
add_executable(YOLODetectionApp
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/detection_client.cpp
    src/detection_client.h
    src/image_processor.cpp
    src/image_processor.h
)

# Link Qt libraries
target_link_libraries(YOLODetectionApp Qt6::Core Qt6::Widgets)

# Include directories
target_include_directories(YOLODetectionApp PRIVATE src)