# Time-Circuits-Display (A10001986)
Firmware for ESP32-based Time-Circuits-Display

This is a fork of a previous version of the firmware for [this](https://circuitsetup.us/product/complete-time-circuits-display-kit/) wonderful piece of hardware:

https://github.com/CircuitSetup/Time-Circuits-Display/blob/master/README.md

![TCD Front](https://raw.githubusercontent.com/CircuitSetup/Time-Circuits-Display/master/Images/tcd_front2.jpg)


"This Time Circuits Display has been meticulously reproduced to be as accurate as possible to the one seen in the Delorean Time Machine in the Back to the Future movies. The LED displays are custom made to the correct size for CircuitSetup. This includes the month 14 segment/3 character displays being closer together, and both the 7 & 14 segment displays being 0.6" high by 0.35" wide." 

During August of 2022, I have re-written almost the entire code. New features and changes include
- Time keeping via NTP (network time) or stand-alone
- "Present time" is always a clock (not stale), even after time travel
- Correct clocking for years 1-9999, including leap years
- Support for time zones and automatic DST (in NTP-mode only)
- Option to make time travels persistent over reboots
- Movie-accurate time travel times in "decorative mode" (automatic time cycling).
- Alarm function
- SD card support for custom audio files
- Night mode (displays off or dimmed; manual or automatic)
- Enhanced time travel sequence
- "Return from Time Travel" (hold "9" for 2 seconds)
- Keypad menu for adjusting various settings and viewing IP address and WiFi status
- Enhanced Config Portal for setup
- More stable WiFi connections at startup
- Configurable WiFi connection timeouts and retries
- 24-hour clock mode for non-Americans ;)
- Optional power-up intro with sound
- Optional sound on the hour
- Optional external fake "power switch"
- Optional externally triggered time travel, with optional delay
- Built-in installer for default audio files in addition to OTA firmware updates
- many bugfixes and internal changes
- etc

**Update:** The nice folks over at circuitsetup.us have taken over this software its in entirety, apart from some sounds, of which they say the like theirs better. As long as I am working on this project, the version here is probably further ahead in development; this might change in the future, when this project eventually no longer is a priority for me.

**Installation**

There are several alternative ways to install this firmware:

1) Using the Arduino IDE: Download the sketch source code, all required libraries (info in the source code) and compile it. Then upload the sketch to the device. Alternatively, you could use PlatformIO. This method is the one for fresh ESP32 boards and/or folks familiar with Arduino programming.

2) If a previous version of this software was installed on your device, you can upload the provided pre-compiled binary to update to the current version: Go to the Config Portal, click on "Update" and select the pre-compiled binary file provided in this repository. 

Audio file installation

While the sound files are part of this software, they need to be installed separately.

If you are familiar with the Arduino IDE (1.8), you could use "Tools"->"ESP32 Sketch Data Upload" to upload the sound files. Version 2.x of the IDE lacks that data upload menu item and/or plug-ins; in this case go the other way:

The other way is to copy all the files in the "data" folder to the root folder of a fresh FAT32 formatted SD card, to insert this card into the device and install them through the keypad menu or the Config portal. If the exact and complete contents of the data folder is found on the SD card during boot, the keypad will have a "INSTALL AUDIO FILES" menu item, and the Config portal will show an "Audio file installation" option at the bottom of the "Setup" page. Write "COPY" into the empty text field, and click on "Save". The device will then copy all the default audio files from the SD to its internal flash file system. The SD can (and should) be removed afterwards.

**Short summary of first steps:**

