# Kicker_Robot_CRFC
## Project Created by Brayden Amoroso, Caden Daffron, Nathan Del Guanto, Shane Wood
### Code Written by Shane Wood


![PXL_20250407_193807232](https://github.com/user-attachments/assets/1728064b-1571-4761-9de4-82eb5c4ab7bd)


# Information
ESP-IDF Version 5.3.1

## Controller
![PXL_20250404_163744640](https://github.com/user-attachments/assets/bdda5c95-97c3-4e1b-8982-32f1728a90d8)

1 SSD1306 (Display) 128 x 32 <br>
1 Micrcontroller: ESP32-S3 <br>
Clock Speed: 240 Mhz <br>
USB port for communication with a Ps4 controller <br>

## Mainboard
![PXL_20250414_172650533](https://github.com/user-attachments/assets/b5df5143-866d-4dd2-8f3e-b4702183e20d)

1 Micrcontroller: ESP32-S3 <br>
Clock Speed: 240 Mhz <br>
2 Odrive S1s <br>
2 D5065 Motors <br>
1 SSD1306 (Display) 128 x 64 <br>

These 2 systems communicate over ESP-NOW <br>


# Setup
Some files (notably SDK config) are not in github, therefore the device config must be set up on the first time the repository is cloned.<br><br>

Upon opening one of the projects, check the bottom bar in vscode.<br>
The device target will likely be esp32 as shown below: <br>
![image](https://github.com/user-attachments/assets/1cf5b333-844a-4bbf-a825-a8a3f3ef10d2) <br>

Click this and change it to ESP32-S3 -> Then Via built in USB-> JTAG <br>

The com port can be found using device manager. Due to this project using offical s3 devkits the com port will be named Silicon Labs CP210x USB to UART Bridge (If using the uart micro usb port, recommended) or USB Serial Device (If using USB micro usb port, required to use debugger). <br>
![image](https://github.com/user-attachments/assets/583216cc-ca1d-46d8-8844-848912bf7fa0) <br>

The com port can be changed in vscode by selecting COMX next to ESP32s3 on the bottom row. <br>
![image](https://github.com/user-attachments/assets/bce5fda6-8e40-48f8-b2f6-25dcf6dd5fe2) <br>

Now the gear can be selected on the bottom row, this opens up the device configuration <br>
![image](https://github.com/user-attachments/assets/5060ec7d-8238-442f-8582-858042766251) <br>

The following settings should be changed (for both microcontrollers): <br>
Bootloader Optimization Level: Optimize for Performance (-02) <br>
Flash Size: 8 MB <br>
Complier options: Optimize for Performance (-02) <br>
CPU Frequency: 240 Mhz <br><br>

Now the project can be build (wrench, bottom bar). The first time this is done on a device it will take a long period of time (up to 10 minutes, depending on the device).<br>
The board can also be programmed from here by using the lighting bolt. <br>
Finally both of these and device monitoring (These boards feature the ability to log information over the built in UART) can all be done by using the fire button on the bottom bar.<br>
