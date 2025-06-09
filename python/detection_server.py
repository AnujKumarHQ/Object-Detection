#!/usr/bin/env python3
"""
YOLO Detection Server
Handles object detection requests from the C++ UI
"""

import sys
import json
import time
import torch
import cv2
import numpy as np
from pathlib import Path

class YOLODetectionServer:
    def __init__(self):
        self.models = {}
        self.device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
        print(f"Using device: {self.device}", file=sys.stderr)
    
    def load_model(self, model_name='yolov5s'):
        """Load YOLO model if not already loaded"""
        if model_name not in self.models:
            try:
                print(f"Loading model: {model_name}", file=sys.stderr)
                model = torch.hub.load('ultralytics/yolov5', model_name, pretrained=True)
                model.to(self.device)
                model.eval()
                self.models[model_name] = model
                print(f"Model {model_name} loaded successfully", file=sys.stderr)
            except Exception as e:
                raise Exception(f"Failed to load model {model_name}: {str(e)}")
        
        return self.models[model_name]
    
    def detect_objects(self, request):
        """Perform object detection on the given image"""
        try:
            # Parse request
            image_path = request['image_path']
            confidence_threshold = request.get('confidence_threshold', 0.5)
            iou_threshold = request.get('iou_threshold', 0.45)
            model_name = request.get('model_name', 'yolov5s')
            save_annotated = request.get('save_annotated', False)
            
            # Validate image path
            if not Path(image_path).exists():
                raise Exception(f"Image file not found: {image_path}")
            
            # Load model
            model = self.load_model(model_name)
            
            # Configure model
            model.conf = confidence_threshold
            model.iou = iou_threshold
            
            # Start timing
            start_time = time.time()
            
            # Run inference
            results = model(image_path)
            
            # Process results
            detections = []
            for *box, conf, cls in results.xyxy[0].cpu().numpy():
                x1, y1, x2, y2 = map(int, box)
                class_name = model.names[int(cls)]
                
                detection = {
                    'class': class_name,
                    'confidence': float(conf),
                    'bbox': [x1, y1, x2 - x1, y2 - y1]  # [x, y, width, height]
                }
                detections.append(detection)
            
            # Calculate processing time
            processing_time = int((time.time() - start_time) * 1000)
            
            # Save annotated image if requested
            annotated_path = None
            if save_annotated:
                annotated_path = str(Path(image_path).with_suffix('.annotated.jpg'))
                results.save(save_dir=Path(annotated_path).parent, exist_ok=True)
            
            # Prepare response
            response = {
                'success': True,
                'detections': detections,
                'processing_time': processing_time,
                'model_used': model_name,
                'device_used': str(self.device)
            }
            
            if annotated_path:
                response['annotated_image_path'] = annotated_path
            
            return response
            
        except Exception as e:
            return {
                'success': False,
                'error': str(e),
                'processing_time': 0
            }

def main():
    if len(sys.argv) != 2:
        print(json.dumps({
            'success': False,
            'error': 'Usage: python detection_server.py <json_request>'
        }))
        sys.exit(1)
    
    try:
        # Parse JSON request
        json_request = sys.argv[1]
        request = json.loads(json_request)
        
        # Create detection server
        server = YOLODetectionServer()
        
        # Process request
        response = server.detect_objects(request)
        
        # Output JSON response
        print(json.dumps(response))
        
    except json.JSONDecodeError as e:
        print(json.dumps({
            'success': False,
            'error': f'Invalid JSON request: {str(e)}'
        }))
        sys.exit(1)
    except Exception as e:
        print(json.dumps({
            'success': False,
            'error': f'Server error: {str(e)}'
        }))
        sys.exit(1)

if __name__ == '__main__':
    main()