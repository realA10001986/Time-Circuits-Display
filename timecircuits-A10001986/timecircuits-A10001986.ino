/*
 * -------------------------------------------------------------------
 * CircuitSetup.us Time Circuits Display
 * Code adapted from Marmoset Electronics 
 * https://www.marmosetelectronics.com/time-circuits-clock
 * by John Monaco
 * Enhanced/modified in 2022 by Thomas Winischhofer (A10001986)
 * -------------------------------------------------------------------
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 */

/* Changelog 
 *  
 *  2022/08/09 (A10001986)
 *    - Fix "animation" (ie. month displayed a tad delayed)
 *    - Added night mode; enabled by holding "4", disabled by holding "5"
 *    - Fix for flakey i2c connection to RTC (check data and retry)
 *      Sometimes the RTC chip loses sync and no longer responds, this
 *      happens rarely and is still under investigation.
 *    - Quick alarm enable/disable: Hold "1" to enable, "2" to disable
 *    - If alarm is enabled, the dot in present time's minute field is lit
 *    - Selectable "persistent" time travel mode (WiFi Setup page):
 *        If enabled, time travel is persistent, which means all times
 *        changed during a time travel are saved to EEPROM, overwriting 
 *        user programmed times. In persistent mode, the fake present time  
 *        also continues to run during power loss, and is NOT reset to 
 *        actual present time upon restart.
 *        If disabled, user programmed times are never overwritten, and
 *        time travels are not persistent. Present time will be reset
 *        to actual present time after power loss.
 *    - Alarm data is now saved to file system, no longer to EEPROM
 *      (reduces wear on flash memory)
 *  2022/08/03-06 (A10001986)
 *    - Alarm function added
 *    - 24-hour mode added for non-Americans (though not authentic at all)
 *    - Keypad menu item to show IP address added
 *    - Configurable WiFi connection timeouts and retries
 *    - Keypad menu re-done
 *    - "Present time" is a clock (not stale) after time travel
 *    - Return from Time Travel (hold "9" for 2 seconds)
 *    - Support for time zones and automatic DST
 *    - More stable sound playback
 *    - various bug fixes
 */

#include <Arduino.h>
#include "clockdisplay.h"
#include "tc_audio.h"
#include "tc_keypad.h"
#include "tc_menus.h"
#include "tc_time.h"
#include "tc_wifi.h"
#include "tc_settings.h"

void setup() 
{
    Serial.begin(115200);
    //delay(1000);  // wait for console opening

    Wire.begin();
    //scan();
    Serial.println();

    settings_setup();
    wifi_setup();
    audio_setup();

    menu_setup();
    keypad_setup();
    time_setup();
}


void loop() 
{
    keypad_loop();
    get_key();
    time_loop();
    wifi_loop();
    audio_loop();
}

//for testing I2C connections and addresses
/*
void scan() 
{
    Serial.println(" Scanning I2C Addresses");
    uint8_t cnt = 0;
    for (uint8_t i = 0; i < 128; i++) {
        Wire.beginTransmission(i);
        uint8_t ec = Wire.endTransmission(true);
        if (ec == 0) {
            if (i < 16) Serial.print('0');
            Serial.print(i, HEX);
            cnt++;
        } else
            Serial.print("..");
        Serial.print(' ');
        if ((i & 0x0f) == 0x0f) Serial.println();
    }
    Serial.print("Scan Completed, ");
    Serial.print(cnt);
    Serial.println(" I2C Devices found.");
}

bool i2cReady(uint8_t adr) 
{
    uint32_t timeout = millis();
    bool ready = false;
    while ((millis() - timeout < 100) && (!ready)) {
        Wire.beginTransmission(adr);
        ready = (Wire.endTransmission() == 0);
    }
    return ready;
}
*/
