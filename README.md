# Firmware for Time-Circuits-Display

This repository holds the current firmware for the wonderful ESP32-based [Time-Circuits-Display](https://circuitsetup.us/product/complete-time-circuits-display-kit/).

Readily available here: https://circuitsetup.us/product/complete-time-circuits-display-kit/

![TCD_Front](https://user-images.githubusercontent.com/76924199/194286089-8f3b6ae8-2e50-46c0-8476-8c4cf0775188.jpg)

Features include
- Time keeping via NTP (network time), GPS or stand-alone
- Time keeping for years 1-9999, based on Gregorian Calendar
- Support for time zones and automatic DST
- "Present time" is always a clock (not stale), also after [time travel](#time-travel)
- Option to make time travels [persistent](#persistent--non-persistent-time-travels) over reboots
- Movie-accurate time travel times in "decorative mode" (automatic time cycling)
- [Alarm function](#how-to-set-up-the-alarm)
- [Night mode](#night-mode) (displays off or dimmed; manual or automatic)
- [SD card](#sd-card) support for custom audio files
- ["Return from Time Travel"](#time-travel) (hold "9" for 2 seconds)
- [Keypad menu](#the-keypad-menu) for adjusting various settings and viewing IP address and WiFi status
- Enhanced network-accessible [Config Portal](#short-summary-of-first-steps) for setup
- 24-hour clock mode for non-Americans ;)
- Optional power-up intro
- Optional [sound on the hour](#additional-custom-sounds)
- Support for external [fake "power switch"](#fake-power-onoff-switch-external-time-travel-trigger)
- Support for [externally triggered time travel](#fake-power-onoff-switch-external-time-travel-trigger), with selectable delay
- Support for external display acting as [Speedometer](#external-speedometer), used in time travel sequences
- Support for external [GPS receiver](#gps-receiver), used as time source, and for speed to be displayed on speedo display
- Support for external [temperature sensor](#temperature-sensor) to display temperature on speedo display while idle
- Built-in installer for default audio files in addition to OTA firmware updates

The code here is based on rudementary code from/by circuitsetup.us, who in the meantime decided to use this software, under the name "v2", as their "official" firmware (apart from some sounds, of which they say they like theirs better). The difference between what you get here and what circuitsetup.us have in their repo is that the code here might be ahead in development, and have more features enabled.

## Installation

There are different alternative ways to install this firmware:

1) If a previous version of this software was installed on your device, you can upload the provided pre-compiled binary to update to the current version: Go to the Config Portal, click on "Update" and select the pre-compiled binary file provided in this repository. 

2) Using the Arduino IDE: Download the sketch source code, all required libraries (info in the source code) and compile it. Then upload the sketch to the device. Alternatively, you could use PlatformIO. This method is the one for fresh ESP32 boards and/or folks familiar with Arduino programming.

### Audio file installation

While the sound files are part of this software, they need to be installed separately.

If you are familiar with the Arduino IDE (1.8), you could use "Tools"->"ESP32 Sketch Data Upload" to upload the sound files. Version 2.x of the IDE lacks that data upload menu item and/or plug-ins; in this case go the other way:

The other way is as follows:
- Copy all the files in the "data" folder to the root folder of a FAT32 formatted SD card (it is recommended to format this card before usage, but if it contains other files, these will be ignored)
- power down the clock,
- insert this SD card into the device and 
- power up the clock.
 
If (and only if) the **exact and complete contents of the data folder** is found on the SD card during boot, the keypad will have an "INSTALL AUDIO FILES" menu item, and the Config portal will show an "Audio file installation" option at the bottom of the "Setup" page. 

If you choose to use the Config Portal, trigger the audio file installation process by writing "COPY" into the empty text field, and clicking on "Save". The device will then copy all the default audio files from the SD to its internal flash file system. The SD can (and should) be removed afterwards. The keypad menu method is described [further down](#the-keypad-menu) in this document.

If you want to re-use the SD card for substituting the default sound files, please remove the default files and "TCD_def_snd.txt" from the SD card first. More information on sound substitution can be found below in the "[SD card](#sd-card)" section of this document.

## Short summary of first steps

- As long as the device is unconfigured or later for some reason fails to connect to a configured WiFi network, it starts in "access point" mode, i.e. it creates a WiFi access point named "TCD-AP". The default IP in access point mode is 192.168.4.1. Please turn the volume knob down to a minimum while in AP mode; this mode needs more power and might - in connection with a high volume level - cause the 3.3V converter on the ESP32 to overload, resulting in a freeze or crash of the software.
- Using your computer or handheld device, connect to the AP and go to http://192.168.4.1 in your browser to enter the Config Portal, click on "Configure WiFi" and configure your Wifi network. Note that the device expects an IP address via DHCP, unless you entered valid data in the fields for static IP addresses (IP, gateway, netmask, DNS). (If the device is inaccessible as a result of wrong static IPs, hold ENTER when powering it up until the white LED lits; static IP data will be deleted and the device will return to DHCP.)
- After saving the WiFi network settings, the device reboots and tries to connect to your configured WiFi network. If it fails to connect to your network, the device will again start in access point mode. 
- Next, if DHCP is used, find out about the IP address assigned to the device. Hold ENTER on the keypad for 2 seconds, then repeatedly press ENTER until "NET-WORK" is shown, then hold ENTER for 2 seconds. The device will then show its current IP address.
- Then re-open the config portal in your browser using this IP address, and configure other settings on the "Setup" page, first and foremost NTP server (if available) and your time zone. A list of valid times zones is accessible by a link on the settings page. If the time zone isn't set correctly, the clock will show a wrong time and DST (daylight saving) will not be switched on/off correctly.

There are two ways of configuring the device: Through the aforementioned Config Portal (ie the network), or the keypad menu, as described [further below](#the-keypad-menu). Not all settings, however, are configurable in both.

If you don't have WiFi access, you can set the time through the keypad menu, see below.

## Operation basics

*Present time* is a clock and, normally, shows the actual present time, as received from the network or set up through the [keypad menu](#the-keypad-menu).

*Destination time* and *Last time departed* are stale. These, by default, work like in the movie: Upon a time travel, "present time" becomes "last time departed", and "destination time" becomes "present time". 

There is also a "decorative" mode where the device cycles through a list of pre-programmed "destination" and "last time departed" times. This mode is chosen by setting the "Time-rotation Interval" in the config portal (via network) or the keypad menu to a non-zero value. The device will then cycle through named list every 5th, 10th, 15th, 30th or 60th minute, and thereby change the displays, regardless of times displayed as a result from a time travel. Set the interval to 0 to disable this "decorative" mode.

The clock only supports the [Gregorian Calendar](https://en.wikipedia.org/wiki/Gregorian_calendar), of which it pretends to have been used since year 1. The [Julian Calendar](https://en.wikipedia.org/wiki/Julian_calendar) is not taken into account. As a result, some years that, in the Julian Calendar, were leap years between years 1 and 1582 in most of today's Europe, 1700 in DK/NO/NL(except Holland and Zeeland), 1752 in the British Empire, 1753 in Sweden, 1760 in Lorraine, 1872 in Japan, 1912 in China, 1915 in Bulgaria, 1917 in the Ottoman Empire, 1918 in Russia and Serbia and 1923 in Greece, are normal years in the Gregorian one. As a result, dates do not match in those two calender systems, the Julian calendar is currently 13 days behind. I wonder if Doc's TC took all this into account. (Then again, he wanted to see Christ's birth on Dec 25, 0. Luckily, he didn't actually try to travel to that date. Assuming a negative roll-over, he might have ended up in eternity.)

## Time travel

To travel through time, hold "0" for 2 seconds. The destination time, as shown in the red display, will be your new present time, the old "present time" will be the "last time departed". The new "present" time will continue to run like a normal clock.

Before holding "0", you can also first quickly set a new destination time by entering a date on the keypad: mmddyyyy, mmddyyyyhhmm or hhmm, then press ENTER. While typing, there is no visual feedback, but the date is then shown on the "destination time" display after pressing ENTER. 

To travel back to actual present time, hold "9" for 2 seconds.

Beware that the alarm function, by default, is based on the real actual present time, not the time displayed. This can be changed in the Config Portal.

## Persistent / Non-persistent time travels

On the Config Portal's "Setup" page, there is an option item named "Make time travels persistent". The default is yes.

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

## Night mode

Night mode will switch off the *destination time* and *last time departed* displays, dim the *present time* display to a minimum, and reduce the volume of sound playback (except alarm). Holding "4" enables night mode, holding "5" disables it.

In the Config Portal, you can also configure "automatic night mode". Enter start and end hours, and the device will go into night mode at the defined start hour (xx:00), and return to normal at the end hour (yy:00). To disable automatic night mode, set start and end to the same value.

Apart from increasing the displays' lifetime, Night Mode reduces the power consumption of the device from around 4.5W to around 2.5W.

## The keypad menu
 
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
 
#### How to enter dates/times for the three displays:

- Hold ENTER to invoke main menu
- Press ENTER repeatedly until the desired display is the only one lit
- Hold ENTER until the display goes off except for the first field to enter data into
- The field to enter data into is shown (exclusively), pre-set with its current value
- Data entry works as follows: If you want to keep the currently shown pre-set, press ENTER to proceed to next field. Otherwise press a digit on the keypad; the pre-set is then overwritten by the value entered. 2 digits can be entered (4 for years), upon which the current value is stored and the next field is activated. You can also enter less than 2/4 digits and press ENTER when done with the field. Note that the month needs to be entered numerically (1-12), and the hour needs to be entered in 24 hour mode, regardless of 12-hour or 24-hour mode as per the Config Portal setting.
- After entering data into all fields, the data is saved and the menu is left automatically.
- Note that when entering dates/times into the *destination time* or *last time departed* displays, the Time-rotation Interval is automatically set to 0. Your entered date/time(s) are shown until overwritten by time travels (see below, section "How to select the Time-rotation Interval").
- By entering a date/time into the *present time* display, the RTC (real time clock) of the device is adjusted, which is useful if you can't use NTP for time keeping, and really helpful when using GPS. Always set the real actual present time here; if you want to display some other time, use the Time Travel function. Note: The time you entered will be overwritten if/when the device has access to network time via NTP or GPS. For DST (daylight saving) and GPS, it is essential that you also set the correct time zone in the Config Portal.

#### How to set the audio volume:

Basically, and by default, the device uses the hardware volume knob to determine the desired volume. You can change this to a software setting as follows:
- Hold ENTER to invoke main menu
- Press ENTER repeatedly until "VOL-UME" is shown
- Hold ENTER
- Press ENTER to toggle between "HW" (volume knob) or "SW" (software)
- Hold ENTER to proceed
- If you chose "SW", you can now select the desired level by pressing ENTER repeatedly. There are 16 levels available.
- Hold ENTER to save and quit the menu

#### How to set up the alarm:

- Hold ENTER to invoke main menu
- Press ENTER repeatedly until "ALA-RM" is shown
- Hold ENTER
- Press ENTER to toggle the alarm on and off, hold ENTER to proceed
- Then enter the hour and minutes. This works as described above.
- The menu is left automatically after entering the minute. "SAVING" is displayed briefly.

Under normal operation (ie outside of the menu), holding "1" enables the alarm, holding "2" disables it. When the alarm is set and enabled, the dot in the present time's minute field is lit. 

Note that the alarm is recurring, ie it rings every day at the programmed time, unless disabled. Also note, as mentioned, that the alarm is by default relative to your actual *present time*, not the time displayed (eg after a time travel). It can, however, be configured to be based on the time displayed, in the Config Portal.
 
#### How to select the Time-rotation Interval:

- Hold ENTER to invoke main menu
- Press ENTER repeatedly until "TIME-ROTATION-INTERVAL" is shown
- Hold ENTER, "INT" is displayed
- Press ENTER repeatedly to cycle through the possible Time-rotation Interval values.
- A value of 0 disables automatic time cycling ("OFF").
- Non-zero values make the device cycle through a number of pre-programmed times. The value means "minutes" (hence "MIN-UTES") between changes.              
- Hold ENTER to select the value shown and exit the menu ("SAVING" is displayed briefly)
 
#### How to adjust the display brightness:

- Hold ENTER to invoke main menu
- Press ENTER repeatedly until "BRIGHTNESS" is shown
- Hold ENTER, the displays show all elements, the top-most display says "LVL"
- Press ENTER repeatedly to cycle through the possible levels (1-5)
- Hold ENTER to use current value and proceed to next display
- After the third display, "SAVING" is displayed briefly and the menu is left automatically.
 
#### How to find out the IP address and WiFi status:

- Hold ENTER to invoke main menu
- Press ENTER repeatedly until "NET-WORK" is shown
- Hold ENTER, the displays show the IP address
- Press ENTER to view the WiFi status
- Hold ENTER to leave the menu

#### How to install the default audio files:

- Hold ENTER to invoke main menu
- Press ENTER repeatedly until "INSTALL AUDIO FILES" is shown. If this menu does not appear, the SD card isn't configured properly; see the "Audio file installation" section at the top of this document.
- Hold ENTER to proceed
- Press ENTER to toggle between "CANCEL" and "COPY"
- Hold ENTER to proceed. If "COPY" was chosen, the display will guide you through the rest of the process. The menu is quit automatically afterwards.
 
#### How to leave the menu:
 
 - While the menu is active, press ENTER repeatedly until "END" is displayed.
 - Hold ENTER to leave the menu
 
## SD card

The provided audio files are, after proper installation (see "[Audio file installation](#audio-file-installation)" section at the top of this document), part of the firmware and stored in the device's flash file system. These default sounds can, however, be substituted by files on a FAT32-formatted SD card, and will be played back from the SD card. No installation necessary. 

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
- "nmoff.mp3". Played when disabling night mode by holding "5"
- "shutdown.mp3". Played when the device is fake "powered down" using an external switch (see below)
- "startup.mp3". Played when the clock is connected to power and finished booting

If a file is not found on the SD card, the default file from the device's flash memory will be played.

Several sounds are time-sync'd to display action, such as "intro", "enter", "baddate", "startup", "timetravel", "travelstart". If you substitute those with custom files on the SD card, synchronicity will naturally be lost.

Note that the "Audio file installer" cannot be used to replace default sounds in the device's flash memory with custom sounds. Your custom sounds will be played back from the SD card, which needs to remain in the slot for that matter.

## Additional Custom Sounds

The software supports some additional user-provided sounds, which must reside on the SD card. If the respective file is present, it will be used. If that file is absent, no sound will be played.

- "hour.mp3": Will be played every hour, on the hour. This feature is disabled in night mode.
- "key3.mp3" and/or "key6.mp3": Will be played if you hold the "3"/"6" key for 2 seconds.

"hour.mp3", "key3.mp3" and "key6.mp3" are not provided here. You can use any mp3, with 128kpbs or less.

## Fake "power on/off" Switch; external Time Travel Trigger

The firmware supports a switch connected to IO13 (active low) to act as a fake "power switch". If corresponding option is enabled in the Config Portal ("Use fake power switch"), the device will power-up, initialize everything, but stay quiet and dark. Only when the fake "power switch" is activated (IO13 is connected to GND), the device will visually "power up". You can also fake "power off" the device using this switch. Fake "off" disables the displays, all audio (except the alarm) and the keypad.

The switch needs to be connected to these two solder pads on the TC control board:

![access_to_io13](https://user-images.githubusercontent.com/76924199/194283241-3bee5fef-f51a-4b1a-a158-ed92292bcf32.jpg)

Furthermore, a button switch can be connected to IO27 (active low; before Oct 5, 2022, IO14 was used instead of IO27). This button switch triggers a time travel. The button must be pressed for 200ms to be recognized. There are some config options for this button in the Config Portal.

Unfortunately, there is no header and no break out for IO27 on existing TC control boards. Some soldering is required. The button needs to be connected to the two marked pins in the image below:

![nodemcuio27](https://user-images.githubusercontent.com/76924199/194284838-635419f9-5eb7-4480-8693-2bf7cfc7c744.jpg)

Luckily, there is a row of solder pads right next to the socket on the control board, where a pin header or cable can easily be soldered on:

![tcboard_io27](https://user-images.githubusercontent.com/76924199/194284336-2fe9fa9b-d5e5-49f5-b1cd-b0fd2abdff53.jpg)

## External speedometer

The firmware supports a speedometer display connected via i2c (address 0x70) as part of the time travel sequence. Unfortunately, CircuitSetup have not yet designed such a speedometer, so you are on your own to build one for the time being. There are, however, various readily available LED segment displays from Adafruit and Grove, that can be used as a basis. Adafruit 878, 1270 and 1922, as well as Grove 0.54" 14-segment 2 or 4 digit alphanumeric displays are supported. (The product numbers vary with color, the numbers here are the red ones.)

[![Watch the video](https://img.youtube.com/vi/FRRecGxAONc/0.jpg)](https://youtu.be/FRRecGxAONc)

The speedo display shown in this video is based on a fairly well-designed stand-alone replica purchased on ebay. I removed the Arduino Uno inside and wired the LED segments to an Adafruit i2c backpack (eg ADA 878). The strange look of the LEDs in the video is due to the iPhone camera's limitations; in real life it looks as good as any LED segment display.

Since the I2C bus is already quite long from the control board to the last display in the chain, I recommend soldering another JST XH 4pin plug onto the control board (there are two additional i2c break-outs available), and to connect the speedometer there.

## GPS receiver

The firmware supports an MT(K)3333-based GPS receiver, connected through i2c. The CircuitSetup-designed speedo display will have such a chip built-in, but since this gadget is not yet available, in the meantime, you can fall back to alternatives such as the Adafruit Mini GPS PA1010D (product id 4415). The GPS receiver can be used both as a source of authoritative time (like NTP), or display speed of movement on a speedo display.

GPS receivers receive signals from satellites, but in order to do so, they need to be "tuned in" (aka get a "fix"). This "tuning" process can take a long time; after first power up, it can take 30 minutes or more for a receiver to be able to determine its position. In order to speed up this process, modern GPS receivers have special "assisting" features. One key element is knowledge of current time, as this helps identifying satellite signals quicker. So, in other words, initially, you need to tell the receiver, what it is supposed to tell you. However, as soon as the receiver has received satellite signals for 15-20 minutes, it saves the data it collected to its battery-backed memory and will find a fix within seconds after power-up in the future.

For using GPS effectively as a long term source of accurate time, it is therefore essential, that 
- the TimeCicuit's RTC (real time clock) is initially set to correct local time, 
- the correct time zone is defined in the Config Portal,
- the GPS receiver has a battery
- and has been receiving data for 15-20 mins at least once a month.

If/as long as the GPS receiver has a fix and receives data from satellites, the dot in the present time's year field is lit.

#### GPS for speed

One nice feature of GPS is that the receiver can deliver current speed of movement. If the Time Cicuits are, for instance, mounted in a car or on a boat, and a speedo display is connected, this display will be just that: A real speedometer.

## Temperature sensor

Unless you do time travelling on a regular basis, the speedo display is idle most of the time in a typical home setup. To give it more of a purpose, the software supports connecting a MCP9808-based temperature sensor to the i2c bus (readily available from Adafruit, product number 1782). After connecting this sensor and enabling it in the Config Portal, the speedo display will show the ambient temperature in idle mode. (Note that you need a speedo display in order to use the temperature sensor.)

## WiFi power saving features

The Config Portal offers two options for WiFi power saving, one for AP-mode (ie when the device acts as an access point), one for station mode (ie when the device is connected to a WiFi network). Both options do the same: They configure a timer after whose expiry WiFi is switched off; the device is no longer transmitting or receiving data over WiFi. 

The timers can be set to 0 (which disables them; WiFi is never switched off; this is the default), or 10-99 minutes. 

The reason for having two different timers for AP-mode and for station mode is that if the device is used in a car, it might act as an access point, while at home it is most probably connected to a WiFi network as a client. Since in a car, WiFi will most likely not be used on a regular basis, the timer for AP mode can be short (eg 10 minutes), while the timer for station mode can be disabled.

After WiFi has been switched off due to timer expiry, it can be re-enabled by holding "7" on the keypad for approx. 2 seconds, in which case the timers are restarted (ie WiFi is again switched off after timer expiry).
