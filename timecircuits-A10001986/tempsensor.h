/*
 * -------------------------------------------------------------------
 * CircuitSetup.us Time Circuits Display
 * (C) 2022 Thomas Winischhofer (A10001986)
 *
 * Temperature Sensor handling
 *
 * This is designed for MCP9808-based sensors.
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

#ifndef _tempSensor_H
#define _tempSensor_H

#include <Arduino.h>
#include <Wire.h>

#define MCP9808_REG_CONFIG        0x01   // MCP9808 config register
#define MCP9808_REG_UPPER_TEMP    0x02   // upper alert boundary
#define MCP9808_REG_LOWER_TEMP    0x03   // lower alert boundary
#define MCP9808_REG_CRIT_TEMP     0x04   // critical temperature
#define MCP9808_REG_AMBIENT_TEMP  0x05   // ambient temperature
#define MCP9808_REG_MANUF_ID      0x06   // manufacturer ID
#define MCP9808_REG_DEVICE_ID     0x07   // device ID
#define MCP9808_REG_RESOLUTION    0x08   // resolution

#define MCP9808_CONFIG_SHUTDOWN   0x0100  // shutdown config
#define MCP9808_CONFIG_CRITLOCKED 0x0080  // critical trip lock
#define MCP9808_CONFIG_WINLOCKED  0x0040  // alarm window lock
#define MCP9808_CONFIG_INTCLR     0x0020  // interrupt clear
#define MCP9808_CONFIG_ALERTSTAT  0x0010  // alert output status
#define MCP9808_CONFIG_ALERTCTRL  0x0008  // alert output control
#define MCP9808_CONFIG_ALERTSEL   0x0004  // alert output select
#define MCP9808_CONFIG_ALERTPOL   0x0002  // alert output polarity
#define MCP9808_CONFIG_ALERTMODE  0x0001  // alert output mode

class tempSensor {

    public:

        tempSensor(uint8_t address);
        bool begin();

        // Setter for custom delay function
        void setCustomDelayFunc(void (*myDelay)(unsigned int));

        void on();
        void off();
        double readTemp(bool celsius = true);

    private:

        void onoff(bool shutDown);

        uint8_t _address;

        uint16_t read16(uint16_t regno);
        uint8_t read8(uint16_t regno);
        void write16(uint16_t regno, uint16_t value);
        void write8(uint16_t regno, uint8_t value);

        // Ptr to custom delay function
        void (*_customDelayFunc)(unsigned int) = NULL;
};

#endif
