<h1 align="center">TTGO Quickstart Guide</h1>
<p align="center">Here are the quickstart guide to start your development with TTGO Development Board</p>

## Setup Arduino IDE
Install Arduino IDE from [arduino.cc](https://www.arduino.cc/en/software).    

## Install USB-to-UART VCP Drivers (Windows and macOS)
TTGO uses connection via USB-to-UART CP210x chip series provided by Silicon Labs,  
Therefore, you need to install it from [Silicon Labs](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers#downloads)

## Setup Espressif ESP32 board definition files
In order to use TTGO Board (which is based on Espressif ESP32), you need to install ESP32 board manager.  

1. Open up arduino settings
   - On Windows, File - Preferences
   - On macOS, Arduino - Preferences
2. Go to Additional Board Manager URLs and click popup button
3. Copy `https://dl.espressif.com/dl/package_esp32_index.json` and paste it to the popup and Click **OK**.
   - you can [right click me](https://dl.espressif.com/dl/package_esp32_index.json) and `Copy Link Address` to copy it easily.
4. Now head back to `Board Manager`
   - On all platforms, Tools - Board Manager
5. Type `ESP32` and press enter.
6. Find `esp32` by `Espressif Systems`, Press `Install`.  
   This may take a long time depending on your network conditions, So take a sip of coffee and have a little break.
7. Done!

## (Optional) Install TFT_eSPI Library
In order to use display (240x135) of TTGO, It is recommended to use [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI). A performance optimized TFT driver library developed by **[@Bodmer](https://github.com/Bodmer)**.
You can install it via `Library Manager`.  

### Installation
1. Go to Tools - Library Manager
2. Search `TFT_eSPI`
3. Find `TFT_eSPI` by `Bodmer` and press `Install`.  

### Configure TFT_eSPI for TTGO
1. Go to Arduino workspace folder 
   - On Windows, It is `%userprofile%\Documents\Arduino`
   - On macOS/\*NIX, It is `~/Documents/Arduino`
2. Go to `libraries` and find `TFT_eSPI`
3. Open `User_Setup_Select.h` with your favorite text editor.
4. Comment the line `#include <User_Setup.h>`
5. Scroll down until you find `#include <User_Setups/Setup25_TTGO_T_Display.h>`. Uncomment that line.
6. Save the changes.
7. You are now ready to rock the display.

## (Optional) ESP32 Filesystem Uploader
With Arduino ESP32 Filesystem uploader, you can easily upload files to ESP32 internal flash memory.  
Depending on your version of ESP32, You might have 4MB or 16MB of memory on board.  
  
With awesome tool developed by **[@me-no-dev](https://github.com/me-no-dev)**, You can now easily upload files to esp32 in no time.

1. Head to [ESP32FS Plugin Repository on GitHub](https://github.com/me-no-dev/arduino-esp32fs-plugin)
2. Go to Releases
3. Download the zip file
4. Go to Arduino workspace folder  
   - On Windows, It is `%userprofile%\Documents\Arduino`
   - On macOS/\*NIX, It is `~/Documents/Arduino`
5. Go to `tools` folder, if the folder doesn't exist, create one.
6. Copy the `ESP32FS` folder to `tools` folder.  
   Path should be like `/tools/ESP32FS/tool/esp32fs.jar`.
7. Done! Restart the Arduino IDE if necessary.

### How to use this?
Create `./data/` from your sketch directory.  
The content of `./data/` will be flashed when you use the upload tool from `tools - ESP32 Sketch Data Upload`.  
_Don't forget to select correct flash size at `Tools - Flash Size`!_  
  
**TIP:** You can open sketch directory using `Sketch - View Sketch Folder`  
     Shortcut `Ctrl-K` or `Cmd-K` would work.

## Use Visual Studio Code instead.
1. [Install Visual Studio Code](https://code.visualstudio.com) (if you didn't).
2. Install [Arduino Extension](https://marketplace.visualstudio.com/items?itemName=vsciot-vscode.vscode-arduino) from Extensions MarketPlace.
3. Install [ESP8266FS Extension](https://marketplace.visualstudio.com/items?itemName=kash4kev.vscode-esp8266fs) from Extensions MarketPlace.  
4. Open up Command Palette and type `Arduino: Initialize`

### Windows
5. Edit your c_cpp_properties.json by following [related issue](https://github.com/Microsoft/vscode-arduino/issues/678#issuecomment-662230997)
   1. Set C/C++ Language Server use Arduino GCC
   ```json
   {
      "configurations": [
         {
            "compilerPath": "C:/Program Files (x86)/Arduino/hardware/tools/avr/bin/avr-gcc.exe",
            "intelliSenseMode": "gcc-x86",
            "cStandard": "c11",
            "cppStandard": "c++11"
         }
      ]
   }
   ```
   2. Add this to your workspace settings.json 
   ```json
      {
          "C_Cpp.default.intelliSenseMode": "gcc-x86",
          "C_Cpp.intelliSenseEngine": "Tag Parser"
      }
   ```

### macOS
5. Run `./generate_properties.macOS.sh`.
6. (Big Sur Only) Update `pyserial` to latest version. [Issue](https://github.com/espressif/esptool/issues/540#issuecomment-741168660)
   `pip install -U pyserial && pip3 install -U pyserial`
7. (Big Sur Only) If problem persists, Do the following:
   1. Use the dev channel version of ESP32 Library at [https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_dev_index.json](https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_dev_index.json).  
   **MAKE SURE YOU RE-RUN THE `./generate_properties.macOS.sh` AFTER UPDATE.** [Issue](https://github.com/espressif/esptool/issues/540#issuecomment-747185562)