- As long as the device is unconfigured or later for some reason fails to connect to a configured network, it starts in "access point" mode, i.e. it creates an access point named "TCD-AP". The default IP in access point mode is 192.168.4.1. 
- Connect to the AP and go to http://192.168.4.1 in your browser to enter the Config Portal, click on "Configure WiFi" and configure your Wifi network. Note that the device expects an IP address via DHCP, unless you entered valid data in the fields for static IP addresses (IP, gateway, netmask, DNS). (If the device is inaccessible as a result of wrong static IPs, hold ENTER when powering it up until the white LED lits; the static IP data will be deleted and the device returns to DHCP.)
- After saving the WiFi network settings, the device reboots and tries to connect to your configured WiFi network. If it fails to connect to your network, the device will again start in access point mode. 
- Next, if DHCP is used, find out about the IP address assigned to the device. Hold ENTER on the keypad for 2 seconds, then press ENTER until "NET-WRK" is shown, then hold ENTER for 2 seconds. The device will then show its current IP address.
- Then re-open the config portal in your browser using this IP address, and configure other settings on the "Setup" page, eg WiFi connection timeout, WiFi connection retries, NTP server and time zone. A list of valid times zones is accessible by a link on the settings page.
- There are two ways of configuring the device: Through the aforementioned Config Portal (ie the network), or the keypad menu, as described further below. Not all settings, however, are configurable in both.
- If you don't have WiFi access, you can set the time through the keypad menu, see below.

**Operation basics**

*Present time* is a clock and, normally, shows the actual present time, as received from the network or set up through the keypad menu.

*Destination time* and *Last time departed* are stale. These, by default, work like in the movie: Upon a time travel, "present time" becomes "last time departed", and "destination time" becomes "present time". 

There is also a "decorative" mode where the device cycles through a list of pre-programmed times, which cannot be changed. This mode is chosen by setting the "Time-rotation Interval" in the config portal (via network) or the keypad menu to a non-zero value. The device will then cycle through named list every 5th, 15th, 30th, 45th or 60th minute, and thereby change the displays, regardless of times displayed as a result from a time travel. Set the interval to 0 to disable this "decorative" mode.

**Time travel**

To travel through time, hold "0" for 2 seconds. The destination time, as shown in the red display, will be your new present time, the old "present time" will be the "last time departed". Note that the new "present" time will continue to run like a normal clock.

Before holding "0", you can also first quickly set a new destination time by entering a date on the keypad: mmddyyyy, mmddyyyyhhmm or hhmm, then press ENTER. While typing, there is no visual feedback, but the date is then shown on the "destination time" display after pressing ENTER. 

To travel back to actual present time, hold "9" for 2 seconds.

Beware that the alarm function, by default, is based on the real actual present time, not the time displayed. This can be changed in the Config Portal.

**Persistent / Non-persistent time travels**

On the Config Portal's "Setup" page, there is a option item named "Make time travels persistent". The default is yes.

If time travels are persistent
- a user-programmed *destination time* is always stored in flash memory, and retrieved from there after a power-loss. It can be programmed through the keypad menu, or ahead of a time travel by typing mmddyyyy/mmddyyyyhhmm/hhmm plus ENTER. In both cases, the time is stored in flash memory and retrieved upon power-on.
- *last time departed* as displayed at any given point is always stored in flash memory, and retrieved upon power-on.
- *present time*, be it actual present time or "fake" after time travelling, will continue to run while the device is not powered, as long as its battery lasts, and displayed on power-up.

If time travels are non-persistent
- a user-programmed *destination time* is only stored to flash memory when programmed through the keypad menu, but not if entered ahead of a time travel (ie outside of the keypad menu, just by typing mmddyyyy/mmddyyyyhhmm/hhmm plus ENTER).
- a user-programmed *last time departed* is only stored to flash memory when programmed through the keypad menu, but not if the result of a time travel.
- *present time* is always reset to actual present time upon power-up.

If you want your device to display exactly the same after a power loss, choose persistent (and set the Time-rotation Interval to 0). 

If you want to display your favorite *destination time* and *last time departed* upon power-up, and not have time travels overwrite them in flash memory, choose "non-persistent", and program your times through the keypad menu (and set the Time-rotation Interval to 0). Those times will never be overwritten in flash memory by later time travels. Note, however, that the times displayed might actually change due to time travels.

