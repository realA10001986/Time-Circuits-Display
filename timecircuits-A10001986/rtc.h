/*
 * -------------------------------------------------------------------
 * Cut-down and customized fork of Adafruit's RTClib
 * 
 * Handles DS3231/PCF2129 RTC and provides DateTime Class
 * 
 * Note: DateTime mirrors the features of RTC; 
 * this means it only works for dates from 2000 to 2099.
 * -------------------------------------------------------------------
 * License: MIT
 * Modifications (C) Thomas Winischhofer (A10001986)
 * 
 * For complete version and credits, see:
 * https://github.com/adafruit/RTClib
 */

#ifndef _RTC_H_
#define _RTC_H_

#include <Arduino.h>
#include <Wire.h>

/*****************************************************************
 * DateTime Class
 * 
 * Simple general-purpose date/time class 
 * (no TZ / DST / leap seconds).
 * Supports dates in the range from 1 Jan 2000 to 31 Dec 2099.
 ****************************************************************/
 
enum timestampOpt {
    TIMESTAMP_FULL, // `YYYY-MM-DDThh:mm:ss`
    TIMESTAMP_TIME, // `hh:mm:ss`
    TIMESTAMP_DATE  // `YYYY-MM-DD`
};

#define SECONDS_FROM_1970_TO_2000 946684800

class DateTime {
  
    public:
    
        DateTime(uint32_t t = SECONDS_FROM_1970_TO_2000);
        DateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour = 0,
                 uint8_t min = 0, uint8_t sec = 0);
        DateTime(const DateTime &copy);
        DateTime(const char *date, const char *time);
        DateTime(const __FlashStringHelper *date, const __FlashStringHelper *time);
        DateTime(const char *iso8601date);
        bool isValid() const;
      
        uint16_t year() const { return 2000U + yOff; }
        uint8_t month() const { return m; }
        uint8_t day() const { return d; }
        uint8_t hour() const { return hh; }
      
        uint8_t isPM() const { return hh >= 12; }
        
        uint8_t minute() const { return mm; }
        uint8_t second() const { return ss; }
      
        uint8_t dayOfTheWeek() const;
      
        uint32_t unixtime(void) const;
      
        String timestamp(timestampOpt opt = TIMESTAMP_FULL) const;
        
        bool operator<(const DateTime &right) const;

        bool operator>(const DateTime &right) const { return right < *this; }
      
        bool operator<=(const DateTime &right) const { return !(*this > right); }
      
        bool operator>=(const DateTime &right) const { return !(*this < right); }
        bool operator==(const DateTime &right) const;
      
        bool operator!=(const DateTime &right) const { return !(*this == right); }

    protected:
    
        uint8_t yOff; // Year offset from 2000
        uint8_t m;    // Month 1-12
        uint8_t d;    // Day 1-31
        uint8_t hh;   // Hours 0-23
        uint8_t mm;   // Minutes 0-59
        uint8_t ss;   // Seconds 0-59
};


/****************************************************************
 * tcRTC Class
 ****************************************************************/

enum rtcType {
    RTCT_DS3231,
    RTCT_PCF2129
};

class tcRTC
{
    public:
    
        tcRTC(int numAddr, uint8_t addrArr[]);
        
        bool begin();

        void adjust(const DateTime &dt);
        void adjust(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year);

        DateTime now();

        void clockOutEnable();
        
        bool lostPower(void);
        bool battLow(void);

        float getTemperature();

        static uint8_t dowToDS3231(uint8_t d) { return d == 0 ? 7 : d; }

    private:

        uint8_t _address;
        uint8_t _addrArr[2];

        uint8_t _rtcType = RTCT_DS3231;
    
        static uint8_t bcd2bin(uint8_t val) { return val - 6 * (val >> 4); }
        static uint8_t bin2bcd(uint8_t val) { return val + 6 * (val / 10); }
        
        uint8_t read_register(uint8_t reg);
        void    write_register(uint8_t reg, uint8_t val);
};

#endif // _RTC_H_
