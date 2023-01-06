/*
 * -------------------------------------------------------------------
 * CircuitSetup.us Time Circuits Display
 * (C) 2021-2022 John deGlavina https://circuitsetup.us
 * (C) 2022-2023 Thomas Winischhofer (A10001986)
 * https://github.com/realA10001986/Time-Circuits-Display-A10001986
 *
 * Settings handling
 *
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
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "tc_global.h"

#include <ArduinoJson.h>  // https://github.com/bblanchon/ArduinoJson
#include <SD.h>
#include <SPI.h>
#include <FS.h>
#ifdef USE_SPIFFS
#include <SPIFFS.h>
#else
#define SPIFFS LittleFS
#include <LittleFS.h>
#endif
#include <EEPROM.h>

#include "tc_settings.h"
#include "tc_menus.h"
#include "tc_audio.h"
#include "tc_time.h"

// Size of main config JSON
#define JSON_SIZE 1536

/*
 * Mount SPIFFS/LittleFS, und SD (if available)
 *
 * Read configuration from JSON config file
 * If config file not found, create one with default settings
 *
 * Read IP settings if a static IP is configured. If the device
 * is powered on or reset while ENTER is held down, the IP
 * settings file will be deleted and the device will use DHCP.
 *
 */

/* If SPIFFS/LittleFS is mounted */
static bool haveFS = false;

/* If a SD card is found */
bool haveSD = false;

/* If SD contains all default audio files */
static bool allowCPA = false;

/* Music Folder Number */
uint8_t musFolderNum = 0;

/* Save alarm/volume on SD? */
static bool configOnSD = false;

#define NUM_AUDIOFILES 17
static const char *audioFiles[NUM_AUDIOFILES] = {
      "/alarm.mp3\0",
      "/alarmoff.mp3\0",
      "/alarmon.mp3\0",
      "/baddate.mp3\0",
      "/ee1.mp3\0",
      "/ee2.mp3\0",
      "/ee3.mp3\0",
      "/ee4.mp3\0",
      "/enter.mp3\0",
      "/intro.mp3\0",
      "/nmoff.mp3\0",
      "/nmon.mp3\0",
      "/ping.mp3\0",
      "/shutdown.mp3",
      "/startup.mp3\0",
      "/timetravel.mp3\0",
      "/travelstart.mp3\0"
};

static const char *cfgName    = "/config.json";
static const char *almCfgName = "/tcdalmcfg.json";
static const char *volCfgName = "/tcdvolcfg.json";
static const char *musCfgName = "/tcdmcfg.json";
static const char *ipCfgName  = "/ipconfig.json";

static bool read_settings(File configFile);

static bool checkValidNumParm(char *text, int lowerLim, int upperLim, int setDefault);
static bool checkValidNumParmF(char *text, double lowerLim, double upperLim, double setDefault);

static void open_and_copy(const char *fn, int& haveErr);
static bool filecopy(File source, File dest);
static bool check_if_default_audio_present();

extern void start_file_copy();
extern void file_copy_progress();
extern void file_copy_done();
extern void file_copy_error();

/*
 * settings_setup()
 *
 */
void settings_setup()
{
    bool writedefault = false;

    // Pre-maturely use ENTER button (initialized again in keypad_setup())
    pinMode(ENTER_BUTTON_PIN, INPUT_PULLUP);
    delay(20);

    // Init our EEPROM
    EEPROM.begin(512);

    #ifdef TC_DBG
    Serial.println(F("settings_setup: Mounting flash FS..."));
    #endif

    if(SPIFFS.begin()) {

        haveFS = true;

    } else {

        #ifdef TC_DBG
        Serial.println(F("settings_setup: Mounting flash FS failed, formatting..."));
        #endif

        destinationTime.showTextDirect("WAIT");

        SPIFFS.format();
        if(SPIFFS.begin()) haveFS = true;

    }

    if(haveFS) {
      
        #ifdef TC_DBG
        Serial.println(F("settings_setup: Loading settings from flash FS"));
        #endif
        
        if(SPIFFS.exists(cfgName)) {
            File configFile = SPIFFS.open(cfgName, "r");
            if(configFile) {
                writedefault = read_settings(configFile);
                configFile.close();
            } else {
                writedefault = true;
            }
        } else {
            writedefault = true;
        }

        if(writedefault) {
            // config file does not exist or is incomplete - create one
            Serial.println(F("settings_setup: Settings missing or incomplete; writing new file"));
            write_settings();
        }

    } else {

        Serial.println(F("settings_setup: Failed to mount flash FS"));

    }
        
    #ifdef TC_DBG
    Serial.println(F("settings_setup: Mounting SD..."));
    #endif

    // Set up SD card
    SPI.begin(SPI_SCK_PIN, SPI_MISO_PIN, SPI_MOSI_PIN);

    haveSD = false;

    uint32_t sdfreq = (settings.sdFreq[0] == '0') ? 16000000 : 4000000;
    #ifdef TC_DBG
    Serial.print(F("settings_setup: SD/SPI frequency "));
    Serial.print(sdfreq / 1000000);
    Serial.println(F("MHz"));
    #endif

    if(!SD.begin(SD_CS_PIN, SPI, sdfreq)) {

        Serial.println(F("No SD card found"));

    } else {

        #ifdef TC_DBG
        Serial.println(F("SD card initialized"));
        #endif

        uint8_t cardType = SD.cardType();
        if(cardType == CARD_NONE) {

            Serial.println(F("No SD card inserted"));

        } else {

            #ifdef TC_DBG
            Serial.print(F("SD card type "));
            Serial.println(cardType, DEC);
            #endif

            haveSD = true;

        }
    }

    // Determine if alarm/volume settings are stored on SD
    configOnSD = (haveSD && settings.CfgOnSD[0] != '0');

    // Check if SD contains our default sound files
    if(haveFS && haveSD) {
        allowCPA = check_if_default_audio_present();
    }

    // Allow user to delete static IP data by holding ENTER
    // while booting
    if(digitalRead(ENTER_BUTTON_PIN)) {

        Serial.println(F("settings_setup: ENTER pressed - deleting ipconfig file"));

        deleteIpSettings();

        // Pre-maturely use white led (initialized again in keypad_setup())
        pinMode(WHITE_LED_PIN, OUTPUT);
        digitalWrite(WHITE_LED_PIN, HIGH);
        while(digitalRead(ENTER_BUTTON_PIN)) { }
        digitalWrite(WHITE_LED_PIN, LOW);
    }
}

