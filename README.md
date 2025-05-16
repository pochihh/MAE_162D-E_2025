# MAE_162D-E_2025
Demo project for MAE 162D/E Winter and Spring quarter 2025.
The system includes a Raspberry Pi and an Arduino Mega 2560; instructions for them are listed below.

## How to Run on the Arduino
- Under MAE_162D-E_2025/Arduino/src/
- Open and run “SimulinkGenerate.m”
- Select **Change Directory** if prompted
- Generate the codes (For both ControlLoop and StateflowBlock)
- Upload the code to the Arduino (aduino.ino)

## How to run on the Raspberry Pi
1. Install and set up your raspberry pi: See [here](doc/Raspberry_Pi_Setup_Guide.md)
2. Set up and install required dependencies: See [here](doc/Raspberry_Pi_Dependencies.md)
3. Download required model weights: See [here](doc/Raspberry_Pi_Models.md)
4. Enter the **rpi/** folder and run the python code 
    ```bash
    cd rpi
    ```
    ```bash
    python main.py [--debug] [--gps]
    ```
    - Option [-d/-\-debug]: Use this option to enter debug mode. There will be debug messages if this option is used. 
    - Option [-gps/-\-gps]: Use this option to enable the gps. Ignore it if you don't need it.

---
Some useful slides here:
1. Communication: See [here](doc/162E%20Lec4%20Week%205%20Serial%20Communication.pdf)

---
Latest FAQ: See [here](doc/Raspberry_Pi_FAQ.md)