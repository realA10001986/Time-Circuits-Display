This folder holds all files necessary for immediate installation on your Time Circuits Display. Here you'll find
- a pre-compiled binary of the current firmware,
- the audio files for this very version of the firmware

### Firmware installation

If a previous version of the Time Circuits firmware was installed on your device, you can upload the provided pre-compiled binary to update to the current version: Go to the Config Portal, click on "Update" and select the pre-compiled binary file provided here ("timecircuits-A10001986.ino.nodemcu-32s.bin").

For a fresh installation, using the Arduino IDE or PlatformIO: Download the sketch source code, all required libraries (info in the source code) and compile it. Then upload the sketch to the device. This method is the one for fresh ESP32 boards and/or folks familiar with Arduino programming.

### Audio file installation

- Download "sound-pack.zip" and extract it to the root directory of of a FAT32 formatted SD card
- power down the clock,
- insert this SD card into the device's slot and 
- power up the clock.

The keypad will have an "INSTALL AUDIO FILES" menu item, and the Config portal will show an "Audio file installation" option at the bottom of the "Setup" page.

Please see [here](https://github.com/realA10001986/Time-Circuits-Display/blob/main/README.md#audio-file-installation) for further information.
