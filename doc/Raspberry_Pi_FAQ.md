# Raspberry Pi FAQ (Last updated: 2025-05-16)

## Camera Not Working After System Update

If your Raspberry Pi camera stops working after an update, it’s likely due to incompatibility between the latest `libcamera-dev` package and `rpi-libcamera`. Specifically, version **0.5.0** of `libcamera-dev` is not supported by `rpi-libcamera`.

To fix this, you’ll need to manually **downgrade** `libcamera-dev` to version **0.4.0**.

### Steps to Downgrade to `libcamera-dev` v0.4.0:

1. Go to the [Raspberry Pi libcamera GitHub page](https://github.com/raspberrypi/libcamera) and locate the release:  
   **`v0.4.0+rpt20250213`**

2. Download the release package directly using this link:  
   [libcamera-0.4.0+rpt20250213.tar.xz](https://github.com/raspberrypi/libcamera/releases/download/v0.4.0%2Brpt20250213/libcamera-0.4.0+rpt20250213.tar.xz)

3. Extract the archive on your Raspberry Pi:
   ```bash
   tar xf libcamera-0.4.0+rpt20250213.tar.xz
   ```

4. Navigate into the extracted directory:
   ```bash
   cd libcamera-0.4.0
   ```

5. Configure the build:
   ```bash
   meson setup build --buildtype=release \
   -Dpipelines=rpi/vc4,rpi/pisp \
   -Dipas=rpi/vc4,rpi/pisp \
   -Dv4l2=true \
   -Dgstreamer=disabled \
   -Dtest=false \
   -Dlc-compliance=disabled \
   -Dcam=disabled \
   -Dqcam=disabled \
   -Ddocumentation=disabled \
   -Dpycamera=enabled
   ```

6. Try building and installing:
   ```bash
   ninja -C build install
   ```

7. If you encounter build errors, you may need to install the following dependencies  
   (make sure you’re in your Conda environment, if using one):
   ```bash
   sudo apt install cmake
   sudo apt install libglib2.0-dev
   pip3 install pybind11 jinja2 pyyaml meson
   ```

8. After resolving any build issues, run the build command again:
   ```bash
   ninja -C build install
   ```

9. Finally, link the correct version by reinstalling it via APT:
   ```bash
   sudo apt reinstall libcamera0.4
   ```
