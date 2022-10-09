/*
 * -------------------------------------------------------------------
 * Cut-down and customized fork of Adafruit's RTClib
 * 
 * Handles DS3231/PCF2129 RTC and provides DateTime Class
 * 
 * Note: DateTime mirrors the features of RTC; 
 * this means it only works for dates from 2000 to 2099.
 * 
 * The original version can be found here:
 * https://github.com/adafruit/RTClib
 * -------------------------------------------------------------------
 * License: MIT
 * Modifications and additions: (C) Thomas Winischhofer (A10001986)
 * Original Version: Copyright (c) 2019 Adafruit Industries
 * 
 * For original and modified version:
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "tc_global.h"
#include "rtc.h"

// Registers
#define DS3231_TIME       0x00 // Time 
#define DS3231_ALARM1     0x07 // Alarm 1 
#define DS3231_ALARM2     0x0B // Alarm 2 
#define DS3231_CONTROL    0x0E // Control
#define DS3231_STATUS     0x0F // Status
#define DS3231_TEMP       0x11 // Temperature

#define PCF2129_CTRL1     0x00 // Control 1
#define PCF2129_CTRL2     0x01 // Control 2
#define PCF2129_CTRL3     0x02 // Control 3
#define PCF2129_TIME      0x03 // Time 
#define PCF2129_ALARM     0x0A // Alarm
#define PCF2129_CLKCTRL   0x0F // CLK Control

// Other definitions
#define SECONDS_PER_DAY   86400L 


/*****************************************************************
 * DateTime Class
 * 
 * Simple general-purpose date/time class 
 * (no TZ / DST / leap seconds).
 * Supports dates in the range from 1 Jan 2000 to 31 Dec 2099.
 ****************************************************************/

const uint8_t daysInMonth[] = {31, 28, 31, 30, 31, 30,
                               31, 31, 30, 31, 30};

/*
 * Given a date, return number of days since 2000/01/01,
 * valid for 2000-2099
 */
static uint16_t date2days(uint16_t y, uint8_t m, uint8_t d) 
{
    if(y >= 2000U)
        y -= 2000U;
    
    uint16_t days = d;
    
    for(uint8_t i = 1; i < m; ++i)
        days += daysInMonth[ + i - 1];
    
    if((m > 2) && (y % 4 == 0))
        ++days;
        
    return days + (365 * y) + ((y + 3) / 4) - 1;
}

/*
 * Num of seconds from number of days, hours, minutes, and seconds
 * 
 */
static uint32_t time2ulong(uint16_t days, uint8_t h, uint8_t m, uint8_t s) 
{
    return((days * 24UL + h) * 60 + m) * 60 + s;
}

/*
 * Convert a string containing two digits to uint8_t
 */
static uint8_t conv2d(const char *p) 
{
    uint8_t v = 0;
    
    if('0' <= *p && *p <= '9')
        v = *p - '0';

    return 10 * v + *++p - '0';
}

/*
 * Constructor from Unix time
 * (Time elapsed in seconds since 1970-01-01 00:00:00)
 */
DateTime::DateTime(uint32_t t) 
{
    t -= SECONDS_FROM_1970_TO_2000; // bring to 2000 timestamp from 1970

    ss = t % 60;
    t /= 60;
    mm = t % 60;
    t /= 60;
    hh = t % 24;
    uint16_t days = t / 24;
    uint8_t leap;
    
    for(yOff = 0;; ++yOff) {
        leap = yOff % 4 == 0;
        if(days < 365U + leap)
            break;
        days -= 365 + leap;
    }
    
    for(m = 1; m < 12; ++m) {
        uint8_t daysPerMonth = pgm_read_byte(daysInMonth + m - 1);
        if(leap && m == 2)
            ++daysPerMonth;
        if(days < daysPerMonth)
            break;
        days -= daysPerMonth;
    }
    
    d = days + 1;
}

/*
 * Constructor from (year, month, day, hour, minute, second) 
 */
DateTime::DateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour,
                   uint8_t min, uint8_t sec) 
{
    if(year >= 2000U)
        year -= 2000U;
    yOff = year;
    m = month;
    d = day;
    hh = hour;
    mm = min;
    ss = sec;
}

/* 
 *  Copy constructor
 */
