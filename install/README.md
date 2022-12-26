This folder holds all files necessary for immediate installation on your Time Circuits Display. Here you'll find
- a binary of the current firmware, ready for upload to the device;
- the latest audio files

### Firmware installation

If a previous version of the Time Circuits firmware was installed on your device, you can upload the provided binary to update to the current version: Go to the Config Portal, click on "Update" and select the binary file provided here ("timecircuits-A10001986.ino.nodemcu-32s.bin").

For a fresh installation, the provided binary is not usable. You'll need to use the Arduino IDE or PlatformIO and download the sketch source code, all required libraries (info in the .ino file) and compile it. Then upload the sketch to the device. This method is the one for fresh ESP32 boards and/or folks familiar with Arduino programming.

### Audio file installation

The sound pack is not updated as often as the firmware itself. If you have previously installed the latest version of the sound-pack, you normally don't have to re-install the audio files when you update the firmware. Only if either a new version of the sound-pack is released here, or your clock is quiet after a firmware update, a re-installation is needed.

- Download "sound-pack-xxxxxxxx.zip" and extract it to the root directory of of a FAT32 formatted SD card
- power down the clock,
- insert this SD card into the device's slot and 
- power up the clock.

The keypad will have an "INSTALL AUDIO FILES" menu item, and the Config portal will show an "Audio file installation" option at the bottom of the "Setup" page.

Please see [here](https://github.com/realA10001986/Time-Circuits-Display/blob/main/README.md#audio-file-installation) for further information.