static bool read_settings(File configFile)
{
    bool writedefault = false;
    //StaticJsonDocument<JSON_SIZE> json;
    DynamicJsonDocument json(JSON_SIZE);
    DeserializationError error = deserializeJson(json, configFile);

    #ifdef TC_DBG
    serializeJson(json, Serial);
    Serial.println(F(" "));
    Serial.print(F("Size of JSON: "));
    Serial.println(json.memoryUsage());
    #endif

    if(!error) {

        if(json["timeTrPers"]) {
            strcpy(settings.timesPers, json["timeTrPers"]);
            writedefault |= checkValidNumParm(settings.timesPers, 0, 1, DEF_TIMES_PERS);
        } else writedefault = true;
        if(json["alarmRTC"]) {
            strcpy(settings.alarmRTC, json["alarmRTC"]);
            writedefault |= checkValidNumParm(settings.alarmRTC, 0, 1, DEF_ALARM_RTC);
        } else writedefault = true;
        if(json["playIntro"]) {
            strcpy(settings.playIntro, json["playIntro"]);
            writedefault |= checkValidNumParm(settings.playIntro, 0, 1, DEF_PLAY_INTRO);
        } else writedefault = true;
        if(json["mode24"]) {
            strcpy(settings.mode24, json["mode24"]);
            writedefault |= checkValidNumParm(settings.mode24, 0, 1, DEF_MODE24);
        } else writedefault = true;
        if(json["autoRotateTimes"]) {
            strcpy(settings.autoRotateTimes, json["autoRotateTimes"]);
            writedefault |= checkValidNumParm(settings.autoRotateTimes, 0, 5, DEF_AUTOROTTIMES);
        } else writedefault = true;

        if(json["hostName"]) {
            strcpy(settings.hostName, json["hostName"]);
        } else writedefault = true;
        if(json["wifiConRetries"]) {
            strcpy(settings.wifiConRetries, json["wifiConRetries"]);
            writedefault |= checkValidNumParm(settings.wifiConRetries, 1, 15, DEF_WIFI_RETRY);
        } else writedefault = true;
        if(json["wifiConTimeout"]) {
            strcpy(settings.wifiConTimeout, json["wifiConTimeout"]);
            writedefault |= checkValidNumParm(settings.wifiConTimeout, 7, 25, DEF_WIFI_TIMEOUT);
        } else writedefault = true;
        if(json["wifiOffDelay"]) {
            strcpy(settings.wifiOffDelay, json["wifiOffDelay"]);
            writedefault |= checkValidNumParm(settings.wifiOffDelay, 0, 99, DEF_WIFI_OFFDELAY);
        } else writedefault = true;
        if(json["wifiAPOffDelay"]) {
            strcpy(settings.wifiAPOffDelay, json["wifiAPOffDelay"]);
            writedefault |= checkValidNumParm(settings.wifiAPOffDelay, 0, 99, DEF_WIFI_APOFFDELAY);
        } else writedefault = true;
        
        if(json["timeZone"]) {
            strcpy(settings.timeZone, json["timeZone"]);
        } else writedefault = true;
        if(json["ntpServer"]) {
            strcpy(settings.ntpServer, json["ntpServer"]);
        } else writedefault = true;
        #ifdef TC_HAVEGPS
        if(json["useGPS"]) {
            strcpy(settings.useGPS, json["useGPS"]);
            writedefault |= checkValidNumParm(settings.useGPS, 0, 1, DEF_USE_GPS);
        } else writedefault = true;
        #endif

        if(json["destTimeBright"]) {
            strcpy(settings.destTimeBright, json["destTimeBright"]);
            writedefault |= checkValidNumParm(settings.destTimeBright, 0, 15, DEF_BRIGHT_DEST);
        } else writedefault = true;
        if(json["presTimeBright"]) {
            strcpy(settings.presTimeBright, json["presTimeBright"]);
            writedefault |= checkValidNumParm(settings.presTimeBright, 0, 15, DEF_BRIGHT_PRES);
        } else writedefault = true;
        if(json["lastTimeBright"]) {
            strcpy(settings.lastTimeBright, json["lastTimeBright"]);
            writedefault |= checkValidNumParm(settings.lastTimeBright, 0, 15, DEF_BRIGHT_DEPA);
        } else writedefault = true;

        if(json["dtNmOff"]) {
            strcpy(settings.dtNmOff, json["dtNmOff"]);
            writedefault |= checkValidNumParm(settings.dtNmOff, 0, 1, DEF_DT_OFF);
        } else writedefault = true;
        if(json["ptNmOff"]) {
            strcpy(settings.ptNmOff, json["ptNmOff"]);
            writedefault |= checkValidNumParm(settings.ptNmOff, 0, 1, DEF_PT_OFF);
        } else writedefault = true;
        if(json["ltNmOff"]) {
            strcpy(settings.ltNmOff, json["ltNmOff"]);
            writedefault |= checkValidNumParm(settings.ltNmOff, 0, 1, DEF_LT_OFF);
        } else writedefault = true;
        if(json["autoNM"]) {
            strcpy(settings.autoNM, json["autoNM"]);
            writedefault |= checkValidNumParm(settings.autoNM, 0, 1, DEF_AUTONM);
        } else writedefault = true;
        if(json["autoNMPreset"]) {
            strcpy(settings.autoNMPreset, json["autoNMPreset"]);
            writedefault |= checkValidNumParm(settings.autoNMPreset, 0, AUTONM_NUM_PRESETS, DEF_AUTONM_PRESET);
        } else writedefault = true;
        if(json["autoNMOn"]) {
            strcpy(settings.autoNMOn, json["autoNMOn"]);
            writedefault |= checkValidNumParm(settings.autoNMOn, 0, 23, DEF_AUTONM_ON);
        } else writedefault = true;
        if(json["autoNMOff"]) {
            strcpy(settings.autoNMOff, json["autoNMOff"]);
            writedefault |= checkValidNumParm(settings.autoNMOff, 0, 23, DEF_AUTONM_OFF);
        } else writedefault = true;
        #ifdef TC_HAVELIGHT
        if(json["useLight"]) {
            strcpy(settings.useLight, json["useLight"]);
            writedefault |= checkValidNumParm(settings.useLight, 0, 1, DEF_USE_LIGHT);
        } else writedefault = true;
        if(json["luxLimit"]) {
            strcpy(settings.luxLimit, json["luxLimit"]);
            writedefault |= checkValidNumParm(settings.luxLimit, 0, 50000, DEF_LUX_LIMIT);
        } else writedefault = true;
        #endif

        #ifdef TC_HAVETEMP
        if(json["useTemp"]) {
            strcpy(settings.useTemp, json["useTemp"]);
            writedefault |= checkValidNumParm(settings.useTemp, 0, 1, DEF_USE_TEMP);
        } else writedefault = true;
        if(json["tempUnit"]) {
            strcpy(settings.tempUnit, json["tempUnit"]);
            writedefault |= checkValidNumParm(settings.tempUnit, 0, 1, DEF_TEMP_UNIT);
        } else writedefault = true;
        if(json["tempOffs"]) {
            strcpy(settings.tempOffs, json["tempOffs"]);
            writedefault |= checkValidNumParmF(settings.tempOffs, -3.0, 3.0, DEF_TEMP_OFFS);
        } else writedefault = true;
        #endif

        #ifdef TC_HAVESPEEDO
        if(json["useSpeedo"]) {
            strcpy(settings.useSpeedo, json["useSpeedo"]);
            writedefault |= checkValidNumParm(settings.useSpeedo, 0, 1, DEF_USE_SPEEDO);
        } else writedefault = true;
        if(json["speedoType"]) {
            strcpy(settings.speedoType, json["speedoType"]);
            writedefault |= checkValidNumParm(settings.speedoType, SP_MIN_TYPE, SP_NUM_TYPES-1, DEF_SPEEDO_TYPE);
        } else writedefault = true;
        if(json["speedoBright"]) {
            strcpy(settings.speedoBright, json["speedoBright"]);
            writedefault |= checkValidNumParm(settings.speedoBright, 0, 15, DEF_BRIGHT_SPEEDO);
        } else writedefault = true;
        if(json["speedoFact"]) {
            strcpy(settings.speedoFact, json["speedoFact"]);
            writedefault |= checkValidNumParmF(settings.speedoFact, 0.5, 5.0, DEF_SPEEDO_FACT);
        } else writedefault = true;
        #ifdef TC_HAVEGPS
        if(json["useGPSSpeed"]) {
            strcpy(settings.useGPSSpeed, json["useGPSSpeed"]);
            writedefault |= checkValidNumParm(settings.useGPSSpeed, 0, 1, DEF_USE_GPS_SPEED);
        } else writedefault = true;
        #endif
        #ifdef TC_HAVETEMP
        if(json["dispTemp"]) {
            strcpy(settings.useTemp, json["dispTemp"]);
            writedefault |= checkValidNumParm(settings.dispTemp, 0, 1, DEF_DISP_TEMP);
        } else writedefault = true;
        if(json["tempBright"]) {
            strcpy(settings.tempBright, json["tempBright"]);
            writedefault |= checkValidNumParm(settings.tempBright, 0, 15, DEF_TEMP_BRIGHT);
        } else writedefault = true;
        #endif
        #endif // HAVESPEEDO
        
        #ifdef FAKE_POWER_ON
        if(json["fakePwrOn"]) {
            strcpy(settings.fakePwrOn, json["fakePwrOn"]);
            writedefault |= checkValidNumParm(settings.fakePwrOn, 0, 1, DEF_FAKE_PWR);
        } else writedefault = true;
        #endif

        #ifdef EXTERNAL_TIMETRAVEL_IN
        if(json["ettDelay"]) {
            strcpy(settings.ettDelay, json["ettDelay"]);
            writedefault |= checkValidNumParm(settings.ettDelay, 0, ETT_MAX_DEL, DEF_ETT_DELAY);
        } else writedefault = true;
        if(json["ettLong"]) {
            strcpy(settings.ettLong, json["ettLong"]);
            writedefault |= checkValidNumParm(settings.ettLong, 0, 1, DEF_ETT_LONG);
        } else writedefault = true;
        #endif
        
        #ifdef EXTERNAL_TIMETRAVEL_OUT
        if(json["useETTO"]) {
            strcpy(settings.useETTO, json["useETTO"]);
            writedefault |= checkValidNumParm(settings.useETTO, 0, 1, DEF_USE_ETTO);
        } else writedefault = true;
        #endif
        if(json["playTTsnds"]) {
            strcpy(settings.playTTsnds, json["playTTsnds"]);
            writedefault |= checkValidNumParm(settings.playTTsnds, 0, 1, DEF_PLAY_TT_SND);
        } else writedefault = true;

        if(json["shuffle"]) {
            strcpy(settings.shuffle, json["shuffle"]);
            writedefault |= checkValidNumParm(settings.shuffle, 0, 1, DEF_SHUFFLE);
        } else writedefault = true;

        if(json["CfgOnSD"]) {
            strcpy(settings.CfgOnSD, json["CfgOnSD"]);
            writedefault |= checkValidNumParm(settings.CfgOnSD, 0, 1, DEF_CFG_ON_SD);
        } else writedefault = true;
        if(json["sdFreq"]) {
            strcpy(settings.sdFreq, json["sdFreq"]);
            writedefault |= checkValidNumParm(settings.sdFreq, 0, 1, DEF_SD_FREQ);
        } else writedefault = true;

    } else {

        Serial.println(F("read_settings: Failed to parse settings file"));

        writedefault = true;

    }

    return writedefault;
}

