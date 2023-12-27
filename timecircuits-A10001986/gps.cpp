/*
 * -------------------------------------------------------------------
 * CircuitSetup.us Time Circuits Display
 * (C) 2022-2023 Thomas Winischhofer (A10001986)
 * https://github.com/realA10001986/Time-Circuits-Display
 * https://tcd.backtothefutu.re
 *
 * GPS Class: GPS receiver handling and data parsing
 *
 * This is designed for MTK3333-based modules.
 * 
 * Some ideas taken from the Adafruit_GPS Library
 * https://github.com/adafruit/Adafruit_GPS
 * -------------------------------------------------------------------
 * License: MIT
 * 
 * Permission is hereby granted, free of charge, to any person 
 * obtaining a copy of this software and associated documentation 
 * files (the "Software"), to deal in the Software without restriction, 
 * including without limitation the rights to use, copy, modify, 
 * merge, publish, distribute, sublicense, and/or sell copies of the 
 * Software, and to permit persons to whom the Software is furnished to 
 * do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be 
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY 
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "tc_global.h"

#ifdef TC_HAVEGPS

#include <Arduino.h>
#include <Wire.h>
#include "gps.h"

#define GPS_MPH_PER_KNOT  1.15077945
#define GPS_KMPH_PER_KNOT 1.852

// For deeper debugging
#define TC_DBG_GPS
#define TC_DBG_PRINT_NMEA
//#define GPS_SPEED_SIMU

#ifdef TC_DBG_GPS
static int DBGloopCnt = 0;
#endif

/*
 * Helpers
 */

static void defaultDelay(unsigned long mydelay)
{
    delay(mydelay);
}

static bool mystrcmp3(char *src, const char *cmp)
{
    if(*(src++) != *(cmp++)) return true;
    if(*(src++) != *(cmp++)) return true;
    return (*src != *cmp);
}

static uint8_t parseHex(char c)
{
    if(c < '0')   return 0;
    if(c <= '9')  return c - '0';
    if(c < 'A')   return 0;
    if(c <= 'F')  return c - 'A' + 10;
    if(c < 'a')   return 0;
    if(c <= 'f')  return c - 'a' + 10;
    return 0;
}

static uint8_t AToI2(char *buf)
{
    return ((*buf - '0') * 10) + (*(buf+1) - '0');
}

static uint16_t AToI4(char *buf)
{
    return (AToI2(buf) * 100) + AToI2(buf+2);
}

static unsigned long getFracs(char *buf)
{
    uint16_t f = 0, c = 1000;
    
    while(*buf && *buf != ',') {
        c /= 10;
        f *= 10;
        f += (*buf - '0');
        buf++;
    }

    return (unsigned long)(f * c);
}

static char * gotoNext(char *t)
{
    return strchr(t, ',') + 1;
}

static void copy6chars(char *a, char *b)
{
    *a++ = *b++;
    *a++ = *b++;
    *a++ = *b++;
    *a++ = *b++;
    *a++ = *b++;
    *a = *b;
}

/*
 * Class functions
 */

// Store i2c address
tcGPS::tcGPS(uint8_t address)
{
    _address = address;
}

