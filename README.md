# Time-Circuits-Display-A10001986
Alternative firmware for ESP32-based Time-Circuits-Display

This is a fork of the firmware for [this](https://circuitsetup.us/product/complete-time-circuits-display-kit/) wonderful piece of hardware:

https://github.com/CircuitSetup/Time-Circuits-Display/blob/master/README.md

![TCD Front](https://raw.githubusercontent.com/CircuitSetup/Time-Circuits-Display/master/Images/tcd_front2.jpg)


"This Time Circuits Display has been meticulously reproduced to be as accurate as possible to the one seen in the Delorean Time Machine in the Back to the Future movies. The LED displays are custom made to the correct size for CircuitSetup. This includes the month 14 segment/3 character displays being closer together, and both the 7 & 14 segment displays being 0.6" high by 0.35" wide.

The Destination Time can be entered via keypad, and the Present Time can keep time via NTP. There is also a time travel mode, which moves the Destination Time to Present Time, and Present Time to Last Time Departed. The startup, keypad dial sounds, and time travel sounds are played using I2S." 

Changes to the original firmware (based on 2022-01 version):
- Return from Time Travel (hold "9" for 2 seconds)
- "present time" is a clock (not stale) after time travel
- Keypad menu for adjusting various settings
- uses time zones, automatic DST
- fixed "month-off-by-one" bug, and others
- More stable WiFi connections at startup
- etc

Short summary:
- Default IP is 192.168.4.1, access point name "TCD-AP".
- In the network config portal, first configure your Wifi.
- After saving the settings, the device reboots and tries to connect to your configured WiFi network. If you entered wrong data, or the network is not available, this can take a while. You can shorten the retry period by changing the parameters in tc_wifi.cpp.
- Then re-open the config portal (first you need to find out about the IP address, of course) and configure other settings, eg NTP server and time zone. A list of valid times zones is accessible by a link on the settings page.

Time travel

To travel in time, first enter a date on the keypad: mmddyyyy or mmddyyyyhhmm, then press ENTER. The date is then shown on the "destination time" display. Hold "0" on the keypad for 2 seconds, and the actual travel will take place. The old present time is now in the "departed" time, the destination time is now "present" time, and continues to run like a normal clock.

To return to actual present time, hold "9" for 2 seconds. NTP time will be updated, and put into "present" time.

The keypad menu
 
The menu is controlled by "pressing" or "holding" the ENTER key on the keypad.

Note: A "press" is shorter than 2 seconds, a "hold" is 2 seconds or longer.
Data entry is done via the keypad's number keys.

The menu is involked by holding the ENTER button.
First step is to choose a menu item. The available "items" are
   - enter custom dates/times for the three displays
   - select the autoInterval ("PRE-SET")
   - select the brightness for the three displays ("BRI-GHT")
   - quit the menu ("END")
 Pressing ENTER cycles through the list, holding ENTER selects an item, ie a mode.
 If mode is "enter custom dates/times":
     - the field to enter data into is shown (exclusively), with its current value
     - 2 or 4 digits can be entered, or ENTER can be pressed, upon which the next field is activated.
       (Note that the month needs to be entered numerically, and the hour needs to be entered in 24
       hour mode.)
     - After entering data into all fields, the data is saved and the menu is left automatically.
     - Note that after entering dates/times into the "destination" or "last departure" displays,
       autoInterval is set to 0 and your entered date/time(s) are shown permanently (see below).
     - If you entered a custom date/time into the "present" time display, this time is then
       used as actual the present time, and continues to run like a clock. (As opposed to the 
       "destination" and "last departure" times, which are stale.)
 If mode is "select AutoInterval" (display shows "INT")
     - Press ENTER to cycle through the possible autoInverval settings.
     - Hold ENTER for 2 seconds to select the shown value and exit the menu ("SAVE" is displayed briefly)
     - 0 makes your custom "destination" and "last departure" times to be shown permanently.
       (CUS-TOM is displayed as a reminder)
     - Non-zero values make the clock cycle through a number of pre-programmed times, your
       custom times are ignored. The value means "minutes" (hence "MIN-UTES").               
 If mode is "select brightness" (display shows "LVL")
     - Press ENTER to cycle through the possible levels (1-5)
     - Hold ENTER to use current value and jump to next display
     - After the third display, "SAVE" is displayed briefly and the menu is left automatically.
 If mode is "END"
     - Hold ENTER to quit the menu