void write_settings()
{
    DynamicJsonDocument json(JSON_SIZE);
    //StaticJsonDocument<JSON_SIZE> json;

    if(!haveFS) {
        Serial.println(F("write_settings: Cannot write settings, FS not available"));
        return;
    }

    #ifdef TC_DBG
    Serial.println(F("write_settings: Writing config file"));
    #endif
    
    json["timeTrPers"] = settings.timesPers;
    json["alarmRTC"] = settings.alarmRTC;
    json["mode24"] = settings.mode24;
    json["playIntro"] = settings.playIntro;
    json["autoRotateTimes"] = settings.autoRotateTimes;

    json["hostName"] = settings.hostName;
    json["wifiConRetries"] = settings.wifiConRetries;
    json["wifiConTimeout"] = settings.wifiConTimeout;
    json["wifiOffDelay"] = settings.wifiOffDelay;
    json["wifiAPOffDelay"] = settings.wifiAPOffDelay;
    
    json["timeZone"] = settings.timeZone;
    json["ntpServer"] = settings.ntpServer;
    #ifdef TC_HAVEGPS
    json["useGPS"] = settings.useGPS;
    #endif
    
    json["destTimeBright"] = settings.destTimeBright;
    json["presTimeBright"] = settings.presTimeBright;
    json["lastTimeBright"] = settings.lastTimeBright;

    json["dtNmOff"] = settings.dtNmOff;
    json["ptNmOff"] = settings.ptNmOff;
    json["ltNmOff"] = settings.ltNmOff;
    json["autoNM"] = settings.autoNM;
    json["autoNMPreset"] = settings.autoNMPreset;
    json["autoNMOn"] = settings.autoNMOn;
    json["autoNMOff"] = settings.autoNMOff;
    #ifdef TC_HAVELIGHT
    json["useLight"] = settings.useLight;
    json["luxLimit"] = settings.luxLimit;
    #endif

    #ifdef TC_HAVETEMP
    json["useTemp"] = settings.useTemp;
    json["tempUnit"] = settings.tempUnit;
    json["tempOffs"] = settings.tempOffs;
    #endif

    #ifdef TC_HAVESPEEDO
    json["useSpeedo"] = settings.useSpeedo;
    json["speedoType"] = settings.speedoType;
    json["speedoBright"] = settings.speedoBright;
    json["speedoFact"] = settings.speedoFact;
    #ifdef TC_HAVEGPS
    json["useGPSSpeed"] = settings.useGPSSpeed;
    #endif
    #ifdef TC_HAVETEMP
    json["dispTemp"] = settings.dispTemp;
    json["tempBright"] = settings.tempBright;
    #endif
    #endif // HAVESPEEDO
    
    #ifdef FAKE_POWER_ON
    json["fakePwrOn"] = settings.fakePwrOn;
    #endif

    #ifdef EXTERNAL_TIMETRAVEL_IN
    json["ettDelay"] = settings.ettDelay;
    json["ettLong"] = settings.ettLong;
    #endif

    #ifdef EXTERNAL_TIMETRAVEL_OUT
    json["useETTO"] = settings.useETTO;
    #endif
    json["playTTsnds"] = settings.playTTsnds;

    json["shuffle"] = settings.shuffle;

    json["CfgOnSD"] = settings.CfgOnSD;
    json["sdFreq"] = settings.sdFreq;

    File configFile = SPIFFS.open(cfgName, FILE_WRITE);

    if(configFile) {

        #ifdef TC_DBG
        serializeJson(json, Serial);
        Serial.println(F(" "));
        #endif
        
        serializeJson(json, configFile);
        configFile.close();

    } else {

        Serial.println(F("write_settings: Failed to open file for writing"));

    }
}

