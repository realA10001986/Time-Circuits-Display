/*
 * -------------------------------------------------------------------
 * CircuitSetup.us Time Circuits Display
 * (C) 2022 Thomas Winischhofer (A10001986)
 * 
 * Optional Speedo Display
 * This is designed for HT16K33-based displays, like the Grove 0.54" 
 * alphanumeric displays or some displays with the Adafruit i2c 
 * backpack (878, 1911, 1270).
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

#ifdef TC_HAVESPEEDO

#include "speeddisplay.h"

const uint16_t font7segGeneric[37] = { 
    S7G_T|S7G_TR|S7G_BR|S7G_B|S7G_BL|S7G_TL,
    S7G_TR|S7G_BR,
    S7G_T|S7G_TR|S7G_B|S7G_BL|S7G_M,
    S7G_T|S7G_TR|S7G_BR|S7G_B|S7G_M,
    S7G_TR|S7G_BR|S7G_TL|S7G_M,
    S7G_T|S7G_BR|S7G_B|S7G_TL|S7G_M,
    S7G_BR|S7G_B|S7G_BL|S7G_TL|S7G_M,
    S7G_T|S7G_TR|S7G_BR,
    S7G_T|S7G_TR|S7G_BR|S7G_B|S7G_BL|S7G_TL|S7G_M,
    S7G_T|S7G_TR|S7G_BR|S7G_TL|S7G_M,
    S7G_T|S7G_TR|S7G_BR|S7G_BL|S7G_TL|S7G_M,
    S7G_BR|S7G_B|S7G_BL|S7G_TL|S7G_M,
    S7G_T|S7G_B|S7G_BL|S7G_TL,
    S7G_TR|S7G_BR|S7G_B|S7G_BL|S7G_M,
    S7G_T|S7G_B|S7G_BL|S7G_TL|S7G_M,
    S7G_T|S7G_BL|S7G_TL|S7G_M,
    S7G_T|S7G_BR|S7G_B|S7G_BL|S7G_TL,
    S7G_TR|S7G_BR|S7G_BL|S7G_TL|S7G_M,
    S7G_BL|S7G_TL,
    S7G_TR|S7G_BR|S7G_B|S7G_BL,
    S7G_T|S7G_BR|S7G_BL|S7G_TL|S7G_M,
    S7G_B|S7G_BL|S7G_TL,
    S7G_T|S7G_BR|S7G_BL,
    S7G_T|S7G_TR|S7G_BR|S7G_BL|S7G_TL,
    S7G_T|S7G_TR|S7G_BR|S7G_B|S7G_BL|S7G_TL,
    S7G_T|S7G_TR|S7G_BL|S7G_TL|S7G_M,
    S7G_T|S7G_TR|S7G_B|S7G_TL|S7G_M,
    S7G_T|S7G_TR|S7G_BL|S7G_TL,
    S7G_T|S7G_BR|S7G_B|S7G_TL|S7G_M,
    S7G_B|S7G_BL|S7G_TL|S7G_M,
    S7G_TR|S7G_BR|S7G_B|S7G_BL|S7G_TL,
    S7G_TR|S7G_BR|S7G_B|S7G_BL|S7G_TL,
    S7G_TR|S7G_B|S7G_TL,
    S7G_TR|S7G_BR|S7G_BL|S7G_TL|S7G_M,
    S7G_TR|S7G_BR|S7G_B|S7G_TL|S7G_M,
    S7G_T|S7G_TR|S7G_B|S7G_BL|S7G_M,
    S7G_DOT
};

const uint16_t font14segGeneric[37] = { 
    S14_T|S14_TL|S14_TR|S14_B|S14_BL|S14_BR,
    S14_TR|S14_BR,
    S14_T|S14_TR|S14_ML|S14_MR|S14_B|S14_BL,
    S14_T|S14_TR|S14_ML|S14_MR|S14_B|S14_BR,
    S14_TL|S14_TR|S14_ML|S14_MR|S14_BR,
    S14_T|S14_TL|S14_ML|S14_MR|S14_B|S14_BR,
    S14_TL|S14_ML|S14_MR|S14_B|S14_BL|S14_BR,
    S14_T|S14_TR|S14_BR,
    S14_T|S14_TL|S14_TR|S14_ML|S14_MR|S14_B|S14_BL|S14_BR,
    S14_T|S14_TL|S14_TR|S14_ML|S14_MR|S14_BR,
    S14_T|S14_TL|S14_TR|S14_ML|S14_MR|S14_BL|S14_BR,
    S14_T|S14_TR|S14_TV|S14_MR|S14_B|S14_BR|S14_BV,
    S14_T|S14_TL|S14_B|S14_BL,
    S14_T|S14_TR|S14_TV|S14_B|S14_BR|S14_BV,
    S14_T|S14_TL|S14_ML|S14_MR|S14_B|S14_BL,
    S14_T|S14_TL|S14_ML|S14_BL,
    S14_T|S14_TL|S14_MR|S14_B|S14_BL|S14_BR,
    S14_TL|S14_TR|S14_ML|S14_MR|S14_BL|S14_BR,
    S14_T|S14_TV|S14_B|S14_BV,
    S14_TR|S14_B|S14_BL|S14_BR,
    S14_TL|S14_TRD|S14_ML|S14_BL|S14_BRD,
    S14_TL|S14_B|S14_BL,
    S14_T|S14_TL|S14_TR|S14_TV|S14_BL|S14_BR,
    S14_TL|S14_TLD|S14_TR|S14_BL|S14_BR|S14_BRD,
    S14_T|S14_TL|S14_TR|S14_B|S14_BL|S14_BR,
    S14_T|S14_TL|S14_TR|S14_ML|S14_MR|S14_BL,
    S14_T|S14_TL|S14_TR|S14_B|S14_BL|S14_BR|S14_BRD,
    S14_T|S14_TL|S14_TR|S14_ML|S14_MR|S14_BL|S14_BRD,
    S14_T|S14_TL|S14_ML|S14_MR|S14_B|S14_BR,
    S14_T|S14_TV|S14_BV,
    S14_TL|S14_TR|S14_B|S14_BL|S14_BR,
    S14_TL|S14_TRD|S14_BL|S14_BLD,
    S14_TL|S14_TR|S14_BL|S14_BLD|S14_BR|S14_BRD,
    S14_TLD|S14_TRD|S14_BLD|S14_BRD,
    S14_TL|S14_TR|S14_ML|S14_MR|S14_B|S14_BR,
    S14_T|S14_TRD|S14_B|S14_BLD,
    S14_DOT
};    

const uint16_t font14segGrove[37] = { 
    S14GR_T|S14GR_TL|S14GR_TR|S14GR_B|S14GR_BL|S14GR_BR,
    S14GR_TR|S14GR_BR,
    S14GR_T|S14GR_TR|S14GR_ML|S14GR_MR|S14GR_B|S14GR_BL,
    S14GR_T|S14GR_TR|S14GR_ML|S14GR_MR|S14GR_B|S14GR_BR,
    S14GR_TL|S14GR_TR|S14GR_ML|S14GR_MR|S14GR_BR,
    S14GR_T|S14GR_TL|S14GR_ML|S14GR_MR|S14GR_B|S14GR_BR,
    S14GR_TL|S14GR_ML|S14GR_MR|S14GR_B|S14GR_BL|S14GR_BR,
    S14GR_T|S14GR_TR|S14GR_BR,
    S14GR_T|S14GR_TL|S14GR_TR|S14GR_ML|S14GR_MR|S14GR_B|S14GR_BL|S14GR_BR,
    S14GR_T|S14GR_TL|S14GR_TR|S14GR_ML|S14GR_MR|S14GR_BR,
    S14GR_T|S14GR_TL|S14GR_TR|S14GR_ML|S14GR_MR|S14GR_BL|S14GR_BR,
    S14GR_T|S14GR_TR|S14GR_TV|S14GR_MR|S14GR_B|S14GR_BR|S14GR_BV,
    S14GR_T|S14GR_TL|S14GR_B|S14GR_BL,
    S14GR_T|S14GR_TR|S14GR_TV|S14GR_B|S14GR_BR|S14GR_BV,
    S14GR_T|S14GR_TL|S14GR_ML|S14GR_MR|S14GR_B|S14GR_BL,
    S14GR_T|S14GR_TL|S14GR_ML|S14GR_BL,
    S14GR_T|S14GR_TL|S14GR_MR|S14GR_B|S14GR_BL|S14GR_BR,
    S14GR_TL|S14GR_TR|S14GR_ML|S14GR_MR|S14GR_BL|S14GR_BR,
    S14GR_T|S14GR_TV|S14GR_B|S14GR_BV,
    S14GR_TR|S14GR_B|S14GR_BL|S14GR_BR,
    S14GR_TL|S14GR_TRD|S14GR_ML|S14GR_BL|S14GR_BRD,
    S14GR_TL|S14GR_B|S14GR_BL,
    S14GR_T|S14GR_TL|S14GR_TR|S14GR_TV|S14GR_BL|S14GR_BR,
    S14GR_TL|S14GR_TLD|S14GR_TR|S14GR_BL|S14GR_BR|S14GR_BRD,
    S14GR_T|S14GR_TL|S14GR_TR|S14GR_B|S14GR_BL|S14GR_BR,
    S14GR_T|S14GR_TL|S14GR_TR|S14GR_ML|S14GR_MR|S14GR_BL,
    S14GR_T|S14GR_TL|S14GR_TR|S14GR_B|S14GR_BL|S14GR_BR|S14GR_BRD,
    S14GR_T|S14GR_TL|S14GR_TR|S14GR_ML|S14GR_MR|S14GR_BL|S14GR_BRD,
    S14GR_T|S14GR_TL|S14GR_ML|S14GR_MR|S14GR_B|S14GR_BR,
    S14GR_T|S14GR_TV|S14GR_BV,
    S14GR_TL|S14GR_TR|S14GR_B|S14GR_BL|S14GR_BR,
    S14GR_TL|S14GR_TRD|S14GR_BL|S14GR_BLD,
    S14GR_TL|S14GR_TR|S14GR_BL|S14GR_BLD|S14GR_BR|S14GR_BRD,
    S14GR_TLD|S14GR_TRD|S14GR_BLD|S14GR_BRD,
    S14GR_TL|S14GR_TR|S14GR_ML|S14GR_MR|S14GR_B|S14GR_BR,
    S14GR_T|S14GR_TRD|S14GR_B|S14GR_BLD,
    S14GR_DOT
};    

const uint16_t font144segGrove[37] = { 
    S14GR4_T|S14GR4_TL|S14GR4_TR|S14GR4_B|S14GR4_BL|S14GR4_BR,
    S14GR4_TR|S14GR4_BR,
    S14GR4_T|S14GR4_TR|S14GR4_ML|S14GR4_MR|S14GR4_B|S14GR4_BL,
    S14GR4_T|S14GR4_TR|S14GR4_ML|S14GR4_MR|S14GR4_B|S14GR4_BR,
    S14GR4_TL|S14GR4_TR|S14GR4_ML|S14GR4_MR|S14GR4_BR,
    S14GR4_T|S14GR4_TL|S14GR4_ML|S14GR4_MR|S14GR4_B|S14GR4_BR,
    S14GR4_TL|S14GR4_ML|S14GR4_MR|S14GR4_B|S14GR4_BL|S14GR4_BR,
    S14GR4_T|S14GR4_TR|S14GR4_BR,
    S14GR4_T|S14GR4_TL|S14GR4_TR|S14GR4_ML|S14GR4_MR|S14GR4_B|S14GR4_BL|S14GR4_BR,
    S14GR4_T|S14GR4_TL|S14GR4_TR|S14GR4_ML|S14GR4_MR|S14GR4_BR,
    S14GR4_T|S14GR4_TL|S14GR4_TR|S14GR4_ML|S14GR4_MR|S14GR4_BL|S14GR4_BR,
    S14GR4_T|S14GR4_TR|S14GR4_TV|S14GR4_MR|S14GR4_B|S14GR4_BR|S14GR4_BV,
    S14GR4_T|S14GR4_TL|S14GR4_B|S14GR4_BL,
    S14GR4_T|S14GR4_TR|S14GR4_TV|S14GR4_B|S14GR4_BR|S14GR4_BV,
    S14GR4_T|S14GR4_TL|S14GR4_ML|S14GR4_MR|S14GR4_B|S14GR4_BL,
    S14GR4_T|S14GR4_TL|S14GR4_ML|S14GR4_BL,
    S14GR4_T|S14GR4_TL|S14GR4_MR|S14GR4_B|S14GR4_BL|S14GR4_BR,
    S14GR4_TL|S14GR4_TR|S14GR4_ML|S14GR4_MR|S14GR4_BL|S14GR4_BR,
    S14GR4_T|S14GR4_TV|S14GR4_B|S14GR4_BV,
    S14GR4_TR|S14GR4_B|S14GR4_BL|S14GR4_BR,
    S14GR4_TL|S14GR4_TRD|S14GR4_ML|S14GR4_BL|S14GR4_BRD,
    S14GR4_TL|S14GR4_B|S14GR4_BL,
    S14GR4_T|S14GR4_TL|S14GR4_TR|S14GR4_TV|S14GR4_BL|S14GR4_BR,
    S14GR4_TL|S14GR4_TLD|S14GR4_TR|S14GR4_BL|S14GR4_BR|S14GR4_BRD,
    S14GR4_T|S14GR4_TL|S14GR4_TR|S14GR4_B|S14GR4_BL|S14GR4_BR,
    S14GR4_T|S14GR4_TL|S14GR4_TR|S14GR4_ML|S14GR4_MR|S14GR4_BL,
    S14GR4_T|S14GR4_TL|S14GR4_TR|S14GR4_B|S14GR4_BL|S14GR4_BR|S14GR4_BRD,
    S14GR4_T|S14GR4_TL|S14GR4_TR|S14GR4_ML|S14GR4_MR|S14GR4_BL|S14GR4_BRD,
    S14GR4_T|S14GR4_TL|S14GR4_ML|S14GR4_MR|S14GR4_B|S14GR4_BR,
    S14GR4_T|S14GR4_TV|S14GR4_BV,
    S14GR4_TL|S14GR4_TR|S14GR4_B|S14GR4_BL|S14GR4_BR,
    S14GR4_TL|S14GR4_TRD|S14GR4_BL|S14GR4_BLD,
    S14GR4_TL|S14GR4_TR|S14GR4_BL|S14GR4_BLD|S14GR4_BR|S14GR4_BRD,
    S14GR4_TLD|S14GR4_TRD|S14GR4_BLD|S14GR4_BRD,
    S14GR4_TL|S14GR4_TR|S14GR4_ML|S14GR4_MR|S14GR4_B|S14GR4_BR,
    S14GR4_T|S14GR4_TRD|S14GR4_B|S14GR4_BLD,
    S14GR4_DOT
};    

struct dispConf displays[SP_NUM_TYPES] = {
  { true,  0, 1, 0, 0, 1, 0, 255, 0,      0, 8, 2, 0, { 0, 1 },       font7segGeneric },  // CircuitSetup TCD add-on (TODO)
  { true,  3, 4, 0, 0, 4, 0,   2, 0, 0x0002, 8, 4, 0, { 0, 1, 3, 4 }, font7segGeneric },  // SP_ADAF_7x4   0.56" (right)
  { true,  0, 1, 0, 0, 1, 0,   2, 0, 0x0002, 8, 4, 0, { 0, 1, 3, 4 }, font7segGeneric },  // SP_ADAF_7x4L  0.56" (left)
  { true,  3, 4, 0, 0, 4, 0,   2, 0, 0x0002, 8, 4, 0, { 0, 1, 3, 4 }, font7segGeneric },  // SP_ADAF_B7x4  1.2" (right)
  { true,  0, 1, 0, 0, 1, 0,   2, 0, 0x0002, 8, 4, 0, { 0, 1, 3, 4 }, font7segGeneric },  // SP_ADAF_B7x4L 1.2" (left)
  { false, 2, 3, 0, 0, 3, 0, 255, 0,      0, 8, 4, 0, { 0, 1, 2, 3 }, font14segGeneric }, // SP_ADAF_14x4  0.56" (right)
  { false, 0, 1, 0, 0, 1, 0, 255, 0,      0, 8, 4, 0, { 0, 1, 2, 3 }, font14segGeneric }, // SP_ADAF_14x4L 0.56" (left)
  { false, 2, 1, 0, 0, 1, 0, 255, 0,      0, 8, 2, 0, { 2, 1 },       font14segGrove },   // SP_GROVE_2DIG14
  { false, 3, 4, 0, 0, 4, 0,   5, 0, 0x2080, 8, 4, 0, { 1, 2, 3, 4 }, font144segGrove },  // SP_GROVE_4DIG14 (right)
  { false, 1, 2, 0, 0, 2, 0,   5, 0, 0x2080, 8, 4, 0, { 1, 2, 3, 4 }, font144segGrove },  // SP_GROVE_4DIG14 (left)
#ifdef TWPRIVATE
  { false, 2, 3, 0, 0, 3, 0, 255, 0,      0, 8, 2, 0, { 2, 3 },       font14segGeneric }, // TW custom
#endif  
// .... for testing only:
//{ true,  7, 7, 0, 8, 7, 8, 255, 0,      0, 8, 2, 1, { 7 },          font7segGeneric },  // SP_TCD_TEST7
//{ false, 1, 2, 0, 0, 2, 0, 255, 0,      0, 8, 3, 0, { 0, 1, 2 },    font14segGeneric }, // SP_TCD_TEST14 right
//{ false, 0, 1, 0, 0, 1, 0, 255, 0,      0, 8, 3, 0, { 0, 1, 2 },    font14segGeneric }  // SP_TCD_TEST14 left
};

// Grove 4-digit special handling
const uint8_t gr4_sh1[4] = { 4,  6, 5, 10 };
const uint8_t gr4_sh2[4] = { 3, 14, 9,  8 };

// Store i2c address
speedDisplay::speedDisplay(uint8_t address) 
{    
    _address = address;
}

// Start the display
void speedDisplay::begin(int dispType) 
{
    if(dispType < SP_MIN_TYPE || dispType >= SP_NUM_TYPES) {
        #ifdef TC_DBG
        Serial.print("Bad speedo display type: ");
        Serial.println(dispType, DEC);
        #endif
        dispType = SP_MIN_TYPE;
    }

    _dispType = dispType;
    _is7seg = displays[dispType].is7seg;
    _speed_pos10 = displays[dispType].speed_pos10;
    _speed_pos01 = displays[dispType].speed_pos01;
    _dig10_shift = displays[dispType].dig10_shift;
    _dig01_shift = displays[dispType].dig01_shift;    
    _dot_pos01 = displays[dispType].dot_pos01;
    _dot01_shift = displays[dispType].dot01_shift;
    _colon_pos = displays[dispType].colon_pos;
    _colon_shift = displays[dispType].colon_shift;
    _colon_bm = displays[dispType].colon_bit;
    _buf_size = displays[dispType].buf_size;
    _num_digs = displays[dispType].num_digs;
    _buf_packed = displays[dispType].buf_packed;
    _bufPosArr = displays[dispType].bufPosArr;
    _fontXSeg = displays[dispType].fontSeg;
    
    Wire.beginTransmission(_address);
    Wire.write(0x20 | 1);  // turn on oscillator
    Wire.endTransmission();

    clear();            // clear buffer
    setBrightness(15);  // setup initial brightness
    clearDisplay();     // clear display RAM
    on();               // turn it on
}

// Turn on the display
void speedDisplay::on() 
{
    Wire.beginTransmission(_address);
    Wire.write(0x80 | 1);  
    Wire.endTransmission();
}

// Turn off the display
void speedDisplay::off() 
{
    Wire.beginTransmission(_address);
    Wire.write(0x80);  
    Wire.endTransmission();
}

// Turn on all LEDs
void speedDisplay::lampTest() 
{  
    Wire.beginTransmission(_address);
    Wire.write(0x00);  // start at address 0x0

    for(int i = 0; i < _buf_size*2; i++) {
        Wire.write(0xFF);
    }
    Wire.endTransmission();

    _lastBufPosCol = 0xffff;
}

// Clear the buffer
void speedDisplay::clear() 
{
    // must call show() to actually clear display
    
    for(int i = 0; i < _buf_size; i++) {
        _displayBuffer[i] = 0;
    }
}

// Set display brightness
// Valid brighness levels are 0 to 15. Default is 15.
// 255 sets it to previous level
uint8_t speedDisplay::setBrightness(uint8_t level, bool isInitial) 
{
    if(level == 255)  
        level = _brightness;    // restore to old val 
        
    _brightness = setBrightnessDirect(level);

    if(isInitial)
        _origBrightness = _brightness;

    return _brightness;
}

uint8_t speedDisplay::setBrightnessDirect(uint8_t level) 
{
    if(level > 15)
        level = 15;

    Wire.beginTransmission(_address);
    Wire.write(0xE0 | level);  // Dimming command
    Wire.endTransmission();

    return level;
}

uint8_t speedDisplay::getBrightness() 
{
    return _brightness;
}

void speedDisplay::setNightMode(bool mymode)
{
    _nightmode = mymode ? true : false;
}

bool speedDisplay::getNightMode(void)
{
    return _nightmode;
}


// Show data in display --------------------------------------------------------


// Show the buffer 
void speedDisplay::show() 
{
    int i; 

    if(_nightmode) {        
        if(_oldnm < 1) { 
            setBrightness(0);
            _oldnm = 1; 
        }
    } else {
        if(_oldnm > 0) {
            setBrightness(_origBrightness);
            _oldnm = 0;
        }
    }

    switch(_dispType) {
    case SP_GROVE_4DIG14:
    case SP_GROVE_4DIG14L:
        _displayBuffer[_colon_pos] &= ~(0x4778);
        for(int i = 0; i < 4; i++) {
            _displayBuffer[_colon_pos] |= ((_displayBuffer[*(_bufPosArr + i)] & 0x02) ? 1 << gr4_sh1[i] : 0);
            _displayBuffer[_colon_pos] |= ((_displayBuffer[*(_bufPosArr + i)] & 0x04) ? 1 << gr4_sh2[i] : 0);
        }
    }

    Wire.beginTransmission(_address);
    Wire.write(0x00);  // start at address 0x0

    for(i = 0; i < _buf_size; i++) {
        Wire.write(_displayBuffer[i] & 0xFF);
        Wire.write(_displayBuffer[i] >> 8);
    }

    // Save last value written to _colon_pos
    if(_colon_pos < 255) {
        _lastBufPosCol = _displayBuffer[_colon_pos];
    }
    
    Wire.endTransmission();
}


// Set data in buffer --------------------------------------------------------


// Write given text to buffer
// (including current colon setting; dots are cleared and ignored)
void speedDisplay::setText(const char *text)
{
    int idx = 0, pos = 0;
    int temp = 0;
    
    clear();
    
    if(_is7seg) {
        while(text[idx] && (pos < (_num_digs / (1<<_buf_packed)))) {
            temp = getLEDChar(text[idx]) << _dig10_shift;
            idx++;
            if(_buf_packed && text[idx]) {
                temp |= (getLEDChar(text[idx]) << _dig01_shift);
                idx++;            
            }
            _displayBuffer[*(_bufPosArr + pos)] = temp;
            pos++;
        }
    } else {
        while(text[idx] && pos < _num_digs) {
            _displayBuffer[*(_bufPosArr + pos)] = getLEDChar(text[idx]);
            idx++;
            pos++;
        }
    }

    handleColon();
}

// Write given speed to buffer
// (including current dot01 settings; colon is cleared and ignored)
void speedDisplay::setSpeed(uint8_t speedNum)
{
    if(speedNum > 99) {
        Serial.print(F("speedDisplay: setSpeed: Bad speed: "));
        Serial.println(speedNum, DEC);
        speedNum = 99;
    }

    _speed = speedNum;

    clear();

    _displayBuffer[_speed_pos10] |= (*(_fontXSeg + (speedNum / 10)) << _dig10_shift);
    _displayBuffer[_speed_pos01] |= (*(_fontXSeg + (speedNum % 10)) << _dig01_shift);    
    if(_dot01) _displayBuffer[_dot_pos01] |= (*(_fontXSeg + 36) << _dot01_shift);    
}

void speedDisplay::setDot(bool dot01)
{
    _dot01 = dot01;
}

void speedDisplay::setColon(bool colon)
{
    _colon = colon;
}


// Query data ------------------------------------------------------------------


uint8_t speedDisplay::getSpeed() 
{
    return _speed;
}

bool speedDisplay::getDot()
{
    return _dot01;
}


// Special purpose -------------------------------------------------------------


// clears the display RAM and only shows the given text
// does not use the buffer, writes directly to display
// (clears and ignores dot and colon)
void speedDisplay::showTextDirect(const char *text)
{
    int idx = 0, pos = 0;
    int temp = 0;
    uint16_t tt = 0, spec = _lastBufPosCol;
    
    clearDisplay();
    
    if(_is7seg) {
        while(text[idx] && (pos < (_num_digs / (1<<_buf_packed)))) {
            temp = getLEDChar(text[idx]) << _dig10_shift;
            idx++;
            if(_buf_packed && text[idx]) {
                temp |= (getLEDChar(text[idx]) << _dig01_shift);
                idx++;            
            }
            directCol(*(_bufPosArr + pos), temp);
            pos++;
        }
    } else {
        while(text[idx] && pos < _num_digs) {
            tt = getLEDChar(text[idx]);
            directCol(*(_bufPosArr + pos), tt);
            switch(_dispType) {
            case SP_GROVE_4DIG14:
            case SP_GROVE_4DIG14L:
                spec |= ((tt & 0x2) ? 1 << gr4_sh1[pos] : 0);
                spec |= ((tt & 0x4) ? 1 << gr4_sh2[pos] : 0);
                break;            
            }
            idx++;
            pos++;
        }
        switch(_dispType) {
        case SP_GROVE_4DIG14:
        case SP_GROVE_4DIG14L:
            directCol(_colon_pos, spec);
        }
    }
}

void speedDisplay::setColonDirect(bool colon)
{
    uint16_t t = _lastBufPosCol;
    
    _colon = colon;

    if(_colon_pos < 255) {
        if(_colon) t |= (_colon_bm << _colon_shift);
        else       t &= (~(_colon_bm << _colon_shift));        
    }

    directCol(_colon_pos, t);
}

// Private functions ###########################################################


void speedDisplay::handleColon()
{
    if(_colon_pos < 255) {        
        if(_colon) _displayBuffer[_colon_pos] |= (_colon_bm << _colon_shift);
        else       _displayBuffer[_colon_pos] &= (~(_colon_bm << _colon_shift));        
    }
}

// Returns bit pattern for provided character
uint16_t speedDisplay::getLEDChar(uint8_t value) 
{    
    if(value >= '0' && value <= '9') {
        return *(_fontXSeg + (value - '0'));        
    } else if(value >= 'A' && value <= 'Z') {
        return *(_fontXSeg + (value - 'A' + 10));
    } else if(value >= 'a' && value <= 'z') {
        return *(_fontXSeg + (value - 'a' + 10));
    }

    return 0;
}

// Directly write to a column with supplied segments
// (leave buffer intact, directly write to display)
void speedDisplay::directCol(int col, int segments) 
{
    Wire.beginTransmission(_address);
    Wire.write(col * 2);  // 2 bytes per col * position    
    Wire.write(segments & 0xFF);
    Wire.write(segments >> 8);
    Wire.endTransmission();

    if(col == _colon_pos)
        _lastBufPosCol = segments;
}

// Directly clear the display 
void speedDisplay::clearDisplay() 
{    
    Wire.beginTransmission(_address);
    Wire.write(0x00);  // start at address 0x0

    for(int i = 0; i < _buf_size*2; i++) {
        Wire.write(0x0);
    }
    
    Wire.endTransmission();

    _lastBufPosCol = 0;
}

#endif
