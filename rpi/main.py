# System imports
import sys, os
import select
import ctypes, struct
import time
import tty
import termios

# Package imports

# Custom imports
from utils import *
from lib.tlvcodec import Encoder, Decoder

def image_processing():
    # print("Running image processing...")
    pass

def algorign_processing():
    # print("Running algorithm processing...")
    pass

def main():
    # Welcome message
    print_seal()
    
    # system start time 
    program_start_time = time.time()
    fd = sys.stdin.fileno()
    old_settings = termios.tcgetattr(fd)

    try:
        tty.setcbreak(fd)  # or tty.setraw(fd)
        while True:
            image_processing()
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