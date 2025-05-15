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
 
   ![Imager Advanced Options](/doc/images/step1.png)

2. **Advanced Options**
     - Click "EDIT SETTINGS" and setup the Wireless LAN and SSH as follow.

    ![EDIT SETTINGS](/doc/images/step2.png)
    ![Some Parameters](/doc/images/step3.png)
    ![SSH](/doc/images/step4.png)

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