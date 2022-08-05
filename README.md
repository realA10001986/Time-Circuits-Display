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

**Modes of operation**

*Present time* basically shows the actual present time, as received from the network.

*Destination time* and *Departure time* are stale. You can have the clock either show your favorite times here (which need to be set up through the keypad menu), or cycle through a list of pre-programmed times, which cannot be changed. 

The mode of operation is chosen by setting "autoInterval" in the config portal (via network) or the keypad menu. If this autoInterval is 0, your times are permanently shown. If autoInverval is non-zero, the clock will cycle through named list with changes every "autoInterval" minutes.

**Time travel**

To travel through time, first enter a date on the keypad: mmddyyyy or mmddyyyyhhmm, then press ENTER. While typing, there is no visual feedback, but the date is then shown on the "destination time" display after pressing ENTER. Hold "0" on the keypad for 2 seconds, and the actual travel will take place. The old present time is now in the "departed" time, the destination time is now "present" time, and continues to run like a normal clock.

To return to actual present time, hold "9" for 2 seconds.

**The keypad menu**
 
The menu is controlled by "pressing" or "holding" the ENTER key on the keypad.

Note: A "press" is shorter than 2 seconds, a "hold" is 2 seconds or longer.
Data entry is done by pressing the keypad's number keys.

The menu is involked by holding the ENTER button.

First step is to choose a menu item. The available "items" are   
- enter custom dates/times for the three displays,
- set an alarm ("ALA-RM"),
- select the autoInterval ("PRE-SET"),
- select the brightness for the three displays ("BRI-GHT"),
- show the current IP address ("NET-WRK"),
- quit the menu ("END").
 
Pressing ENTER cycles through the list, holding ENTER selects an item, ie a mode.
 
How to enter custom dates/times:
- Hold ENTER to invoke main menu
- Press ENTER until the desired display is the only one lit
- Hold ENTER until the display goes off except for the first field to enter data into
- The field to enter data into is shown (exclusively), pre-set with its current value
- Data entry works as follows: If you want to keep the currently shown pre-set, press ENTER to proceed to next field. Otherwise press a digit on the keypad; the pre-set is then overwritten by the value entered. 2 or 4 digits can be entered, or ENTER can be pressed, upon which the current value is stored and the next field is activated. Note that the month needs to be entered numerically (1-12), and the hour needs to be entered in 24 hour mode, regardless of 12-hour or 24-hour mode as per the config portal setting.
- After entering data into all fields, the data is saved and the menu is left automatically.
- Note that after entering dates/times into the "destination" or "last departure" displays, autoInterval is set to 0 and your entered date/time(s) are shown permanently (see below).
- If you entered a custom date/time into the "present" time display, this time is then used as actual the present time, and continues to run like a clock. (As opposed to the "destination" and "last departure" times, which are stale.) Also, the present time is not stored. When the clock is powered down, the present time will return to the actual present time upon power up.

How to set up the alarm:
- Hold ENTER to invoke main menu
- Press ENTER until "ALA-RM" is shown
- Hold ENTER
- Press ENTER to toggle the alarm on and off, hold ENTER to proceed
- Then enter the hour and minutes. This works as described above.
- The menu is left automatically after entering the minute.
Note that the alarm is recurring, ie it rings every day at the programmed time. It must be switched off in the keypad menu.
 
How to select the autoInterval:
- Hold ENTER to invoke main menu
- Press ENTER until "PRE-SET" is shown
- Hold ENTER, "INT" is displayed
- Press ENTER to cycle through the possible autoInverval settings.
- 0 makes the clock show your custom "destination" and "last departure" times permanently (CUS-TOM is displayed as a reminder).
- Non-zero values make the clock cycle through a number of pre-programmed times, your custom times are ignored. The value means "minutes" (hence "MIN-UTES") between changes.              
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
 