DateTime::DateTime(const DateTime &copy)
    : yOff(copy.yOff), 
      m(copy.m), 
      d(copy.d), 
      hh(copy.hh), 
      mm(copy.mm),
      ss(copy.ss) {}

/*
 * Constructor for generating the build time
 */
DateTime::DateTime(const char *date, const char *time) 
{
    yOff = conv2d(date + 9);
    // Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec
    switch(date[0]) {
    case 'J':
        m = (date[1] == 'a') ? 1 : ((date[2] == 'n') ? 6 : 7);
        break;
    case 'F':
        m = 2;
        break;
    case 'A':
        m = date[2] == 'r' ? 4 : 8;
        break;
    case 'M':
        m = date[2] == 'r' ? 3 : 5;
        break;
    case 'S':
        m = 9;
        break;
    case 'O':
        m = 10;
        break;
    case 'N':
        m = 11;
        break;
    case 'D':
        m = 12;
        break;
    }
    d  = conv2d(date + 4);
    hh = conv2d(time);
    mm = conv2d(time + 3);
    ss = conv2d(time + 6);
}

/*
 * Memory friendly constructor for generating the build time
 * 
 * DateTime buildTime(F(__DATE__), F(__TIME__));
 */
DateTime::DateTime(const __FlashStringHelper *date,
                   const __FlashStringHelper *time) 
{
    char buff[11];
    memcpy_P(buff, date, 11);
    yOff = conv2d(buff + 9);
    // Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec
    switch (buff[0]) {
    case 'J':
        m = (buff[1] == 'a') ? 1 : ((buff[2] == 'n') ? 6 : 7);
        break;
    case 'F':
        m = 2;
        break;
    case 'A':
        m = buff[2] == 'r' ? 4 : 8;
        break;
    case 'M':
        m = buff[2] == 'r' ? 3 : 5;
        break;
    case 'S':
        m = 9;
        break;
    case 'O':
        m = 10;
        break;
    case 'N':
        m = 11;
        break;
    case 'D':
        m = 12;
      break;
    }
    d = conv2d(buff + 4);
    memcpy_P(buff, time, 8);
    hh = conv2d(buff);
    mm = conv2d(buff + 3);
    ss = conv2d(buff + 6);
}

/* 
 *  Constructor for creating a DateTime from an ISO8601 date string
 *  
 *  "2020-06-25T15:29:37"
 */
DateTime::DateTime(const char *iso8601dateTime) 
{
    char ref[] = "2000-01-01T00:00:00";

    memcpy(ref, iso8601dateTime, min(strlen(ref), strlen(iso8601dateTime)));
    yOff = conv2d(ref + 2);
    m  = conv2d(ref + 5);
    d  = conv2d(ref + 8);
    hh = conv2d(ref + 11);
    mm = conv2d(ref + 14);
    ss = conv2d(ref + 17);
}

/* 
 *  Check whether this DateTime is valid.
 */
bool DateTime::isValid() const 
{
    if(yOff >= 100)
        return false;
      
    DateTime other(unixtime());
    
    return yOff == other.yOff && 
           m == other.m && 
           d == other.d && 
           hh == other.hh &&
           mm == other.mm && 
           ss == other.ss;
}

/*
 * Return week day (0=Sun)
 */
uint8_t DateTime::dayOfTheWeek() const 
{
    uint16_t day = date2days(yOff, m, d);

    // Jan 1, 2000 was a Saturday
    return (day + 6) % 7; 
}

/*
 * Return Unix time: seconds since 1 Jan 1970.
 */ 
uint32_t DateTime::unixtime(void) const 
{
    uint32_t t;
    uint16_t days = date2days(yOff, m, d);

    t = time2ulong(days, hh, mm, ss);
    t += SECONDS_FROM_1970_TO_2000;

    return t;
}

/* 
 *  Test if one DateTime is less (earlier) than another
 */
bool DateTime::operator<(const DateTime &right) const 
{
    return (yOff + 2000U < right.year() ||
            (yOff + 2000U == right.year() &&
              (m < right.month() ||
                (m == right.month() &&
                  (d < right.day() ||
                    (d == right.day() &&
                      (hh < right.hour() ||
                        (hh == right.hour() &&
                          (mm < right.minute() ||
                            (mm == right.minute() && ss < right.second()))))))))));
}

