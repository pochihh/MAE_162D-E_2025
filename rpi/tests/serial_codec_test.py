import serial
import time
import sys
import os
sys.path.append('..')
from lib.tlvcodec import Encoder, Decoder
import ctypes

# Configure the serial port
ser = serial.Serial('/dev/ttyUSB0', 9600)  # Replace with your actual port and baud rate

# prepare message to send 
# test encoder
encoder = Encoder(199) # device id
encoder.addPacket(987, ctypes.sizeof(ctypes.c_uint32), ctypes.c_uint32(111))
length, buffer = encoder.wrapupBuffer()

try:
    while True:
        # Send data
        ser.write(buffer[:length])
        print("Encoder message sent")

        # Receive data
        if ser.in_waiting > 0:
            data = ser.readline().decode('utf-8').strip()
            print(f"Received: {data}")

        time.sleep(1)

except KeyboardInterrupt:
    print("Exiting...")
finally:
    ser.close()