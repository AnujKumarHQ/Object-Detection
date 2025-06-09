#!/usr/bin/env python3
"""
Webcam Frame Capture Script
Captures a single frame from webcam and saves it
"""

import sys
import cv2
import os

def capture_frame(device_id, output_path):
    """Capture a single frame from webcam"""
    try:
        # Open camera
        cap = cv2.VideoCapture(device_id)
        if not cap.isOpened():
            print(f"Error: Could not open camera {device_id}", file=sys.stderr)
            return False
        
        # Set camera properties for better performance
        cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
        cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)
        cap.set(cv2.CAP_PROP_FPS, 30)
        
        # Capture frame
        ret, frame = cap.read()
        if not ret:
            print("Error: Could not read frame", file=sys.stderr)
            cap.release()
            return False
        
        # Save frame
        success = cv2.imwrite(output_path, frame)
        cap.release()
        
        if success:
            print(f"Frame saved to: {output_path}", file=sys.stderr)
            return True
        else:
            print(f"Error: Could not save frame to {output_path}", file=sys.stderr)
            return False
            
    except Exception as e:
        print(f"Exception in capture_frame: {str(e)}", file=sys.stderr)
        return False

def main():
    if len(sys.argv) != 3:
        print("Usage: python capture_frame.py <device_id> <output_path>", file=sys.stderr)
        sys.exit(1)
    
    try:
        device_id = int(sys.argv[1])
        output_path = sys.argv[2]
        
        # Create output directory if it doesn't exist
        os.makedirs(os.path.dirname(output_path), exist_ok=True)
        
        success = capture_frame(device_id, output_path)
        sys.exit(0 if success else 1)
        
    except ValueError:
        print("Error: device_id must be an integer", file=sys.stderr)
        sys.exit(1)
    except Exception as e:
        print(f"Error: {str(e)}", file=sys.stderr)
        sys.exit(1)

if __name__ == '__main__':
    main()