bool checkConfigExists()
{
    return SPIFFS.exists(cfgName);
}

// Helper for checking validity of numerical user-entered parameters
static bool checkValidNumParm(char *text, int lowerLim, int upperLim, int setDefault)
{
    int i, len = strlen(text);

    if(len == 0) {
        sprintf(text, "%d", setDefault);
        return true;
    }

    for(i = 0; i < len; i++) {
        if(text[i] < '0' || text[i] > '9') {
            sprintf(text, "%d", setDefault);
            return true;
        }
    }

    i = (int)(atoi(text));

    if(i < lowerLim) {
        sprintf(text, "%d", lowerLim);
        return true;
    }
    if(i > upperLim) {
        sprintf(text, "%d", upperLim);
        return true;
    }

    return false;
}

static bool checkValidNumParmF(char *text, double lowerLim, double upperLim, double setDefault)
{
    int i, len = strlen(text);
    double f;

    if(len == 0) {
        sprintf(text, "%1.1f", setDefault);
        return true;
    }

    for(i = 0; i < len; i++) {
        if(text[i] != '.' && text[i] != '-' && (text[i] < '0' || text[i] > '9')) {
            sprintf(text, "%1.1f", setDefault);
            return true;
        }
    }

    f = (double)(atof(text));

    if(f < lowerLim) {
        sprintf(text, "%1.1f", lowerLim);
        return true;
    }
    if(f > upperLim) {
        sprintf(text, "%1.1f", upperLim);
        return true;
    }

    return false;
}

