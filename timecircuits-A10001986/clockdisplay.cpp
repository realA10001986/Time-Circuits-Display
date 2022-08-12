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

#include "clockdisplay.h"
#include "tc_font.h"

const char months[12][4] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};

// Gets the i2c address and eeprom save location, default saveAddress is -1 if not provided to disable saving
clockDisplay::clockDisplay(uint8_t address, int saveAddress) 
{    
    // call begin() to start
    _address = address;
    _saveAddress = saveAddress;
}

// Private functions

// Returns bit pattern for provided number 0-9 or number provided as a char 0-9 for display on 7 segment display
uint8_t clockDisplay::getLED7SegChar(uint8_t value) 
{    
    if(value >= '0' && value <= '9') {  
        // if provided as a char
        return numDigs[value - 48];
    } else if(value <= 9) {
        return numDigs[value];
    }
    return 0x0;   // blank on invalid
}

// Returns bit pattern for provided character for display on alphanumeric 14 segment display
uint16_t clockDisplay::getLEDAlphaChar(char value) 
{    
#ifdef IS_ACAR_DISPLAY
    if(value == ' ' || value == 0) {
        return 0;
    } else {
        return numDigs[value - 'A' + 10];
    }
#else  
    if(value == ' ') {
        return alphaChars[0];
    } else {
        return pgm_read_word(alphaChars + value);        
    }
#endif    
}

// Make a 2 digit number from the array and place it in the buffer at pos
// (makes leading 0s)
uint16_t clockDisplay::makeNum(uint8_t num) 
{
    uint16_t segments = 0;

    // Each position holds two digits, high byte is 1's, low byte is 10's
    
    segments = getLED7SegChar(num % 10) << 8;        // Place 1's in upper byte
    segments = segments | getLED7SegChar(num / 10);  // 10's in lower byte
    
    return segments;
}

// Make a 2 digit number from the array and place it in the buffer at pos
// (no leading 0s)
uint16_t clockDisplay::makeNumN0(uint8_t num) 
{    
    uint16_t segments = 0;

    // Each position holds two digits, high byte is 1's, low byte is 10's
    
    segments = getLED7SegChar(num % 10) << 8;        // Place 1's in upper byte
    if(num / 10) {
        segments = segments | getLED7SegChar(num / 10);  // 10's in lower byte
    }
    
    return segments;
}

uint16_t clockDisplay::makeAlpha(uint8_t value) 
{
    // valid positions are 0 to 2
    return getLEDAlphaChar(value);
}

// directly clear the display RAM
void clockDisplay::clearDisplay() 
{    
    Wire.beginTransmission(_address);
    Wire.write(0x00);  // start at address 0x0

    for(int i = 0; i < CD_BUF_SIZE*2; i++) {
        Wire.write(0x0);
    }
    Wire.endTransmission();
}

// turn on all LEDs
void clockDisplay::lampTest() 
{  
    Wire.beginTransmission(_address);
    Wire.write(0x00);  // start at address 0x0

    for(int i = 0; i < CD_BUF_SIZE*2; i++) {
        Wire.write(0xFF);
    }
    Wire.endTransmission();
}

// Start the display
void clockDisplay::begin() 
{
    Wire.beginTransmission(_address);
    Wire.write(0x20 | 1);  // turn on oscillator
    Wire.endTransmission();

    clear();            // clear buffer
    setBrightness(15);  // be sure in case coming up for mc reset
    clearDisplay();     // clear display RAM, comes up random.
    on();               // turn it on
}

void clockDisplay::on() 
{
    // Turn on the display
    Wire.beginTransmission(_address);
    Wire.write(0x80 | 1);  // turn on the display
    Wire.endTransmission();
}

// Turn off the display
void clockDisplay::off() 
{
    Wire.beginTransmission(_address);
    Wire.write(0x80);  // turn off the display
    Wire.endTransmission();
}

// clears the buffer
void clockDisplay::clear() 
{
    // must call show() to actually clear display

    #ifdef TC_DBG
    Serial.println("Clockdisplay: Clear Buffer");
    #endif

    // Holds the LED segment status
    // Month 0,1,2
    // Day 3
    // Year 4 and 5
    // Hour 6
    // Min 7
    
    for(int i = 0; i < CD_BUF_SIZE; i++) {
        _displayBuffer[i] = 0;
    }
}

