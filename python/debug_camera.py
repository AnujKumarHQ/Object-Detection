#!/usr/bin/env python3
"""
Camera Debug Script
Provides detailed camera debugging information
"""

import sys
import cv2
import numpy as np

def debug_camera_info():
    """Debug camera availability and information"""
    print("=== Camera Debug Information ===")
    print(f"OpenCV version: {cv2.__version__}")
    print(f"Python version: {sys.version}")
    
    # Test multiple camera indices
    available_cameras = []
    for i in range(5):  # Test cameras 0-4
        try:
            cap = cv2.VideoCapture(i)
            if cap.isOpened():
                ret, frame = cap.read()
                if ret and frame is not None:
                    height, width = frame.shape[:2]
                    print(f"Camera {i}: Available ({width}x{height})")
                    available_cameras.append(i)
                else:
                    print(f"Camera {i}: Opens but no frame")
                cap.release()
            else:
                print(f"Camera {i}: Not available")
        except Exception as e:
            print(f"Camera {i}: Error - {str(e)}")
    
    if available_cameras:
        print(f"\nAvailable cameras: {available_cameras}")
        return available_cameras[0]  # Return first available camera
    else:
        print("\nNo cameras found!")
        return -1

def test_camera_capture(device_id):
    """Test camera capture with detailed output"""
    print(f"\n=== Testing Camera {device_id} ===")
    
    try:
        cap = cv2.VideoCapture(device_id)
        if not cap.isOpened():
            print(f"Failed to open camera {device_id}")
            return False
        
        # Get camera properties
        width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
        height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
        fps = cap.get(cv2.CAP_PROP_FPS)
        
        print(f"Camera properties: {width}x{height} @ {fps} FPS")
        
        # Capture test frames
        for i in range(5):
            ret, frame = cap.read()
            if ret:
                print(f"Frame {i+1}: OK ({frame.shape})")
            else:
                print(f"Frame {i+1}: FAILED")
        
        cap.release()
        return True
        
    except Exception as e:
        print(f"Camera test failed: {str(e)}")
        return False

def main():
    try:
        # Debug camera information
        available_camera = debug_camera_info()
        
        if available_camera >= 0:
            # Test the first available camera
            success = test_camera_capture(available_camera)
            sys.exit(0 if success else 1)
        else:
            print("\nNo cameras available for testing")
            sys.exit(1)
            
    except Exception as e:
        print(f"Debug script error: {str(e)}")
        sys.exit(1)

if __name__ == '__main__':
    main()