# Firmware for Time Circuits Display

This repository holds the most current firmware for CircuitSetup's wonderful ESP32-based [Time Circuits Display](https://circuitsetup.us/product/complete-time-circuits-display-kit/).

The hardware is available [here](https://circuitsetup.us/product/complete-time-circuits-display-kit/) and built around a NodeMCU ESP32-S dev board.

| ![TCD Front](https://user-images.githubusercontent.com/76924199/200327688-cfa7b1c2-abbd-464d-be6d-5d295e51056e.jpg) |
|:--:|
| *This very one is now in the Universal Studios [BTTF Escape Room](https://www.universalorlando.com/web/en/us/things-to-do/entertainment/universals-great-movie-escape) (Orlando)* |

Features include
- Time keeping for years 1-9999, based on Gregorian Calendar
- Support for time zones and automatic DST
- Time synchronization via NTP or [GPS](#gps-receiver)
- Movie-accurate [time travel](#time-travel) function with *Present time* always running as a clock (as opposed to being stale)
- ["Return from Time Travel"](#time-travel) (hold "9" for 2 seconds)
- ["Decorative mode"](#operation-basics): Movie-accurate time travel times displayed in *destination time* and *last time departed* displays, and cycled in configurable interval
- Option to make time travels [persistent](#persistent--non-persistent-time-travels) over reboots
- [Alarm function](#how-to-set-up-the-alarm)
- [Night mode](#night-mode) (displays off or dimmed; manual, scheduled or sensor-controlled)
- &#127381; (coming soon) [Music player](#the-music-player)
- [SD card](#sd-card) support for custom audio files for effects, &#127381; and music for the Music Player
- Advanced network-accessible [Config Portal](#short-summary-of-first-steps) for setup &#127381; with mDNS support for easy access (http://timecircuits.local, hostname configurable)
- [Keypad menu](#the-keypad-menu) for adjusting various settings and viewing IP address and WiFi status
- 24-hour clock mode for non-Americans
- &#127381; [Room condition mode](#room-condition-mode-temperaturehumidity-sensor): Display temperature and humidity instead of destination/departed time (requires suitable sensor)
- Optional power-up intro
- Optional [sound on the hour](#additional-custom-sounds)
- Support for external [fake "power switch"](#fake-power-onoff-switch-external-time-travel-trigger)
- Support for external display acting as [Speedometer](#speedometer)
- Support for external [GPS receiver](#gps-receiver), used as time source, and for speed to be displayed on speedo display
- &#127381; Support for external [temperature/humidity sensor](#room-condition-mode-temperaturehumidity-sensor) for "room condition mode" and to display temperature on speedo display while idle
- Support for time travel [triggered by external source](#fake-power-onoff-switch-external-time-travel-trigger), with selectable delay
- Support for triggering time travels on [other props](#controlling-other-props); TCD can act as master controller for SID, flux capacitor, etc.
- Built-in installer for default audio files in addition to OTA firmware updates

The code here is based on rudimentary code from/by circuitsetup.us, who in the meantime decided to use this firmware, under the name "v2", as their official firmware (apart from some sounds, of which they say they like theirs better). The difference between what you get here and what circuitsetup.us have in their repo is that the code here might be ahead in development, and the pre-compiled binary has all the features enabled.

## Installation

There are different alternative ways to install this firmware:

1) If a previous version of the Time Circuits firmware was installed on your device, you can upload the provided pre-compiled binary to update to the current version: Go to the Config Portal, click on "Update" and select the pre-compiled binary file provided in this repository ("install/timecircuits-A10001986.ino.nodemcu-32s.bin").

2) Using the Arduino IDE or PlatformIO: Download the sketch source code, all required libraries (see info in timecircuits-A10001986.ino) and compile it. Then upload the sketch to the device. This method is the one for fresh ESP32 boards and/or folks familiar with Arduino programming.

 *Important: After a firmware update, the displays might stay dark or display "WAIT" for up to a minute after reboot. Do NOT unplug the clock during this time.*

### Audio file installation

The firmware comes with a number of sound files which need to be installed separately. These sound files are not updated as often as the firmware itself. If you have previously installed the latest version of the sound-pack, you normally don't have to re-install the audio files when you update the firmware. Only if either a new version of the sound-pack is released, or your clock is quiet after a firmware update, a re-installation is needed.

- Download "install/sound-pack-xxxxxxxx.zip" and extract it to the root directory of of a FAT32 formatted SD card
- power down the clock,
- insert this SD card into the device's slot and 
- power up the clock.
 
If (and only if) the **exact and complete contents of sound-pack archive** is found on the SD card during boot, the keypad will have an "INSTALL AUDIO FILES" menu item, and the Config portal will show an "Audio file installation" option at the bottom of the "Setup" page.

If you choose to use the Config Portal, trigger the audio file installation process by writing "COPY" (without the quotation marks) into the empty text field in the "Audio file installation" section, and clicking on "Save". The device will then copy all the default audio files from the SD card to its internal flash file system. The SD card can - and should - be removed afterwards. The keypad menu method is described [here](#how-to-install-the-default-audio-files).

## Short summary of first steps

- As long as the device is unconfigured or later for some reason fails to connect to a configured WiFi network, it starts in "access point" mode, i.e. it creates a WiFi network of its own named "TCD-AP".
- Using your computer or handheld device, connect to the WiFi network "TCD-AP" and navigate your browser to http://timecircuits.local or http://192.168.4.1 to enter the Config Portal.
- In order to connect the device to your WiFi network, click on "Configure WiFi" and configure your Wifi network. The bare minimum is to select an SSID and a WiFi password. Note that the device requests an IP address via DHCP, unless you entered valid data in the fields for static IP addresses (IP, gateway, netmask, DNS). (If the device is inaccessible as a result of wrong static IPs, hold ENTER when powering it up until the white LED lits; static IP data will be deleted and the device will return to DHCP.)
- After saving the WiFi network settings, the device reboots and tries to connect to your configured WiFi network. If it fails to connect to your network, the device will again start in access point mode. 

NTP (time synchronization over the internet) is only possible if the clock is connected to a WiFi network. If no WiFi network is available, please set your local time through the [keypad menu](#how-to-enter-datestimes-for-the-three-displays) after first power-up.

### The Config Portal

The Config Portal is accessible in two ways: As outlined above, if the device is not connected to a WiFi network, it creates its own WiFi network, to which your WiFi-enabled hand held device or computer can connect ("TCD-AP", http://timecircuits.local or http://192.168.4.1) to enter the Config Portal. If the clock is connected to your WiFi network, direct your browser to http://timecircuits.local. If that fails, hold ENTER on the keypad for 2 seconds, then repeatedly press ENTER until "NET-WORK" is shown, then hold ENTER for 2 seconds. The device will then show its current IP address. Then, on your handheld or computer, navigate to http://a.b.c.d (a.b.c.d being the Ip address as shown on the display) in order to enter the Config Portal.

In the main menu, click on "Setup" to configure your clock, first and foremost your time zone. If the time zone isn't set correctly, the clock might show a wrong time, and DST (daylight saving) will not be switched on/off correctly.

| ![The_Config_Portal](https://user-images.githubusercontent.com/76924199/198831635-8cfef3ca-a06e-4321-b2fd-d23cc1e91c56.jpg) |
|:--:| 
| *The Config Portal's Setup page* |

A full reference of the Config Portal is [here](#appendix-the-config-portal).

## Operation basics

*Present time* is a clock and normally shows the actual local present time, as received from the network or set up through the [keypad menu](#how-to-enter-datestimes-for-the-three-displays).

*Destination time* and *Last time departed* are stale. These, by default, work like in the movie: Upon a time travel, *present time* becomes *last time departed*, and *destination time* becomes *present time*.

There is also a "decorative" mode where the device cycles through a list of pre-programmed *destination* and *last time departed* times. This mode is chosen by setting the "Time-rotation Interval" in the Config Portal (via network) or the [keypad menu](#how-to-select-the-time-rotation-interval) to a non-zero value. The device will then cycle through named list every 5th, 10th, 15th, 30th or 60th minute, and thereby change the displays, regardless of times displayed as a result from a time travel. Set the interval to 0 to disable this "decorative" mode.

The clock only supports the [Gregorian Calendar](https://en.wikipedia.org/wiki/Gregorian_calendar), of which it pretends to have been used since year 1. The [Julian Calendar](https://en.wikipedia.org/wiki/Julian_calendar) is not taken into account. As a result, some years that, in the then-used Julian Calendar, were leap years between years 1 and 1582 in most of today's Europe, 1700 in DK/NO/NL(except Holland and Zeeland), 1752 in the British Empire, 1753 in Sweden, 1760 in Lorraine, 1872 in Japan, 1912 in China, 1915 in Bulgaria, 1917 in the Ottoman Empire, 1918 in Russia and Serbia and 1923 in Greece, are normal years in the Gregorian one. As a result, dates do not match in those two calender systems, the Julian calendar is currently 13 days behind. I wonder if Doc's TC took all this into account. (Then again, he wanted to see Christ's birth on Dec 25, 0. Luckily, he didn't actually try to travel to that date. Assuming a negative roll-over, he might have ended up in eternity.)

Neither the Gregorian nor the Julian Calendar know a "year 0"; 1AD followed after 1BC. Nevertheless, it is possible to travel to year 0. In good old Hollywood tradition, I won't let facts and science stand in the way of an authentic movie experience.

### Keypad reference

In the following, "pressing" means briefly pressing a key, "holding" means keeping the key pressed for 2 seconds or longer.

mm = month (01-12, 2 digits); dd = day (01-31, 2 digits); yyyy = year (4 digits); hh = hour (00-23, 2 digits); MM = minute (00-59, 2 digits)

<table>
    <tr>
     <td align="center" colspan="2">Keypad reference: Destination time programming<br>(&#9166; = ENTER key)</td>
    </tr>
    <tr>
     <td align="center">mmddyyyyhhMM&#9166;</td>
     <td align="center">Set complete date/time for <a href="#time-travel">Time Travel</a></td>
    </tr>
    <tr>
     <td align="center">mmddyyyy&#9166;</td>
     <td align="center">Set date for <a href="#time-travel">Time Travel</a></td>
    </tr>
    <tr>
     <td align="center">hhMM&#9166;</td>
     <td align="center">Set time for <a href="#time-travel">Time Travel</a></td>
    </tr>
</table>

<table>
    <tr>
     <td align="center" colspan="2">Keypad reference: Special key sequences<br>(&#9166; = ENTER key)</td>
    </tr>
    <tr>
     <td align="center">11hhMM&#9166;</td>
     <td align="center">Set <a href="#how-to-set-up-the-alarm">alarm</a> to hh:MM</td>
    </tr>
    <tr>
     <td align="center">111&#9166;</td>
     <td align="center">Toggle <a href="#room-condition-mode-temperaturehumidity-sensor">Room Condition mode</a></td>
    </tr>
    <tr>
     <td align="center">222&#9166;</td>
     <td align="center"><a href="#the-music-player">Music Player</a>: Shuffle off</td>
    </tr>
    <tr>
     <td align="center">555&#9166;</td>
     <td align="center"><a href="#the-music-player">Music Player</a>: Shuffle on</td>
    </tr>
    <tr>
     <td align="center">888&#9166;</td>
     <td align="center"><a href="#the-music-player">Music Player</a>: Goto song 0</td>
    </tr>
    <tr>
     <td align="center">888xxx&#9166;</td>
     <td align="center"><a href="#the-music-player">Music Player</a>: Goto song xxx</td>
    </tr>
    <tr>
     <td align="center">64738&#9166;</td>
     <td align="center">Reboot the device</td>
    </tr>
</table>

<table>
    <tr>
     <td align="center" colspan="3">Keypad reference: Holding keys for 2 seconds<br>(Firmware versions from 2023 and later)</td>
    </tr>
    <tr>
     <td align="center">1<br>Toggle <a href="#how-to-set-up-the-alarm">Alarm</a> on/off</td>
     <td align="center">2<br><a href="#the-music-player">Music Player</a>: Previous song</td>
     <td align="center">3<br><a href="#additional-custom-sounds">Play "key3.mp3"</a></td>
    </tr>
    <tr>
     <td align="center">4<br>Toggle <a href="#night-mode">Night mode</a> on/off</td>
     <td align="center">5<br><a href="#the-music-player">Music Player</a>: Play/Stop</a></td>
     <td align="center">6<br><a href="#additional-custom-sounds">Play "key6.mp3"</a></td>
    </tr>
    <tr>
     <td align="center">7<br><a href="#wifi-power-saving-features">Re-enable WiFi</a></td>
     <td align="center">8<br><a href="#the-music-player">Music Player</a>: Next song</td>
     <td align="center">9<br><a href="#time-travel">Return from Time Travel</a></td>
    </tr>
    <tr>
     <td align="center"></td>
     <td align="center">0<br><a href="#time-travel">Time Travel</a></td>
     <td align="center"></td>
    </tr>
</table>

<table>
    <tr>
     <td align="center" colspan="3">Keypad reference: Holding keys for 2 seconds<br>(Firmware versions from 2022)</td>
    </tr>
    <tr>
     <td align="center">1<br>Enable <a href="#how-to-set-up-the-alarm">Alarm</a></td>
     <td align="center">2<br>Disable <a href="#how-to-set-up-the-alarm">Alarm</a></td>
     <td align="center">3<br><a href="#additional-custom-sounds">Play "key3.mp3"</a></td>
    </tr>
    <tr>
     <td align="center">4<br>Enable <a href="#night-mode">Night mode</a></td>
     <td align="center">5<br>Disable <a href="#night-mode">Night mode</a></td>
     <td align="center">6<br><a href="#additional-custom-sounds">Play "key6.mp3"</a></td>
    </tr>
    <tr>
     <td align="center">7<br><a href="#wifi-power-saving-features">Re-enable WiFi</a></td>
     <td align="center">8<br>n/a</td>
     <td align="center">9<br><a href="#time-travel">Return from Time Travel</a></td>
    </tr>
    <tr>
     <td align="center"></td>
     <td align="center">0<br><a href="#time-travel">Time Travel</a></td>
     <td align="center"></td>
    </tr>
</table>

## Time travel

To travel through time, hold "0" for 2 seconds. The *destination time*, as shown in the red display, will be your new *present time*, the old *present time* will be the *last time departed*. The new *present time* will continue to run like a normal clock.

Before holding "0", you can also first quickly set a new destination time by entering a date on the keypad: mmddyyyy, mmddyyyyhhMM or hhMM, then press ENTER. While typing, there is no visual feedback, but the date is then shown on the *destination time* display after pressing ENTER.

To travel back to actual present time, hold "9" for 2 seconds.

Beware that the alarm function, by default, is based on the real actual present time, not the time displayed. This can be changed in the Config Portal.

## Persistent / Non-persistent time travels

On the Config Portal's "Setup" page, there is an option item named "Make time travels persistent". The default is yes.

If time travels are persistent
- a user-programmed *destination time* is always stored in flash memory, and retrieved from there after a power-loss. It can be programmed through the keypad menu, or ahead of a time travel by typing mmddyyyyhhMM/mmddyyyy/hhMM plus ENTER. In both cases, the time is stored in flash memory and retrieved upon power-on.
- *last time departed* as displayed at any given point is always stored in flash memory, and retrieved upon power-on.
- *present time*, be it actual present time or "fake" after time travelling, will continue to run while the device is not powered, as long as its battery lasts, and displayed on power-up.

If time travels are non-persistent
- a user-programmed *destination time* is only stored to flash memory when programmed through the keypad menu, but not if entered ahead of a time travel (ie outside of the keypad menu, just by typing mmddyyyyhhMM/mmddyyyy/hhMM plus ENTER).
- a user-programmed *last time departed* is only stored to flash memory when programmed through the keypad menu, but not if the result of a time travel.
- *present time* is always reset to actual present time upon power-up.

If you want your device to display exactly the same after a power loss, choose persistent (and set the Time-rotation Interval to 0). 

If you want to display your favorite *destination time* and *last time departed* upon power-up, and not have time travels overwrite them in flash memory, choose "non-persistent", and program your times through the keypad menu (and set the Time-rotation Interval to 0). Those times will never be overwritten in flash memory by later time travels. Note, however, that the times displayed might actually change due to time travels.

Note that a non-zero Time-rotation Interval will force the device to cycle through the list of pre-programmed times, regardless of your time travel persistence setting. This cycling will, however, be paused for 30 minutes after you entered a new destination time and/or travelled in time.

## Night mode

In Night-mode, by default, the *destination time* and *last time departed* displays are switched off, the *present time* display is dimmed to a minimum, and the volume of sound playback is reduced (except the alarm). Apart from considerably increasing the displays' lifetime, night-mode reduces the power consumption of the device from around 4.5W to around 2.5W.

To toggle night-mode on/off manually, hold "4". (For firmware from 2022: To switch on night-mode manually, hold "4". To switch off night-mode, hold "5".)

In the Config Portal, you can configure the displays' behavior in night-mode, as well as "scheduled night mode": You can choose from currently four time schedule presets, or enter daily start and end hour manually.

The presets are for (hopefully) typical home, office and shop setups, and they assume the clock to be in use (ie night-mode off) at the following times:
- Home: Mon-Thu 5pm-11pm, Fri 1pm-1am, Sat 9am-1am, Sun 9am-11pm
- Office (1): Mon-Fri 9am-5pm
- Office (2): Mon-Thu 7am-5pm, Fri 7am-2pm
- Shop: Mon-Wed 8am-8pm, Thu-Fri 8am-9pm, Sat 8am-5pm

The "daily schedule" works by entering start and end in the text fields below. The clock will go into night-mode at the defined start hour (xx:00), and return to normal operation at the end hour (yy:00). 

Finally, you can also connect a light sensor to the device. If the measured lux level is below or equal the configured threshold, the device will go into night-mode. Three sensor types are supported: TSL2561, BH1750, VEML7700/VEML6030, connected through i2c with their respective default slave address. The VEML7700 can only be connected if no GPS receiver is connected at the same time; the 6030 needs its address to be set to 0x48 if a GPS receiver is present at the same time. All these sensor types are readily available from Adafruit or Seeed (Grove). Only one light sensor can be used at the same time.

If both a schedule is enabled and the light sensor option is checked in the Config Portal, the sensor will overrule the schedule only in non-night-mode hours; ie it will never switch off night-mode when night-mode is active according to the schedule.

Switching on/off night-mode manually deactivates any schedule and the light sensor for 30 minutes. Afterwards, a programmed schedule and/or the light sensor will overrule the manual setting.

## SD card

Preface note on SD cards: For unknown reasons, some SD cards simply do not work with this device. For instance, I had no luck with a Sandisk Ultra 32GB card. If your SD card is not recognized, check if it is formatted in FAT32 format (not exFAT!). Also, the size must not exceed 32GB (as larger cards cannot be formatted with FAT32).

The SD card, apart from being used to [install](#audio-file-installation) the default audio files, can be used for substituting default sounds, some additional custom sounds, and for music played back by the [Music player](#the-music-player).

### Sound file substitution

The provided audio files ("sound-pack") are, after [proper installation](#audio-file-installation), integral part of the firmware and stored in the device's flash memory. 

These sounds can be substituted by your own sound files on a FAT32-formatted SD card. These files will be played back directly from the SD card during operation, so the SD card has to remain in the slot. The built-in [Audio file installer](#audio-file-installation) cannot be used to replace default sounds in the device's flash memory with custom sounds.

Your replacements need to be put in the root (top-most) directoy of the SD card, be in mp3 format (128kbps) and named as follows:
- "alarm.mp3". Played when the alarm sounds.
- "alarmon.mp3". Played when enabling the alarm
- "alarmoff.mp3". Played when disabling the alarm
- "nmon.mp3". Played when manually enabling night mode
- "nmoff.mp3". Played when manually disabling night mode

The following sounds are time-sync'd to display action. If you decide to substitute these with your own, be prepared to lose synchronicity:
- "enter.mp3". Played when a date was entered and ENTER was pressed
- "baddate.mp3". Played when a bad (too short or too long) date was entered and ENTER was pressed
- "intro.mp3": Played during the power-up intro
- "travelstart.mp3". Played when a time travel starts.
- "timetravel.mp3". Played when re-entry of a time travel takes place.
- "shutdown.mp3". Played when the device is fake "powered down" using an external switch (see below)
- "startup.mp3". Played when the clock is connected to power and finished booting

If you intend to use the very same SD card that you used for installing the default sound files, please remove the file "TCD_def_snd.txt" from the SD card first.

### Additional Custom Sounds

The firmware supports some additional user-provided sound effects, which must reside on the SD card. If the respective file is present, it will be used. If that file is absent, no sound will be played.

- "hour.mp3": Will be played every hour, on the hour. This feature is disabled in night mode.
- "hour-xx.mp3", xx being 00 through 23: Sounds-on-the-hour for specific hours that will be played instead of "hour.mp3". If a sound for a specific hour is not present, "hour.mp3" will be played, if that one exists.
- "key3.mp3" and/or "key6.mp3": Will be played if you hold the "3"/"6" key for 2 seconds.

"hour.mp3"/"hour-xx.mp3", "key3.mp3" and "key6.mp3" are not provided here. You can use any mp3, with 128kpbs or less.

## The Music Player

Coming soon

## The keypad menu
 
The menu is controlled by "pressing" or "holding" the ENTER key on the keypad.

A "press" is shorter than 2 seconds, a "hold" is 2 seconds or longer.

The menu is involked by holding the ENTER button.

Data entry, such as for dates and times, is done through the keypad's number keys and works as follows: Whenever a data entry is requested, the field for that data is lit (while the rest of the display is dark) and a pre-set value is shown. If you want to keep that pre-set, press ENTER to proceed to next field. Otherwise press a digit on the keypad; the pre-set is then overwritten by the value entered. 2 digits can be entered (4 for years), upon which the new value is stored and the next field is activated. You can also enter less than 2 digits (4 for years) and press ENTER when done with the field. Note that a month needs to be entered numerically (1-12), and hours need to be entered in 24 hour notation (0-23), regardless of 12-hour or 24-hour mode as per the Config Portal setting.

After invoking the keypad menu, the first step is to choose a menu item. The available items are   
- set the alarm ("ALA-RM"),
- set the audio volume (VOL-UME),
- set the Music Player folder number ("MUSIC FOLDER NUMBER")
- select the Time-rotation Interval ("TIME-ROTATION-INTERVAL"),
- select the brightness for the three displays ("BRIGHTNESS"),
- show network information ("NET-WORK"),
- enter dates/times for the three displays (and set the RTC),
- show light/temperature/humidity sensor info (if such a sensor is connected) ("SENSORS"),
- install the default audio files ("INSTALL AUDIO FILES")
- quit the menu ("END").
 
Pressing ENTER cycles through the list, holding ENTER selects an item, ie a mode.

#### How to set up the alarm:

- Hold ENTER to invoke main menu
- (Currently, the alarm is the first menu item; otherwise press ENTER repeatedly until "ALA-RM" is shown)
- Hold ENTER
- Press ENTER to toggle the alarm on and off, hold ENTER to proceed
- Then enter the hour and minutes. This works as described above.
- Choose the weekday(s) by repeatedly pressing ENTER
- Hold ENTER to finalize your weekday selection. "SAVING" is displayed briefly.

Under normal operation (ie outside of the menu), holding "1" toggles the alarm on/off. (For firmware from 2022: Holding "1" enables the alarm, holding "2" disables it.) 

When the alarm is set and enabled, the dot in the present time's minute field will light up. 

The alarm time can also quickly be set by typing 11hhMM (eg. 110645 for 6:45am, or 112300 for 11:00pm) and pressing ENTER, just like when setting a time travel destination time. (The weekday selection has still to be done via the keypad menu.)

Note that the alarm is recurring, ie it rings at the programmed time, unless disabled. Also note, as mentioned, that the alarm is by default relative to your actual *present time*, not the time displayed (eg after a time travel). It can, however, be configured to be based on the time displayed, in the Config Portal.

#### How to set the audio volume:

Basically, and by default, the device uses the hardware volume knob to determine the desired volume. You can change this to a fixed-level setting as follows:
- Hold ENTER to invoke main menu
- Press ENTER repeatedly until "VOL-UME" is shown
- Hold ENTER
- Press ENTER to toggle between "USE VOLUME KNOB" (formerly "HW VOLUME KNOB") and "FIXED LEVEL" (formerly "SW")
- Hold ENTER to proceed
- If you chose "FIXED LEVEL/"SW", you can now select the desired level by pressing ENTER repeatedly. There are 16 levels available. The volume knob is now ignored.
- Hold ENTER to save and quit the menu

#### How to select the music folder number:

- Hold ENTER to invoke main menu
- Press ENTER repeatedly until "MUSIC FOLDER NUMBER" is shown
- Hold ENTER, "NUM" is displayed
- Press ENTER repeatedly to cycle through the possible values.              
- Hold ENTER to select the value shown and exit the menu ("SAVING" is displayed briefly)

If shuffle was enabled before, the new folder is also played in shuffled order.

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

The [Config Portal](#the-config-portal) offers more granularity as it provides 16 levels to choose from.
 
#### How to find out the IP address and WiFi status:

- Hold ENTER to invoke main menu
- Press ENTER repeatedly until "NET-WORK" is shown
- Hold ENTER, the displays show the IP address
- Repeatedly press ENTER to cycle between IP address, WiFi status and MAC address (in station mode)
- Hold ENTER to leave the menu

#### How to enter dates/times for the three displays:

By entering a date/time into the *present time* display, the **RTC (real time clock)** of the device is adjusted, which is useful if you can't use NTP for time keeping, and really helpful when using GPS. Always set the real actual present time here; if you want to display some other time, use the Time Travel function. Note: The time you entered will be overwritten if/when the device has access to network time via NTP or GPS. For DST (daylight saving) and GPS, it is essential that you also set the correct time zone in the [Config Portal](#the-config-portal).

Note that when entering dates/times into the *destination time* or *last time departed* displays, the Time-rotation Interval is automatically set to 0. Your entered date/time(s) are shown until overwritten by time travels (see [here](#how-to-select-the-time-rotation-interval)).

- Hold ENTER to invoke main menu
- Press ENTER repeatedly until the desired display is the only one lit and shows a date and time
- Hold ENTER until the display goes off except for the first field to enter data into
- The field to enter data into is shown (exclusively), pre-set with its current value
- Data entry works as described above; remember that months need to be entered numerically (01-12), and hours in 24-hour notation (0-23).
- After entering data into all fields, the data is saved and the menu is left automatically.

#### How to view sensor info

- Hold ENTER to invoke main menu
- Press ENTER repeatedly until "SENSORS" is shown. If that menu item is missing, a light or temperature sensor was not detected during boot.
- Hold ENTER
- Now the currently measured lux level or temperature is displayed.
- Press ENTER to toggle between light sensor and temperature sensor info (if both are connected)
- Hold ENTER to exit the menu

#### How to install the default audio files:

- Hold ENTER to invoke main menu
- Press ENTER repeatedly until "INSTALL AUDIO FILES" is shown. If this menu does not appear, the SD card isn't configured properly; see the [Audio file installation](#audio-file-installation) section.
- Hold ENTER to proceed
- Press ENTER to toggle between "CANCEL" and "COPY"
- Hold ENTER to proceed. If "COPY" was chosen, the display will guide you through the rest of the process. The menu is quit automatically afterwards.
 
#### How to leave the menu:
 
 - While the menu is active, press ENTER repeatedly until "END" is displayed.
 - Hold ENTER to leave the menu


## Fake "power on/off" Switch; External Time Travel Trigger

The firmware supports a switch connected to IO13 (active low) to act as a fake "power switch". If corresponding option is enabled in the Config Portal ("Use fake power switch"), the device will power-up, initialize everything, but stay quiet and dark. Only when the fake "power switch" is activated (IO13 is connected to GND), the device will visually "power up". You can also fake "power off" the device using this switch. Fake "off" disables the displays, all audio (except the alarm) and the keypad.

The switch needs to be connected to these two solder pads on the TC control board:

![access_to_io13](https://user-images.githubusercontent.com/76924199/194283241-3bee5fef-f51a-4b1a-a158-ed92292bcf32.jpg)

Furthermore, a time travel can be triggered from a button switch or another prop. This switch/prop must be connected to IO27 (active low; before Oct 5, 2022, IO14 was used instead of IO27). The IO and GND pins must be shorted for 200ms to be recognized. The Config Portal allows configuring a delay and the type of sequence played, in order match the device to another prop.

Unfortunately, there is no header and no break out for IO27 on existing TC control boards. Some soldering is required. The button needs to be connected to the two marked pins in the image below:

![nodemcuio27](https://user-images.githubusercontent.com/76924199/194284838-635419f9-5eb7-4480-8693-2bf7cfc7c744.jpg)

Luckily, there is a row of solder pads right next to the socket on the control board, where a pin header or cable can easily be soldered on:

![tcboard_io27](https://user-images.githubusercontent.com/76924199/194284336-2fe9fa9b-d5e5-49f5-b1cd-b0fd2abdff53.jpg)

## Speedometer

The firmware supports a speedometer display connected via i2c (slave address 0x70) as part of the time travel sequence. Unfortunately, CircuitSetup have not yet designed such a speedometer, so you are on your own to build one for the time being. There are, however, various readily available LED segment displays from Adafruit and Seeed (Grove), that can be used as a basis. Adafruit 878, 1270 and 1922, as well as Grove 0.54" 14-segment 2 or 4 digit alphanumeric displays are supported. (The product numbers vary with color, the numbers here are the red ones.)

[![Watch the video](https://img.youtube.com/vi/FRRecGxAONc/0.jpg)](https://youtu.be/FRRecGxAONc)

The speedo display shown in this video is based on a fairly well-designed stand-alone replica purchased on ebay. I removed the Arduino Uno inside and wired the LED segments to an Adafruit i2c backpack (eg ADA 878). The strange look of the speedo's LEDs in the video is due to camera limitations in combination with the layer of semi-clear plastic covering the LED display to make it like the originally used Stanley Readout displays plus the originally used diffusers; in real life it looks much better.

Since the I2C bus is already quite long from the control board to the last display in the chain, I recommend soldering another JST XH 4pin plug onto the control board (there are two additional i2c break-outs available), and to connect the speedometer there.

## GPS receiver

The firmware supports an MT(K)3333-based GPS receiver, connected through i2c (slave address 0x10). The CircuitSetup-designed speedo display will have such a chip built-in, but since this gadget is not yet available, in the meantime, you can use alternatives such as the Adafruit Mini GPS PA1010D (product id 4415) or the Pimoroni P1010D GPS Breakout (PIM525). The GPS receiver can be used as a source of authoritative time (like NTP), and/or speed of movement (to be displayed on a [speedo display](#speedometer)).

GPS receivers receive signals from satellites, but in order to do so, they need to be "tuned in" (aka get a "fix"). This "tuning" process can take a long time; after first power up, it can take 30 minutes or more for a receiver to be able to determine its position. In order to speed up this process, modern GPS receivers have special "assisting" features. One key element is knowledge of current time, as this helps identifying satellite signals quicker. So, in other words, initially, you need to tell the receiver, what it is supposed to tell you. However, as soon as the receiver has received satellite signals for 15-20 minutes, it saves the data it collected to its battery-backed memory and will find a fix within seconds after power-up in the future.

For using GPS effectively as a long term source of accurate time, it is therefore essential, that 
- the TimeCircuit's RTC (real time clock) is initially [set to correct local time](#how-to-enter-datestimes-for-the-three-displays), 
- the correct time zone is defined in the Config Portal,
- the GPS receiver has a battery
- and has been receiving data for 15-20 mins at least once a month.

If/as long as the GPS receiver has a fix and receives data from satellites, the dot in the present time's year field is lit.

#### GPS for speed

One nice feature of GPS is that the receiver can deliver current speed of movement. If the Time Circuits are, for instance, mounted in a car or on a boat, and a [speedo display](#speedometer) is connected, this display will be just that: A real speedometer.

## Room Condition Mode, Temperature/humidity sensor

The firmware supports connecting a temperature/humidity sensor for "room condition mode"; in this mode, *destination* and *last departed* times are replaced by temperature and humidity, respectively. To toggle between normal and room condition mode, enter "111" and press ENTER.

![rcmode](https://user-images.githubusercontent.com/76924199/208133653-f0fb0a38-51e4-4436-9506-d841ef1bfa6c.jpg)

Furthermore, unless you do time travelling on a regular basis, the [speedo display](#speedometer) is idle most of the time in a typical home setup. To give it more of a purpose, the firmware can display ambient temperature on the speedo while idle.

Seven sensor types are supported: MCP9808 (i2c address 0x18), BMx820 (0x77), SI7021 (0x40), SHT40 (0x44), TMP117 (0x49), AHT20/AM2315C (0x38), HTU31D (0x41). All of those are readily available from Adafruit or Seeed (Grove). Note that BMP820 (unlike BME820), MCP9808 and TMP117 are pure temperature sensors, the others deliver temperature and humidity.

## WiFi power saving features

The Config Portal offers two options for WiFi power saving, one for AP-mode (ie when the device acts as an access point), one for station mode (ie when the device is connected to a WiFi network). Both options do the same: They configure a timer after whose expiry WiFi is switched off; the device is no longer transmitting or receiving data over WiFi. 

The timers can be set to 0 (which disables them; WiFi is never switched off; this is the default), or 10-99 minutes. 

The reason for having two different timers for AP-mode and for station mode is that if the device is used in a car, it might act as an access point, while at home it is most probably connected to a WiFi network as a client. Since in a car, WiFi will most likely not be used on a regular basis, the timer for AP mode can be short (eg 10 minutes), while the timer for station mode can be disabled.

After WiFi has been switched off due to timer expiry, it can be re-enabled by holding "7" on the keypad for approx. 2 seconds, in which case the timers are restarted (ie WiFi is again switched off after timer expiry).

## Controlling other props

The device can tell other props about a time travel, and in essence act as a "master controller" for several props. It does so via IO14, see diagram below.

```
|<---------- speedo accleration ---------->|                         |<-speedo de-acceleration->|
0....10....20....................xx....87..88------------------------88...87....................0
                                           |<--Actual Time Travel -->|
                                           |  (Display disruption)   |
                                      TT starts                      Reentry phase
                                           |                         |
             |<---------ETTO lead--------->|                         |
             |                                                       |
             |                                                       |
             |                                                       |
     IO14: LOW->HIGH                                           IO14: HIGH->LOW
 ```

"ETTO lead", ie the lead time between IO14 going high and the actual start of a time travel is defined as 5000ms (ETTO_LEAD_TIME). In this window of time, the prop can play its pre-time-travel (warm-up/acceleration/etc) sequence. The sequence inside the time "tunnel" follows after that lead time, and when IO14 goes LOW, the re-entry into the destination time takes place.

## My custom-made Wall Clock

![204099461-6499d8a4-4c8f-446c-b0a8-7ef13b558c67](https://user-images.githubusercontent.com/76924199/206280615-e466c2ab-8670-4ff2-a055-6dfec356a966.jpg)

More here: [Link](https://github.com/realA10001986/Time-Circuits-Display/wiki/Time-Circuits-Wall-Clock)

## Appendix: The Config Portal

### Basic settings

#### Make time travels persistent

See [here](#persistent--non-persistent-time-travels)

#### Alarm base is real present time

Selects whether the alarm, sound-on-the-hour and night-mode schedules are based on real, actual present time, or "present" time as displayed (eg after a time travel).

#### Play intro

Selects whether the animated "Back to the Future" intro should be played upon power-up.

#### 24-hour clock mode

Selects 24-hour clock mode, ie hours go from 0 to 23, "AM" and "PM" are permanently dark.

#### Time-rotation interval

In "decorative" mode the device cycles through a list of pre-programmed, movie-accurate *destination* and *last time departed* times. This mode is enabled by setting the "Time-rotation Interval" to anything but "off". The device will cycle through named list every 5th, 10th, 15th, 30th or 60th minute, and thereby change the displays. Set the interval to "off" to disable "decorative" mode. See [here](#operation-basics)

### Network & time synchronization settings

#### Hostname

The device's hostname in the WiFi network. Defaults to 'timecircuits'. This also is the domain name at which the Config Portal is accessible from a browser in the same local network. The URL of the Config Portal then is http://<i>hostname</i>.local (the default is http://timecircuits.local)

If you have more than one clock in your local network, please give them unique hostnames.

#### WiFi connection attempts

Number of times the firmware tries to reconnect to a WiFi network, before falling back to AP-mode. See [here](#short-summary-of-first-steps)

#### WiFi connection timeout

Number of seconds before a timeout occurs when connecting to a WiFi network. When a timeout happens, another attempt is made (see immediately above), and if all attempts fail, the device falls back to AP-mode. See [here](#short-summary-of-first-steps)

#### WiFi power save timer

See [here](#wifi-power-saving-features)

#### WiFi power save timer (AP-mode)

See [here](#wifi-power-saving-features)

#### Timezone

The time zone of the place where the device is operated. Has to be in POSIX format. Needs to be set in order to use NTP or GPS, and for DST (daylight saving). Defaults to UTC0.

#### NTP Server

Name of your preferred NTP (network time protocol) server for time synchronization. Leave this empty to disable NTP.

#### Use GPS as time source

If a [GPS receiver](#gps-receiver) is connected, this selects whether GPS time is used for time synchronization.

### Display preferences

#### Destination Time display brightness

Selects the brightness of the *destination time* display. Ranges from 0 (darkest) to 15 (brightest).

#### Present Time display brightness (0-15)

Selects the brightness of the *present time* display. Ranges from 0 (darkest) to 15 (brightest).

#### Last Time Dep. display brightness

Selects the brightness of the *last time departed* display. Ranges from 0 (darkest) to 15 (brightest).

### Night-mode

#### Destination time off in night mode

Selects whether the *destination time* display is dimmed or switched of in night mode.

#### Present time off in night mode

Selects whether the *present time* display is dimmed or switched of in night mode.

#### Last time dep. off in night mode

Selects whether the *last time departed* display is dimmed or switched of in night mode.

#### Scheduled night-mode

Enables or disables schedules night mode, as configured by the following options. See [here](#night-mode)

#### Schedule

Selects a schedule for night-mode. See [here](#night-mode)

#### Daily night-mode start hour (0-23)

If "Daily" is chosen in the "Schedule"-drop-down, enter night-mode start hour here.

#### Daily night-mode end hour (0-23)

If "Daily" is chosen in the "Schedule"-drop-down, enter night-mode end hour here.

#### Use light sensor

Select whether to use a light sensor's data to enable night-mode. See [here](#night-mode)

#### Light (lux) threshold

If the light sensor reports a number of lux below or equal to this value, night-mode is activated. See [here](#night-mode)

### Temperature/humidity sensor settings

#### Use temperature sensor

Select whether to use a temperature/humidity sensor. The sensor data is used for room condition mode, and the temperature is optionally displayed on a connected Speedo display. See [here](#room-condition-mode-temperaturehumidity-sensor)

#### Display in °Celsius

Selects between Fahrenheit and Celsius for temperature display.

#### Temperature offset

This offset, which can range from -3.0 to 3.0, is added to the sensor measurement, in order to compensate sensor inaccuracy or suboptimal sensor placement.

### Speedometer settings

#### Use speedometer display

Check this if you have an external speedo display connected and want to use it. See [here]()

#### Display type

Type of speedo display. Several types are supported, mostly Adafruit or Seeed/Grove LED segement displays. See [here](#speedometer)

#### Speedo brightness

Brightness of speedo display when displaying speed.

#### Speedo sequence speed factor

Since the DMC-12 wasn't the world's fastest car, it's acceleration figures might soon cause boredom if played in real-time as part of the time travel sequence with a speedo display. This factor speeds up the acceleration. 2.0 means twice as fast as the real car.

#### Display GPS speed

If a GPS receiver is connected, this selects whether to display GPS speed on the speedo display. This only makes sense if the Time Circuits are mounted in a car. See [here](#gps-receiver)

#### Display temperature

Selects whether temperature as received from a suitable sensor is displayed on the speedo display, when it is idle (ie no time travel is in progress).

#### Temperature brightness

Brightness of speedo display when displaying temperature.

### External switches/buttons

#### Use fake power switch

Check this if you want to use a fake power switch. See [here](#fake-power-onoff-switch-external-time-travel-trigger)

#### External time travel button: Delay

Selects a delay (in millisconds) from when pressing the external time travel button until the time travel sequence starts. See [here](#fake-power-onoff-switch-external-time-travel-trigger)

#### Play complete time travel sequence

For externally triggered time travels, this selects if the entire sequence (including acceleration, if a speedo is available) should be played, or the only the short re-entry sequence. See [here](#fake-power-onoff-switch-external-time-travel-trigger)

### Settings for other peripherals

#### Use compatible external props

This selects whether a GPIO pin is activated upon a time-travel in order to control external props. See [here](#controlling-other-props)

#### Play time travel sounds

If other props are connected, they might bring their own time travel sound effects. In this case, you can uncheck this to disable the Time Circuit's own time travel sounds. Note that this only covers sounds played during time travel, not for other sound effects.

### Music Player settings

#### Music folder number

Selects the folder number for the Music Player. The folders are named "music0" through "music9"; this config option selects the digit in the folder name.

#### Shuffle at startup

When checked, songs are shuffled when the device is booted. When unchecked, songs will be played in order.

### Other settings

#### 4MHz SD clock speed

By default, the clock speed for SD card access is set to 16MHz. If you experience problems with your SD card, such as sound cut-offs or the card being inaccessible, try setting the clock speed to 4MHz by checking this option. (General note: Not all SD cards work with the device. If the SD card is inaccessible with both clock speed settings, try another SD card brand. For instance, I had no luck with a Sandisk Ultra 32GB.)
