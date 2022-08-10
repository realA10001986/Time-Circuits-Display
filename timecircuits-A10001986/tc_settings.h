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

#ifndef _TC_SETTINGS_H
#define _TC_SETTINGS_H

#include <ArduinoJson.h>  // https://github.com/bblanchon/ArduinoJson
#include <FS.h>
#include <SPIFFS.h>

#include <EEPROM.h>

#include "tc_global.h"

extern void settings_setup();
extern void write_settings();

extern bool loadAlarm();
extern void saveAlarm();
bool loadAlarmEEPROM();
void saveAlarmEEPROM();

extern bool    alarmOnOff;
extern uint8_t alarmHour;
extern uint8_t alarmMinute;

extern bool    timetravelPersistent;

//default settings - change settings in the web interface 192.168.4.1

// For list of time zones, see https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv

struct Settings {
#ifndef TWPRIVATE
    char ntpServer[64] = "pool.ntp.org";
    char timeZone[64] = "CET-1CEST,M3.5.0/02,M10.5.0/03"; 
#else  
    char ntpServer[64] = "at.pool.ntp.org";
    char timeZone[64] = "CET-1CEST,M3.5.0/02,M10.5.0/03"; 
#endif    
    char autoRotateTimes[4] = "1";
    char destTimeBright[4] = "15";
    char presTimeBright[4] = "15";
    char lastTimeBright[4] = "15";
    //char beepSound[3] = "0";
    char wifiConRetries[4] = "3";   // Default: 3 retries
    char wifiConTimeout[4] = "7";   // Default: 7 seconds time-out
    char mode24[4] = "0";           // Default: 0 = 12-hour-mode
#ifndef TWPRIVATE    
    char timesPers[4] = "1";        // Default: TimeTravel persistent (like before) 
#else
    char timesPers[4] = "0";        // My default: TimeTravel not persistent
#endif
#ifdef FAKE_POWER_ON 
    char fakePwrOn[4] = "0";       
#endif
};

extern struct Settings settings;

#endif