/*
 *  Load/save the Alarm time and settings
 */

bool loadAlarm()
{
    bool writedefault = true;
    bool haveConfigFile = false;
    File configFile;

    if( (!configOnSD && !haveFS) ||
        (configOnSD && !haveSD) ) {
        Serial.println(F("loadAlarm: FS not available"));
        return false;
    }

    if(configOnSD) {
        #ifdef TC_DBG
        Serial.println(F("loadAlarm: Loading from SD"));
        #endif
        if(SD.exists(almCfgName)) {
            haveConfigFile = (configFile = SD.open(almCfgName, "r"));
        }
    } else {
        #ifdef TC_DBG
        Serial.println(F("loadAlarm: Loading from flash FS"));
        #endif
        if(SPIFFS.exists("/alarmconfig.json")) {
            SPIFFS.rename("/alarmconfig.json", almCfgName);
        }
        if(SPIFFS.exists(almCfgName)) {
            haveConfigFile = (configFile = SPIFFS.open(almCfgName, "r"));
        }
    }

    if(haveConfigFile) {
        StaticJsonDocument<512> json;
        DeserializationError error = deserializeJson(json, configFile);

        #ifdef TC_DBG
        serializeJson(json, Serial);
        Serial.println(F(" "));
        #endif

        if(!error) {
            if(json["alarmonoff"] && json["alarmhour"] && json["alarmmin"]) {
                int aoo = atoi(json["alarmonoff"]);
                alarmHour = atoi(json["alarmhour"]);
                alarmMinute = atoi(json["alarmmin"]);
                alarmOnOff = ((aoo & 0x0f) != 0);
                alarmWeekday = (aoo & 0xf0) >> 4;
                if(alarmWeekday > 9) alarmWeekday = 0;
                if(((alarmHour   == 255) || (alarmHour   >= 0 && alarmHour   <= 23)) &&
                   ((alarmMinute == 255) || (alarmMinute >= 0 && alarmMinute <= 59))) {
                    writedefault = false;
                }
            }
        } 
        configFile.close();
    }

    if(writedefault) {

        alarmHour = alarmMinute = 255;
        alarmOnOff = false;
        alarmWeekday = 0;

        Serial.println(F("loadAlarm: Alarm settings missing or incomplete; writing new file"));

        saveAlarm();

    }

    return true;
}

