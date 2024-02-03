This folder holds all files necessary for immediate installation on your Time Circuits Display. Here you'll find
- a binary of the current firmware, ready for upload to the device;
- the latest audio data

### Firmware installation

If a previous version of the Time Circuits firmware is installed on your device, you can upload the provided binary to update to the current version: Go to the Config Portal, click on "Update" and select the binary file provided here ("timecircuits-A10001986.ino.nodemcu-32s.bin").

For a fresh installation, the provided binary is not usable. You'll need to use the Arduino IDE or PlatformIO, download the sketch source code, all required libraries (info in the .ino file) and compile it. Then upload the sketch to the device. This method is the one for fresh ESP32 boards and/or folks familiar with Arduino programming.

### Audio data installation

The sound pack is not updated as often as the firmware itself. If you have previously installed the latest version of the sound-pack, you normally don't have to re-install it when you update the firmware. Only if either a new version of the sound-pack is released here, or your TCD is quiet after a firmware update or displays "PLEASE INSTALL AUDIO FILES", a re-installation is needed.

**You cannot mix firmwares and audio data from this repository and CircuitSetup's. If you install the firmware from this repository, you need to install the audio data from this repository as well, and vice versa.**

- Download "sound-pack-xxxxxxxx.zip" and extract it. It contains one file named "TCDA.bin". Copy this file to the root directory of of a FAT32 formatted SD card,
- power down the TCD,
- insert this SD card into the device's slot and 
- power up the TCD.

If the TCDA.bin file is found on the SD card, the TCD will show "INSTALL AUDIO FILES?" after power-up. Press ENTER briefly to toggle between "CANCEL" and "PROCEED". Choose "PROCEED" and hold the ENTER key for 2 seconds.

See also [here](https://github.com/realA10001986/Time-Circuits-Display/blob/main/README.md#audio-file-installation).
