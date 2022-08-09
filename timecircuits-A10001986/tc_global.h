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
 */


#ifndef _TC_GLOBAL_H
#define _TC_GLOBAL_H

//#define TC_DBG         // more debug output on Serial

// EEPROM map
// We use 1(padded to 8) + 10*3 + 4 bytes of EEPROM space at 0x0. 
#define AUTOINTERVAL_PREF 0x00    // autoInterval save location   (1 byte, padded 8; unused)
#define DEST_TIME_PREF    0x08    // destination time prefs       (10 bytes)
#define PRES_TIME_PREF    0x12    // present time prefs           (10 bytes; unused)
#define DEPT_TIME_PREF    0x1c    // departure time prefs         (10 bytes)
#define ALARM_PREF        0x26    // alarm prefs                  (4 bytes; only used if fs unavailable)

//#define TWPRIVATE       // A10001986's private customizations

#endif