// Start and init the GPS module
bool tcGPS::begin(unsigned long powerupTime, int quickUpdates, void (*myDelay)(unsigned long))
{
    uint8_t testBuf;
    int i2clen;
    int rmc, vtg;
    char *cmd1, *cmd2;
    char cmdbuf[64];
    
    _customDelayFunc = defaultDelay;

    _currentline = _line1;
    _lenIdx = 0;

    fix = false;
    _speed = -1;
    _haveSpeed = false;
    _haveDateTime = false;
    _haveDateTime2 = false;

    // Give the receiver some time to boot
    unsigned long millisNow = millis();
    if(millisNow - powerupTime < 1000) {
        delay(1000 - (millisNow - powerupTime));
    }

    // Check for GPS module on i2c bus
    Wire.beginTransmission(_address);
    if(Wire.endTransmission(true))
        return false;

    // Test reading the sensor
    i2clen = Wire.requestFrom(_address, (uint8_t)8);
    if(i2clen == 8) {
        for(int i = 0; i < 8; i++) {
            testBuf = Wire.read();
            // Bail if illegal characters returned
            if(testBuf != 0x0a && testBuf != 0x0d && (testBuf < ' ' || testBuf > 0x7e)) {
                return false;
            }
        }
    } else
        return false;

    // Send xxRMC and xxZDA only
    // If we use GPS for speed, we need more frequent updates.
    // The value in PKT 314 is a multiplier for the value of PKT 220.
    
    rmc = 1;
    vtg = 0;
    
    if(quickUpdates < 0) {
        // For mere time, we set the fix update to 5000ms, and the
        // multiplier to 1 as well, so we get it every 5th second.
        // $PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0*34
        // $PMTK220,5000*1B
        cmd1 = (char *)"1,0,0*34";
        cmd2 = (char *)"5000*1B";
    } else if(!quickUpdates) {
        // For speedo-less setups, but with GPS-aware BTTFN clients:
        #if defined(TC_GPSSPEED500) || defined(TC_GPSSPEED250) || defined(TC_GPSSPEED200)
        // Set update rate to 500ms
        // RMC 2x per sec, ZDA every 10th second
        // $PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,20,0,0*07
        // $PMTK220,500*2B
        cmd1 = (char *)"20,0,0*07";
        cmd2 = (char *)"500*2B";
        #else
        // Set update rate to 1000ms
        // RMC 1x per sec, ZDA every 5th second
        // $PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0,0*30
        // $PMTK220,1000*1F
        cmd1 = (char *)"5,0,0*30";
        cmd2 = (char *)"1000*1F";
        #endif
    } else {
        // For when displaying speed on speedo
        #if defined(TC_GPSSPEED200)
        // Set update rate to 200ms
        // --- using RMC:
        // RMC 5x per sec, ZDA every 10th second
        // $PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,50,0,0*00
        // $PMTK220,200*2C
        //cmd1 = (char *)"50,0,0*00";
        // ---- using VTG:
        // VTG 5x per sec, RMC every approx 6th sec, ZDA every 10th second
        // $PMTK314,0,31,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,50,0,0*32
        rmc = 31;
        vtg = 1;
        cmd1 = (char *)"50,0,0*32";
        cmd2 = (char *)"200*2C";
        // For reading 128 bytes per call:
        _lenLimit = 0x01;
        _lenArr[0] = 128; _lenArr[1] = 127;
        #elif defined(TC_GPSSPEED250)
        // Set update rate to 250ms
        // ---- using RMC
        // RMC 4x per sec, ZDA every 10th second
        // $PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40,0,0*01
        // $PMTK220,250*29"
        // cmd1 = (char *)"40,0,0*01";
        // ---- using VTG:
        // VTG 4x per sec, RMC every approx 6th sec, ZDA every 10th second
        // $PMTK314,0,25,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40,0,0*36
        rmc = 25;
        vtg = 1;
        cmd1 = (char *)"40,0,0*36";
        cmd2 = (char *)"250*29";
        // For reading 128 bytes per call:
        _lenLimit = 0x01;
        _lenArr[0] = 128; _lenArr[1] = 127;
        #elif defined(TC_GPSSPEED500)
        // Set update rate to 500ms
        // RMC 2x per sec, ZDA every 10th second
        // $PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,20,0,0*07
        // $PMTK220,500*2B
        cmd1 = (char *)"20,0,0*07";
        cmd2 = (char *)"500*2B";
        #else
        // Set update rate to 1000ms
        // RMC 1x per sec, ZDA every 5th second
        // $PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0,0*30
        // $PMTK220,1000*1F
        cmd1 = (char *)"5,0,0*30";
        cmd2 = (char *)"1000*1F";
        #endif
    }
    sprintf(cmdbuf, "$PMTK314,0,%d,%d,0,0,0,0,0,0,0,0,0,0,0,0,0,0,%s", rmc, vtg, cmd1);
    sendCommand(NULL, cmdbuf);
    sendCommand("$PMTK220,", cmd2);

    // No antenna status
    sendCommand(NULL, "$PGCMD,33,0*6D");

    // Search for GPS, GLONASS, Galileo satellites
    //sendCommand(NULL, "$PMTK353,1,1,1,0,0*2A");   // GPS, GLONASS, Galileo
    sendCommand(NULL, "$PMTK353,1,1,0,0,0*2B");     // GPS, GLONASS
    //sendCommand(NULL, "$PMTK353,1,0,0,0,0*2A");   // GPS

    // Send "hot start" to clear i2c buffer 
    sendCommand(NULL, "$PMTK101*32");
    delay(800);

    _customDelayFunc = myDelay;

    return true;
}

