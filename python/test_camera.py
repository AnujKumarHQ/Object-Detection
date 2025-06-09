#!/usr/bin/env python3
"""
Camera Test Script
Tests if a camera device is available
"""

import sys
import cv2

def test_camera(device_id):
    """Test if camera device is available"""
    try:
        cap = cv2.VideoCapture(device_id)
        if not cap.isOpened():
            return False
        
        # Try to read a frame
        ret, frame = cap.read()
        cap.release()
        
        return ret and frame is not None
        
    except Exception as e:
        print(f"Camera test error: {str(e)}", file=sys.stderr)
        return False

def main():
    if len(sys.argv) != 2:
        print("Usage: python test_camera.py <device_id>", file=sys.stderr)
        sys.exit(1)
    
    try:
        device_id = int(sys.argv[1])
        success = test_camera(device_id)
        sys.exit(0 if success else 1)
        
    except ValueError:
        print("Error: device_id must be an integer", file=sys.stderr)
        sys.exit(1)

if __name__ == '__main__':
    main()