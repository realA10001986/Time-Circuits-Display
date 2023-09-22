# Firmware for Time Circuits Display

This repository holds the most current firmware for CircuitSetup's wonderful [Time Circuits Display](https://circuitsetup.us/product/complete-time-circuits-display-kit/) ("TCD").

The hardware is available [here](https://circuitsetup.us/product/complete-time-circuits-display-kit/).

| ![TCD Front](https://user-images.githubusercontent.com/76924199/200327688-cfa7b1c2-abbd-464d-be6d-5d295e51056e.jpg) |
|:--:|
| *This very one is now in the Universal Studios [BTTF Escape Room](https://www.universalorlando.com/web/en/us/things-to-do/entertainment/universals-great-movie-escape) (Orlando)* |

Features include
- Time keeping for years 1-9999, based on Gregorian Calendar
- Support for time zones and automatic DST (Daylight Saving adjustment)
- Time synchronization via NTP or [GPS](#gps-receiver)
- Movie-accurate [time travel](#time-travel) function with *Present time* always running as a clock (as opposed to being stale)
- ["Return from Time Travel"](#time-travel): Return to real present time easily
- ["Decorative mode"](#time-cycling): Movie-accurate time travel times displayed in *destination time* and *last time departed* displays, and cycled in configurable interval
- [World Clock mode](#world-clock-mode): Show current time in different time zones in *destination time* and/or *last time departed* displays
- [Alarm function](#how-to-set-up-the-alarm): Daily alarms, with weekday selection
- [Count-down timer](#count-down-timer): Count down from up to 99 minutes
- [Yearly/monthly reminder](#yearlymonthly-reminder): Get a yearly or monthly acoustic reminder
- [Night mode](#night-mode): Dim or switch off displays on schedule, manually or sensor-controlled.
- [Music player](#the-music-player): Play mp3 files located on an SD card
- [SD card](#sd-card) support for custom audio files for effects, and music for the Music Player
- Advanced network-accessible [Config Portal](#the-config-portal) for setup with mDNS support for easy access (http://timecircuits.local, hostname configurable)
- [Keypad-controlled menu](#the-keypad-menu) for adjusting various settings and viewing network status via the three displays
- 24-hour clock mode
- [Room condition mode](#room-condition-mode-temperaturehumidity-sensor): Display temperature and humidity instead of destination/departed time (requires suitable sensor)
- Power-up intro
- [Sound on the hour](#additional-custom-sounds)
- [Beep](#beep-on-the-second) every second (4 modes)
- Support for external [fake "power switch"](#fake-power-switch)
- Support for time travel [triggered by external source](#external-time-travel-trigger), with selectable delay
- Support for external display acting as [Speedometer](#speedometer)
- Support for [GPS receiver](#gps-receiver), used as time source, and for [speed](#gps-for-speed) to be displayed on speedo display
- Support for [temperature/humidity sensor](#room-condition-mode-temperaturehumidity-sensor) for "room condition mode" and to display temperature on speedo display while idle
- Support for wireless communication ("[BTTF-Network](#bttf-network-bttfn)") with compatible other props such as CircuitSetup's [SID](https://github.com/realA10001986/SID) and [Flux Capacitor](https://github.com/realA10001986/Flux-Capacitor) 
- [Home Assistant](#home-assistant--mqtt) (MQTT 3.1.1) support
- Support for triggering time travels on [other props](#controlling-other-props) connected by wire; TCD can act as master controller for [SID](https://github.com/realA10001986/SID), [Flux Capacitor](https://github.com/realA10001986/Flux-Capacitor), etc.
- Built-in installer for default audio files in addition to OTA firmware updates

Since Sep 2022, CircuitSetup are distributing this very firmware as their official one (starting with 2.0). The difference between what you get here and CircuitSetup's releases is that the code here might be ahead in development, uses some different sounds, and the pre-compiled binary has all the features enabled.

## Installation

There are different alternative ways to install this firmware:

1) If a previous version of the TCD firmware was installed on your device, you can upload the provided pre-compiled binary to update to the current version: Enter the [Config Portal](#the-config-portal), click on "Update" and select the pre-compiled binary file provided in this repository ("install/timecircuits-A10001986.ino.nodemcu-32s.bin"). *Note: If the previously installed firmware was from CircuitSetup (ie. pre-installed or downloaded from their github), you need to re-install the sound-files using the sound-pack from this very repo.*

2) Using the Arduino IDE or PlatformIO: Download the sketch source code, all required libraries, compile and upload it. This method is the one for fresh ESP32 boards and/or folks familiar with the programming tool chain. Detailed build information is in [timecircuits-A10001986.ino](https://github.com/realA10001986/Time-Circuits-Display/blob/main/timecircuits-A10001986/timecircuits-A10001986.ino).

 *Important: After a firmware update, the displays might stay dark or display "WAIT" for up to a minute after reboot. Do NOT unplug the TCD during this time.*

### Audio file installation

The firmware comes with a number of sound files which need to be installed separately. These sound files are not updated as often as the firmware itself. If you have previously installed the latest version of the sound-pack, you normally don't have to re-install the audio files when you update the firmware. Only if either a new version of the sound-pack is released, or your TCD is quiet after a firmware update (what will the case if you see "WAIT" during boot), a re-installation is needed.

*If your TCD previously ran a firmware from CircuitSetup (ie. pre-installed or downloaded from their github), a re-installation of the audio files **from this repository** is required. You cannot use CircuitSetup's audio files with the firmware available here, or vice versa.*

- Download "install/sound-pack-xxxxxxxx.zip" and extract it to the root directory of of a FAT32 formatted SD card
- power down the TCD,
- insert this SD card into the device's slot and 
- power up the TCD.
 
If (and only if) the **exact and complete contents of sound-pack archive** is found on the SD card, the TCD will show "INSTALL AUDIO FILES?" after power-up. Press ENTER briefly to toggle between "CANCEL" and "PROCEED". Choose "PROCEED" and hold the ENTER key for 2 seconds.

If you, for whatever reason, chose "CANCEL", you can still invoke the installer through the [keypad menu](#how-to-install-the-default-audio-files), or by rebooting the TCD.

After installation, the SD card can be re-used for [other purposes](#sd-card).

## Short summary of first steps

The first step is to establish access to the Config Portal in order to configure your TCD.

As long as the device is unconfigured, as is the case with a brand new TCD, or later if it for some reason fails to connect to a configured WiFi network, it starts in "access point" mode, i.e. it creates a WiFi network of its own named "TCD-AP". This is called "Access point mode", or "AP-mode".

- Power up the TCD and wait until it shows a time (which is probably wrong).
- Connect your computer or handheld device to the WiFi network "TCD-AP".
- Navigate your browser to http://timecircuits.local or http://192.168.4.1 to enter the Config Portal.
 
If you want your TCD to connect to your WiFi network, click on "Configure WiFi". The bare minimum is to select an SSID (WiFi network name) and a WiFi password. 

Note that the TCD requests an IP address via DHCP, unless you entered valid data in the fields for static IP addresses (IP, gateway, netmask, DNS). If the device is inaccessible as a result of incorrect static IPs, hold ENTER when powering it up until the white LED lits; static IP data will be deleted and the device will return to DHCP. 

After saving the WiFi network settings, the TCD reboots and tries to connect to your configured WiFi network. If that fails, it will again start in access point mode.

The next step is to set the TCD's time zone and actual time.

- If your TCD is connected to a WiFi network with internet access, it will receive time information through NTP (network time protocol). If the TCD shows a wrong time initially, don't worry: This is due to a wrong time zone.
- If the internet is inaccessible (such as is the case if the TCD is in access point mode), please set your local time through the [keypad menu](#how-to-set-the-real-time-clock-rtc) after setting up the time zone.

In both cases it is, again, important to set the TCD's time zone. This is done in the Config Portal, so read on.

### The Config Portal

The Config Portal is accessible exclusively through WiFi. As outlined above, if the device is not connected to a WiFi network, it creates its own WiFi network (named "TCD-AP"), to which your WiFi-enabled hand held device or computer first needs to connect in order to access the Config Portal.

If the operating system on your handheld or computer supports Bonjour (or "mDNS"), you can enter the Config Portal by directing your browser to http://timecircuits.local. (mDNS is supported on Windows 10 version TH2 (1511) [other sources say 1703] and later, Android 13 and later, MacOS, iOS)

If that fails, the way to enter the Config Portal depends on whether the TCD is in access point mode or not. 
- If it is in access point mode (and your handheld/computer is connected to the WiFi network "TCP-AP"), navigate your browser to http://192.168.4.1 
- If the device is connected to your WiFi network, you need to find out its IP address first: Hold ENTER on the keypad for 2 seconds, then repeatedly press ENTER until "NET-WORK" is shown, then hold ENTER for 2 seconds. The device will then show its current IP address. Then, on your handheld or computer, navigate to http://a.b.c.d (a.b.c.d being the IP address as shown on the display) in order to enter the Config Portal.

In the main menu, click on "Setup" to configure your TCD, first and foremost your time zone. If the time zone isn't set correctly, the TCD might show a wrong time, and DST (daylight saving) will not be switched on/off correctly.

| ![The_Config_Portal](https://github.com/realA10001986/Time-Circuits-Display/assets/76924199/95888ad5-5e3e-4f62-8a6c-e945de1bd281) |
|:--:| 
| *The Config Portal's Setup page* |

A full reference of the Config Portal is [here](#appendix-a-the-config-portal).

## Basic Operation

*Present time* is a clock and normally shows the actual local present time, as received from the network or set up through the [keypad menu](#how-to-set-the-real-time-clock-rtc).

*Destination time* and *Last time departed* are stale. These, by default, work like in the movie: Upon a time travel, *present time* becomes *last time departed*, and *destination time* becomes *present time*.

The TCD only supports the [Gregorian Calendar](https://en.wikipedia.org/wiki/Gregorian_calendar), which it pretends to have been used since year 1. The [Julian Calendar](https://en.wikipedia.org/wiki/Julian_calendar) is not taken into account. Therefore, some years that, in the then-used Julian Calendar, were leap years between years 1 and 1582 most of today's Europe (and its Spanish colonies in the Americas), 1700 in DK/NO/NL (except Holland and Zeeland), 1752 in the British Empire (and therefore today's USA, Canada and India), 1753 in Sweden, 1760 in Lorraine, 1872 in Japan, 1912 in China, 1915 in Bulgaria, 1917 in the Ottoman Empire, 1918 in Russia and Serbia and 1923 in Greece, are normal years in the Gregorian one. As a result, dates do not match in those two calender systems, the Julian calendar is currently 13 days behind. I wonder if Doc's TC took all this into account. (Then again, he wanted to see Christ's birth on Dec 25, 0. Luckily, he didn't actually try to travel to that date. Assuming a negative roll-over, he might have ended up in eternity.)

Neither the Gregorian nor the Julian Calendar know a "year 0"; 1AD followed after 1BC. Nevertheless, it is possible to travel to year 0. In good old Hollywood tradition, I won't let facts and science stand in the way of an authentic movie experience.

If "REPLACE BATTERY" is shown upon boot, the onboard CR2032 battery is depleted and needs to be replaced. Note that, for technical reasons, "REPLACE BATTERY" will also show up the very first time you power-up the TCD *after* changing the battery. You can, of course, disregard that message in this case.

### Time-cycling

"Time cycling" is a kind of decorative mode in which the device cycles through a list of pre-programmed *destination* and *last time departed* times. These pre-programmed times match the dates/times of all time-travels that take place in the three movies.

Time-cycling is enabled by setting the **_Time-cycling Interval_** in the Config Portal or the [keypad menu](#how-to-select-the-time-cycling-interval) to a non-zero value. The device will then cycle through named list every 5th, 10th, 15th, 30th or 60th minute. 

Time-cycling will, if enabled, change the *Destination* and *Last Time Departed* displays regardless of the times already displayed, for instance as a result from an earlier time travel. Triggering a time-travel will, however, pause time-cycling for 30 minutes.

Set the interval to OFF (0) to disable Time-cycling.

### World Clock mode

World Clock (WC) mode is another kind of decorative mode where the red and yellow displays show not some stale times, but current time in other time zones. These time zones need to be configured in Config Portal. At least one time zone (for either the red or yellow display) must be configured in order to use WC mode. Optionally, also names for cities/locations for these time zones can be entered in the Config Portal; if a name for a time zone is configured, this name and the time will alternately be shown; if no name is configured, time will be shown permanently. Note that names can only contain letters a-z, numbers 0-9, space and minus.

WC mode is toggled by typing "112" followed by ENTER. 

For logical reasons, WC mode will be automatically disabled in some situations:

- Time travel. The time travel sequence runs like in non-WC mode: If a time travel is triggered while WC mode is enabled (and no new destination time was entered before), the currently shown *Destination Time* will be the travel target, and the *Last Time Departed* display will show your formerly current time. However: Both *Destination Time* as well as *Last Time Departed* become stale after the time travel as per the nature of the sequence.
- After entering a destination time. WC mode is disabled at this point, because your new *Destination Time* would be overwritten otherwise.

#### WC/RC hybrid mode

[Room Condition (RC) mode](#room-condition-mode-temperaturehumidity-sensor) can be enabled together with WC mode. In that case, only one timezone is used, and the other display shows the temperature. If there is a time zone configured for the red display, the temperature will be shown in the yellow display. If there no time zone for the red display, the temperature will be shown there, and the yellow display will show time for the time zone you set up for the yellow display.

To toggle WC/RC hybrid mode, type "113" followed by ENTER.

### Common usage scenarios

####  &#9654; I want my TCD to work like in the movie

In this case, head to the Config Portal and
- set the **_Time Cycling Interval_** to OFF
- check or uncheck **_Make time travels persistent_** depending on whether you care about keeping your times across reboots

Note that *actual* time travel is not supported.

#### 	&#9654; I want my TCD to show/cycle movie times

In this case, head to the Config Portal and
- set the **_Time Cycling Interval_** to the desired interval
- uncheck **_Make time travels persistent_**

Time-travelling will interrupt the cycling of movie times for 30 minutes.

#### 	&#9654; I want my TCD to always show my favorite *Destination* and *last time departed* times

In this case, head to the Config Portal and
- set the **_Time Cycling Interval_** to OFF
- uncheck **_Make time travels persistent_**

Then enter the [keypad menu](#how-to-enter-datestimes-for-the-destination-and-last-time-departed-displays) and set your favorite *Destination* and *Last time departed* times.

Note that time-travelling will naturally lead to the displays showing other times. After a reboot, your times will be displayed again.

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
     <td align="left">Disable <a href="#beep-on-the-second">beep</a> sound</td>
     <td align="left">000&#9166;</td>
    </tr>
    <tr>
     <td align="left">Enable <a href="#beep-on-the-second">beep</a> sound</td>
     <td align="left">001&#9166;</td>
    </tr>
   <tr>
     <td align="left">Enable <a href="#beep-on-the-second">beep</a> sound (30 seconds)</td>
     <td align="left">002&#9166;</td>
    </tr>
   <tr>
     <td align="left">Enable <a href="#beep-on-the-second">beep</a> sound (60 seconds)</td>
     <td align="left">003&#9166;</td>
    </tr>
   <tr>
     <td align="left">Toggle <a href="#room-condition-mode-temperaturehumidity-sensor">Room Condition mode</a></td>
     <td align="left">111&#9166;</td>
    </tr>
    <tr>
     <td align="left">Toggle <a href="#">World Clock mode</a></td>
     <td align="left">112&#9166;</td>
    </tr>
    <tr>
     <td align="left">Synchronously toggle <a href="#">World Clock mode</a> and <a href="#room-condition-mode-temperaturehumidity-sensor">Room Condition mode</a></td>
     <td align="left">113&#9166;</td>
    </tr>
    <tr>
     <td align="left">Show current <a href="#how-to-set-up-the-alarm">alarm</a> time/weekday</td>
     <td align="left">11&#9166;</td>
    </tr>
    <tr>
     <td align="left">Set <a href="#how-to-set-up-the-alarm">alarm</a> to hh:MM</td>
     <td align="left">11hhMM&#9166;</td>
    </tr>
    <tr>
     <td align="left"><a href="#count-down-timer">Timer</a>: Show remaining time</td>
     <td align="left">44&#9166;</td>
    </tr>
    <tr>
     <td align="left"><a href="#count-down-timer">Timer</a>: Set timer to MM minutes</td>
     <td align="left">44MM&#9166;</td>
    </tr>
   <tr>
     <td align="left"><a href="#count-down-timer">Timer</a>: Delete timer</td>
     <td align="left">440</td>
    </tr>
    <tr>
     <td align="left"><a href="#yearlymonthly-reminder">Reminder</a>: Display reminder</td>
     <td align="left">77&#9166;</td>
    </tr>
    <tr>
     <td align="left"><a href="#yearlymonthly-reminder">Reminder</a>: Display time until reminder</td>
     <td align="left">777&#9166;</td>
    </tr>
    <tr>
     <td align="left"><a href="#yearlymonthly-reminder">Reminder</a>: Program reminder</td>
     <td align="left">77mmdd&#9166;</td>
    </tr>
    <tr>
     <td align="left"><a href="#yearlymonthly-reminder">Reminder</a>: Program reminder</td>
     <td align="left">77mmddhhMM&#9166;</td>
    </tr>
    <tr>
     <td align="left"><a href="#yearlymonthly-reminder">Reminder</a>: Delete reminder</td>
     <td align="left">770&#9166;</td>
    </tr>
    <tr>
     <td align="left"><a href="#the-music-player">Music Player</a>: Show currently played song number</td>
     <td align="left">55&#9166;</td>
    </tr>
    <tr>
     <td align="left"><a href="#the-music-player">Music Player</a>: Shuffle off</td>
     <td align="left">222&#9166;</td>
    </tr>
    <tr>
     <td align="left"><a href="#the-music-player">Music Player</a>: Shuffle on</td>
     <td align="left">555&#9166;</td>
    </tr> 
    <tr>
     <td align="left"><a href="#the-music-player">Music Player</a>: Go to song 0</td>
     <td align="left">888&#9166;</td>
    </tr>
    <tr>
     <td align="left"><a href="#the-music-player">Music Player</a>: Go to song xxx</td>
     <td align="left">888xxx&#9166;</td>
    </tr>
   <tr>
     <td align="left">Disable / enable <a href="#car-mode"><i>car mode</i></a></td>
     <td align="left">990&#9166; / 991&#9166;</td>
    </tr>
    <tr>
     <td align="left">Reboot the device</td>
     <td align="left">64738&#9166;</td>
    </tr>
</table>

<table>
    <tr>
     <td align="center" colspan="3">Keypad reference: Holding keys for 2 seconds</td>
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

[Here](https://github.com/realA10001986/Time-Circuits-Display/blob/main/CheatSheet.pdf) is a cheat sheet for printing or screen-use. (Note that MacOS' *preview* application has a bug that scrambles the links in the document. Acrobat Reader does it correctly.)

## Time travel

To travel through time, hold "0" for 2 seconds (or use an [external trigger](#external-time-travel-trigger)). The *destination time*, as shown in the red display, will be your new *present time*, the old *present time* will be the *last time departed*. The new *present time* will continue to run like a normal clock.

Before triggering the time travel, you can also first quickly set a new destination time by entering a date on the keypad: mmddyyyy, mmddyyyyhhMM or hhMM, then press ENTER. While typing, there is no visual feedback, but the date is then shown on the *destination time* display after pressing ENTER.

To travel back to actual present time, hold "9" for 2 seconds.

Beware that the alarm function, by default, is based on the real actual present time, not the time displayed. This can be changed in the Config Portal.

### Persistent / Non-persistent time travels

On the Config Portal's "Setup" page, there is an option item named **_Make time travels persistent_**. The default is off.

If time travels are persistent
- a user-programmed *destination time* is always stored in flash memory, and retrieved from there after a reboot. It can be programmed through the keypad menu, or ahead of a time travel by typing mmddyyyyhhMM/mmddyyyy/hhMM plus ENTER. In both cases, the time is stored in flash memory and retrieved upon power-up/reboot.
- *last time departed* as displayed at any given point is always stored in flash memory, and retrieved upon power-up/reboot.
- *present time*, be it actual present time or "fake" after time travelling, will continue to run while the device is not powered, as long as its battery lasts, and displayed on power-up/reboot.

If time travels are non-persistent
- a user-programmed *destination time* is only stored to flash memory when programmed through the keypad menu, but not if entered ahead of a time travel (ie outside of the keypad menu, just by typing mmddyyyyhhMM/mmddyyyy/hhMM plus ENTER).
- a user-programmed *last time departed* is only stored to flash memory when programmed through the keypad menu, but not if the result of a time travel.
- *present time* is always reset to actual present time upon power-up/reboot.

If you want your device to display exactly the same after a power loss, choose persistent (and disable [Time-cycling](#time-cycling)).

If you want to display your favorite *destination time* and *last time departed* upon power-up, and not have time travels overwrite them in flash memory, choose "non-persistent", and program your times through the [keypad menu](#how-to-enter-datestimes-for-the-destination-and-last-time-departed-displays) (and disable [Time-cycling](#time-cycling)). Those times will never be overwritten in flash memory by later time travels. Note, however, that the times displayed might actually change due to time travels.

Note that Time-cycling, if enabled, will force the device to cycle through the list of pre-programmed times, regardless of your time travel persistence setting. So, if Time-cycling is enabled, the only effect of persistence is that *Present Time* is kept at what it was before vs. reset to actual present time after a power loss.

Persistent time travels, if done often, will cause [Flash Wear](#flash-wear).

## Beep on the second

In the movies, the Time Circuits emit a "beep" sound every second, which is only really audible in the scene in which Doc explains to Marty how the time machine works. The firmware supports that beep, too.

The beep can be permanently disabled, permanently enabled, or enabled for 30 or 60 seconds
- after a destination time is entered (and ENTER is pressed),
- upon triggering a time travel,
- after switching on the TCD (real power-up or fake power-up).

The different modes are selected by typing 000 (disabled), 001 (enabled), 002 (enabled for 30 secs) or 003 (enabled for 60 secs), followed by ENTER. The power-up default is selected in the Config Portal.

Since the hardware only has one audio channel, the beep is suppressed whenever other sounds are played-back.

## Night mode

In night-mode, by default, the *destination time* and *last time departed* displays are switched off, the *present time* display is dimmed to a minimum, and the volume of sound playback is reduced (except the alarm). Apart from considerably increasing the displays' lifetime, night-mode reduces the power consumption of the device from around 4.5W to around 2.5W.

You can configure the displays' behavior in night-mode in the Config Portal: They can individually be dimmed or switched off in night-mode.

#### Manually switching to night-mode

To toggle night-mode on/off manually, hold "4".

#### Scheduled night-mode

In the Config Portal, a schedule for night-mode can be programmed. You can choose from currently four time schedule presets, or a daily schedule with selectable start and end hours.

The presets are for (hopefully) typical home, office and shop setups, and they assume the TCD to be in use (ie night-mode off) at the following times:
- Home: Mon-Thu 5pm-11pm, Fri 1pm-1am, Sat 9am-1am, Sun 9am-11pm
- Office (1): Mon-Fri 9am-5pm
- Office (2): Mon-Thu 7am-5pm, Fri 7am-2pm
- Shop: Mon-Wed 8am-8pm, Thu-Fri 8am-9pm, Sat 8am-5pm

The *daily* schedule works by entering start and end in the text fields below. The TCD will go into night-mode at the defined start hour (xx:00), and return to normal operation at the end hour (yy:00). 

Night mode schedules are always based on actual local present time.

#### Sensor-controlled night-mode

You can also connect a light sensor to the device. Four sensor types/models are supported: TSL2561, BH1750, VEML7700/VEML6030, LTR303/LTR329, connected through i2c with their respective default slave address. The VEML7700 can only be connected if no GPS receiver is connected at the same time; the VEML6030 needs its address to be set to  0x48 if a GPS receiver is present at the same time. All these sensor types are readily available on breakout boards from Adafruit or Seeed (Grove). Only one light sensor can be used at the same time. *Note: You cannot connect the sensor chip directly to the TCD control board; most sensors need at least a power converter/level-shifter.* This is why I exclusively used Adafruit or Seeed breakouts ([TSL2561](https://www.adafruit.com/product/439) or [here](https://www.seeedstudio.com/Grove-Digital-Light-Sensor-TSL2561.html), [BH1750](https://www.adafruit.com/product/4681), [VEML7700](https://www.adafruit.com/product/4162), [LTR303](https://www.adafruit.com/product/5610)), which all allow connecting named sensors to the 5V the TCD board operates on. For wiring information, see [here](#appendix-b-sensor-wiring).

If the measured lux level is below or equal the threshold set in the Config Portal, the device will go into night-mode. To view the currently measured lux level, use the [keypad menu](#how-to-view-sensor-info).

If both a schedule is enabled and the light sensor option is checked in the Config Portal, the sensor will overrule the schedule only in non-night-mode hours; ie it will never switch off night-mode when night-mode is active according to the schedule.

Switching on/off night-mode manually deactivates any schedule and the light sensor for 30 minutes. Afterwards, a programmed schedule and/or the light sensor will overrule the manual setting.

## Count-down timer

The firmware features a simple count-down timer. This timer can count down from max 99 minutes and plays a sound when expired.

To set the timer to MM minutes, type 44MM and press ENTER. Note that for minutes, 2 digits must be entered. To cancel a running timer, enter 440 and press ENTER.

The check the remaining time, type 44 and press ENTER.

## Yearly/monthly reminder

The reminder is similar to the alarm, the difference being that the reminder is yearly or monthly, not daily. 

To program a yearly reminder, enter 77mmddhhMM and press ENTER. For example: 7705150900 sets the reminder to May 15 9am. Now a reminder sound will play every year on May 15 at 9am.

To program a monthly reminder, enter 7700ddhhMM and press ENTER. For example: 7700152300 sets the reminder to the 15th of each month, at 11pm.

You can also leave out the hhMM part; in that case the time remains unchanged from a previous setting, unless both hour and minute were 0 (zero), in which case the reminder time is set to 9am.

Note that, as usual, all fields consist of two digits, and hours are entered in 24-hour notation.

Type 77 followed by ENTER to display the programmed reminder, 770 to delete it, and 777 to display the days/hours/minutes until the next reminder.

The reminder only plays a sound file. The current sound-pack contains a default file; if your SD contains a file named "reminder.mp3", this will played instead of the default file.

Note: While the alarm and the sound-on-the-hour adhere to the "Alarm base is real present time" setting in the config portal, the Reminder does not. It is always based on real local time.

## SD card

Preface note on SD cards: For unknown reasons, some SD cards simply do not work with this device. For instance, I had no luck with Sandisk Ultra 32GB and  "Intenso" cards. If your SD card is not recognized, check if it is formatted in FAT32 format (not exFAT!). Also, the size must not exceed 32GB (as larger cards cannot be formatted with FAT32). I am currently using Transcend SDHC 4GB cards and those work fine.

The SD card, apart from being used to [install](#audio-file-installation) the default audio files, can be used for substituting default sounds, some additional custom sounds, and for music played back by the [Music player](#the-music-player).

Note that the SD card must be inserted before powering up the TCD. It is not recognized if inserted while the TCD is running. Furthermore, do not remove the SD card while the TCD is powered.

### Sound file substitution

The provided audio files ("sound-pack") are, after [proper installation](#audio-file-installation), integral part of the firmware and stored in the device's flash memory. 

These sounds can be substituted by your own sound files on a FAT32-formatted SD card. These files will be played back directly from the SD card during operation, so the SD card has to remain in the slot. The built-in [Audio file installer](#audio-file-installation) cannot be used to replace default sounds in the device's flash memory with custom sounds.

Your replacements need to be put in the root (top-most) directory of the SD card, be in mp3 format (128kbps max) and named as follows:
- "alarm.mp3". Played when the alarm sounds.
- "alarmon.mp3". Played when enabling the alarm
- "alarmoff.mp3". Played when disabling the alarm
- "nmon.mp3". Played when manually enabling night mode
- "nmoff.mp3". Played when manually disabling night mode
- "reminder.mp3". Played when the reminder is due.
- "timer.mp3". Played when the count-down timer expires.

The following sounds are time-sync'd to display action. If you decide to substitute these with your own, be prepared to lose synchronicity:
- "enter.mp3". Played when a date was entered and ENTER was pressed
- "baddate.mp3". Played when a bad (too short or too long) date was entered and ENTER was pressed
- "intro.mp3": Played during the power-up intro
- "travelstart.mp3". Played when a time travel starts.
- "timetravel.mp3". Played when re-entry of a time travel takes place.
- "shutdown.mp3". Played when the device is fake "powered down" using an external switch (see below)
- "startup.mp3". Played when the TCD is connected to power and finished booting

If you intend to use the very same SD card that you used for installing the default sound files, please remove the files from the sound-pack from the SD card first.

### Additional Custom Sounds

The firmware supports some additional user-provided sound effects, which it will load from the SD card. If the respective file is present, it will be used. If that file is absent, no sound will be played.

- "hour.mp3": Will be played every hour, on the hour. This feature is disabled in night mode.
- "hour-xx.mp3", xx being 00 through 23: Sounds-on-the-hour for specific hours that will be played instead of "hour.mp3". If a sound for a specific hour is not present, "hour.mp3" will be played, if that one exists.
- "key3.mp3" and/or "key6.mp3": Will be played if you hold the "3"/"6" key for 2 seconds.
- "ha-alert.mp3" will be played when a [HA/MQTT](#home-assistant--mqtt) message is received.

"hour.mp3"/"hour-xx.mp3", "key3.mp3", "key6.mp3" and "ha-alert.mp3" are not provided here. You can use any mp3, with 128kpbs or less.

## The Music Player

The firmware contains a simple music player to play mp3 files located on the SD card. 

In order to be recognized, your mp3 files need to be organized in music folders named *music0* through *music9*. The folder number is 0 by default, ie the player starts searching for music in folder *music0*. This folder number can be changed in the [keypad menu](#how-to-select-the-music-folder-number).

The names of the audio files must only consist of three-digit numbers, starting at 000.mp3, in consecutive order. No numbers should be left out. Each folder can hold 1000 files (000.mp3-999.mp3). *The maximum bitrate is 128kpbs.*

Since renaming mp3 files manually is somewhat cumbersome, the firmware can do this for you - provided you can live with the files being sorted in alphabetical order: Just copy your files with their original filenames to the music folder; upon boot or upon selecting a folder containing such files, they will be renamed following the 3-digit name scheme (as mentioned: in alphabetic order). You can also add files to a music folder later, they will be renamed properly; when you do so, delete the file "TCD_DONE.TXT" from the music folder on the SD card so that the firmware knows that something has changed. The renaming process can take a while (10 minutes for 1000 files in bad cases). Mac users are advised to delete the ._ files from the SD before putting it back into the TCD as this speeds up the process.

To start and stop music playback, hold 5. Holding 2 jumps to the previous song, holding 8 to the next one.

By default, the songs are played in order, starting at 000.mp3, followed by 001.mp3 and so on. By entering 555 and pressing ENTER, you can switch to shuffle mode, in which the songs are played in random order. Enter 222 followed by ENTER to switch back to consecutive mode.

Entering 888 followed by ENTER re-starts the player at song 000, and 888xxx (xxx = three-digit number) jumps to song #xxx.

See [here](#keypad-reference) for a list of controls of the music player.

While the music player is playing music, most sound effects are disabled/muted, such as keypad sounds, sound-on-the-hour, sounds for switching on/off the alarm and night-mode. Initiating a time travel stops the music player, as does activating the keypad menu. The alarm, the reminder and the count-down timer will sound as usual and stop the music player.

## The keypad menu
 
The keypad menu is an additional way to configure your TCD; it only involves the three displays and the keypad. It is controlled by "pressing" or "holding" the ENTER key on the keypad.

A "press" is shorter than 2 seconds, a "hold" is 2 seconds or longer.

The menu is invoked by holding the ENTER button.

*Note that if the keypad menu is active at a time when the alarm, the reminder, the count-down timer or sound-on-the-hour are due, those events will be missed and no sounds are played.*

Data entry, such as for dates and times, is done through the keypad's number keys and works as follows: Whenever a data entry is requested, the field for that data is lit (while the rest of the display is dark) and a pre-set value is shown. If you want to keep that pre-set, press ENTER to proceed to next field. Otherwise press a digit on the keypad; the pre-set is then overwritten by the value entered. 2 digits can be entered (4 for years), upon which the new value is stored and the next field is activated. You can also enter less than 2 digits (4 for years) and press ENTER when done with the field. Note that a month needs to be entered numerically (1-12), and hours need to be entered in 24-hour notation (0-23), regardless of 12-hour or 24-hour mode as per the Config Portal setting.

After invoking the keypad menu, the first step is to choose a menu item. The available items are  
- install the default audio files ("INSTALL AUDIO FILES")
- set the alarm ("ALA-RM"),
- set the audio volume (VOL-UME),
- set the Music Player folder number ("MUSIC FOLDER NUMBER")
- select the Time-cycling Interval ("TIME-CYCLING"),
- select the brightness for the three displays ("BRIGHTNESS"),
- show network information ("NET-WORK"),
- set the internal Real Time Clock (RTC) ("SET RTC"),
- enter dates/times for the *Destination* and *Last Time Departed* displays,
- show light/temperature/humidity sensor info (if such a sensor is connected) ("SENSORS"),
- show when time was last sync'd with NTP or GPS ("TIME SYNC"),
- see a list of [BTTFN-Clients](#bttf-network-bttfn) currently connected ("BTTFN CLIENTS"),
- quit the menu ("END").
 
Pressing ENTER cycles through the list, holding ENTER selects an item.

#### How to install the default audio files:

- Hold ENTER to invoke main menu
- If the SD card holds the files of the sound-pack archive from this repo, "INSTALL AUDIO FILES" is shown as the first menu item. See the [Audio file installation](#audio-file-installation) section.
- Hold ENTER to proceed
- Press ENTER to toggle between "CANCEL" and "PROCEED"
- Hold ENTER to proceed. If "PROCEED" was chosen, the audio files fill be installed and the TCD will reboot.
 
#### How to set up the alarm:

- Hold ENTER to invoke main menu
- (Currently, the alarm is the first menu item; otherwise press ENTER repeatedly until "ALA-RM" is shown)
- Hold ENTER
- Press ENTER to toggle the alarm on and off, hold ENTER to proceed
- Then enter the hour and minutes. This works as described above.
- Choose the weekday(s) by repeatedly pressing ENTER
- Hold ENTER to finalize your weekday selection. "SAVING" is displayed briefly.

When the alarm is set and enabled, the dot in the present time's minute field will light up. 

Under normal operation (ie outside of the menu), holding "1" toggles the alarm on/off.

The alarm time can also quickly be set by typing 11hhMM (eg. 110645 for 6:45am, or 112300 for 11:00pm) and pressing ENTER, just like when setting a time travel destination time. (The weekday selection has still to be done via the keypad menu.) Typing 11 followed by ENTER shows the currently set time and weekday selection briefly.

Note that the alarm is recurring, ie it rings at the programmed time, unless disabled. Also note, as mentioned, that the alarm is by default relative to your actual *present time*, not the time displayed (eg after a time travel). It can, however, be configured to be based on the time displayed, in the Config Portal.

*Important: The alarm will not sound when the keypad menu is active at the programmed alarm time.*

#### How to set the audio volume:

Basically, and by default, the device uses the hardware volume knob to determine the desired volume. You can change this to a fixed-level setting as follows:
- Hold ENTER to invoke main menu
- Press ENTER repeatedly until "VOL-UME" is shown
- Hold ENTER
- Press ENTER to toggle between "USE VOLUME KNOB" and "FIXED LEVEL"
- Hold ENTER to proceed
- If you chose "FIXED LEVEL/"SW", you can now select the desired level by pressing ENTER repeatedly. There are 20 levels available. The volume knob is now ignored.
- Hold ENTER to save and quit the menu

#### How to select the music folder number:

In order for this menu item to show up, an SD card is required.

- Hold ENTER to invoke main menu
- Press ENTER repeatedly until "MUSIC FOLDER NUMBER" is shown
- Hold ENTER, "NUM" is displayed
- Press ENTER repeatedly to cycle through the possible values. The message "NOT FOUND" appears if either the folder itself or 000.mp3 in that very folder is not present.            
- Hold ENTER to select the value shown and exit the menu ("SAVING" is displayed briefly)

If shuffle was enabled before, the new folder is also played in shuffled order.

Note that the Music Folder Number is saved in a config file on the SD card.

#### How to select the Time-cycling Interval:

- Hold ENTER to invoke main menu
- Press ENTER repeatedly until "TIME-CYCLING" is shown
- Hold ENTER, "INTERVAL" is displayed
- Press ENTER repeatedly to cycle through the possible Time-cycling Interval values.
- A value of 0 disables automatic time cycling ("OFF").
- Non-zero values make the device cycle through a number of pre-programmed times. The value means "minutes" (hence "MIN-UTES") between changes.              
- Hold ENTER to select the value shown and exit the menu ("SAVING" is displayed briefly)
 
#### How to adjust the display brightness:

- Hold ENTER to invoke main menu
- Press ENTER repeatedly until "BRIGHTNESS" is shown
- Hold ENTER, the displays show all elements, the top-most display says "LVL"
- Press ENTER repeatedly to cycle through the possible levels (1-15)
- Hold ENTER to use current value and proceed to next display
- After the third display, "SAVING" is displayed briefly and the menu is left automatically.
 
#### How to find out the IP address and WiFi status:

- Hold ENTER to invoke main menu
- Press ENTER repeatedly until "NET-WORK" is shown
- Hold ENTER, the displays show the IP address
- Repeatedly press ENTER to cycle between IP address, WiFi status, MAC address (in station mode) and Home Assistant connection status.
- Hold ENTER to leave the menu

#### How to set the Real Time Clock (RTC):

Adjusting the RTC is useful if you can't use NTP for time keeping, and really helpful when using GPS. Always set your actual local present time here; if you want to display some other time, use the Time Travel function. Note: The time you entered will be overwritten if/when the device has access to authoritative time such as via NTP or GPS. For DST (daylight saving) and GPS, it is essential that you first set the correct time zone in the [Config Portal](#the-config-portal).

- Hold ENTER to invoke main menu
- Press ENTER repeatedly until "SET RTC" is displayed and the *Present Time* display shows a date and time 
- Hold ENTER until the *Present Time* display goes off except for the first field to enter data into
- The field to enter data into is shown (exclusively), pre-set with its current value
- Data entry works as described [above](#the-keypad-menu); remember that months need to be entered numerically (01-12), and hours in 24-hour notation (0-23).
- After entering data into all fields, the data is saved and the menu is left automatically.

#### How to enter dates/times for the *Destination* and *Last Time Departed* displays:

Note that when entering dates/times into the *destination time* or *last time departed* displays, the Time-cycling Interval is automatically set to 0. Your entered date/time(s) are shown until overwritten by time travels.

- Hold ENTER to invoke main menu
- Press ENTER repeatedly until the desired display is the only one lit and shows a date and time
- Hold ENTER until the display goes off except for the first field to enter data into
- The field to enter data into is shown (exclusively), pre-set with its current value
- Data entry works as described [above](#the-keypad-menu); remember that months need to be entered numerically (01-12), and hours in 24-hour notation (0-23).
- After entering data into all fields, the data is saved and the menu is left automatically.

#### How to view sensor info

- Hold ENTER to invoke main menu
- Press ENTER repeatedly until "SENSORS" is shown. If that menu item is missing, a light or temperature sensor was not detected during boot.
- Hold ENTER
- Now the currently measured lux level or temperature is displayed.
- Press ENTER to toggle between light sensor and temperature sensor info (if both are connected)
- Hold ENTER to exit the menu

Note: Sometimes, light sensors report a lux value of -1. This is mostly due to the fact that all the supported sensors are adjusted for indoor usage and might overload in broad daylight. Also, some sensors might have issues with halogen lamps (reportedly TSL2561), and most sensors also "overload" if too much IR light is directed at them, for instance from surveillance cameras.

#### How to see a list of BTTF-Network clients

- Hold ENTER to invoke main menu
- Press ENTER repeatedly until "BTTFN CLIENTS" is shown.
- Hold ENTER
- Now one of the connected clients is displayed, or "NO CLIENTS" if currently no clients are connected.
- Press ENTER to scroll through the list of connected clients.
- Hold ENTER to exit the menu

#### How to leave the menu:
 
 - While the menu is active, press ENTER repeatedly until "END" is displayed.
 - Hold ENTER to leave the menu

## Peripherals and connection

![The BTTF Prop Family](https://github.com/realA10001986/Time-Circuits-Display/assets/76924199/fd0de57c-00a2-4f28-ac35-db2b7a2c0e60)

- [Fake Power Switch](#fake-power-switch)
- [Speedometer](#speedometer)
- [GPS-Receiver](#gps-receiver)
- [Temperature Sensor](#room-condition-mode-temperaturehumidity-sensor)
- [Light Sensor](#night-mode)
- [Other Props](#controlling-other-props) (Flux Capacitor, SID, ...)

## Fake power Switch 

You probably noticed that the device takes longer to boot than would be required to re-create the movie experience where Doc turns the knob and the Time Circuits immediately turn on. As a remedy, the firmware supports a fake "power switch": 

If the **_Use fake power switch_** option is checked in the Config Portal, the device will power-up, initialize everything, but stay quiet and dark. Only when the fake "power switch" is activated, the device will visually "power up". Likewise, you can also fake "power off" the device using this switch. Fake "off" disables the displays, all audio (except the alarm) and the keypad. Just like in the movie.

On Control Boards V1.3 and later, there is a dedicated header labeled "Fake PWR" to connect the switch to. The pins to be shortened by the switch are labeled "GND" and "PWR Trigger":

![pwr_trigger](https://github.com/realA10001986/Time-Circuits-Display/assets/76924199/59bbcfcb-1898-4d9c-b34f-d1a6cf3892fd)

On earlier Control Boards (1.2 and below), the switch needs shorten the pins labeled "IO13" and "GND" as shown here:

![access_to_io13](https://user-images.githubusercontent.com/76924199/194283241-3bee5fef-f51a-4b1a-a158-ed92292bcf32.jpg)

Here is a close-up of one of my boards; I have soldered some headers on the io ports.

![io13neu](https://user-images.githubusercontent.com/76924199/212369689-f945dcf1-55f9-41e9-8fd7-fc0dbc69906c.jpg)

Note that the switch actually needs to be a switch with a maintained contact; the pins need to remain connected for as long as the device is fake-switched-on.

In order to use the Fake Power Switch, check **_Use fake power switch_** in the Config Portal.

[Here](https://github.com/realA10001986/Time-Circuits-Display/wiki/Time-Circuits-Switch) are some hints for building a TFC Switch.

## External Time Travel Trigger

As mentioned above, a time travel can be triggered by holding "0" on the keypad. Since this doesn't really allow for an authentic movie-like experience, the firmware also supports an external trigger, such as a button switch or even another prop to trigger a time travel. Note that, unlike the [Fake Power Switch](#fake-power-switch), this trigger must be a momentary toggle.

On Control Boards V1.3 and later, there is a dedicated header for the button labeled "Time Travel". The button needs to shorten pins "TT IN" and "GND".

| ![ttin](https://github.com/realA10001986/Time-Circuits-Display/assets/76924199/8ec8d88a-f2df-484c-b9b9-1d389dc6d480) |
|:--:|
| TT_IN on TCB 1.3 |

Unfortunately, there is no header and no break out for IO27 on TC control boards V1.2 and below. Some soldering is required. The button needs to be 
connected to the two marked pins in the image below:

![nodemcuio272](https://github.com/realA10001986/Time-Circuits-Display/assets/76924199/9ace6814-87d6-48db-babe-39d44c03eaed)

Luckily, there is a row of solder pads right next to the socket on the control board, where a pin header or cable can easily be soldered on:

| ![tcboard_io27](https://user-images.githubusercontent.com/76924199/194284336-2fe9fa9b-d5e5-49f5-b1cd-b0fd2abdff53.jpg) |
|:--:|
| IO27 (TT_IN) on TCB 1.2 |

In order to trigger a time-travel sequence on the Time Circuits, "TT IN"/IO27 and GND must be shortened for at least 200ms and then opened; the time travel is triggered upon release of the button. If the button is pressed for 3000ms (3 seconds), a ["Return from Time Travel"](#time-travel) is triggered.

The Config Portal allows configuring a delay for matching/synchronizing the TCD to another prop. The delay, if any, starts running after the button is released. The time travel sequence starts after the delay has expired.

## Speedometer

The firmware supports a speedometer display connected via i2c (slave address 0x70) as part of the time travel sequence. As CircuitSetup's upcoming Speedo display is not available yet, you are on your own to build one for the time being.

What you need is a box, the LED segment displays and a HT16K33-based PCB that allows accessing the LED displays via i2c. There are various readily available LED segment displays with suitable i2c break-outs from Adafruit and Seeed (Grove) that can be used as a basis. Adafruit [878](https://www.adafruit.com/product/878), [1270](https://www.adafruit.com/product/1270) and [1911](https://www.adafruit.com/product/1911), as well as Grove 0.54" 14-segment [2-digit](https://www.seeedstudio.com/Grove-0-54-Red-Dual-Alphanumeric-Display-p-4031.html) or [4-digit](https://www.seeedstudio.com/Grove-0-54-Red-Quad-Alphanumeric-Display-p-4032.html) alphanumeric displays are supported. (The product numbers vary with color, the numbers here are the red ones.)

| [![Watch the video](https://img.youtube.com/vi/opAZugb_W1Q/0.jpg)](https://youtu.be/opAZugb_W1Q) |
|:--:|
| Click to watch the video |

The speedo display shown in this video is based on a fairly well-designed stand-alone replica I purchased on ebay. I removed the electronics inside and wired the LED segments to an Adafruit i2c backpack (from the Adafruit 878 product) and connected it to the TCD. Yes, it is really that simple. (The switch was made by me, see [here](https://github.com/realA10001986/Time-Circuits-Display/wiki/Time-Circuits-Switch), it uses the [Fake Power Switch](#fake-power-switch) feature of the TCD.)

In order to use the Speedometer display, select the correct display type in the Config Portal. There are two special options in the Speedo Display Type drop-down: *Ada 1911 (left tube)* and *Ada 878 (left tube)*. These two can be used if you connect only one 2-digit-tube to the respective Adafruit i2c backpack, as I did in case of my speedo replica as well as my [Wall Clock](#my-custom-made-time-circuits-wall-clock).

Since the I2C bus is already quite long from the control board to the last display in the chain, I recommend soldering another XH 4pin plug onto the control board (there are two additional i2c break-outs available), and to connect the speedometer there.

## GPS receiver

The firmware supports an MT(K)3333-based GPS receiver, connected through i2c (slave address 0x10). The CircuitSetup-designed speedo display will have such a chip built-in, but since this gadget is not yet available, in the meantime, you can use alternatives such as the Adafruit Mini GPS PA1010D (product id [4415](https://www.adafruit.com/product/4415)) or the Pimoroni P1010D GPS Breakout ([PIM525](https://shop.pimoroni.com/products/pa1010d-gps-breakout?variant=32257258881107)). The GPS receiver can be used as a source of authoritative time (like NTP), and speed of movement (to be displayed on a [speedo display](#speedometer)).

GPS receivers receive signals from satellites, but in order to do so, they need to be "tuned in" (aka get a "fix"). This "tuning" process can take a long time; after first power up, it can take 30 minutes or more for a receiver to be able to determine its position. In order to speed up this process, modern GPS receivers have special "assisting" features. One key element is knowledge of current time, as this helps identifying satellite signals quicker. So, in other words, initially, you need to tell the receiver, what it is supposed to tell you. However, as soon as the receiver has received satellite signals for 15-20 minutes, it saves the data it collected to its battery-backed memory and will find a fix within seconds after power-up in the future.

For using GPS effectively as a long-term source of accurate time, it is therefore essential, that 
- the TimeCircuit's RTC (real time clock) is initially [set to correct local time](#how-to-set-the-real-time-clock-rtc), 
- the correct time zone is defined in the Config Portal,
- the GPS receiver has a battery
- and has been receiving data for 15-20 mins at least once a month.

If/as long as the GPS receiver has a fix and receives data from satellites, the dot in the present time's year field is lit.

In order to use the GPS receiver as a source of time, no special configuration is required. If it is detected during boot, it will be used.

#### GPS for speed

One nice feature of GPS is that the receiver can deliver current speed of movement. If the Time Circuits are, for instance, mounted in a car or on a boat, and a [speedo display](#speedometer) is connected, this display will be just that: A real speedometer.

| [![Watch the video](https://img.youtube.com/vi/wbeXZJaDLa8/0.jpg)](https://youtu.be/wbeXZJaDLa8) |
|:--:|
| Click to watch the video |

In order to use the GPS receiver for speed, check **_Display GPS speed_** in the Config Portal.

If other props using GPS speed are connected via [BTTF-Network](#bttf-network-bttfn), check the option **_Provide GPS speed for wireless props_** in the Config Portal.

## Room Condition Mode, Temperature/humidity sensor

The firmware supports connecting a temperature/humidity sensor for "room condition mode"; in this mode, *destination* and *last departed* times are replaced by temperature and humidity (if applicable), respectively. To toggle between normal and room condition mode, enter "111" and press ENTER. 

![rcmode](https://user-images.githubusercontent.com/76924199/208133653-f0fb0a38-51e4-4436-9506-d841ef1bfa6c.jpg)

Room condition mode can be used together with [World Clock mode](#world-clock-mode); if both are enabled, only one alternative time and only temperature is shown. To toggle RC and WC mode simultaniously, type "113" and press ENTER.

Temperature on speedometer display: Unless you do time travelling on a regular basis, the [speedo display](#speedometer) is idle most of the time in a typical home setup. To give it more of a purpose, the firmware can display ambient temperature on the speedo while idle.

#### Sensor hardware

In order to use a temperature/humidity sensor, no special configuration is required. If a sensor is detected by the firmware during boot, it will be used.

Seven sensor types are supported: MCP9808 (i2c address 0x18), BMx280 (0x77), SI7021 (0x40), SHT40 (0x44), TMP117 (0x49), AHT20/AM2315C (0x38), HTU31D (0x41). All of those are readily available on breakout boards from Adafruit or Seeed (Grove). The BMP280 (unlike BME280), MCP9808 and TMP117 work as pure temperature sensors, the others for temperature and humidity. For wiring information, see [here](#appendix-b-sensor-wiring).

*Note: You cannot connect the sensor chip directly to the TCD control board; most sensors need at least a power converter/level-shifter.* This is why I exclusively used Adafruit or Seeed breakouts ([MCP9808](https://www.adafruit.com/product/1782), [BME280](https://www.adafruit.com/product/2652), [SI7021](https://www.adafruit.com/product/3251), [SHT40](https://www.adafruit.com/product/4885), [TMP117](https://www.adafruit.com/product/4821), [AHT20](https://www.adafruit.com/product/4566), [HTU31D](https://www.adafruit.com/product/4832)), which all allow connecting named sensors to the 5V the TCD board operates on.

## Car Mode

If the TCD, perhaps along with other props such as Flux Capacitor or SID, is mounted in a car, there are a few things to be considered:

- There is no point in attempting to connect a WiFi network.
- If the TCD acts as WiFi access point for Flux Capacitor and/or SID, it should boot quickly so that the other props can connect to it as soon as possible.

This is what *car mode* is for: If enabled, the TCD will always boot in AP mode, regardless of a configured WiFi network. This speeds up booting.

To enable *car mode*, type 991 followed by ENTER. The TCD will reboot in AP mode.

To disable *car mode*, type 990 followed by ENTER. The TCD will reboot and attempt to connect to a previously configured WiFi network.

*Car mode* is persistent, i.e. it remains active (even accross reboots and power-downs) until disabled.

Note that the TCD has no internet access while in car mode; this means that it cannot update its clock automatically. If the time runs off over time, you need to re-adjust it using the [keypad menu](#how-to-set-the-real-time-clock-rtc).

## Controlling other props

### Connecting props by wire

The device can tell other props about a time travel, and in essence act as a "master controller" in a setup of several props. It does so via IO14 (labeled "TT OUT" on Control Boards 1.3 and later).

![Wired connection](https://github.com/realA10001986/Time-Circuits-Display/assets/76924199/2e9be0a2-2180-417a-9d04-6f36f7cd1697)

You need two wires for connecting the TCD to the other prop: TT_OUT (IO14) and GND, which need to be connected to the respective pins of the prop.

| ![ttout](https://github.com/realA10001986/Time-Circuits-Display/assets/76924199/b1330e4d-12ba-48ef-a454-3d6167fb2a5d) |
|:--:|
| IO14 on TCB 1.3 |

| ![ttout](https://github.com/realA10001986/Time-Circuits-Display/assets/76924199/112c0240-6a3b-44c3-a15d-5af7477a8791) |
|:--:|
| IO14 on TCB 1.2 |

In case you want to design your own props, here's the timing diagram:

1) Option **_Signal Time Travel without 5s lead_** unchecked

```
|<---------- speedo acceleration --------->|                         |<-speedo de-acceleration->|
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

"ETTO lead", ie the lead time between TT_OUT/IO14 going high and the actual start of a time travel is defined as 5000ms (ETTO_LEAD_TIME). In this window of time, the prop can play its pre-time-travel (warm-up/acceleration/etc) sequence. The sequence inside the time "tunnel" follows after that lead time, and when IO14 goes LOW, the re-entry into the destination time takes place.

2) Option **_Signal Time Travel without 5s lead_** checked

```
|<---------- speedo acceleration --------->|                         |<-speedo de-acceleration->|
0....10....20....................xx....87..88------------------------88...87....................0
                                           |<--Actual Time Travel -->|
                                           |  (Display disruption)   |
                                      TT starts                      Reentry phase
                                           |                         |
                                           |                         |
                                           |                         |
                                           |                         |
                                           |                         |
                                    IO14: LOW->HIGH           IO14: HIGH->LOW
 ```

If external gear is connected to TT_OUT/IO14 and you want to use this control feature, check **_Control props connected by wire_** in the Config Portal.

For CircuitSetup original props connected by wire, the option **_Signal Time Travel without 5s lead_** must not be set. If those props are connected wirelessly, this option has no effect.

Note that a wired connection only allows for synchronized time travel sequences, no other communication takes place.

### Connecting props wirelessly

#### BTTF-Network (BTTFN)

The TCD can communicate with other compatible props wirelessly, via WiFi. It can send out information about a time travel and an alarm, and other props can query the TCD for time, speed and some other data. Unlike with MQTT, no broker or other third party software is needed.

![bttfn connection](https://github.com/realA10001986/Time-Circuits-Display/assets/76924199/4c831d02-e6c9-4e80-9e05-37e782c68c79)

On the TCD, no special configuration is required. However, if the TCD is supposed to send out notifications about time travel and alarm to connected BTTFN clients, usage of MQTT must be disabled or the **_Send event notifications_** option in the MQTT section of the Config Portal must be unchecked. The TCD only sends out such notifications either via MQTT or BTTFN, never both.

On the other prop, such as CircuitSetup's upcoming [Flux Capacitor](https://github.com/realA10001986/Flux-Capacitor) or [SID](https://github.com/realA10001986/SID), the TCD's IP address (not hostname!) must be entered into the *IP address of TCD* field on the Setup page in their Config Portal - that's all.

The fact that the devices communicate directly with each other makes BTTFN the ideal solution for car setups. Also, while at home, the devices might be connected to an existing WiFi network, in a car, the TCD can act as access point for Flux Capacitor and SID (ie they are connecting to the *TCD-AP* WiFi network), and those then can talk the TCD wirelessly. The TCD has *car mode* for doing exactly this, see [here](#car-mode) and the documentation of the respective prop. 

To see which BTTFN devices are currently known to the TCD, enter the keypad menu and select "BTTFN CIENTS".

#### Home Assistant/MQTT

The other way of wireless communication is, of course, [Home Assistant/MQTT](#home-assistant--mqtt). If both the TCD and the other props are connected to the same broker, and the option **_Send event notifications_** is checked on the TCD's side, other compatible props will receive information on time travel and alarm and play their sequences in sync with the TCD.

![MQTT connection](https://github.com/realA10001986/Time-Circuits-Display/assets/76924199/fdfcaabf-96e9-45b1-a626-ded91f37d128)

MQTT and BTTFN can co-exist: 

BTTFN must be configured on the prop - regardless of MQTT usage - if that prop is to receive data from the TCD (like, for instance, GPS speed). 

However, as mentioned, the TCD only sends out time travel and alarm notifications through either MQTT or BTTFN, never both. If you have other MQTT-aware devices listening to the TCD's public topic (bttf/tcd/pub) in order to react to time travel or alarm messages, use MQTT (ie check **_Send event notifications_**). If only BTTFN-aware devices are to be used, uncheck this option to use BTTFN as it has less latency.

## Home Assistant / MQTT

The TCD supports the MQTT protocol version 3.1.1 for the following features:

### Display messages on TCD

The TCD can subscribe to a user-configured topic and display messages received for this topic on the *Destination Time* display. This can be used to display the status of other HA/MQTT devices, for instance alarm systems. If the SD card contains a file named "ha-alert.mp3", this file will be played upon reception of a message (there is no default sound).

Only ASCII messages are supported, the maximum length is 255 characters.

### Control the TCD via MQTT

The TCD can - to a limited extent - be controlled through messages sent to topic **bttf/tcd/cmd**. Support commands are
- TIMETRAVEL: Start a time travel
- RETURN: Return from time travel
- BEEP_ON: Enables the *annoying beep*(tm)
- BEEP_OFF: Disables the *annoying beep*(tm)
- BEEP_30, BEEP_60: Set the beep modes as described [here](#beep-on-the-second)
- ALARM_ON: Enable the alarm
- ALARM_OFF: Disable the alarm
- NIGHTMODE_ON: Enable manual night mode
- NIGHTMODE_OFF: Disable manual night mode
- MP_PLAY: Starts the Music Player
- MP_STOP: Stops the Music Player
- MP_NEXT: Jump to next song
- MP_PREV: Jump to previous song
- MP_SHUFFLE_ON: Enables shuffle mode in Music Player
- MP_SHUFFLE_OFF: Disables shuffle mode in Music Player

### Notify other devices of a time travel or alarm

Upon a time travel, the TCD can send messages to topic **bttf/tcd/pub**. This can be used to control other props wirelessly, such as Flux Capacitor, SID, etc. The timing is identical to the wired protocol, see [here](#controlling-other-props). TIMETRAVEL is sent when IO14 goes high, ie with a lead time (ETTO LEAD) of 5 seconds. REENTRY is sent when the re-entry sequence starts (ie when IO14 goes low). Note that network traffic has some latency, so timing might not be as exact as a wired connection.

When the [alarm](#how-to-set-up-the-alarm) sounds, the TCD can send "ALARM" to **bttf/tcd/pub**.

### Setup

In order to connect to a MQTT network, a "broker" (such as [mosquitto](https://mosquitto.org/), [EMQ X](https://www.emqx.io/), [Cassandana](https://github.com/mtsoleimani/cassandana), [RabbitMQ](https://www.rabbitmq.com/), [Ejjaberd](https://www.ejabberd.im/), [HiveMQ](https://www.hivemq.com/) to name a few) must be present in your network, and its address needs to be configured in the Config Portal. The broker can be specified either by domain or IP (IP preferred, spares us a DNS call). The default port is 1883. If a different port is to be used, append a ":" followed by the port number to the domain/IP, such as "192.168.1.5:1884". 

If your broker does not allow anonymous logins, a username and password can be specified.

If you want your TCD to display messages as described above, you also need to specify the topic in the respective field.

If you want your TCD to publish messages to bttf/tcd/pub (ie if you want to notify other devices about a timetravel and/or alarm), check the **_Send event notifications_** option.

Limitations: MQTT Protocol version 3.1.1; TLS/SSL not supported; ".local" domains (MDNS) not supported; maximum message length 255 characters; server/broker must respond to PING (ICMP) echo requests. For proper operation with low latency, it is recommended that the broker is on your local network. Note that using HA/MQTT will disable WiFi power saving (as described below).

## WiFi power saving features

The Config Portal offers two options for WiFi power saving, one for station mode (ie when the device is connected to a WiFi network), one for AP-mode (ie when the device acts as an access point). Both options do the same: They configure a timer after whose expiry WiFi is switched off; the device is no longer transmitting or receiving data over WiFi. 

The timers can be set to 0 (which disables them; WiFi is never switched off; this is the default), or 10-99 minutes. 

The reason for having two different timers for AP-mode and for station mode is that if the device is used in a car, it might act as an access point (such as in [car mode](#car-mode)), while at home it is most probably connected to a WiFi network as a client. Since in a car, unless other props are installed that communicate wirelessly, WiFi will most likely not be used on a regular basis, the timer for AP mode can be short (eg 10 minutes), while the timer for station mode can be disabled.

After WiFi has been switched off due to timer expiry, it can be re-enabled by holding "7" on the keypad for approx. 2 seconds, in which case the timers are restarted (ie WiFi is again switched off after timer expiry).

Note that if your configured WiFi network was not available when the TCD was trying to connect, it will end up in AP-mode. Holding "7" in that case will trigger another attempt to connect to your WiFi network.

## Flash Wear

Flash memory has a somewhat limited life-time. It can be written to only between 10.000 and 100.000 times before becoming unreliable. The firmware writes to the internal flash memory when saving settings and other data. Every time you change settings through the keypad menu or the Config Portal, data is written to flash memory. The same goes for changing alarm settings (including enabling/disabling the alarm), and time travelling if time travels are [persistent](#persistent--non-persistent-time-travels).

In order to reduce the number of write operations and thereby prolong the life of your TCD, it is recommended
- to uncheck the option **_[Make time travels persistent](#persistent--non-persistent-time-travels)_** in the Config Portal,
- to use a good-quality SD card and to check **_[Save alarm/volume settings on SD](#-save-alarmvolume-settings-on-sd)_** in the Config Portal; alarm and volume settings are then stored on the SD card (which also suffers from wear but is easy to replace). If you want to swap the SD card but preserve your alarm/volume settings, go to the Config Portal while the old SD card is still in place, uncheck the **_Save alarm/volume settings on SD_** option, click on Save and wait until the TCD has rebooted. You can then power down the TCD, swap the SD card and power-up again. Then go to the Config Portal, change the option back on and click on Save. Your settings are now on the new SD card.

## My custom-made Time Circuits Wall Clock

![Back to the Future Wall Clock](https://user-images.githubusercontent.com/76924199/214323242-00b7b6da-8fbd-43f5-8ec0-68c06131939e.jpg)

More here: [Link](https://github.com/realA10001986/Time-Circuits-Display/wiki/Time-Circuits-Wall-Clock)

## My custom-made TFC switch

![Time Circuits Switch](https://user-images.githubusercontent.com/76924199/226289995-afc79f34-50d7-4454-988b-a4a26163fba9.jpg)

More here: [Link](https://github.com/realA10001986/Time-Circuits-Display/wiki/TFC-Switch)

## Appendix A: The Config Portal

### Main page

##### &#9654; Configure WiFi

Clicking this leads to the WiFi configuration page. On that page, you can connect your TCD to your WiFi network by selecting/entering the SSID (WiFi network name) as well as a password (WPA2). By default, the TCD requests an IP address via DHCP. However, you can also configure a static IP for the TCD by entering the IP, netmask, gateway and DNS server. All four fields must be filled for a valid static IP configuration. If you want to stick to DHCP, leave those four fields empty.

Note that this page has nothing to do with Access Point mode; it is strictly for connecting your TCD to an existing WiFi network as a client.

##### &#9654; Setup

This leads to the [Setup page](#setup-page).

##### &#9654; Restart

This reboots the TCD. No confirmation dialog is displayed.

##### &#9654; Update

This leads to the firmware update page. You can select a locally stored firmware image file to upload (such as the ones published here in the install/ folder).

##### &#9654; Erase WiFi Config

Clicking this (and saying "yes" in the confirmation dialog) erases the WiFi configuration (WiFi network and password) and reboots the device; it will restart in "access point" mode. See [here](#short-summary-of-first-steps).

---

### Setup page

#### Basic settings

##### &#9654; Make time travels persistent

See [here](#persistent--non-persistent-time-travels)

##### &#9654; Alarm base is real present time

Selects whether the alarm and sound-on-the-hour are based on real, actual present time, or "present" time as displayed (eg after a time travel).

Note that the night mode schedules and the Reminder are always based on actual present time.

##### &#9654; Play intro

Selects whether the animated "Back to the Future" intro should be played upon power-up.

##### &#9654; 24-hour clock mode

Selects 24-hour clock mode, ie hours go from 0 to 23, "AM" and "PM" are permanently dark.

##### &#9654; Power-up beep mode

Selects the default, power-up ["beep"](#beep-on-the-second) mode. "Auto: xx secs" enables the beep for xx seconds after entering a destination time, after triggering a time travel, and upon (real or fake) power-on. Can be changed at any time by typing 000 (off), 001 (on), 002 (Auto 30secs) or 003 (Auto 60secs) followed by ENTER.

##### &#9654; Time-cycling interval

In this decorative mode the device cycles through a list of pre-programmed, movie-accurate *destination* and *last time departed* times. This mode is enabled by setting the "Time-cycling Interval" to anything but "off". The device will cycle through named list every 5th, 10th, 15th, 30th or 60th minute, and thereby change the displays. Set the interval to "off" to disable. See [here](#time-cycling)

#### Network & time synchronization settings

##### &#9654; Hostname

The device's hostname in the WiFi network. Defaults to 'timecircuits'. This also is the domain name at which the Config Portal is accessible from a browser in the same local network. The URL of the Config Portal then is http://<i>hostname</i>.local (the default is http://timecircuits.local)

If you have more than one TCD in your local network, please give them unique hostnames.

##### &#9654; AP Mode: Network name appendix

By default, if the TCD creates a WiFi network of its own ("AP-mode"), this network is named "TCD-AP". In case you have multiple TCDs in your vicinity, you can have a string appended to create a unique network name. If you, for instance, enter "-ABC" here, the WiFi network name will be "TCD-AP-ABC". Characters A-Z, a-z, 0-9 and - are allowed.

##### &#9654; AP Mode: WiFi password

By default, and if this field is empty, the TCD's own WiFi network ("AP-mode") will be unprotected. If you want to protect your TCD access point, enter your password here. It needs to be 8 characters in length and only characters A-Z, a-z, 0-9 and - are allowed.

If you forget this password and are thereby locked out of your TCD, power-down, hold the ENTER key, power-up and wait until the white LED flashes, then release the ENTER key. The TCD will boot and start the access point temporarily without a password. Then connect to the TCD's AP with your computer or handheld, enter the Config Portal ( http:://192.168.4.1 ) and either look up or change your AP WiFi password. Note that this ENTER-key-procedure is not persistent: When you reboot or re-power the TCD, the AP will be password protected again.

##### &#9654; WiFi connection attempts

Number of times the firmware tries to reconnect to a WiFi network, before falling back to AP-mode. See [here](#short-summary-of-first-steps)

##### &#9654; WiFi connection timeout

Number of seconds before a timeout occurs when connecting to a WiFi network. When a timeout happens, another attempt is made (see immediately above), and if all attempts fail, the device falls back to AP-mode. See [here](#short-summary-of-first-steps)

##### &#9654; Periodic reconnection attempts

Selects whether periodic re-connection attempts (between 12am and 6am) should be made after a failure to connect to a configured WiFi network.

In typical home setups with 24/7 WiFi, this option hardly matters. However, if you have connected your TCD to your WiFi network, but move it between environments (eg home and car) on a regular basis, uncheck this to keep the TCD from searching for your configured WiFi network unnecessarily. 

##### &#9654; WiFi power save timer

See [here](#wifi-power-saving-features)

##### &#9654; WiFi power save timer for AP-mode

See [here](#wifi-power-saving-features)

##### &#9654; Time zone

The time zone of the place where the device is operated. Has to be in POSIX format. Needs to be set in order to use NTP or GPS, and for DST (daylight saving). Defaults to UTC0. See [here](https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv) or [here](https://github.com/realA10001986/Time-Circuits-Display/blob/main/timezones.csv) for a list of valid time zones.

##### &#9654; NTP Server

Name of your preferred NTP (network time protocol) server for time synchronization. Leave this empty to disable NTP.

#### World Clock mode

##### &#9654; Time zone for Destination Time display

The time zone for the red display in [World Clock mode](#world-clock-mode). Default: unset. Needs to be in [Posix](https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv) format.

##### &#9654; Time zone for Last Time Dep. display

The time zone for the yellow display in [World Clock mode](#world-clock-mode). Default: unset. Needs to be in [Posix](https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv) format.

##### &#9654; City/location name

For each World Clock time zones, a city or location name can be configured. For example "SYDNEY" or "LORD HOWE". This name will be shown every few seconds alternately with time.

#### Night-mode

##### &#9654; Destination time off in night mode

Selects whether the *destination time* display is dimmed or switched of in night mode.

##### &#9654; Present time off in night mode

Selects whether the *present time* display is dimmed or switched of in night mode.

##### &#9654; Last time dep. off in night mode

Selects whether the *last time departed* display is dimmed or switched of in night mode.

##### &#9654; Schedule

Selects a schedule for night-mode. See [here](#night-mode)

##### &#9654; Daily night-mode start hour

If "Daily" is chosen in the "Schedule"-drop-down, enter night-mode start hour here.

##### &#9654; Daily night-mode end hour

If "Daily" is chosen in the "Schedule"-drop-down, enter night-mode end hour here.

##### &#9654; Use light sensor

Select whether to use a light sensor's data to enable night-mode. See [here](#night-mode).

##### &#9654; Light (lux) threshold

If the light sensor reports a number of lux below or equal to this value, night-mode is activated. See [here](#night-mode).

#### Temperature/humidity sensor settings

##### &#9654; Display in °Celsius

Selects between Fahrenheit and Celsius for temperature display. See [here](#room-condition-mode-temperaturehumidity-sensor)

##### &#9654; Temperature offset

This offset, which can range from -3.0 to 3.0, is added to the sensor measurement, in order to compensate sensor inaccuracy or suboptimal sensor placement.

#### Speedometer settings

##### &#9654; Speedo display type

Type of [speedo](#speedometer) display. Several types are supported, mostly Adafruit or Seeed/Grove LED segment displays. For 4-digit-displays there are options to display speed left- or right-aligned. Two special options for Adafruit 828 and 1911 are at the end of the list; use these if you connect only one 2-digit-tube to the respective i2c backpack.

##### &#9654; Speedo brightness

Brightness of speedo display when displaying speed.

##### &#9654; Speedo sequence speed factor

Since the DMC-12 wasn't the world's fastest car, it's acceleration figures might soon cause boredom if played in real-time as part of the time travel sequence with a speedo display. This factor speeds up the acceleration. 2.0 means twice as fast as the real car.

##### &#9654; Display GPS speed

If a GPS receiver is connected, this selects whether to display GPS speed on the speedo display. This only makes sense if the Time Circuits are mounted in a car. See [here](#gps-receiver)

##### &#9654; Display temperature

Selects whether temperature as received from a suitable sensor is displayed on the speedo display, when it is idle (ie no time travel is in progress).

##### &#9654; Temperature brightness

Brightness of speedo display when displaying temperature.

##### &#9654; Temperature off in night mode

Selects whether the temperature display is dimmed or switched of in night mode.

#### External switches/buttons

##### &#9654; Use fake power switch

Check this if you want to use a fake power switch. See [here](#fake-power-switch).

##### &#9654; External time travel button: Delay

Selects a delay (in milliseconds) from when pressing the external time travel button until the time travel sequence starts. See [here](#external-time-travel-trigger).

#### Settings for other peripherals

##### &#9654; Control props connected by wire

This selects whether the TT_OUT/IO14 pin is activated upon a time-travel in order to play synchronized time travel sequences on other props, if those props are connected by wire. See [here](#controlling-other-props).

##### &#9654; Signal Time Travel without 5s lead

If this option is unchecked (which is the default), a time travel is signalled for wired props with a 5 second lead, to give the prop time to play an acceleraton sequence. If this option is checked, TT_OUT/IO14 is activated when the time travel actually starts. 

For CircuitSetup original props, if they are connected by wire, this option must not be set. For wirelessly connected props this option has no effect. Also see [here](#controlling-other-props).

##### &#9654; Provide GPS speed for wireless props

Some [BTTF-Network](#bttf-network-bttfn) clients query the TCD for GPS speed, such as CircuitSetup's upcoming FC and SID. Check this option if such clients are connected, so that they get frequent GPS speed updates.

##### &#9654; Play time travel sounds

If other props are connected, they might bring their own time travel sound effects. In this case, you can uncheck this to disable the Time Circuit's own time travel sounds. Note that this only covers sounds played during time travel, not other sound effects.

#### Home Assistant / MQTT settings

##### &#9654; Use Home Assistant (MQTT 3.1.1)

If checked, the TCD will connect to the broker (if configured) and send and receive messages via [MQTT](#home-assistant--mqtt).

##### &#9654; Broker IP[:port] or domain[:port]

The broker server address. Can be a domain (eg. "myhome.me") or an IP address (eg "192.168.1.5"). The default port is 1883. If different port is to be used, it can be specified after the domain/IP and a colon ":", for example: "192.168.1.5:1884". Specifiying the IP address is preferred over a domain since the DNS call adds to the network overhead. Note that ".local" (MDNS) domains are not supported.

##### &#9654; User[:Password]

The username (and optionally the password) to be used when connecting to the broker. Can be left empty if the broker accepts anonymous logins.

##### &#9654; Topic to display

An optional topic the TCD subscribes to in order to display messages on the *Destination Time" display.

##### &#9654; Send event notifications

Check this if you want the TCD to send notifications on time travel and alarm via [MQTT](#home-assistant--mqtt).

Note that if this option is checked, the TCD will not send out such notifications via [BTTF-Network](#bttf-network-bttfn).

#### Music Player settings

##### &#9654; Shuffle at startup

When checked, songs are shuffled when the device is booted. When unchecked, songs will be played in order.

#### Other settings

##### &#9654; Save alarm/volume settings on SD

If this is checked, alarm and volume settings are stored on the SD card. This helps to minimize write operations to the internal flash memory and to prolong the lifetime of your TCD. See [Flash Wear](#flash-wear).

## Appendix B: Sensor wiring

The light and temperature sensors, the speedo display as well as the GPS receiver are to be wired as follows:

On the TCD control board, there are currently three i2c bus breakouts, one of which has a XH header soldered on; it does not matter which one you use to connect your sensors/GPS. To avoid soldering wires directly to the board, I recommend to solder on [XH](https://www.amazon.com/s?k=jst+xh) 4-pin headers to the other two i2c breakouts as well (like in the second picture).

![i2c](https://user-images.githubusercontent.com/76924199/212406576-fbd71836-1862-4632-a3ac-decbfa6b7eb2.jpg)

![i2c](https://user-images.githubusercontent.com/76924199/212406002-c6887b39-8bac-49b1-8318-35838fba44d0.jpg)

Important: The TCD control board delivers and drives the i2c bus on 5V. Most sensors/GPS receivers operate on 3.3V. Therefore, you cannot connect the sensor chips directly to the TCD control board without a level-shifter. This goes for the power supply as well as the i2c bus signals (SDA, SCL).

I am no electronics expert and therefore only used readily available sensor breakout boards for testing and in my production pieces.

On most sensor boards the pins are named as follows, and need to be connected to the corresponding pins on the control board:

<table>
    <tr>
     <td align="center">Sensor PCB</td><td align="center">TCD control board</td>
    </tr>
    <tr>
     <td align="center">VIN or 5V</a></td>
     <td align="center">5V</td>
    </tr>
    <tr>
     <td align="center">GND</td>
     <td align="center">GND</td>
    </tr>
    <tr>
     <td align="center">SDA<br>(SDI on BME280)</td>
     <td align="center">SDA</td>
    </tr>
    <tr>
     <td align="center">SCL<br>(SCK on BME280)</td>
     <td align="center">SCL</td>
    </tr>
</table>

Do not connect anything to "3Vo".

For longer cables, ie >50cm (>20in), I recommend using a twisted pair cable, and to connect it as follows to avoid cross-talk:

![i2clongcable](https://user-images.githubusercontent.com/76924199/236399502-8c059fe4-2c1e-4aa4-9132-9ba40da34a91.png)

I had a GPS receiver connected through such a cable and had no issues over a distance of 5m/16ft.

#### i2c addresses

Most sensors either only support one i2c address, or are recognized by the firmware (only) by their default address. For those, nothing must be done in order to use them with the Time Circuits Display.

For the TMP117 and HTU31D sensors, their address needs to changed in order to be recognized by the firmware. On the Adafruit breakouts, this is done by connecting two solder pads on the back side of the PCB:

<img src="https://user-images.githubusercontent.com/76924199/211666279-c578566f-e517-4d70-b50c-929b12c57c86.png">

This image shows the HTU31D PCB's back side. Connect (shorten) those two pads in order to change the address. It looks similar on the TMP117.

#### Multiple i2c devices

If going by the book, multiple i2c devices should be daisy chained; however, I had sensors, speedo and GPS receivers run in parallel without issues for months.

Note that you can only use one per device type (temperature/humidity sensor, light sensor, GPS receiver, Speedo display) at a time.