/*
 * loop()
 *
 */
void tcGPS::loop(bool doDelay)
{
    unsigned long myNow;
    
    #ifdef TC_DBG_GPS
    myNow = millis();
    #endif

    readAndParse(doDelay);
    // takes (with delay):
    // 6ms? when reading blocks of 32 bytes
    // 8ms " " " 64 bytes   (6ms without delay)  (ex: 11 w/o)
    // 13ms " " " 128 bytes  (12ms without delay) (ex: 17 w/o)

    #ifdef TC_DBG_GPS
    if((DBGloopCnt++) % 10 == 0) {
        Serial.printf("readAndParse took %d ms\n", millis()-myNow);
    }
    #endif

    myNow = millis();

    // Speed "simulator" for debugging
    #ifdef GPS_SPEED_SIMU
    _haveSpeed = true;        
    _curspdTS = myNow;
    #endif

    // Expire time/date info after 15 minutes
    if(_haveDateTime && (myNow - _curTS >= 15*60*1000))
        _haveDateTime = false;
    if(_haveDateTime2 && (myNow - _curTS2 >= 15*60*1000))
        _haveDateTime2 = false;

    // Expire speed info after 2 seconds
    if(_haveSpeed && (myNow - _curspdTS >= 2000))
        _haveSpeed = false;
}

/*
 * Return current speed
 *
 */
int16_t tcGPS::getSpeed()
{
    float _speedfk;
    
    if(_haveSpeed) {
        #ifdef GPS_SPEED_SIMU
        // Speed "simulator" for debugging
        _speed = (79 + (rand() % 10));
        if(_speed <= 80) _speed -= (79-8);
        #else
        _speedfk = strtof(_sbuf, NULL);
        if(_speedfk >= 2.61) {        // 3.00mph
            _speed = (int)roundf(_speedfk * GPS_MPH_PER_KNOT);
        } else if(_speedfk <= 2.00) {  // 2.30mph
            _speed = 0;
        } else if(_speedfk <= 2.30) { // 2.65mph
            _speed = 1;
        } else {
            _speed = 2;
        }
        #endif
        return _speed;
    }

    return -1;
}


bool tcGPS::haveTime()
{
    return (_haveDateTime || _haveDateTime2);
}

/*
 * Get GPS time
 * timeinfo returned is UTC
 */
bool tcGPS::getDateTime(struct tm *timeinfo, unsigned long *fixAge, unsigned long updInt)
{

    if(_haveDateTime) {

        timeinfo->tm_mday = _curDay;
        timeinfo->tm_mon  = _curMonth - 1;
        timeinfo->tm_year = _curYear - 1900;

        timeinfo->tm_hour = AToI2(&_curTime[0]);
        timeinfo->tm_min  = AToI2(&_curTime[2]);
        timeinfo->tm_sec  = AToI2(&_curTime[4]);
        
        timeinfo->tm_wday = 0;

        *fixAge = (unsigned long)(millis() - _curTS + _curFrac + updInt);

        return true;

    } else if(_haveDateTime2) {

        timeinfo->tm_mday = _curDay2;
        timeinfo->tm_mon  = _curMonth2 - 1;
        timeinfo->tm_year = _curYear2 - 1900;
        
        if(timeinfo->tm_year < (TCEPOCH_GEN-1900))
            timeinfo->tm_year += 100;

        timeinfo->tm_hour = AToI2(&_curTime2[0]);
        timeinfo->tm_min  = AToI2(&_curTime2[2]);
        timeinfo->tm_sec  = AToI2(&_curTime2[4]);

        timeinfo->tm_wday = 0;

        *fixAge = (unsigned long)(millis() - _curTS2 + _curFrac2 + updInt);

        return true;

    }

    return false;
}

