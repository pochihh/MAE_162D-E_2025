# MAE_162D-E_2025
Tutorial material for MAE 162D/E Winter and Spring quarter 2025

# Raspberry Pi Setup Guide
---

## Requirements
🛒 **Hardware**:
- Raspberry Pi (any model)
- microSD card (16GB+ recommended)
- microSD card reader


🖥️ **Software**:
- Raspberry Pi Imager: [Download Here](https://www.raspberrypi.com/software/)
- Terminal / Command Prompt

---

## Setup Steps
1. **Flash OS onto SD Card**
   - Download and open **Raspberry Pi Imager**.
   - Select board: Raspberry Pi 5.
   - Select OS: Raspberry Pi OS (64-bits).
   - Select the SD card.
 
   ![Imager Advanced Options](/images/step1.png)

2. **Advanced Options**
     - Click "EDIT SETTINGS" and setup the Wireless LAN and SSH as follow.

    ![EDIT SETTINGS](/images/step2.png)
    ![Some Parameters](/images/step3.png)
    ![SSH](/images/step4.png)

3. **Flash the SD card**
    - Click "SAVE" and start flashing the SD card.

2. **Insert the SD Card and Power Up**
   - Insert SD card into the Raspberry Pi.
   - Connect the power supply.
   - Wait a few minutes for it to boot.

3. **SSH into Your Pi**
   - Check your router’s connected devices by
        ```bash
        ssh {YOUR_USERNAME}@{YOUR_IP_ADDRESS}
        ```

4. **Setup your camera**
    - Run the following command.
        ```bash
        sudo apt update
        ```
        ```bash 
        sudo nano /boot/firmware/config.txt
        ```
    - Find the following line and then modify the value to 0.
        ```
        camera_auto_detect=1
        ```
    - Then, add these two lines to the end of the files.
        ```
        dtoverlay=imx708,cam0
        dtoverlay=imx708,cam1
        ```
    - Save the file and then reboot your Rpi.

5. **Test with your camera**
    - Install [miniconda3](https://www.anaconda.com/docs/getting-started/miniconda/install#aws-graviton2-arm-64)
        - Make sure you install the arm64 version!
    - Clone the week2 repository
        ```bash
        git clone https://github.com/Kevin75311/UCLA_MAE162E_week2_materials.git
        ```
    - Run the following command and wait
        ```bash
        cd UCLA_MAE162E_week2_materials
        ```
        ```bash
        bash install.sh
        ```
    - After finish installing, you can test your camera with the following commands
        ```bash
        conda activate week2_new
        ```
        ```bash
        cd YOLOv4/
        ```
        ```bash
        python main.py
        ```


    