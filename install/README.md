This folder holds all files necessary for immediate installation on your Time Circuits Display. Here you'll find
- a binary of the current firmware, ready for upload to the device;
- the latest matching sound-pack

## Firmware Installation

If a previous version of the TCD firmware is installed on your device, you can update easily using the pre-compiled binary. Enter the [Config Portal](#the-config-portal), click on "Update" and select the pre-compiled binary file ("xxx.bin") provided in the Release package.

If you are using a fresh ESP32 board, please see [timecircuits-A10001986.ino](https://github.com/realA10001986/Time-Circuits-Display/blob/main/timecircuits-A10001986/timecircuits-A10001986.ino) for detailed build and upload information, or, if you don't want to deal with source code, compilers and all that nerd stuff, go [here](https://install.out-a-ti.me) and follow the instructions.

## Sound-pack installation

The firmware comes with a sound-pack which needs to be installed separately. The sound-pack is not updated as often as the firmware itself. If you have previously installed the latest version of the sound-pack, you normally don't have to re-install it when you update the firmware. Only if the TCD displays "PLEASE INSTALL SOUND PACK" during boot, a re-installation/update is needed.

_Note that installing the sound-pack requires an [SD card](#sd-card)._

>A10001986 and CircuitSetup use different sound-packs. If you switch from one version to the other, the matching sound-pack must be re-installed. [A10001986-releases](https://github.com/realA10001986/Time-Circuits-Display/releases) use "sound-pack-twXX", while [CircuitSetup's](https://github.com/CircuitSetup/Time-Circuits-Display/releases) are named "sound-pack-csXX".

The first step is to extract the zipped sound-pack. It contains one file named "TCDA.bin".

Next, head to the [Config Portal](#the-config-portal), click on *Update*, select the "TCDA.bin" file in the bottom file selector and click on *Upload*.

<details>
<summary>More...</summary>

Alternatively, you can install the sound-pack the following way:
- Using a computer, copy "TCDA.bin" to the root directory of a FAT32 formatted SD card;
- power down the TCD,
- insert this SD card into the slot and 
- power up the TCD; the sound-pack will be installed automatically.
</details>
