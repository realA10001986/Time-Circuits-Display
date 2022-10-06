/*
 * -------------------------------------------------------------------
 * CircuitSetup.us Time Circuits Display
 * (C) 2022 Thomas Winischhofer (A10001986)
 * 
 * Optional GPS module
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

#include "tc_global.h"

#ifdef TC_HAVEGPS

#include "gps.h"

static uint8_t parseHex(char c);
static void defaultDelay(unsigned int mydelay);


// Store i2c address
tcGPS::tcGPS(uint8_t address)
{
    _address = address;
}

// Start and init the GPS module
bool tcGPS::begin() 
{
    _customDelayFunc = defaultDelay;

    _currentline = _line1;
    _lastline = _line2;

    _lenIdx = 0;

    fix = false;
    speed = -1;
    _haveSpeed = false;
    _haveDateTime = false;
    _haveDateTime2 = false;
    
    // Check for GPS module on i2c bus
    Wire.beginTransmission(_address);
    if(Wire.endTransmission(true))
        return false;

    // Set update rate to 1Hz
    sendCommand("$PMTK220,1000*1F");
    delay(30);

    // No antenna status
    sendCommand("$PGCMD,33,0*6D");
    delay(30);

    // Send xxRMC and xxZDA only
    //sendCommand("$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0*28");
    sendCommand("$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0,0*30");
    delay(30);
    
    return true;
}

void tcGPS::loop()
{
    unsigned long myNow = millis();
    unsigned long myLater, myLater2;  //DBG
    
    //read();
    
    myLater = millis();               //DBG

    // Test
    //strcpy(_lastline, "$GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A");
    //strcpy(_lastline, "$GPRMC,150619,A,4807.038,N,01131.000,E,022.4,084.4,151022,003.1,W*67");
    //strcpy(_lastline, "$GPRMC,150619,A,4807.038,N,01131.000,E,,084.4,151022,003.1,W*4D");
    //_lineready = true;
    //
    
    if(_lineready) {
        parseNMEA(lastNMEA());          
    }

    myLater2 = millis();              //DBG

    //Serial.print("GPS loop(): read ");
    //Serial.print(myLater-myNow, DEC);
    //Serial.print(" parse ");
    //Serial.println(myLater2-myLater, DEC);

    // Expire time/date info after 15 minutes
    if(_haveDateTime && (myNow - _curTS >= 15*60*1000))
        _haveDateTime = false;
    if(_haveDateTime2 && (myNow - _curTS2 >= 15*60*1000))
        _haveDateTime2 = false;

    // Expire speed info after 2 seconds
    if(_haveSpeed && (myNow - _curspdTS >= 2000))
        _haveSpeed = false;
    
}

void tcGPS::sendCommand(const char *str)
{ 
    println(str); 
}

int16_t tcGPS::getSpeed()
{
    if(_haveSpeed) return speed;

    return -1;
}

bool tcGPS::getDateTime(struct tm *timeinfo, time_t *fixAge)
{
    char tmp[4] = { 0, 0, 0, 0 };

    /*
    timeinfo->tm_mday = 4;
    timeinfo->tm_mon = 4 - 1;
    timeinfo->tm_year = 2013 - 1900;
    timeinfo->tm_hour  = 14;
    timeinfo->tm_min = 32;
    timeinfo->tm_sec = 11;
    timeinfo->tm_wday = 0;
    *fixAge = 0;
    return true;
    */

    if(_haveDateTime) {

        timeinfo->tm_mday = atoi(_curDay);
        timeinfo->tm_mon = atoi(_curMonth) - 1;
        timeinfo->tm_year = atoi(_curYear) - 1900;

        tmp[0] = _curTime[0];
        tmp[1] = _curTime[1];
        timeinfo->tm_hour = atoi(tmp); 
        tmp[0] = _curTime[2];
        tmp[1] = _curTime[3];
        timeinfo->tm_min = atoi(tmp); 
        tmp[0] = _curTime[4];
        tmp[1] = _curTime[5];
        timeinfo->tm_sec = atoi(tmp); 

        timeinfo->tm_wday = 0;

        *fixAge = (time_t)(millis() - _curTS);

        return true;

    } else if(_haveDateTime2) {

        timeinfo->tm_mday = atoi(_curDay2);
        timeinfo->tm_mon = atoi(_curMonth2) - 1;
        timeinfo->tm_year = atoi(_curYear2) - 1900;

        tmp[0] = _curTime2[0];
        tmp[1] = _curTime2[1];
        timeinfo->tm_hour  = atoi(tmp); 
        tmp[0] = _curTime2[2];
        tmp[1] = _curTime2[3];
        timeinfo->tm_min  = atoi(tmp); 
        tmp[0] = _curTime2[4];
        tmp[1] = _curTime2[5];
        timeinfo->tm_sec = atoi(tmp); 

        timeinfo->tm_wday = 0;

        *fixAge = (time_t)(millis() - _curTS2);

        return true;

    }

    return false;
}