/*
 * Test if two DateTime objects are equal.
 */
bool DateTime::operator==(const DateTime &right) const 
{
  return (right.year() == yOff + 2000U && right.month() == m &&
          right.day() == d && right.hour() == hh && right.minute() == mm &&
          right.second() == ss);
}

/*
 * Return a ISO 8601 timestamp as a 'String' object
 */
String DateTime::timestamp(timestampOpt opt) const 
{
    char buffer[25]; // large enough for any DateTime, including invalid ones

    // Generate timestamp according to opt
    switch (opt) {
    case TIMESTAMP_TIME:
        sprintf(buffer, "%02d:%02d:%02d", hh, mm, ss);
        break;
    case TIMESTAMP_DATE:
        sprintf(buffer, "%u-%02d-%02d", 2000U + yOff, m, d);
        break;
    default:
        sprintf(buffer, "%u-%02d-%02dT%02d:%02d:%02d", 2000U + yOff, m, d, hh, mm, ss);
    }

    return String(buffer);
}

/****************************************************************
 * tcRTC Class for DS3231/PCF2129
 ****************************************************************/

tcRTC::tcRTC(int numTypes, uint8_t addrArr[])
{
    _numTypes = numTypes;

    for(int i = 0; i < min(2, numTypes) * 2; i++) {
        _addrArr[i] = addrArr[i];
    }

    _address = addrArr[0];
    _rtcType = addrArr[1];
}

/*
 *  Test i2c connection and detect chip type
 */
bool tcRTC::begin()
{
    for(int i = 0; i < min(2, _numTypes) * 2; i += 2) {

        // Check for RTC on i2c bus
        Wire.beginTransmission(_addrArr[i]);

        if(!(Wire.endTransmission(true))) {

            _address = _addrArr[i];
            _rtcType = _addrArr[i+1];

            #ifdef TC_DBG
            const char *tpArr[2] = { "DS3231", "PCF2129" };
            Serial.print(F("RTC: Detected "));
            Serial.println(tpArr[_rtcType]);
            #endif

            return true;
        }
    }

    return false;
}

/*
 * Set the date/time
 *
 * (dayOfWeek: 0=Sun..6=Sat)
 */
void tcRTC::adjust(const DateTime &dt)
{
    adjust(dt.second(),
           dt.minute(),
           dt.hour(),
           dt.dayOfTheWeek(),
           dt.day(),
           dt.month(),
           dt.year() - 2000U);
}

void tcRTC::adjust(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year)
{
    uint8_t buffer[8];
    uint8_t statreg;

    switch(_rtcType) {

    case RTCT_PCF2129:
        buffer[0] = PCF2129_TIME;
        buffer[1] = bin2bcd(second);
        buffer[2] = bin2bcd(minute);
        buffer[3] = bin2bcd(hour);
        buffer[4] = bin2bcd(dayOfMonth);
        buffer[5] = bin2bcd(dayOfWeek);
        buffer[6] = bin2bcd(month);
        buffer[7] = bin2bcd(year);

        Wire.beginTransmission(_address);
        for(int i = 0; i < 8; i++) {
            Wire.write(buffer[i]);
        }
        Wire.endTransmission();

        // OSF bit cleared by writing seconds
        break;

    case RTCT_DS3231:
    default:
        buffer[0] = DS3231_TIME;
        buffer[1] = bin2bcd(second);
        buffer[2] = bin2bcd(minute);
        buffer[3] = bin2bcd(hour);
        buffer[4] = bin2bcd(dowToDS3231(dayOfWeek));
        buffer[5] = bin2bcd(dayOfMonth);
        buffer[6] = bin2bcd(month);
        buffer[7] = bin2bcd(year);

        Wire.beginTransmission(_address);
        for(int i = 0; i < 8; i++) {
            Wire.write(buffer[i]);
        }
        Wire.endTransmission();   

        // clear OSF bit
        statreg = read_register(DS3231_STATUS);
        statreg &= ~0x80;
        write_register(DS3231_STATUS, statreg);
        break;
    }
}

/*
 * Get current date/time
 */