Note that a non-zero Time-rotation Interval will force the device to cycle through the list of pre-programmed times, regardless of your time travel persistence setting. This cycling will, however, paused for 30 minutes if entered a new destination time and/or travelled in time.

**Night mode**

Night mode will switch off the *destination time* and *last time departed* displays, dim the *present time* display to a minimum, and reduce the volume of sound playback (except alarm). Holding "4" enables night mode, holding "5" disables it. 

**The keypad menu**
 
The menu is controlled by "pressing" or "holding" the ENTER key on the keypad.

A "press" is shorter than 2 seconds, a "hold" is 2 seconds or longer.
Data entry is done by pressing the keypad's number keys.

The menu is involked by holding the ENTER button.

First step is to choose a menu item. The available "items" are   
- enter dates/times for the three displays,
- set the audio volume (VOL-UME),
- set an alarm ("ALA-RM"),
- select the Time-rotation Interval ("TIME-ROTATION-INT"),
- select the brightness for the three displays ("BRIGHTNESS"),
- show network information ("NET-WORK"),
- install the default audio files ("INSTALL AUDIO FILES")
- quit the menu ("END").
 
Pressing ENTER cycles through the list, holding ENTER selects an item, ie a mode.
 
How to enter dates/times for the three displays:
- Hold ENTER to invoke main menu
- Press ENTER until the desired display is the only one lit
- Hold ENTER until the display goes off except for the first field to enter data into
- The field to enter data into is shown (exclusively), pre-set with its current value
- Data entry works as follows: If you want to keep the currently shown pre-set, press ENTER to proceed to next field. Otherwise press a digit on the keypad; the pre-set is then overwritten by the value entered. 2 digits can be entered (4 for years), upon which the current value is stored and the next field is activated. You can also enter less than 2/4 digits and press ENTER when done with the field. Note that the month needs to be entered numerically (1-12), and the hour needs to be entered in 24 hour mode, regardless of 12-hour or 24-hour mode as per the Config Portal setting.
- After entering data into all fields, the data is saved and the menu is left automatically.
- Note that when entering dates/times into the *destination time* or *last time departed* displays, the Time-rotation Interval is automatically set to 0. Your entered date/time(s) are shown until overwritten by time travels (see below, section "How to select the Time-rotation Interval").
- By entering a date/time into the *present time* display, the RTC (real time clock) of the device is adjusted, which is useful if you can't use NTP for time keeping. The time you entered will be overwritten if/when the device has access to network time via NTP.

How to set the audio volume:

Basically, and by default, the device uses the hardware volume knob to determine the desired volume. You can change this to a software setting as follows:
- Hold ENTER to invoke main menu
- Press ENTER until "VOL-UME" is shown
- Hold ENTER
- Press ENTER to toggle between "HW" (volume knob) or "SW" (software)
- Hold ENTER to proceed
- If you chose "SW", you can now select the desired level by pressing ENTER repeatedly. There are 16 levels available.
- Hold ENTER to save and quit the menu

How to set up the alarm:
- Hold ENTER to invoke main menu
- Press ENTER until "ALA-RM" is shown
- Hold ENTER
- Press ENTER to toggle the alarm on and off, hold ENTER to proceed
- Then enter the hour and minutes. This works as described above.
- The menu is left automatically after entering the minute. "SAVE" is displayed briefly.

Under normal operation (ie outside of the menu), holding "1" enables the alarm, holding "2" disables it. 

Note that the alarm is recurring, ie it rings every day at the programmed time, unless disabled. Also note, as mentioned, that the alarm is by default relative to your actual *present time*, not the time displayed (eg after a time travel). It can, however, be configured to be based on the time displayed, in the Config Portal.

If the alarm is set and enabled, the dot in the present time's minute field is lit. 
 