// Set display brightness
uint8_t clockDisplay::setBrightness(uint8_t level) 
{
    // Valid brighness levels are 0 to 15. Default is 15.
    if(level > 15)
        return _brightness;

    Wire.beginTransmission(_address);
    Wire.write(0xE0 | level);  // Dimming command
    Wire.endTransmission();

    #ifdef TC_DBG
    Serial.print("Clockdisplay: Setting brightness: ");
    Serial.println(level, DEC);
    #endif

    _brightness = level;
    return _brightness;
}

uint8_t clockDisplay::getBrightness() 
{
    return _brightness;
}

/* 
 * Save date/time and other settings to EEPROM 
 * Only non-RTC displays save their time.
 * We stick with the EEPROM here because the times
 * probably won't be changed that often to cause
 * a problem with flash wear.
 * 
 */
bool clockDisplay::save() 
{    
    uint16_t sum = 0;  // a simple checksum
    int i;

    if(!isRTC() && _saveAddress >= 0) {  

        // Non-RTC: Save time

        #ifdef TC_DBG      
        Serial.println("Clockdisplay: Saving non-RTC settings to EEPROM");
        #endif

        uint8_t savBuf[10];
        savBuf[0] = _year & 0xff;
        savBuf[1] = (_year >> 8) & 0xff;
        savBuf[2] = _yearoffset & 0xff;
        savBuf[3] = (_yearoffset >> 8) & 0xff;
        savBuf[4] = _month;
        savBuf[5] = _day;
        savBuf[6] = _hour;
        savBuf[7] = _minute;
        savBuf[8] = 0;        // was _brightness, now in settings

        for(i = 0; i < 9; i++) {
            sum += savBuf[i];
            EEPROM.write(_saveAddress + i, savBuf[i]);
        }
        
        EEPROM.write(_saveAddress + 9, sum & 0xff);        
        
        EEPROM.commit();
        
    } else if(isRTC() && _saveAddress >= 0) {

        // RTC: Save yearoffs, bogus (time comes from battery-backed RTC)

        uint8_t savBuf[10];

        #ifdef TC_DBG  
        Serial.println("Clockdisplay: Saving RTC settings to EEPROM");
        #endif
                
        savBuf[0] = presentTimeBogus ? 1 : 0;        
        savBuf[1] = _yearoffset & 0xff;
        savBuf[2] = (_yearoffset >> 8) & 0xff;
        for(i = 0; i < 3; i++) {
            sum += savBuf[i] ^ 0x55;
            EEPROM.write(_saveAddress + i, savBuf[i]);
        }
        
        EEPROM.write(_saveAddress + 3, sum & 0xff);        
        
        EEPROM.commit();
        
        
    } else {
      
        return false;
        
    }

    return true;
}

/* 
 * Load saved date/time from eeprom 
 * 
 */
bool clockDisplay::load() 
{
    uint8_t loadBuf[10];
    uint16_t sum = 0;
    int i;

    if(_saveAddress < 0) 
        return false;
        
    if(!isRTC()) {  

        for(i = 0; i < 10; i++) {
            loadBuf[i] = EEPROM.read(_saveAddress + i);
            if(i < 9) sum += loadBuf[i];                 
        }    
      
        // Non-RTC: Load saved time
        // 16bit sum cannot be zero; if it is, the data
        // is clear, which means it is invalid.

        if( (sum != 0) && ((sum & 0xff) == loadBuf[9])) { 
                                   
            #ifdef TC_DBG  
            Serial.println("Clockdisplay: Loading non-RTC settings from EEPROM");
            #endif
            
            setYearOffset((loadBuf[3] << 8) | loadBuf[2]);
            setYear((loadBuf[1] << 8) | loadBuf[0]);
            setMonth(loadBuf[4]);
            setDay(loadBuf[5]);
            setHour(loadBuf[6]);
            setMinute(loadBuf[7]);

            // Reinstate _brightness to keep old behavior
            if (_saveAddress == DEST_TIME_PREF) { 
                setBrightness((int)atoi(settings.destTimeBright));  
            } else if (_saveAddress == DEPT_TIME_PREF) {
                setBrightness((int)atoi(settings.lastTimeBright));
            }
            
            return true;
            
        } 

    } else {

        // RTC: only "bogus" flag and yearoffs is saved
        
        for(i = 0; i < 4; i++) {
            loadBuf[i] = EEPROM.read(_saveAddress + i);
            if(i < 3) sum += loadBuf[i] ^ 0x55;                 
        }    

        if((sum & 0xff) == loadBuf[3]) { 

              presentTimeBogus = loadBuf[0] ? true : false;
              setYearOffset((loadBuf[2] << 8) | loadBuf[1]);  

              #ifdef TC_DBG  
              Serial.println("Clockdisplay: Loading RTC settings from EEPROM");
              #endif
                     
        } else {

              presentTimeBogus = false;
              setYearOffset(0);

              Serial.println("Clockdisplay: Invalid presetTime EEPROM data");
              
        }

        // Reinstate _brightness to keep old behavior
        setBrightness((int)atoi(settings.presTimeBright));        

        return true;             
    }
     
    Serial.println("Clockdisplay: Invalid EEPROM data");

    // Do NOT clear EEPROM if data is invalid.
    // All 0s are as bad, wait for EEPROM to be
    // written by application on purpose
        
    return false;
}

