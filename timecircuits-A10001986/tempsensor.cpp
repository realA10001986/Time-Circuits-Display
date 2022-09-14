/*
 * -------------------------------------------------------------------
 * CircuitSetup.us Time Circuits Display
 * (C) 2022 Thomas Winischhofer (A10001986)
 * 
 * Optional Temperature Sensor
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

#include "tc_global.h"

#ifdef TC_HAVETEMP

#include "tempsensor.h"

static void defaultDelay(unsigned int mydelay);

// Mode Resolution SampleTime
//  0    0.5°C       30 ms
//  1    0.25°C      65 ms
//  2    0.125°C     130 ms
//  3    0.0625°C    250 ms
#define TC_TEMP_RESOLUTION 0
uint16_t wakeDelay[4] = { 30, 65, 130, 250 };


// Store i2c address
tempSensor::tempSensor(uint8_t address) 
{    
    _address = address;
}

// Start the display
bool tempSensor::begin() 
{   
    _customDelayFunc = defaultDelay;
    
    // Check if supported sensor is connected
    if(read16(MCP9808_REG_MANUF_ID) != 0x0054)
        return false;
    if(read16(MCP9808_REG_DEVICE_ID) != 0x0400)
        return false;

    // Write config register
    write16(MCP9808_REG_CONFIG, 0x0);

    // Set resolution
    write8(MCP9808_REG_RESOLUTION, TC_TEMP_RESOLUTION & 0x03);
    
    on();

    return true;
}

void tempSensor::setCustomDelayFunc(void (*myDelay)(unsigned int))
{
    _customDelayFunc = myDelay;
}

// Turn on the sensor
void tempSensor::on() 
{
    onoff(false);
}

// Turn off the sensor
void tempSensor::off() 
{
    onoff(true);
}


double tempSensor::readTemp(bool celsius)
{
    double temp = NAN;
    uint16_t t = read16(MCP9808_REG_AMBIENT_TEMP);

    if(t != 0xffff) {
        temp = ((double)(t & 0x0fff)) / 16.0;        
        if(t & 0x1000) temp = 256.0 - temp;
        if(!celsius) temp = temp * 9.0 / 5.0 + 32.0;
    }

    return temp;
}

// Private functions ###########################################################

void tempSensor::onoff(bool shutDown)
{
    uint16_t temp = read16(MCP9808_REG_CONFIG);
    
    if(shutDown) {
        temp |= MCP9808_CONFIG_SHUTDOWN;
    } else {
        temp &= ~MCP9808_CONFIG_SHUTDOWN;
    }

    temp &= ~(MCP9808_CONFIG_CRITLOCKED|MCP9808_CONFIG_WINLOCKED);
    
    write16(MCP9808_REG_CONFIG, temp);
    
    if(!shutDown) {
        (*_customDelayFunc)(wakeDelay[TC_TEMP_RESOLUTION]);
    }
}

uint16_t tempSensor::read16(uint16_t regno)
{
    uint16_t value = 0;

    Wire.beginTransmission(_address);
    Wire.write((uint8_t)(regno & 0x0f));
    Wire.endTransmission(false);
    
    Wire.requestFrom(_address, (uint8_t)2);
    
    value = Wire.read() << 8;    
    value |= Wire.read();
   
    return value;
}

uint8_t tempSensor::read8(uint16_t regno) 
{
    uint16_t value = 0;

    Wire.beginTransmission(_address);
    Wire.write((uint8_t)(regno & 0x0f));
    Wire.endTransmission(false);
    
    Wire.requestFrom(_address, (uint8_t)1);
       
    value = Wire.read();
   
    return value;
}

void tempSensor::write16(uint16_t regno, uint16_t value) 
{
    Wire.beginTransmission(_address);
    Wire.write((uint8_t)(regno & 0x0f));
    Wire.write((uint8_t)(value >> 8));
    Wire.write((uint8_t)(value & 0xff));
    Wire.endTransmission();
}

void tempSensor::write8(uint16_t regno, uint8_t value) 
{
    Wire.beginTransmission(_address);
    Wire.write((uint8_t)(regno & 0x0f));
    Wire.write((uint8_t)(value & 0xff));
    Wire.endTransmission();
}

static void defaultDelay(unsigned int mydelay)
{
    delay(mydelay);
}
#endif
