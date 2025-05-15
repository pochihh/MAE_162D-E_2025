# Raspberry Pi Dependencies 
## Setup picamera2 dependencies 
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

## Install conda and the required environment
- Install [miniconda3](https://www.anaconda.com/docs/getting-started/miniconda/install#aws-graviton2-arm-64)
    - Make sure you install the **arm64** version!
- Clone the week2 repository
    ```bash
    git clone https://github.com/Kevin75311/UCLA_MAE162E_week2_materials.git
    ```
- Enter the rpi/ folder and run installation script
    ```bash
    cd rpi
    ```
    ```bash
    bash install.sh
    ```
- After finish installing, you can test your camera with the following commands
    ```bash
    conda activate week2_new
    ```
    - This part needs to be updated. For now, you can test the environment with week2 YOLO example.

    