// Show the buffer 
void clockDisplay::showInt(bool animate) 
{
    int i = 0; 

    if(_nightmode) {        
        if(!isRTC()) {    
            off();   
            _oldnm = 1;         
            return;
        } else {          
            if(_oldnm < 1) { 
                setBrightness(0); 
            }
            _oldnm = 1;
        }
    } else if(isRTC()) {
        if(_oldnm > 0) {
            setBrightness((int)atoi(settings.presTimeBright));             
        }
        _oldnm = 0;
    }

    if(animate) off();

    if(!_mode24) {
        (_hour < 12) ? AM() : PM();
    } else {    
        AMPMoff();
    }

    (_colon) ? colonOn() : colonOff();

    Wire.beginTransmission(_address);
    Wire.write(0x00);  // start at address 0x0

    if(animate) {
        for(int c = 0; c < CD_MONTH_SIZE; c++) {
            Wire.write(0x00);  //blank month
            Wire.write(0x00);
        }        
        i = CD_DAY_POS;
    }
    
    for(; i < CD_BUF_SIZE; i++) {
        Wire.write(_displayBuffer[i] & 0xFF);
        Wire.write(_displayBuffer[i] >> 8);
    }
    
    Wire.endTransmission();

    if(animate || (!isRTC() && (_oldnm > 0)) ) on();
    
    if(!isRTC()) _oldnm = 0;
}

// Show the buffer 
void clockDisplay::show() 
{
    showInt(false);
}

// Show all but month
void clockDisplay::showAnimate1() 
{    
    showInt(true);
}

// Show month, assumes showAnimate1() was already called
void clockDisplay::showAnimate2() 
{
    if(_nightmode && !isRTC()) {
        return;
    }
    
    Wire.beginTransmission(_address);
    Wire.write(0x00);  // start at address 0x0
    for (int i = 0; i < CD_BUF_SIZE; i++) {
        Wire.write(_displayBuffer[i] & 0xFF);
        Wire.write(_displayBuffer[i] >> 8);
    }
    Wire.endTransmission();
}

// Makes characters for 3 char month, valid months 1-12
void clockDisplay::setMonth(int monthNum) 
{
    if(monthNum < 1 || monthNum > 12) {        
        Serial.print("Clockdisplay: setMonth: Bad month: "); 
        Serial.println(monthNum, DEC);
        monthNum = 12;                   
    } 
  
    _month = monthNum;
    
#ifdef IS_ACAR_DISPLAY
    _displayBuffer[CD_MONTH_POS] = makeNum(monthNum);
#else
    // We always work with months 1-12, not 0-11
    // Exception: timeinfo (tm) works with 0-11, but we only use this in getNTPtime 
    // for syncing. Therefore, we must ALWAYS decrease monthNum here
    monthNum--;    
    _displayBuffer[CD_MONTH_POS]     = makeAlpha(months[monthNum][0]);
    _displayBuffer[CD_MONTH_POS + 1] = makeAlpha(months[monthNum][1]);
    _displayBuffer[CD_MONTH_POS + 2] = makeAlpha(months[monthNum][2]);
#endif
}