/*
 * Set GPS' RTC time
 * timeinfo needs to be in UTC
 * 
 * Different firmwares support different packet types. 
 * We send both to make sure. If the GPS has updated 
 * its RTC from the GPS signal, "supported, but failed" 
 * is returned; so there is no danger of overwriting 
 * the GPS's RTC with less accurate time.
 * 
 */
bool tcGPS::setDateTime(struct tm *timeinfo)
{
    char pkt335[64] = "$PMTK335";
    char pkt740[64] = "$PMTK740";
    char pktgen[64];
    char temp[8];
    int  checksum1 = 0, checksum2 = 0;

    // If the date is clearly invalid, bail
    if(timeinfo->tm_year + 1900 < TCEPOCH_GEN)
        return false;

    // Generate command string
    sprintf(pktgen, ",%d,%d,%d,%d,%d,%d",
                  timeinfo->tm_year + 1900,
                  timeinfo->tm_mon + 1,
                  timeinfo->tm_mday,
                  timeinfo->tm_hour,
                  timeinfo->tm_min,
                  timeinfo->tm_sec);

    strcat(pkt335, pktgen);
    strcat(pkt740, pktgen);
    
    for(int i = 1; pkt335[i] != 0; i++) {
        checksum1 ^= pkt335[i];
        checksum2 ^= pkt740[i];
    }
    sprintf(temp, "*%02X", (checksum1 & 0xff));
    strcat(pkt335, temp);

    sprintf(temp, "*%02X", (checksum2 & 0xff));
    strcat(pkt740, temp);
    
    sendCommand(NULL, pkt740);
    sendCommand(NULL, pkt335);

    #ifdef TC_DBG
    Serial.printf("GPS setDateTime(): %s\n", pkt740);  
    #endif

    return true;
}

// Private functions ###########################################################


void tcGPS::sendCommand(const char *prefix, const char *str)
{ 
    Wire.beginTransmission(_address);
    if(prefix) {
        for(int i = 0; i < strlen(prefix); i++) {
            Wire.write((uint8_t)prefix[i]);
        }
    }
    for(int i = 0; i < strlen(str); i++) {
        Wire.write((uint8_t)str[i]);
    }
    Wire.write(0x0d);
    Wire.write(0x0a);
    Wire.endTransmission(true);
    (*_customDelayFunc)(30);
}

bool tcGPS::readAndParse(bool doDelay)
{
    char   curr_char = 0;
    size_t i2clen = 0;
    int    buff_max = 0;
    int    buff_idx = 0;
    bool   haveParsedSome = false;
    unsigned long myNow = millis();

    i2clen = Wire.requestFrom(_address, _lenArr[_lenIdx++]);
    _lenIdx &= _lenLimit;

    if(i2clen) {

        // Read i2c data to _buffer
        for(int i = 0; i < i2clen; i++) {
            curr_char = Wire.read();
            // Skip "empty data" (ie LF if not preceeded by CR)
            if((curr_char != 0x0a) || (_last_char == 0x0d)) {
                 _buffer[buff_max++] = curr_char;
            }
            _last_char = curr_char;
        }

        if(doDelay) (*_customDelayFunc)(1);

    }  

    // Form line(s) out of buffer data

    buff_max--;
    buff_idx = 0;
    while(buff_idx <= buff_max) {
      
        if(!_lineidx) _currentTS = myNow;
        
        curr_char = _currentline[_lineidx++] = _buffer[buff_idx++];

        if(_lineidx >= GPS_MAXLINELEN) {
            _lineidx = GPS_MAXLINELEN - 1;
        }

        // Finish a line on LF
        if(curr_char == 0x0a) {

            // terminate current line
            _currentline[_lineidx] = 0;

            // parse this line
            parseNMEA(_currentline, _currentTS);
            haveParsedSome = true;
            
            _lineidx = 0;
        }

        // Continue loop and write remaining data into new line

    }

    return haveParsedSome;
}

#if defined(TC_DBG_GPS) && defined(TC_DBG_PRINT_NMEA)
unsigned long lastMillis = 0;
#endif

