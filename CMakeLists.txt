cmake_minimum_required(VERSION 3.16)
project(YOLODetectionApp VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Windows-specific settings
if(WIN32)
    add_definitions(-DUNICODE -D_UNICODE)
    set(CMAKE_WIN32_EXECUTABLE TRUE)
endif()

# Add executable
add_executable(YOLODetectionApp WIN32
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/detection_client.cpp
    src/detection_client.h
    src/image_processor.cpp
    src/image_processor.h
    src/resource.h
    src/app.rc
)

# Link Windows libraries
if(WIN32)
    target_link_libraries(YOLODetectionApp 
        user32 
        gdi32 
        comctl32 
        comdlg32 
        shell32
        ole32
        oleaut32
        uuid
    )
endif()

# Include directories
target_include_directories(YOLODetectionApp PRIVATE src)