// Place LED pattern in day position in buffer, which is 3.
void clockDisplay::setDay(int dayNum) 
{
    if(dayNum < 1 || dayNum > 31) {          
        Serial.print("Clockdisplay: setDay: Bad day: ");
        Serial.println(dayNum, DEC);
        dayNum = 1;
    }
    
    _day = dayNum;
    
    _displayBuffer[CD_DAY_POS] = makeNum(dayNum);
}

// Place LED pattern in year position in buffer, which is 4 and 5.
void clockDisplay::setYear(uint16_t yearNum) 
{
    if(yearNum > 9999) {        
        Serial.print("Clockdisplay: setYear: Bad year: ");
        Serial.println(yearNum, DEC);
        yearNum = 9999;        
    }
    
    _year = yearNum;
    yearNum -= _yearoffset;
    
    _displayBuffer[CD_YEAR_POS]     = makeNum(yearNum / 100);
    _displayBuffer[CD_YEAR_POS + 1] = makeNum(yearNum % 100);
}

// Place LED pattern in year position directly, which is 4 and 5.
// useful for setting the year to the present time
void clockDisplay::setYearDirect(uint16_t yearNum) 
{
    if(yearNum > 9999) {                
        Serial.print("Clockdisplay: setYearDirect: Bad year: ");
        Serial.println(yearNum, DEC);
        yearNum = 9999;
    } 
    
    _year = yearNum;
    yearNum -= _yearoffset;
    
    directCol(CD_YEAR_POS,     makeNum(yearNum / 100));
    directCol(CD_YEAR_POS + 1, makeNum(yearNum % 100));
}

void clockDisplay::setYearOffset(int16_t yearOffs) 
{
    _yearoffset = yearOffs;
}

// Place LED pattern in hour position in buffer, which is 6.
void clockDisplay::setHour(uint16_t hourNum) 
{   
    if(hourNum > 23) {                
        Serial.print("Clockdisplay: setHour: Bad hour: ");
        Serial.println(hourNum, DEC);
        hourNum = 23;
    }

    _hour = hourNum;

    if(!_mode24) {
      
        if(hourNum == 0) {
            _displayBuffer[CD_HOUR_POS] = makeNum(12);
        } else if(hourNum > 12) {
            // pm
            _displayBuffer[CD_HOUR_POS] = makeNum(hourNum - 12);
        } else if(hourNum <= 12) {
            // am
            _displayBuffer[CD_HOUR_POS] = makeNum(hourNum);
        }
        
    } else {
      
        _displayBuffer[CD_HOUR_POS] = makeNum(hourNum);      
        
    }

    // AM/PM will be set on show() to avoid being overwritten
}

// Place LED pattern in minute position in buffer, which is 7
void clockDisplay::setMinute(int minNum) 
{
    if(minNum < 0 || minNum > 59) {
        Serial.print("Clockdisplay: setMinute: Bad Minute: ");
        Serial.println(minNum, DEC);
        minNum = 0;  
    }
    
    _minute = minNum;

    if(minNum < 60) {
        _displayBuffer[CD_MIN_POS] = makeNum(minNum);
    } else if(minNum >= 60) {
        _displayBuffer[CD_MIN_POS] = makeNum(0);
    }

    if(isRTC()) {
       if(alarmOnOff) 
          _displayBuffer[CD_MIN_POS] |= 0x8000;
    }
}

void clockDisplay::AM() 
{
    _displayBuffer[CD_AMPM_POS] |= 0x0080;
    _displayBuffer[CD_AMPM_POS] &= 0x7FFF;
}

void clockDisplay::PM() 
{
    _displayBuffer[CD_AMPM_POS] |= 0x8000;
    _displayBuffer[CD_AMPM_POS] &= 0xFF7F;   
}

void clockDisplay::AMPMoff() 
{
    _displayBuffer[CD_AMPM_POS] &= 0x7F7F;
}

void clockDisplay::colonOn() 
{
    _displayBuffer[CD_COLON_POS] |= 0x8080;    
}

void clockDisplay::colonOff() 
{
    _displayBuffer[CD_COLON_POS] &= 0x7F7F;    
}