void saveAlarm()
{
    char aooBuf[8];
    char hourBuf[8];
    char minBuf[8];
    bool haveConfigFile = false;
    File configFile;
    StaticJsonDocument<512> json;

    if( (!configOnSD && !haveFS) ||
        (configOnSD && !haveSD) ) {
        Serial.println(F("saveAlarm: FS not available"));
        return;
    }

    sprintf(aooBuf, "%d", (alarmWeekday * 16) + (alarmOnOff ? 1 : 0));
    json["alarmonoff"] = (char *)aooBuf;

    sprintf(hourBuf, "%d", alarmHour);
    sprintf(minBuf, "%d", alarmMinute);

    json["alarmhour"] = (char *)hourBuf;
    json["alarmmin"] = (char *)minBuf;

    if(configOnSD) {
        haveConfigFile = (configFile = SD.open(almCfgName, FILE_WRITE));
    } else {
        haveConfigFile = (configFile = SPIFFS.open(almCfgName, FILE_WRITE));
    }

    #ifdef TC_DBG
    serializeJson(json, Serial);
    Serial.println(F(" "));
    #endif

    if(haveConfigFile) {
        serializeJson(json, configFile);
        configFile.close();
    } else {
        Serial.println(F("saveAlarm: Failed to open file for writing"));
    }
}

/*
 *  Load/save the Volume
 */

bool loadCurVolume()
{
    uint8_t loadBuf[2];
    bool writedefault = true;
    bool haveConfigFile = false;
    File configFile;

    if( (!configOnSD && !haveFS) ||
        (configOnSD && !haveSD) ) {
        Serial.println(F("loadCurVolume: FS not available"));
        return false;
    }

    if(configOnSD) {
        #ifdef TC_DBG
        Serial.println(F("loadCurVolume: Loading from SD"));
        #endif
        if(SD.exists(volCfgName)) {
            haveConfigFile = (configFile = SD.open(volCfgName, "r"));
        }
    } else {
        #ifdef TC_DBG
        Serial.println(F("loadCurVolume: Loading from flash FS"));
        #endif
        if(SPIFFS.exists(volCfgName)) {
            haveConfigFile = (configFile = SPIFFS.open(volCfgName, "r"));
        } else {
            loadBuf[0] = EEPROM.read(SWVOL_PREF);
            loadBuf[1] = EEPROM.read(SWVOL_PREF + 1) ^ 0xff;
            if(loadBuf[0] == loadBuf[1]) {
                curVolume = loadBuf[0];
            }
        }
    }

    if(haveConfigFile) {
        StaticJsonDocument<512> json;
        DeserializationError error = deserializeJson(json, configFile);

        #ifdef TC_DBG
        serializeJson(json, Serial);
        Serial.println(F(" "));
        #endif

        if(!error) {
            if(json["volume"]) {
                curVolume = atoi(json["volume"]);
                if((curVolume >= 0 && curVolume <= 19) || curVolume == 255) {
                    writedefault = false;
                }
            }
        } 
        configFile.close();
    }

    if(writedefault) {
        Serial.println(F("loadCurVolume: Settings missing or incomplete; writing new file"));
        saveCurVolume();
    }

    return true;
}

void saveCurVolume()
{
    char buf[6];
    bool haveConfigFile = false;
    File configFile;
    StaticJsonDocument<512> json;

    if( (!configOnSD && !haveFS) ||
        (configOnSD && !haveSD) ) {
        Serial.println(F("saveCurVolume: FS not available"));
        return;
    }

    sprintf(buf, "%d", curVolume);
    json["volume"] = (char *)buf;

    if(configOnSD) {
        haveConfigFile = (configFile = SD.open(volCfgName, FILE_WRITE));
    } else {
        haveConfigFile = (configFile = SPIFFS.open(volCfgName, FILE_WRITE));
    }

    #ifdef TC_DBG
    serializeJson(json, Serial);
    Serial.println(F(" "));
    #endif

    if(haveConfigFile) {
        serializeJson(json, configFile);
        configFile.close();
    } else {
        Serial.println(F("saveCurVolume: Failed to open file for writing"));
    }
}

/* Copy alarm/volume settings from/to SD if user
 * changed "save to SD"-option in CP
 */

void copySettings()
{   
    if(!haveSD) return;

    #ifdef TC_DBG
    Serial.println(F("copySettings: Copying alarm/vol settings to other medium"));
    #endif

    configOnSD = !configOnSD;

    saveCurVolume();
    saveAlarm();

    configOnSD = !configOnSD;
}

/*
 * Load/save Music Folder Number
 */
