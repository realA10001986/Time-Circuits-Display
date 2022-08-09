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

#ifndef CLOCKDISPLAY_H
#define CLOCKDISPLAY_H

#include <Arduino.h>
#include <RTClib.h>
#include <Wire.h>
#include <EEPROM.h>

#include "tc_global.h"
#include "tc_settings.h"

#define DS3231_I2CADDR 0x68

// Uncomment if  month is 2 digits, as per the original A Car display.
// Support is incomplete, as menu system expects month to be literal.
//define IS_ACAR_DISPLAY 

extern bool alarmOnOff;
extern bool presentTimeBogus;
 
struct dateStruct {
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
};

class clockDisplay {
   public:
    clockDisplay(uint8_t address, int saveAddress);
    void begin();
    void lampTest();
    void on();
    void off();
    void clear();
    uint8_t setBrightness(uint8_t level);
    uint8_t getBrightness();

    void set1224(bool hours24);
    bool get1224();

    void setNightMode(bool mode);
    bool getNightMode();
    
    void setMonth(int monthNum);
    void setDay(int dayNum);
    void setYear(uint16_t yearNum);
    void setYearOffset(int16_t yearOffs);
    void setYearDirect(uint16_t yearNum);
    void setHour(uint16_t hourNum);
    void setMinute(int minNum);
    void setColon(bool col);
    void colonOn();
    void colonOff();

    uint8_t getMonth();
    uint8_t getDay();
    uint16_t getYear();
    uint8_t getHour();
    uint8_t getMinute();
    int16_t getYearOffset();

    void setRTC(bool rtc);  // make this an RTC display
    bool isRTC();

    void AM();
    void PM();
    void AMPMoff();

    void showOnlyMonth(int monthNum);  // Show only the supplied month, do not modify object's month
    void showOnlyDay(int dayNum);
    void showOnlyHour(int hourNum);
    void showOnlyMinute(int minuteNum);
    void showOnlyYear(int yearNum);

    void showOnlySettingVal(const char* setting, int8_t val = -1, bool clear = false);
    void showOnlySave();
    void showOnlyUtes();    
    void showOnlyHalfIP(int a, int b, bool clear = false);

    void setDateTime(DateTime dt);  // Set object date & time using a DateTime
    void setFromStruct(dateStruct* s);
    void setDS3232time(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year);

    //DateTime getDateTime();  // Return object's date & time as a DateTime

    void show();
    void showAnimate1();
    void showAnimate2();

    bool save();
    bool load();
    bool resetClocks();
    bool isPrefData(const char* key);

   private:
    uint8_t _address;
    int _saveAddress;
    uint16_t _displayBuffer[8]; // Segments to make current time.

    uint16_t _year = 2021;      // keep track of these
    int16_t  _yearoffset = 0;   // Offset for faking years < 2000 or > 2159
    
    uint8_t _month = 1;
    uint8_t _day = 1;
    uint8_t _hour = 0;
    uint8_t _minute = 0;
    bool _colon = false;        // should colon be on?
    bool _rtc = false;          // will this be displaying real time
    uint8_t _brightness = 15;   // display brightness
    bool _mode24 = false;       // true = 24 hour mode, false 12 hour mode
    bool _nightmode = false;    // true = dest/dept times off
    int _oldnm = -1;

    uint8_t getLED7SegChar(uint8_t value);
    uint16_t getLEDAlphaChar(char value);

    uint16_t makeNum(uint8_t num);
    uint16_t makeNumN0(uint8_t num);
    uint16_t makeAlpha(uint8_t value);

    void clearDisplay();                    // clears display RAM
    void showInt(bool animate = false); 
    void directCol(int col, int segments);  // directly writes column RAM

    void directAMPM(int val1, int val2);
    void directAM();
    void directPM();
    void directAMPMoff();
    
    byte decToBcd(byte val);
};

#endif