// clears the display RAM and only shows the provided month
void clockDisplay::showOnlyMonth(int monthNum) 
{
    clearDisplay();
    
    if(monthNum < 1 || monthNum > 12) {
        Serial.println("Clockdisplay: showOnlyMonth: Bad month");
        if(monthNum < 1) monthNum = 1;
        if(monthNum > 12) monthNum = 12;
    }
    
#ifdef IS_ACAR_DISPLAY    
    directCol(CD_MONTH_POS, makeNum(monthNum));
#else
    monthNum--;
    directCol(CD_MONTH_POS,     makeAlpha(months[monthNum][0]));
    directCol(CD_MONTH_POS + 1, makeAlpha(months[monthNum][1]));
    directCol(CD_MONTH_POS + 2, makeAlpha(months[monthNum][2]));
#endif    
}

// clears the display RAM and only shows the word save
void clockDisplay::showOnlySave() 
{
    clearDisplay();
    
#ifdef IS_ACAR_DISPLAY
    directCol(CD_MONTH_POS,     numDigs[28]);
    directCol(CD_MONTH_POS + 1, numDigs[31]);
#else
    directCol(CD_MONTH_POS,     makeAlpha('S'));
    directCol(CD_MONTH_POS + 1, makeAlpha('A'));
    directCol(CD_MONTH_POS + 2, makeAlpha('V'));
    directCol(CD_DAY_POS,       numDigs[14]);    // 14 is 'E'
#endif    
}

// clears the display RAM and only shows the word "UTES" (as in "MINUTES")
void clockDisplay::showOnlyUtes() 
{
    clearDisplay();

#ifdef IS_ACAR_DISPLAY
    directCol(CD_MONTH_POS,     numDigs[28]);
    directCol(CD_MONTH_POS + 1, numDigs[31]);
#else
    directCol(CD_MONTH_POS,     makeAlpha('U'));
    directCol(CD_MONTH_POS + 1, makeAlpha('T'));
    directCol(CD_MONTH_POS + 2, makeAlpha('E'));
    directCol(CD_DAY_POS,       numDigs[28]);    // 28 is 'S'
#endif    
}

void clockDisplay::showOnlySettingVal(const char* setting, int8_t val, bool clear) 
{
    int8_t c = CD_MONTH_POS;
    
    if(clear)
        clearDisplay();

#ifdef IS_ACAR_DISPLAY
    while(c < CD_MONTH_SIZE && setting[c]) {
        directCol(c, numDigs[setting[c] - 'A' + 10]);
        c++;
    }
#else
    while(c < CD_MONTH_SIZE && setting[c]) {
        directCol(c, makeAlpha(setting[c]));
        c++;
    }
#endif

    if(val >= 0 && val < 100)
        directCol(CD_DAY_POS, makeNum(val));
    else
        directCol(CD_DAY_POS, 0x00);
}

// clears the display RAM and only shows the provided day
void clockDisplay::showOnlyDay(int dayNum) 
{    
    clearDisplay();
    
    directCol(CD_DAY_POS, makeNum(dayNum));
}

// clears the display RAM and only shows the provided year
void clockDisplay::showOnlyYear(int yearNum) 
{    
    clearDisplay();

    directCol(CD_YEAR_POS,     makeNum(yearNum / 100));
    directCol(CD_YEAR_POS + 1, makeNum(yearNum % 100));
}

// clears the display RAM and only shows the provided hour
void clockDisplay::showOnlyHour(int hourNum) 
{
    clearDisplay();

    if(!_mode24) {
      
        if(hourNum == 0) {
            directCol(CD_HOUR_POS, makeNum(12));
            directAM();
        } else if(hourNum > 12) {
            // pm
            directCol(CD_HOUR_POS, makeNum(hourNum - 12));
        } else {
            // am
            directCol(CD_HOUR_POS, makeNum(hourNum));
            directAM();
        }
    
        (hourNum > 11) ? directPM() : directAM();
        
    }  else {
      
        directCol(CD_HOUR_POS, makeNum(hourNum));
        
        directAMPMoff();
      
    }
}

// clears the display RAM and only shows the provided minute
void clockDisplay::showOnlyMinute(int minuteNum) 
{
    clearDisplay();
    
    directCol(CD_MIN_POS, makeNum(minuteNum));
}