bool tcGPS::setDateTime(struct tm *timeinfo)
{
    char pkt335[64];
    char temp[8];
    int checksum = 0;
    
    sprintf(pkt335, "$PMTK335,%d,%d,%d,%d,%d,%d",
                  timeinfo->tm_year + 1900,
                  timeinfo->tm_mon + 1,
                  timeinfo->tm_mday,
                  timeinfo->tm_hour,
                  timeinfo->tm_min,
                  timeinfo->tm_sec);

    for(int i = 1; pkt335[i] != 0; i++) {
        checksum ^= pkt335[i];
    }

    sprintf(temp, "*%02x\r\n", (checksum & 0xff));
    strcat(pkt335, temp);

    #ifdef TC_DBG
    Serial.println("SetGPStime();");
    Serial.println(pkt335);
    #endif

    sendCommand(pkt335);
    delay(30);
}

void tcGPS::setCustomDelayFunc(void (*myDelay)(unsigned int))
{
    _customDelayFunc = myDelay;
}


// Private functions ###########################################################

bool tcGPS::read()
{
    char   curr_char = 0;
    char   c = 0;
    size_t i2clen = 0;
    int8_t buff_max = 0;
    int8_t buff_idx = 0;

    // Read i2c data in small blocks, call delay between
    // transfers to allow uninterrupted sound playback

    for(int j = 0; j < 1; j++) {    // TESTME
      
        i2clen = Wire.requestFrom(_address, _lenArr[_lenIdx++]);
        _lenIdx &= 0x07;
        
        if(!i2clen) break;
    
        // Read i2c data to _buffer
        for(int i = 0; i < i2clen; i++) {
            curr_char = Wire.read();
            // Skip "empty data" (ie LF if not preceeded by CR)
            if((curr_char != 0x0A) || (_last_char == 0x0D)) {           
                 _buffer[buff_max++] = _last_char = curr_char;
            }
        }

        (*_customDelayFunc)(5);
        
    }

    // Check if we read "empty data" (ie only 0x0As)
    
    buff_max--;
    if((buff_max == 0) && (_buffer[0] == 0x0A)) {
        buff_max = -1;
    }

    // Form line(s) out of buffer data
    
    buff_idx = 0;
    while(buff_idx <= buff_max) {
        c = _currentline[_lineidx++] = _buffer[buff_idx++];
         
        if(_lineidx >= MAXLINELENGTH) {
            _lineidx = MAXLINELENGTH - 1;
        }

        // Finish a line on LF
        if(c == 0x0A) {
            _currentline[_lineidx] = 0;

            // Swap line buffers
            if(_currentline == _line1) {
                _currentline = _line2;
                _lastline = _line1;
            } else {
                _currentline = _line1;
                _lastline = _line2;
            }

            _lineidx = 0;
            _lineready = true;

            #ifdef TC_DBG
            Serial.println(_lastline);
            #endif
        }

        // Continue loop and write remaining data into new line

    }

    return true;
}

