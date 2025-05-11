# export_onnx.py
import torch

# Load YOLOv5s model (pretrained)
model = torch.hub.load('ultralytics/yolov5', 'yolov5s', pretrained=True)

# Export to ONNX
model.eval()
model.export(format='onnx')  # Exports to yolov5s.onnx by default