bool loadMusFoldNum()
{
    bool writedefault = true;

    if(!haveSD) {
        return false;
    }

    if(SD.exists(musCfgName)) {

        File configFile = SD.open(musCfgName, "r");

        if(configFile) {

            StaticJsonDocument<512> json;
            DeserializationError error = deserializeJson(json, configFile);

            #ifdef TC_DBG
            serializeJson(json, Serial);
            Serial.println(F(" "));
            #endif

            if(!error) {

                if(json["folder"]) {
                    musFolderNum = atoi(json["folder"]);
                    if(musFolderNum >= 0 && musFolderNum <= 9) {
                        writedefault = false;
                    }
                }
                
            } else {

                Serial.println(F("loadMusFoldNum: Failed to parse file"));

            }

            configFile.close();

        }

    }

    if(writedefault) {
        musFolderNum = 0;
        saveMusFoldNum();
    }

    return true;
}

void saveMusFoldNum()
{
    StaticJsonDocument<512> json;
    char buf[4];

    if(!haveSD) {
        return;
    }

    sprintf(buf, "%1d", musFolderNum);
    json["folder"] = buf;
    
    File configFile = SD.open(musCfgName, FILE_WRITE);

    #ifdef TC_DBG
    Serial.println(F("saveMusFoldNum: Writing file"));
    serializeJson(json, Serial);
    Serial.println(F(" "));
    #endif

    if(configFile) {
        serializeJson(json, configFile);
        configFile.close();
    } else {
        Serial.println(F("saveMusFoldNum: Failed to open file for writing"));
    }
}

/*
 * Load/save/delete settings for static IP configuration
 */

bool loadIpSettings()
{
    bool invalid = false;
    bool haveConfig = false;

    if(!haveFS) {
        return false;
    }

    if(SPIFFS.exists(ipCfgName)) {

        File configFile = SPIFFS.open(ipCfgName, "r");

        if(configFile) {

            #ifdef TC_DBG
            Serial.println(F("loadIpSettings: Opened ip config file"));
            #endif

            StaticJsonDocument<512> json;
            DeserializationError error = deserializeJson(json, configFile);

            #ifdef TC_DBG
            serializeJson(json, Serial);
            Serial.println(F(" "));
            #endif

            if(!error) {

                #ifdef TC_DBG
                Serial.println(F("loadIpSettings: Parsed json"));
                #endif

                if(json["IpAddress"]) {
                    strcpy(ipsettings.ip, json["IpAddress"]);
                } else invalid = true;
                if(json["Gateway"]) {
                    strcpy(ipsettings.gateway, json["Gateway"]);
                } else invalid = true;
                if(json["Netmask"]) {
                    strcpy(ipsettings.netmask, json["Netmask"]);
                } else invalid = true;
                if(json["DNS"]) {
                    strcpy(ipsettings.dns, json["DNS"]);
                } else invalid = true;

                if(!invalid) haveConfig = true;

            } else {

                Serial.println(F("loadIpSettings: Failed to parse file"));

                invalid = true;

            }

            configFile.close();

        }

    }

    if(invalid) {

        // config file is invalid - delete it

        Serial.println(F("loadIpSettings: IP settings invalid; deleting file"));

        deleteIpSettings();

        ipsettings.ip[0] = '\0';
        ipsettings.gateway[0] = '\0';
        ipsettings.netmask[0] = '\0';
        ipsettings.dns[0] = '\0';

    }

    return haveConfig;
}

void writeIpSettings()
{
    StaticJsonDocument<512> json;

    if(!haveFS) {
        Serial.println(F("writeIpSettings: FS not mounted"));
        return;
    }

    #ifdef TC_DBG
    Serial.println(F("writeIpSettings: Writing file"));
    #endif

    json["IpAddress"] = ipsettings.ip;
    json["Gateway"] = ipsettings.gateway;
    json["Netmask"] = ipsettings.netmask;
    json["DNS"] = ipsettings.dns;

    File configFile = SPIFFS.open(ipCfgName, FILE_WRITE);

    #ifdef TC_DBG
    serializeJson(json, Serial);
    Serial.println(F(" "));
    #endif

    if(configFile) {
        serializeJson(json, configFile);
        configFile.close();
    } else {
        Serial.println(F("writeIpSettings: Failed to open file for writing"));
    }
}

void deleteIpSettings()
{
    if(!haveFS) {
        return;
    }

    #ifdef TC_DBG
    Serial.println(F("deleteIpSettings: Deleting ipconfig.json"));
    #endif

    SPIFFS.remove(ipCfgName);
}

/*
 * Audio file installer
 *
 * Copies our default audio files from SD to flash FS.
 * The is restricted to the original default audio
 * files that came with the software. If you want to
 * customize your sounds, put them on a FAT formatted
 * SD card and leave this SD card in the slot.
 */

bool check_allow_CPA()
{
    return allowCPA;
}

