#!/usr/bin/env python3
"""
Camera Test Script
Tests if a camera device is available
"""

import sys
import cv2
import time

def test_camera(device_id):
    """Test if camera device is available"""
    try:
        print(f"Testing camera device {device_id}...", file=sys.stderr)
        
        # Try to open camera
        cap = cv2.VideoCapture(device_id)
        if not cap.isOpened():
            print(f"Failed to open camera device {device_id}", file=sys.stderr)
            return False
        
        # Set basic properties
        cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
        cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)
        
        # Give camera time to initialize
        time.sleep(0.5)
        
        # Try to read a frame
        ret, frame = cap.read()
        cap.release()
        
        if ret and frame is not None:
            print(f"Camera device {device_id} is working", file=sys.stderr)
            return True
        else:
            print(f"Camera device {device_id} failed to capture frame", file=sys.stderr)
            return False
        
    except Exception as e:
        print(f"Camera test error: {str(e)}", file=sys.stderr)
        return False

def main():
    if len(sys.argv) != 2:
        print("Usage: python test_camera.py <device_id>", file=sys.stderr)
        sys.exit(1)
    
    try:
        device_id = int(sys.argv[1])
        
        # Test OpenCV availability first
        print(f"OpenCV version: {cv2.__version__}", file=sys.stderr)
        
        success = test_camera(device_id)
        
        if success:
            print("Camera test PASSED", file=sys.stderr)
            sys.exit(0)
        else:
            print("Camera test FAILED", file=sys.stderr)
            sys.exit(1)
        
    except ValueError:
        print("Error: device_id must be an integer", file=sys.stderr)
        sys.exit(1)
    except ImportError as e:
        print(f"Error: OpenCV not available - {str(e)}", file=sys.stderr)
        sys.exit(1)
    except Exception as e:
        print(f"Error: {str(e)}", file=sys.stderr)
        sys.exit(1)

if __name__ == '__main__':
    main()