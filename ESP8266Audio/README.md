How to install the patched version of ESP8266Audio 1.9.5 for use under esp32-arduino 2.0.x

There are two ways of doing this:

1) Patch your installed version: For users who know their way with a terminal/cli and have a unix-compliant *patch* utility installed:

- Install version 1.9.5 of the ESP8266Audio library
- Download ESP8266Audio_1_9_5.diff
- cd /path/to/your/ESP8266Audio
- execute *patch --dry-run -p1 <../ESP8266Audio_1_9_5.diff* and see if there are any errors.
- finally: execute *patch -p1 <../ESP8266Audio_1_9_5.diff*

2) Overwrite your installation of the source code. This is the brute-force way, if patching as described above is not an option:

- Install version 1.9.5 of the ESP8266Audio library
- Download ESP8266Audio_1_9_6A10001986.zip
- Extract the downloaded zip file somewhere
- Copy the contents of the newly created ESP8266Audio folder over your installed copy of the library