static bool check_if_default_audio_present()
{
    File file;
    size_t ts;
    int i, idx = 0;
    char dtmf_buf[16] = "/Dtmf-0.mp3\0";
    size_t sizes[10+NUM_AUDIOFILES] = {
#ifndef TWSOUND
      4178, 4178, 4178, 4178, 4178, 4178, 3760, 3760, 4596, 3760, // DTMF
      70664, 71500, 60633, 10478,           // alarm, alarmoff, alarmon, baddate
      15184, 22983, 33364, 51701,           // ee1, ee2, ee3, ee4
      13374, 125804, 33853, 47228,          // enter, intro, nmoff, nmon
      16747, 3790, 21907, 38899, 135447     // ping, shutdown, startup, timetravel, travelstart
#else
      4178, 4178, 4178, 4178, 4178, 4178, 3760, 3760, 4596, 3760, //DTMF
      70664, 71500, 60633, 10478,           // alarm, alarmoff, alarmon, baddate
      15184, 22983, 33364, 51701,           // ee1, ee2, ee3, ee4
      12149, 125804, 33853, 47228,          // enter, intro, nmoff, nmon
      16747, 3790, 18419, 38899, 135447     // ping, shutdown, startup, timetravel, travelstart
#endif
    };

    if(!haveSD)
        return false;

    // If identifier missing, quit now
    if(!(SD.exists("/TCD_def_snd.txt"))) {
        #ifdef TC_DBG
        Serial.println("SD: ID file missing");
        #endif
        return false;
    }

    for(i = 0; i < 10; i++) {
        dtmf_buf[6] = i + '0';
        if(!(SD.exists(dtmf_buf))) {
            #ifdef TC_DBG
            Serial.print("missing: ");
            Serial.println(dtmf_buf);
            #endif
            return false;
        }
        if(!(file = SD.open(dtmf_buf)))
            return false;
        ts = file.size();
        file.close();
        #ifdef TC_DBG
        sizes[idx++] = ts;
        #else
        if(sizes[idx++] != ts)
            return false;
        #endif
    }

    for(i = 0; i < NUM_AUDIOFILES; i++) {
        if(!SD.exists(audioFiles[i])) {
            #ifdef TC_DBG
            Serial.print("missing: ");
            Serial.println(audioFiles[i]);
            #endif
            return false;
        }
        if(!(file = SD.open(audioFiles[i])))
            return false;
        ts = file.size();
        file.close();
        #ifdef TC_DBG
        sizes[idx++] = ts;
        #else
        if(sizes[idx++] != ts)
            return false;
        #endif
    }

    #ifdef TC_DBG
    for(i = 0; i < (10+NUM_AUDIOFILES); i++) {
        Serial.print(sizes[i], DEC);
        Serial.print(", ");
    }
    Serial.println("");
    #endif

    return true;
}

bool copy_audio_files()
{
    char dtmf_buf[16] = "/Dtmf-0.mp3\0";
    int i, haveErr = 0;

    if(!allowCPA) {
        Serial.println(F("copy_audio_files: SD missing or flash FS not mounted"));
        return false;
    }

    start_file_copy();

    /* Copy DTMF files */
    for(i = 0; i < 10; i++) {
        dtmf_buf[6] = i + '0';
        open_and_copy(dtmf_buf, haveErr);
    }

    for(i = 0; i < NUM_AUDIOFILES; i++) {
        open_and_copy(audioFiles[i], haveErr);
    }

    if(haveErr) {
        file_copy_error();
    } else {
        file_copy_done();
    }

    return (haveErr == 0);
}

static void open_and_copy(const char *fn, int& haveErr)
{
    File sFile, dFile;

    if((sFile = SD.open(fn, FILE_READ))) {
        #ifdef TC_DBG
        Serial.print(F("copy_audio_files: Opened source file: "));
        Serial.println(fn);
        #endif
        if((dFile = SPIFFS.open(fn, FILE_WRITE))) {
            #ifdef TC_DBG
            Serial.print(F("copy_audio_files: Opened destination file: "));
            Serial.println(fn);
            #endif
            if(!filecopy(sFile, dFile)) {
                haveErr++;
            }
            dFile.close();
        } else {
            Serial.print(F("copy_audio_files: Error opening destination file: "));
            Serial.println(fn);
            haveErr++;
        }
        sFile.close();
    } else {
        Serial.print(F("copy_audio_files: Error opening source file: "));
        Serial.println(fn);
        haveErr++;
    }
}

static bool filecopy(File source, File dest)
{
    uint8_t buffer[1024];
    size_t bytesr, bytesw;

    while((bytesr = source.read(buffer, 1024))) {
        if((bytesw = dest.write(buffer, bytesr)) != bytesr) {
            Serial.println(F("filecopy: Error writing data"));
            return false;
        }
        file_copy_progress();
    }

    return true;
}

void formatFlashFS()
{
    #ifdef TC_DBG
    Serial.println(F("Formatting flash FS"));
    #endif
    SPIFFS.format();
}

void rewriteSecondarySettings()
{
    #ifdef TC_DBG
    Serial.println("Re-writing IP settings");
    #endif
    writeIpSettings();    // re-write IP settings
    #ifdef TC_DBG
    Serial.println("Re-writing alarm settings");
    #endif
    saveAlarm();          // re-write alarm settings
}
