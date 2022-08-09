# Time-Circuits-Display-A10001986
Alternative firmware for ESP32-based Time-Circuits-Display

This is a fork of the firmware for [this](https://circuitsetup.us/product/complete-time-circuits-display-kit/) wonderful piece of hardware:

https://github.com/CircuitSetup/Time-Circuits-Display/blob/master/README.md

![TCD Front](https://raw.githubusercontent.com/CircuitSetup/Time-Circuits-Display/master/Images/tcd_front2.jpg)


"This Time Circuits Display has been meticulously reproduced to be as accurate as possible to the one seen in the Delorean Time Machine in the Back to the Future movies. The LED displays are custom made to the correct size for CircuitSetup. This includes the month 14 segment/3 character displays being closer together, and both the 7 & 14 segment displays being 0.6" high by 0.35" wide.

The Destination Time can be entered via keypad, and the Present Time can keep time via NTP. There is also a time travel mode, which moves the Destination Time to Present Time, and Present Time to Last Time Departed. The startup, keypad dial sounds, and time travel sounds are played using I2S." 

**Changes to the original firmware (based on 2022-01 version):**
- Return from Time Travel (hold "9" for 2 seconds)
- "Present time" is a clock (not stale) after time travel
- Keypad menu for adjusting various settings
- Support for time zones and automatic DST
- More stable WiFi connections at startup
- Configurable WiFi connection timeouts and retries
- Alarm function
- Night mode (displays off or dimmed)
- Network keypad menu to show the current IP address
- 24-hour clock mode for non-Americans ;)
- Fixed "month-off-by-one" bug, and others
- etc

**Short summary of first steps:**
- As long as the device is unconfigured or later for some reason fails to connect to a configured network, it starts in "access point" mode, i.e. it creates an access point named "TCD-AP". The default IP in access point mode is 192.168.4.1. 
- Go to http://192.168.4.1 to enter the config portal, the go to the "WiFi Setup" page and configure your Wifi network.
- After saving the settings, the device reboots and tries to connect to your configured WiFi network. If it fails to connect to your network, the device will again start in access point mode ("TCD-AP").
- Next, find out about the IP address assigned to the clock. Hold ENTER on the keypad for 2 seconds, then press ENTER until "NET-WRK" is shown, then hold ENTER for 2 seconds. The clock will then show the current IP address.
- Then re-open the config portal using your IP address, and configure other settings on the "Setup" page, eg WiFi connection timeout, WiFi connection retries, NTP server and time zone. A list of valid times zones is accessible by a link on the settings page.
- There are two ways of configuring the clock: Through the aforementioned config portal (ie the network), or the keypad menu, as described further below. Not all settings, however, are configurable in both.

**Operation basics**

*Present time* is a clock and, normally, shows the actual present time, as received from the network.

*Destination time* and *Last time departed* are stale. You can have the clock either show your favorite times here (which need to be set up through the keypad menu), or cycle through a list of pre-programmed times, which cannot be changed. 

The mode of operation is chosen by setting the "Time-rotation Interval" in the config portal (via network) or the keypad menu. If this interval is 0, your times are permanently shown. If the inverval is non-zero, the device will cycle through named list every 5th, 15th, 30th or 60th minute.

**Time travel**

To travel through time, hold "0" for 2 seconds. The destination time, as shown in the red display, will be your new present time, the old "present time" will be the "last time departed". Note that the new "present" time will continue to run like a normal clock.

Before holding "0", you can also first quickly set a new destination time by entering a date on the keypad: mmddyyyy or mmddyyyyhhmm, then press ENTER. While typing, there is no visual feedback, but the date is then shown on the "destination time" display after pressing ENTER. (If the Time-rotation Interval is non-zero, your entered time will, however, be overwritten at the next 5th/15th/30th/60th minute and the next pre-programmed time will be put in place.)

To travel back to actual present time, hold "9" for 2 seconds.

Regardless of your Time-rotation Interval setting, the *last time departed* display will be updated in the course of a time travel event. In case the Time-rotation Interval is 0, the *last time departed* will only ever change in case of another trip through time. If the Time-rotation Interval is non-zero, "last time departed" will resume cycling through the pre-programmed list at the next 5th/15th/30th/60th minute and the next pre-programmed time will be put in place.

Beware that the alarm function is based on whatever your current "present" time is. The alarm will sound when the displayed "present time"'s hour and minute match the alarm time, be it actual present time or "fake".

**Persistent / Non-persistent time travels**

On the network-accessible settings page, there is a menu item named "Make time travels persistent". The default is yes.

If time travels are persistent
- a user-programmed *destination time* is always stored in flash memory, and retrieved from there after a power-loss. It can be programmed through the keypad menu, or ahead of a time travel by typing mmddyyyy/mmddyyyyhhmm plus ENTER. In both cases, the time is stored in flash memory.
- *last time departed* as displayed at any given point is always stored in flash memory, and retrieved from there after a power-loss.
- *present time*, whether actual present time or "fake" after time travelling, will continue to run while the device is not powered, as long as its battery lasts, and displayed on power-up.

If time travels are non-persistent
- a user-programmed *destination time* is only stored to flash memory when programmed through the keypad menu, but not if entered ahead of a time travel (ie outside of the keypad menu, just by typing mmddyyyy/mmddyyyyhhtt plus ENTER). Upon power-up, the time stored in flash memory is displayed.
- user-programmed *last time departed*s is only stored to flash memory when programmed through the keypad menu, but not if the result of a time travel. Upon power-up, the time stored in flash memory is displayed.
- *present time* is always reset to actual present time upon power-up.

If you want your clock to display exactly the same after a power loss, choose persistent. 

If you want to display your favorite *destination time* and *last time departed* upon power-up, and not have time travels overwrite them in flash memory, choose "non-persistent", and program your times through the keypad menu (and set the Time-rotation Interval to 0). Those times will never be overwritten in flash memory by later time travels. Note, however, that the times displayed might actually change due to time travels.

**Night mode**

Night mode will switch off the *destination time* and *last time departed* displays, dim the *present time* display to a minimum, and reduce the volume of sound playback (except alarm). Holding "4" enables night mode, holding "5" disables it. 

**The keypad menu**
 
The menu is controlled by "pressing" or "holding" the ENTER key on the keypad.

Note: A "press" is shorter than 2 seconds, a "hold" is 2 seconds or longer.
Data entry is done by pressing the keypad's number keys.

The menu is involked by holding the ENTER button.

First step is to choose a menu item. The available "items" are   
- enter custom dates/times for the three displays,
- set an alarm ("ALA-RM"),
- select the Time-rotation Interval ("ROT-INT"),
- select the brightness for the three displays ("BRI-GHT"),
- show the current IP address ("NET-WRK"),
- quit the menu ("END").
 
Pressing ENTER cycles through the list, holding ENTER selects an item, ie a mode.
 
How to enter custom dates/times:
- Hold ENTER to invoke main menu
- Press ENTER until the desired display is the only one lit
- Hold ENTER until the display goes off except for the first field to enter data into
- The field to enter data into is shown (exclusively), pre-set with its current value
- Data entry works as follows: If you want to keep the currently shown pre-set, press ENTER to proceed to next field. Otherwise press a digit on the keypad; the pre-set is then overwritten by the value entered. 2 or 4 digits can be entered, upon which the current value is stored and the next field is activated. You can also enter less than 2/4 digits and press ENTER when done with the field. Note that the month needs to be entered numerically (1-12), and the hour needs to be entered in 24 hour mode, regardless of 12-hour or 24-hour mode as per the config portal setting.
- After entering data into all fields, the data is saved and the menu is left automatically.
- Note that after entering dates/times into the *destination time* or *last time departed* displays, the Time-rotation Interval is set to 0 and your entered date/time(s) are shown permanently (see below, section "How to select the Time-rotation Interval").
- If you entered a custom date/time into the *present time* display, this time is then used as a starting point, and continues to run like a clock. (As opposed to the "destination" and "last time departed" times, which are stale.) 

How to set up the alarm:
- Hold ENTER to invoke main menu
- Press ENTER until "ALA-RM" is shown
- Hold ENTER
- Press ENTER to toggle the alarm on and off, hold ENTER to proceed
- Then enter the hour and minutes. This works as described above.
- The menu is left automatically after entering the minute. "SAVE" is displayed briefly.

Under normal operation (ie outside of the menu), holding "1" enables the alarm, holding "2" disables it. 

Note that the alarm is recurring, ie it rings every day at the programmed time, unless disabled. Also note, as mentioned, that the alarm is relative to your *present time*, whether it is actual time, or some "fake" time after a time travel.

If the alarm is set and enabled, the dot in the present time's minute field is lit. 
 
How to select the Time-rotation Interval:
- Hold ENTER to invoke main menu
- Press ENTER until "ROT-INT" is shown
- Hold ENTER, "INT" is displayed
- Press ENTER to cycle through the possible Time-rotation Interval values.
- A value of 0 makes the device show your custom "destination" and "last departure" times permanently (CUS-TOM is displayed as a reminder).
- Non-zero values make the device cycle through a number of pre-programmed times, your custom times are ignored. The value means "minutes" (hence "MIN-UTES") between changes.              
- Hold ENTER to select the value shown and exit the menu ("SAVE" is displayed briefly)
 
How to adjust the display brightness:
- Hold ENTER to invoke main menu
- Press ENTER until "BRI-GHT" is shown
- Hold ENTER, the displays show all elements, the top-most display says "LVL"
- Press ENTER to cycle through the possible levels (1-5)
- Hold ENTER to use current value and proceed to next display
- After the third display, "SAVE" is displayed briefly and the menu is left automatically.
 
How to find out the IP address:
- Hold ENTER to invoke main menu
- Press ENTER until "NET-WRK" is shown
- Hold ENTER, the displays shows the IP address
- Hold ENTER to leave the menu
 
 How to leave the menu:
 - While the menu is active, press ENTER until "END" is displayed.
 - Hold ENTER to leave the menu
 