bool tcGPS::parseNMEA(char *nmea, unsigned long nmeaTS)
{
    char *t = nmea, *t2;
    char *bufp = _sbuf;

    if(!checkNMEA(nmea)) {
        #ifdef TC_DBG
        Serial.printf("parseNMEA: Bad NMEA %s", nmea);
        #endif
        return false;
    }

    #if defined(TC_DBG_GPS) && defined(TC_DBG_PRINT_NMEA)
    Serial.printf("%d ", millis() - lastMillis);
    lastMillis = millis();
    Serial.print(nmea);
    #endif

    if(*(t+3) == 'R') {   // RMC

        t2 = t = gotoNext(t);   // Skip to time
        t = gotoNext(t);        // Skip to validity
        fix = (*t == 'A');

        if(!fix) return true;
        
        copy6chars(_curTime2, t2);
        _curFrac2 = (*(t2+6) == '.') ? getFracs(t2+7) : 0;

        t = gotoNext(t);        // Skip to lat
        t = gotoNext(t);        // Skip to n/s
        t = gotoNext(t);        // Skip to long
        t = gotoNext(t);        // Skip to e/w
        t2 = t = gotoNext(t);   // Skip to speed
        *bufp = 0;
        while(*t2 && *t2 != ',' && (bufp - 16 < _sbuf))
            *bufp++ = *t2++;
        *bufp = 0;
        if(*_sbuf) {
            _haveSpeed = true;
            _curspdTS = nmeaTS;
        }

        t = gotoNext(t);        // Skip to track made good
        t = gotoNext(t);        // Skip to date
        _curDay2   = AToI2(t);
        _curMonth2 = AToI2(t+2);
        _curYear2  = AToI2(t+4) + 2000;
          
        _curTS2 = nmeaTS;
        _haveDateTime2 = true;

    } else if(*(t+3) == 'V') {

        t = gotoNext(t);        // Skip to Course
        t = gotoNext(t);        // Skip to Reference
        t = gotoNext(t);        // Skip to Course (2)
        t = gotoNext(t);        // Skip to Reference (2)
        t2 = t = gotoNext(t);   // Skip to speed (knots)
        t = gotoNext(t);        // Skip to Unit
        t = gotoNext(t);        // Skip to speed (kph)
        t = gotoNext(t);        // Skip to Unit
        t = gotoNext(t);        // Skip to Mode
        fix = (*t != 'N');
        
        if(fix) {
            *bufp = 0;
            while(*t2 && *t2 != ',' && (bufp - 16 < _sbuf))
                *bufp++ = *t2++;
            *bufp = 0;
            if(*_sbuf) {
                _haveSpeed = true;
                _curspdTS = nmeaTS;
            }
        }

    } else {      // ZDA

        // Only use ZDA if we have a fix, no point in reading back
        // the GPS' own RTC.
        if(fix) {
            t = gotoNext(t);    // Skip to time
            copy6chars(_curTime, t);
            _curFrac = (*(t+6) == '.') ? getFracs(t+7) : 0;
            t = gotoNext(t);    // Skip to day
            _curDay = AToI2(t);
            t = gotoNext(t);    // Skip to month
            _curMonth = AToI2(t);
            t = gotoNext(t);    // Skip to year
            _curYear = AToI4(t);
            
            _curTS = nmeaTS;
            _haveDateTime = true;
        }

    }

    return true;
}

bool tcGPS::checkNMEA(char *nmea)
{
    uint16_t sum = 0;
    char *p = nmea;

    // check sentence type

    if(*nmea != '$')
        return false;

    char *ast = nmea;
    while(*ast)
        ast++;

    if(ast - nmea < 16)
        return false;

    if(mystrcmp3(nmea+3, "RMC") && mystrcmp3(nmea+3, "ZDA") && mystrcmp3(nmea+3, "VTG"))
        return false;

    // check checksum

    while(*ast != '*' && ast > nmea)
        ast--;

    if(*ast != '*')
        return false;

    sum = parseHex(*(ast+1)) << 4;
    sum += parseHex(*(ast+2));

    for(char *p1 = p + 1; p1 < ast; p1++)
        sum ^= *p1;

    return (sum == 0);
}

#endif
