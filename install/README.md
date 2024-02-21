This folder holds all files necessary for immediate installation on your Time Circuits Display. Here you'll find
- a binary of the current firmware, ready for upload to the device;
- the latest audio data

### Firmware installation

If a previous version of the Time Circuits firmware is installed on your device, you can upload the provided binary to update to the current version: Go to the Config Portal, click on "Update" and select the binary file provided here ("timecircuits-A10001986.ino.nodemcu-32s.bin").

For a fresh installation, the provided binary is not usable. Please see [here](https://github.com/realA10001986/Time-Circuits-Display/blob/main/timecircuits-A10001986/timecircuits-A10001986.ino) for build and installation instructions.

### Audio data installation

The sound pack is not updated as often as the firmware itself. If you have previously installed the latest version of the sound-pack, you normally don't have to re-install it when you update the firmware. Only if either a new version of the sound-pack is released here, or your TCD is quiet after a firmware update or displays "PLEASE INSTALL AUDIO FILES", a re-installation is needed.

**You cannot mix firmwares and audio data from this repository and CircuitSetup's. If you install the firmware from this repository, you need to install the audio data from this repository as well, and vice versa.**

The first step is to download "install/sound-pack-xxxxxxxx.zip" and extract it. It contains one file named "TCDA.bin".

Then there are two alternative ways to proceed. Note that both methods *require an SD card*.

1) Through the Config Portal. Click on *Update*, select the "TCDA.bin" file in the bottom file selector and click on *Upload*. Note that an SD card must be in the slot during this operation.

2) Via SD card:
- Copy "TCDA.bin" to the root directory of of a FAT32 formatted SD card;
- power down the TCD,
- insert this SD card into the slot and 
- power up the TCD; the audio data will be installed automatically.

See also [here](https://github.com/realA10001986/Time-Circuits-Display/blob/main/README.md#audio-data-installation).