DateTime tcRTC::now() 
{
    uint8_t buffer[7];

    switch(_rtcType) {

    case RTCT_PCF2129:
        Wire.beginTransmission(_address);
        Wire.write(PCF2129_TIME); 
        Wire.endTransmission();
        Wire.requestFrom(_address, (uint8_t)7);
        for(int i = 0; i < 7; i++) {
            buffer[i] = Wire.read();
        }

        return DateTime(bcd2bin(buffer[6]) + 2000U,
                        bcd2bin(buffer[5] & 0x7F),
                        bcd2bin(buffer[3]),
                        bcd2bin(buffer[2]),
                        bcd2bin(buffer[1]),
                        bcd2bin(buffer[0] & 0x7F));

    case RTCT_DS3231:
    default:
        Wire.beginTransmission(_address);
        Wire.write(DS3231_TIME); 
        Wire.endTransmission();
        Wire.requestFrom(_address, (uint8_t)7);
        for(int i = 0; i < 7; i++) {
            buffer[i] = Wire.read();
        }

        return DateTime(bcd2bin(buffer[6]) + 2000U,
                        bcd2bin(buffer[5] & 0x7F),
                        bcd2bin(buffer[4]),
                        bcd2bin(buffer[2]),
                        bcd2bin(buffer[1]),
                        bcd2bin(buffer[0] & 0x7F));
    }
}

/*
 * Enable 1Hz clock output
 */
void tcRTC::clockOutEnable()
{
    uint8_t readValue = 0;

    switch(_rtcType) {
      
    case RTCT_PCF2129:
        Wire.beginTransmission(_address);
        Wire.write((byte)PCF2129_CLKCTRL);  
        Wire.endTransmission();

        Wire.requestFrom(_address, (uint8_t)1);
        readValue = Wire.read();
        // set squarewave to 1Hz
        // Bits 2:0 - 110  sets 1Hz
        readValue &= B11111000;
        readValue |= B11111110;

        Wire.beginTransmission(_address);
        Wire.write((byte)PCF2129_CLKCTRL);
        Wire.write(readValue);
        Wire.endTransmission();
        break;

    case RTCT_DS3231:
    default:
        Wire.beginTransmission(_address);
        Wire.write((byte)DS3231_CONTROL);
        Wire.endTransmission();

        Wire.requestFrom(_address, (uint8_t)1);
        readValue = Wire.read();
        // enable squarewave and set to 1Hz
        // Bit 2 INTCN - 0 enables wave output
        // Bit 3 and 4 - 0 0 sets 1Hz
        readValue &= B11100011;

        Wire.beginTransmission(_address);
        Wire.write((byte)DS3231_CONTROL);
        Wire.write(readValue);
        Wire.endTransmission();
        break;
    }
}

/*
 * Check if RTC is stopped due to power-loss
 */
bool tcRTC::lostPower(void)
{
    switch(_rtcType) {

    case RTCT_PCF2129:
        // Check Oscillator Stop Flag to see 
        // if RTC stopped due to power loss
        return read_register(PCF2129_TIME) >> 7;

    case RTCT_DS3231:
    default:
        // Check Oscillator Stop Flag to see 
        // if RTC stopped due to power loss
        return read_register(DS3231_STATUS) >> 7;
    }

    return false;
}

/*
 * Check for "low batt warning"
 */
bool tcRTC::battLow(void)
{
    switch(_rtcType) {

    case RTCT_PCF2129:
        return (read_register(PCF2129_CTRL3) & 0x04) >> 2;
    }

    return false;
}

/*
 * Get DS3231 temperature
 * (Not supported on PCF2129)
 */
float tcRTC::getTemperature() 
{
    uint8_t buffer[2];

    switch(_rtcType) {

    case RTCT_DS3231:
    default:
        Wire.beginTransmission(_address);
        Wire.write(DS3231_TEMP); 
        Wire.endTransmission();
        Wire.requestFrom(_address, (uint8_t)2);
  
        return (float)buffer[0] + (buffer[1] >> 6) * 0.25f;
    }

    return 0.0f;
}

/*
 * Write value to register
 */
void tcRTC::write_register(uint8_t reg, uint8_t val)
{
    Wire.beginTransmission(_address);
    Wire.write(reg);
    Wire.write(val);
    Wire.endTransmission();
}

/*
 * Read value from register
 */
uint8_t tcRTC::read_register(uint8_t reg)
{
    Wire.beginTransmission(_address);
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom(_address, (uint8_t)1);
    return Wire.read();
}
