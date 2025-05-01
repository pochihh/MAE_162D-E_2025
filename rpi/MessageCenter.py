from lib.tlvcodec import Encoder, Decoder
import time
import serial
import ctypes
import struct
import sys

NO_OBJECT_DETECTED = 100
YOLO_OBJECT_DETECTED = 101

class MessageCenter:
    def __init__(self, serial_port, baudrate=9600, debug=False):
        self.encoder = Encoder(5, 1024, True) # device id, buffer size, use crc
        self.decoder = Decoder(self.decoder_callback, True) # use crc
        
        self.serial = serial.Serial(serial_port, baudrate=baudrate, timeout=0.01)
        self.messageCount = 0
        
        self.debug = debug
        
    def decoder_callback(self, error, frameHeader, tlvs):
        """Callback function for the decoder."""
        if self.debug:
            print(f"Decoder_callback: {error}")
        
        # print out tlvs
        if error.value == 0:
            # print type length value
            for i in range(frameHeader.numTlvs):
                if self.debug:
                    print(f'Type: {tlvs[i][0]}; length: {tlvs[i][1]}')
                
                
    def processing_tick(self):
        """To be called every tick to process input and output messages."""
        # read data from serial port and decode it
        if self.serial.in_waiting > 0:
            data = self.serial.read(self.serial.in_waiting)
            self.decoder.decode(data)
        
        # check if there are messages to send
        if self.messageCount > 0:
            # send the message
            length, buffer = self.encoder.wrapupBuffer()
            self.serial.write(buffer[:length])
            
            if self.debug:
                print(f"Encoder message sent.")
                print(f"Data [{length}]:")	
                for i in range(length):
                    print(f'{buffer[i]:02d}', end = ' ')
                print("\n")
            
            # reset message count
            self.encoder.reset()
            self.messageCount = 0
            
    def add_message(self, type, length, value):
        """Add a message to the encoder."""
        self.encoder.addPacket(type, length, value)
        self.messageCount += 1
        
    def add_yolo_detection(self, object, bbox, confidence):
        """Add a YOLO detection message to the encoder."""
        if self.debug:
            print(f"Adding YOLO detection: {object}, BBox: {bbox}, Confidence: {confidence}")
            
        # create a ctypes structure for the bounding box (4 int) and confidence (1 float)
        class BBox(ctypes.Structure):
            _fields_ = [("x", ctypes.c_int), ("y", ctypes.c_int), ("w", ctypes.c_int), ("h", ctypes.c_int)]
        
        class Detection(ctypes.Structure):
            _fields_ = [("object", ctypes.c_int), ("bbox", BBox), ("confidence", ctypes.c_float)]
        
        detection = Detection()
        detection.object = ctypes.c_int(object)
        detection.bbox.x = ctypes.c_int(bbox[0])
        detection.bbox.y = ctypes.c_int(bbox[1])
        detection.bbox.w = ctypes.c_int(bbox[2])
        detection.bbox.h = ctypes.c_int(bbox[3])
        detection.confidence = ctypes.c_float(confidence)
        
        if self.debug:
            print(f"Detection: {detection.object}, BBox: ({detection.bbox.x}, {detection.bbox.y}, {detection.bbox.w}, {detection.bbox.h}), Confidence: {detection.confidence}")
    
        self.add_message(YOLO_OBJECT_DETECTED, ctypes.sizeof(detection), detection)
        
    def add_no_object_detected(self):
        if self.debug:
            print("No object detected")
        # don't send anything for now
        # self.add_message(NO_OBJECT_DETECTED, 0, 0)