// clears the display RAM and only shows the provided 2 numbers (parts of IP)
void clockDisplay::showOnlyHalfIP(int a, int b, bool clear) 
{
    char buf[6];
    int c = CD_MONTH_POS;
    
    if(clear)
          clearDisplay();
  
    sprintf(buf, "%d", a);
    while(c < CD_MONTH_SIZE && buf[c]) {
          directCol(c, makeAlpha(buf[c]));
          c++;
    }

    if(b >= 100) {
        directCol(CD_YEAR_POS, makeNumN0(b / 100));
    }
    directCol(CD_YEAR_POS + 1, ((b / 100) ? makeNum(b % 100) : makeNumN0(b % 100)));      
}

// write directly to a column with supplied segments
// leave buffer intact, directly write to display
void clockDisplay::directCol(int col, int segments) 
{
    Wire.beginTransmission(_address);
    Wire.write(col * 2);  // 2 bytes per col * position    
    Wire.write(segments & 0xFF);
    Wire.write(segments >> 8);
    Wire.endTransmission();
}

void clockDisplay::directAMPM(int val1, int val2) 
{
    Wire.beginTransmission(_address);
    Wire.write(CD_DAY_POS * 2);    
    Wire.write(val1 & 0xff);
    Wire.write(val2 & 0xff);
    Wire.endTransmission();
}
    
void clockDisplay::directAM() 
{
    directAMPM(0x80, 0x00);
}

void clockDisplay::directPM() 
{
    directAMPM(0x00, 0x80);
}

void clockDisplay::directAMPMoff() 
{
    directAMPM(0x00, 0x00);
}

// Set the displayed time with supplied DateTime object
void clockDisplay::setDateTime(DateTime dt) 
{
    // ATTN: DateTime implemention does not work for years < 2000!    
    
    setMonth(dt.month());
    setDay(dt.day());
    setYear(dt.year());
    setHour(dt.hour());
    setMinute(dt.minute());
}

void clockDisplay::setDS3232time(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year) 
{
    Wire.beginTransmission(DS3231_I2CADDR);
    Wire.write(0);                     // sends 00h - seconds register
    Wire.write(decToBcd(second));      // set seconds
    Wire.write(decToBcd(minute));      // set minutes
    Wire.write(decToBcd(hour));        // set hours
    Wire.write(decToBcd(dayOfWeek));   // set day of week (1-7; user defined; we use 1=Sunday, 7=Saturday)
    Wire.write(decToBcd(dayOfMonth));  // set date (1~31)
    Wire.write(decToBcd(month));       // set month (1~12)
    Wire.write(decToBcd(year));        // set year (0~99) 
    Wire.endTransmission();
}

// Convert normal decimal numbers to binary coded decimal
byte clockDisplay::decToBcd(byte val) 
{
    return ((val / 10 * 16) + (val % 10));
}

// Set time from array, YEAR, MONTH, DAY, HOUR, MIN
void clockDisplay::setFromStruct(dateStruct* s) 
{    
    setYear(s->year);
    setMonth(s->month);
    setDay(s->day);
    setHour(s->hour);
    setMinute(s->minute);
}

uint8_t clockDisplay::getMonth() 
{
    return _month;
}

uint8_t clockDisplay::getDay() 
{
    return _day;
}

uint16_t clockDisplay::getYear() 
{
    return _year;
}

int16_t clockDisplay::getYearOffset() 
{
    return _yearoffset;
}

uint8_t clockDisplay::getHour() 
{
    return _hour;
}

uint8_t clockDisplay::getMinute() {
    return _minute;
}

void clockDisplay::setColon(bool col) 
{
    // set true to turn it on
    _colon = col;
}

// track if this is will be holding real time.
void clockDisplay::setRTC(bool rtc) 
{
    _rtc = rtc;
}

// is this an real time display?
bool clockDisplay::isRTC() 
{    
    return _rtc;
}

void clockDisplay::set1224(bool hours24)
{
    _mode24 = hours24 ? true : false;
}

bool clockDisplay::get1224(void)
{
    return _mode24;
}

void clockDisplay::setNightMode(bool mymode)
{
    _nightmode = mymode ? true : false;
}

bool clockDisplay::getNightMode(void)
{
    return _nightmode;
}
    