How to select the Time-rotation Interval:
- Hold ENTER to invoke main menu
- Press ENTER until "TIME-ROTATION-INTERVAL" is shown
- Hold ENTER, "INT" is displayed
- Press ENTER to cycle through the possible Time-rotation Interval values.
- A value of 0 disables automatic time cycling ("OFF").
- Non-zero values make the device cycle through a number of pre-programmed times. The value means "minutes" (hence "MIN-UTES") between changes.              
- Hold ENTER to select the value shown and exit the menu ("SAVE" is displayed briefly)
 
How to adjust the display brightness:
- Hold ENTER to invoke main menu
- Press ENTER until "BRIGHTNESS" is shown
- Hold ENTER, the displays show all elements, the top-most display says "LVL"
- Press ENTER to cycle through the possible levels (1-5)
- Hold ENTER to use current value and proceed to next display
- After the third display, "SAVE" is displayed briefly and the menu is left automatically.
 
How to find out the IP address and WiFi status:
- Hold ENTER to invoke main menu
- Press ENTER until "NET-WORK" is shown
- Hold ENTER, the displays shows the IP address
- Press ENTER to view the WiFi status
- Hold ENTER to leave the menu

How to install the default audio files:
- Hold ENTER to invoke main menu
- Press ENTER until "INSTALL AUDIO FILES" is shown. If this menu does not appear, the SD card isn't configured properly; see the "Audio file installation" section at the top of this document.
- Hold ENTER to proceed
- Press ENTER to toggle between "CANCEL" and "COPY"
- Hold ENTER to proceed. If "COPY" was chosen, the display will guide you through the rest of the process. The menu is quit automatically afterwards.
 
 How to leave the menu:
 - While the menu is active, press ENTER until "END" is displayed.
 - Hold ENTER to leave the menu
 

**SD card**

The provided audio files are, after proper installation (see "Audio file installation" section at the top of this document), part of the firmware and stored in the device's flash file system. These default sounds can, however, be substituted by files on a FAT32-formatted SD card, and will be played back from the SD card. No installation necessary. 

Your custom replacements need to be put in the root directoy of the card, be in mp3 format (128kbps recommended) and named as follows:
- "enter.mp3". Played when a date was entered and ENTER was pressed
- "baddate.mp3". Played when a bad (too short or too long) date was entered and ENTER was pressed
- "intro.mp3": Played during the power-up intro
- "travelstart.mp3". Played when a time travel starts.
- "timetravel.mp3". Played when re-entry of a time travel takes place.
- "alarm.mp3". Played when the alarm sounds.
- "alarmon.mp3". Played when enabling the alarm by holding "1"
- "alarmoff.mp3". Played when disabling the alarm by holding "2"
- "nmon.mp3". Played when enabling night mode by holding "4"
- "nmoff.mp3". Played when disabling night might by holding "5"
- "shutdown.mp3". Played when the device is fake "powered down" using an external switch (see below)
- "startup.mp3". Played when the clock is connected to power and finished booting

If a file is not found on the SD card, the default file from the device's flash memory will be played.

Several sounds are time-sync'd to display action, such as "intro", "enter", "baddate", "startup", "timetravel", "travelstart". If you substitute those with custom files on the SD card, synchronicity will naturally be lost.

Note that the "Audio file installer" cannot be used to replace default sounds in the device's flash memory with custom sounds. Your custom sounds will be played back from the SD card, which needs to remain in the slot for that matter.

**Additional Custom Sounds**

If the SD card holds a file named "hour.mp3", this file will be played every hour, on the hour. This feature is disabled in night mode.

If the SD card holds files named "key3.mp3" and/or "key6.mp3", these files will be played if you hold the "3"/"6" key for 2 seconds. 

**External switch and button**

The software supports a switch connected to IO13 (active low) to act as a fake "power switch". If this option is enabled, the device will power-up, initialize everything, but stay quiet and dark. Only when the fake "power switch" is activated, the device will visually "power up". You can also fake "power off" the device using this switch. Fake "off" disables the displays, the audio (except the alarm) and the keypad.

Furthermore, a button switch can be connected to IO14 (active low). This button triggers a re-entry from a time travel and plays the respective sequence. The button must be pressed for 200ms to be recognized.
