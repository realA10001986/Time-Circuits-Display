/*
 * -------------------------------------------------------------------
 * CircuitSetup.us Time Circuits Display
 * (C) 2022 Thomas Winischhofer (A10001986)
 *
 * GPS receiver handling and data parsing
 *
 * This is designed for MTK3333-based modules.
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
 */

#ifndef _tcGPS_H
#define _tcGPS_H

#include <Arduino.h>
#include <Wire.h>

#define GPS_MAX_I2C_LEN   255

#define MAXLINELENGTH     256

#define GPS_MPH_PER_KNOT  1.15077945
#define GPS_KMPH_PER_KNOT 1.852

class tcGPS : public Print {

    public:

        tcGPS(uint8_t address);
        bool begin();

        // Setter for custom delay function
        void setCustomDelayFunc(void (*myDelay)(unsigned int));

        void loop();

        int16_t getSpeed();
        bool    getDateTime(struct tm *timeInfo, time_t *fixAge);
        bool    setDateTime(struct tm *timeinfo);

        int16_t speed = -1;
        bool    fix = false;

    private:

        uint8_t _address;

        uint8_t _lenArr[32] = { 32, 32, 32, 32, 32, 32, 32, 31 };
        int     _lenIdx = 0;

        char    _buffer[GPS_MAX_I2C_LEN];
        char    _last_char = 0;

        char    _line1[MAXLINELENGTH];
        char    _line2[MAXLINELENGTH];
        char    *_currentline;
        char    *_lastline;
        uint8_t _lineidx = 0;
        bool    _lineready = false;

        bool    _haveSpeed = false;
        unsigned long _curspdTS = 0;

        char    _curTime[8]   = { 0, 0, 0, 0, 0, 0, 0, 0 };
        char    _curDay[4]    = { 0, 0, 0, 0 };
        char    _curMonth[4]  = { 0, 0, 0, 0 };
        char    _curYear[6]   = { 0, 0, 0, 0, 0, 0 };
        char    _curTime2[8]  = { 0, 0, 0, 0, 0, 0, 0, 0 };
        char    _curDay2[4]   = { 0, 0, 0, 0 };
        char    _curMonth2[4] = { 0, 0, 0, 0 };
        char    _curYear2[6]  = { '2', '0', 0, 0, 0, 0 };
        unsigned long _curTS = 0;
        unsigned long _curTS2 = 0;
        bool    _haveDateTime = false;
        bool    _haveDateTime2 = false;

        bool    read(void);
        size_t  write(uint8_t);

        void    sendCommand(const char *);

        bool    checkNMEA(char *nmea);
        bool    parseNMEA(char *nmea);
        char    *lastNMEA(void);

        // Ptr to custom delay function
        void (*_customDelayFunc)(unsigned int) = NULL;
};

#endif