size_t tcGPS::write(uint8_t value) 
{
    Wire.beginTransmission(_address);
    Wire.write((uint8_t)(value & 0xff));
    Wire.endTransmission(true);
    return 1;
}

bool tcGPS::parseNMEA(char *nmea)
{
    char *t = nmea, *t2;
    char buf[16];
    char *bufp = buf;
    
    if(!checkNMEA(nmea))
        return false;

    if(*(t+3) == 'R') {   // RMC
      
        t = strchr(t, ',') + 1;  // Skip to time
        strncpy(_curTime2, t, 6);
        t = strchr(t, ',') + 1;  // Skip to validity
        if(*t == 'A') fix = true;
        else {
            fix = false;
            return true;
        }
        t = strchr(t, ',') + 1;  // Skip to lat
        t = strchr(t, ',') + 1;  // Skip to n/s
        t = strchr(t, ',') + 1;  // Skip to long
        t = strchr(t, ',') + 1;  // Skip to e/w
        t2 = t = strchr(t, ',') + 1;  // Skip to speed
    
        *bufp = 0;
        while(*t2 && *t2 != ',' && bufp - 16 < buf)
            *bufp++ = *t2++;
        *bufp = 0;
        if(!*buf) {
            _haveSpeed = false;
        } else {
            speed = (int16_t)(round(atof(buf) * GPS_MPH_PER_KNOT));
            _haveSpeed = true;
            _curspdTS = millis();
        }

        t = strchr(t, ',') + 1;  // Skip to track made good
        t = strchr(t, ',') + 1;  // Skip to date
        strncpy(_curDay2, t, 2);
        strncpy(_curMonth2, t+2, 2);
        strncpy(&_curYear2[2], t+4, 2);
        
        _curTS2 = millis();
        _haveDateTime2 = true;

    } else {      // ZDA

        // Only use ZDA if we have a fix, no point in
        // reading back the GPS' mere RTC time.
        if(fix) {
            t = strchr(t, ',') + 1; // Skip to time
            strncpy(_curTime, t, 6);
            t = strchr(t, ',') + 1; // Skip to day
            strncpy(_curDay, t, 2);
            t = strchr(t, ',') + 1; // Skip to month
            strncpy(_curMonth, t, 2);
            t = strchr(t, ',') + 1; // Skip to year
            strncpy(_curYear, t, 4);
    
            _curTS = millis();
            _haveDateTime = true;
        }

    }

    return true;
}

char *tcGPS::lastNMEA(void) 
{
    _lineready = false;
    return (char *)_lastline;
}

bool tcGPS::checkNMEA(char *nmea)
{
    uint16_t sum = 0;
    char *p = nmea; 

    // check sentence type

    if(*nmea != '$')
        return false;
    
    if((strncmp(nmea+3, "RMC", 3)) && (strncmp(nmea+3, "ZDA", 3)))
        return false;

    // check checksum
    
    char *ast = nmea;
    while(*ast)
        ast++;
  
    while(*ast != '*' && ast > nmea)
        ast--;
  
    if(*ast != '*')
        return false; 
        
    sum = parseHex(*(ast + 1)) << 4; 
    sum += parseHex(*(ast + 2));
    
    for(char *p1 = p + 1; p1 < ast; p1++)
        sum ^= *p1;
        
    if(sum != 0)
        return false;         
    
    return true;
}

static uint8_t parseHex(char c) 
{
  if(c < '0')   return 0;
  if(c <= '9')  return c - '0';
  if(c < 'A')   return 0;
  if(c <= 'F')  return (c - 'A') + 10;
  if(c < 'a')   return 0;
  if(c <= 'f')  return (c - 'a') + 10;  
  return 0;
}

static void defaultDelay(unsigned int mydelay)
{
    delay(mydelay);
}
#endif
