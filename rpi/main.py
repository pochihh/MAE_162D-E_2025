# System imports
import sys, os
import select
import ctypes, struct
import time
import tty
import termios

# Package imports
import cv2

# Custom imports
from utils import *
from yolo_utils import *
from gps_utils import *
from lib.tlvcodec import Encoder, Decoder

def image_processing():
    # print("Running image processing...")
    frame = picam2.capture_array()
    frame = cv2.cvtColor(frame, cv2.COLOR_RGB2BGR)
    
    # Object detection
    outputs = convert_to_blob(frame, network, 128, 128)    
    bounding_boxes, class_objects, confidence_probs = object_detection(outputs, frame, 0.5)   

def gps_processing(): 
    data, addr = sock.recvfrom(1024)
    line = data.decode().strip()

    try:
        distances_m = list(map(float, line.split(',')))
    except ValueError:
        pass

    position, error = trilaterate_2D(distances_m)
    # if position is not None:
    #     print(f"[POS] x = {position[0]:.2f} ft, y = {position[1]:.2f} ft, RMSE = {error:.2f} ft")
            
def algorign_processing():
    # print("Running algorithm processing...")
    pass

def main():
    # Welcome message
    print_seal()
    camera_initialization()
    gps_initialization()
    
    # system start time 
    program_start_time = time.time()
    fd = sys.stdin.fileno()
    old_settings = termios.tcgetattr(fd)

    try:
        tty.setcbreak(fd)  # or tty.setraw(fd)
        while True:
            image_processing()
            # gps_processing()
            algorign_processing()
            
            if select.select([sys.stdin], [], [], 0)[0]:
                ch = sys.stdin.read(1)
                # print(f"You typed: {ch}")
                if ch == 'q':
                    print("\nUser quit command detected. Exiting program...")
                    break
                else:
                    print(f"\nUnrecognized command: {ch}")
                    
            # delay for a short period to avoid busy waiting
            time.sleep(0.1)
            # print time elapsed since start without newline
            print(f"\rTime elapsed: {get_time_millis(program_start_time):.2f} ms", end="")
            print("\b" * 30, end="")  # Clear the line
            
    except KeyboardInterrupt:
        print("Keyboard interrupt detected. Exiting...")
        # do some cleanup if necessary
    
    except Exception as e:
        print(f"An error occurred: {e}")
        
    finally:
        termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
    
    return

    
if __name__ == "__main__":
    main()