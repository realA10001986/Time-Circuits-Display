/*
 * -------------------------------------------------------------------
 * CircuitSetup.us Time Circuits Display
 * (C) 2021-2022 John deGlavina https://circuitsetup.us
 * (C) 2022-2023 Thomas Winischhofer (A10001986)
 * https://github.com/realA10001986/Time-Circuits-Display
 * https://tcd.backtothefutu.re
 *
 * Settings & file handling
 * 
 * Main and IP settings are stored on flash FS only
 * Alarm, Reminder and volume either on SD or on flash FS
 * Music Folder number always on SD
 *
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

#define ARDUINOJSON_USE_LONG_LONG 0
#define ARDUINOJSON_USE_DOUBLE 0
#define ARDUINOJSON_ENABLE_ARDUINO_STRING 0
#define ARDUINOJSON_ENABLE_ARDUINO_STREAM 0
#define ARDUINOJSON_ENABLE_STD_STREAM 0
#define ARDUINOJSON_ENABLE_STD_STRING 0
#define ARDUINOJSON_ENABLE_NAN 0
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

#include "tc_settings.h"
#include "tc_menus.h"
#include "tc_audio.h"
#include "tc_time.h"
#include "tc_wifi.h"
#ifdef HAVE_STALE_PRESENT
#include "clockdisplay.h"
#endif

// Size of main config JSON
// Needs to be adapted when config grows
#define JSON_SIZE 2500

/* If SPIFFS/LittleFS is mounted */
bool haveFS = false;

/* If a SD card is found */
bool haveSD = false;

/* If SD contains default audio files */
static bool allowCPA = false;

/* Music Folder Number */
uint8_t musFolderNum = 0;

/* Save alarm/volume on SD? */
static bool configOnSD = false;

/* Paranoia: No writes Flash-FS  */
bool FlashROMode = false;

#define NUM_AUDIOFILES 21
#define SND_ENTER_IDX  9
#ifndef TWSOUND
#define SND_ENTER_LEN   13374
#define SND_STARTUP_LEN 21907
#else
#define SND_ENTER_LEN   12149
#define SND_STARTUP_LEN 18419
#endif
static const char *audioFiles[NUM_AUDIOFILES] = {
      "/alarm.mp3",
      "/alarmoff.mp3",
      "/alarmon.mp3",
      "/baddate.mp3",
      "/ee1.mp3",
      "/ee2.mp3",
      "/ee3.mp3",
      "/ee4.mp3",
      "/ee5.mp3",
      "/enter.mp3",
      "/intro.mp3",
      "/nmoff.mp3",
      "/nmon.mp3",
      "/ping.mp3",
      "/reminder.mp3",
      "/shutdown.mp3",
      "/startup.mp3",
      "/timer.mp3",
      "/timetravel.mp3",
      "/travelstart.mp3",
      "/travelstart2.mp3"
};
static const char *IDFN = "/TCD_def_snd.txt";

static const char *cfgName    = "/config.json";     // Main config (flash)
static const char *almCfgName = "/tcdalmcfg.json";  // Alarm config (flash/SD)
static const char *remCfgName = "/tcdremcfg.json";  // Reminder config (flash/SD)
static const char *volCfgName = "/tcdvolcfg.json";  // Volume config (flash/SD)
static const char *musCfgName = "/tcdmcfg.json";    // Music config (SD)
static const char *ipCfgName  = "/ipconfig.json";   // IP config (flash)
static const char *cmCfgName  = "/cmconfig.json";   // Carmode (flash/SD)
#ifdef HAVE_STALE_PRESENT
static const char *stCfgName  = "/stconfig";        // Exhib Mode (flash/SD)
#endif

static const char *fsNoAvail = "File System not available";
static const char *failFileWrite = "Failed to open file for writing";
#ifdef TC_DBG
static const char *badConfig = "Settings bad/missing/incomplete; writing new file";
#endif

static bool read_settings(File configFile);

static void deleteReminder();

static void loadCarMode();

static bool CopyCheckValidNumParm(const char *json, char *text, uint8_t psize, int lowerLim, int upperLim, int setDefault);
static bool CopyCheckValidNumParmF(const char *json, char *text, uint8_t psize, float lowerLim, float upperLim, float setDefault);
static bool checkValidNumParm(char *text, int lowerLim, int upperLim, int setDefault);
static bool checkValidNumParmF(char *text, float lowerLim, float upperLim, float setDefault);

static void open_and_copy(const char *fn, int& haveErr);
static bool filecopy(File source, File dest);
static bool check_if_default_audio_present();

static bool CopyIPParm(const char *json, char *text, uint8_t psize);

extern void start_file_copy();
extern void file_copy_progress();
extern void file_copy_done();
extern void file_copy_error();

static DeserializationError readJSONCfgFile(JsonDocument& json, File& configFile, const char *funcName);
static bool writeJSONCfgFile(const JsonDocument& json, const char *fn, bool useSD, const char *funcName);

/*
 * settings_setup()
 * 
 * Mount SPIFFS/LittleFS and SD (if available).
 * Read configuration from JSON config file
 * If config file not found, create one with default settings
 *
 * If the device is powered on or reset while ENTER is held down, 
 * the IP settings file will be deleted and the device will use DHCP.
 */
void settings_setup()
{
    const char *funcName = "settings_setup";
    bool writedefault = false;
    bool SDres = false;

    // Pre-maturely use ENTER button (initialized again in keypad_setup())
    pinMode(ENTER_BUTTON_PIN, INPUT_PULLUP);
    delay(20);

    #ifdef TC_DBG
    Serial.printf("%s: Mounting flash FS... ", funcName);
    #endif

    if(SPIFFS.begin()) {

        haveFS = true;

    } else {

        #ifdef TC_DBG
        Serial.print(F("failed, formatting... "));
        #endif

        destinationTime.showTextDirect("WAIT");

        SPIFFS.format();
        if(SPIFFS.begin()) haveFS = true;

        destinationTime.showTextDirect("");

    }

    if(haveFS) {
      
        #ifdef TC_DBG
        Serial.println(F("ok, loading settings"));
        int tBytes = SPIFFS.totalBytes(); int uBytes = SPIFFS.usedBytes();
        Serial-printf("FlashFS: %d total, %d used\n", tBytes, uBytes);
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

        // Write new config file after mounting SD and determining FlashROMode

    } else {

        Serial.println(F("\n***Mounting flash FS failed."));
        Serial.println(F("*** All settings will be stored on the SD card (if available)"));

    }
    
    // Set up SD card
    SPI.begin(SPI_SCK_PIN, SPI_MISO_PIN, SPI_MOSI_PIN);

    haveSD = false;

    uint32_t sdfreq = (settings.sdFreq[0] == '0') ? 16000000 : 4000000;
    #ifdef TC_DBG
    Serial.printf("%s: SD/SPI frequency %dMHz\n", funcName, sdfreq / 1000000);
    #endif

    #ifdef TC_DBG
    Serial.printf("%s: Mounting SD... ", funcName);
    #endif

    if(!(SDres = SD.begin(SD_CS_PIN, SPI, sdfreq))) {
        #ifdef TC_DBG
        Serial.printf("Retrying at 25Mhz... ");
        #endif
        SDres = SD.begin(SD_CS_PIN, SPI, 25000000);
    }

    if(SDres) {

        #ifdef TC_DBG
        Serial.println(F("ok"));
        #endif

        uint8_t cardType = SD.cardType();
       
        #ifdef TC_DBG
        const char *sdTypes[5] = { "No card", "MMC", "SD", "SDHC", "unknown (SD not usable)" };
        Serial.printf("SD card type: %s\n", sdTypes[cardType > 4 ? 4 : cardType]);
        #endif

        haveSD = ((cardType != CARD_NONE) && (cardType != CARD_UNKNOWN));

    } else {

        Serial.println(F("No SD card found"));

    }

    if(haveSD) {
        if(SD.exists("/TCD_FLASH_RO") || !haveFS) {
            bool writedefault2 = false;
            FlashROMode = true;
            Serial.println(F("Flash-RO mode: All settings stored on SD. Reloading settings."));
            if(SD.exists(cfgName)) {
                File configFile = SD.open(cfgName, "r");
                if(configFile) {
                    writedefault2 = read_settings(configFile);
                    configFile.close();
                } else {
                    writedefault2 = true;
                }
            } else {
                writedefault2 = true;
            }
            if(writedefault2) {
                #ifdef TC_DBG
                Serial.printf("%s: %s\n", funcName, badConfig);
                #endif
                write_settings();
            }
        }
    }

    // Now write new config to flash FS if old one somehow bad
    // Only write this file if FlashROMode is off
    if(haveFS && writedefault && !FlashROMode) {
        #ifdef TC_DBG
        Serial.printf("%s: %s\n", funcName, badConfig);
        #endif
        write_settings();
    }

    // Determine if alarm/reminder/volume/carmode settings are to be stored on SD
    configOnSD = (haveSD && ((settings.CfgOnSD[0] != '0') || FlashROMode));

    // Check if SD contains our default sound files
    // We don't do that if in FlashROMode mode, of course
    if(haveFS && haveSD && !FlashROMode) {
        allowCPA = check_if_default_audio_present();
    }

    // Load carMode setting
    loadCarMode();

    // Allow user to delete static IP data by holding ENTER
    // while booting
    // (10 secs timeout to wait for button-release to allow
    // to run fw without control board attached)
    if(digitalRead(ENTER_BUTTON_PIN)) {

        unsigned long mnow = millis();

        Serial.printf("%s: Deleting ip config; temporarily clearing AP mode WiFi password\n", funcName);

        deleteIpSettings();

        // Set AP mode password to empty (not written, only until reboot!)
        settings.appw[0] = 0;

        // Pre-maturely use white led (initialized again in keypad_setup())
        pinMode(WHITE_LED_PIN, OUTPUT);
        digitalWrite(WHITE_LED_PIN, HIGH);
        while(digitalRead(ENTER_BUTTON_PIN)) {
            if(millis() - mnow > 10*1000) break;
        }
        digitalWrite(WHITE_LED_PIN, LOW);
    }
}

void unmount_fs()
{
    if(haveFS) {
        SPIFFS.end();
        #ifdef TC_DBG
        Serial.println(F("Unmounted Flash FS"));
        #endif
        haveFS = false;
    }
    if(haveSD) {
        SD.end();
        #ifdef TC_DBG
        Serial.println(F("Unmounted SD card"));
        #endif
        haveSD = false;
    }
}

static bool read_settings(File configFile)
{
    const char *funcName = "read_settings";
    bool wd = false;
    size_t jsonSize = 0;
    //StaticJsonDocument<JSON_SIZE> json;
    DynamicJsonDocument json(JSON_SIZE);

    DeserializationError error = readJSONCfgFile(json, configFile, funcName);

    jsonSize = json.memoryUsage();
    if(jsonSize > JSON_SIZE) {
        Serial.printf("%s: ERROR: Config too large (%d vs %d), memory corrupted.\n", funcName, jsonSize, JSON_SIZE);
    }
    
    #ifdef TC_DBG
    if(jsonSize > JSON_SIZE - 256) {
          Serial.printf("%s: WARNING: JSON_SIZE needs to be adapted **************\n", funcName);
    }
    Serial.printf("%s: Size of document: %d (JSON_SIZE %d)\n", funcName, jsonSize, JSON_SIZE);
    #endif

    if(!error) {

        wd |= CopyCheckValidNumParm(json["timeTrPers"], settings.timesPers, sizeof(settings.timesPers), 0, 1, DEF_TIMES_PERS);
        wd |= CopyCheckValidNumParm(json["alarmRTC"], settings.alarmRTC, sizeof(settings.alarmRTC), 0, 1, DEF_ALARM_RTC);
        wd |= CopyCheckValidNumParm(json["playIntro"], settings.playIntro, sizeof(settings.playIntro), 0, 1, DEF_PLAY_INTRO);
        wd |= CopyCheckValidNumParm(json["mode24"], settings.mode24, sizeof(settings.mode24), 0, 1, DEF_MODE24);
        wd |= CopyCheckValidNumParm(json["beep"], settings.beep, sizeof(settings.beep), 0, 3, DEF_BEEP);
        wd |= CopyCheckValidNumParm(json["autoRotateTimes"], settings.autoRotateTimes, sizeof(settings.autoRotateTimes), 0, 5, DEF_AUTOROTTIMES);

        if(json["hostName"]) {
            memset(settings.hostName, 0, sizeof(settings.hostName));
            strncpy(settings.hostName, json["hostName"], sizeof(settings.hostName) - 1);
        } else wd = true;

        if(json["systemID"]) {
            memset(settings.systemID, 0, sizeof(settings.systemID));
            strncpy(settings.systemID, json["systemID"], sizeof(settings.systemID) - 1);
        } else wd = true;

        if(json["appw"]) {
            memset(settings.appw, 0, sizeof(settings.appw));
            strncpy(settings.appw, json["appw"], sizeof(settings.appw) - 1);
        } else wd = true;
        
        wd |= CopyCheckValidNumParm(json["wifiConRetries"], settings.wifiConRetries, sizeof(settings.wifiConRetries), 1, 10, DEF_WIFI_RETRY);
        wd |= CopyCheckValidNumParm(json["wifiConTimeout"], settings.wifiConTimeout, sizeof(settings.wifiConTimeout), 7, 25, DEF_WIFI_TIMEOUT);
        wd |= CopyCheckValidNumParm(json["wifiOffDelay"], settings.wifiOffDelay, sizeof(settings.wifiOffDelay), 0, 99, DEF_WIFI_OFFDELAY);
        wd |= CopyCheckValidNumParm(json["wifiAPOffDelay"], settings.wifiAPOffDelay, sizeof(settings.wifiAPOffDelay), 0, 99, DEF_WIFI_APOFFDELAY);
        wd |= CopyCheckValidNumParm(json["wifiPRetry"], settings.wifiPRetry, sizeof(settings.wifiPRetry), 0, 1, DEF_WIFI_PRETRY);
        
        if(json["timeZone"]) {
            memset(settings.timeZone, 0, sizeof(settings.timeZone));
            strncpy(settings.timeZone, json["timeZone"], sizeof(settings.timeZone) - 1);
        } else wd = true;
        if(json["ntpServer"]) {
            memset(settings.ntpServer, 0, sizeof(settings.ntpServer));
            strncpy(settings.ntpServer, json["ntpServer"], sizeof(settings.ntpServer) - 1);
        } else wd = true;

        if(json["timeZoneDest"]) {
            memset(settings.timeZoneDest, 0, sizeof(settings.timeZoneDest));
            strncpy(settings.timeZoneDest, json["timeZoneDest"], sizeof(settings.timeZoneDest) - 1);
        } else wd = true;
        if(json["timeZoneDep"]) {
            memset(settings.timeZoneDep, 0, sizeof(settings.timeZoneDep));
            strncpy(settings.timeZoneDep, json["timeZoneDep"], sizeof(settings.timeZoneDep) - 1);
        } else wd = true;
        if(json["timeZoneNDest"]) {
            memset(settings.timeZoneNDest, 0, sizeof(settings.timeZoneNDest));
            strncpy(settings.timeZoneNDest, json["timeZoneNDest"], sizeof(settings.timeZoneNDest) - 1);
        } else wd = true;
        if(json["timeZoneNDep"]) {
            memset(settings.timeZoneNDep, 0, sizeof(settings.timeZoneNDep));
            strncpy(settings.timeZoneNDep, json["timeZoneNDep"], sizeof(settings.timeZoneNDep) - 1);
        } else wd = true;

        wd |= CopyCheckValidNumParm(json["destTimeBright"], settings.destTimeBright, sizeof(settings.destTimeBright), 0, 15, DEF_BRIGHT_DEST);
        wd |= CopyCheckValidNumParm(json["presTimeBright"], settings.presTimeBright, sizeof(settings.presTimeBright), 0, 15, DEF_BRIGHT_PRES);
        wd |= CopyCheckValidNumParm(json["lastTimeBright"], settings.lastTimeBright, sizeof(settings.lastTimeBright), 0, 15, DEF_BRIGHT_DEPA);

        wd |= CopyCheckValidNumParm(json["dtNmOff"], settings.dtNmOff, sizeof(settings.dtNmOff), 0, 1, DEF_DT_OFF);
        wd |= CopyCheckValidNumParm(json["ptNmOff"], settings.ptNmOff, sizeof(settings.ptNmOff), 0, 1, DEF_PT_OFF);
        wd |= CopyCheckValidNumParm(json["ltNmOff"], settings.ltNmOff, sizeof(settings.ltNmOff), 0, 1, DEF_LT_OFF);

        wd |= CopyCheckValidNumParm(json["autoNMPreset"], settings.autoNMPreset, sizeof(settings.autoNMPreset), 0, 10, DEF_AUTONM_PRESET);
        if(json["autoNM"]) {    // transition old boolean "autoNM" setting
            char temp[4] = { '0', 0, 0, 0 };
            CopyCheckValidNumParm(json["autoNM"], temp, sizeof(temp), 0, 1, 0);
            if(temp[0] == '0') strcpy(settings.autoNMPreset, "10");
        }
        
        wd |= CopyCheckValidNumParm(json["autoNMOn"], settings.autoNMOn, sizeof(settings.autoNMOn), 0, 23, DEF_AUTONM_ON);
        wd |= CopyCheckValidNumParm(json["autoNMOff"], settings.autoNMOff, sizeof(settings.autoNMOff), 0, 23, DEF_AUTONM_OFF);
        #ifdef TC_HAVELIGHT
        wd |= CopyCheckValidNumParm(json["useLight"], settings.useLight, sizeof(settings.useLight), 0, 1, DEF_USE_LIGHT);
        wd |= CopyCheckValidNumParm(json["luxLimit"], settings.luxLimit, sizeof(settings.luxLimit), 0, 50000, DEF_LUX_LIMIT);
        #endif

        #ifdef TC_HAVETEMP
        wd |= CopyCheckValidNumParm(json["tempUnit"], settings.tempUnit, sizeof(settings.tempUnit), 0, 1, DEF_TEMP_UNIT);
        wd |= CopyCheckValidNumParmF(json["tempOffs"], settings.tempOffs, sizeof(settings.tempOffs), -3.0, 3.0, DEF_TEMP_OFFS);
        #endif

        #ifdef TC_HAVESPEEDO
        wd |= CopyCheckValidNumParm(json["speedoType"], settings.speedoType, sizeof(settings.speedoType), SP_MIN_TYPE, 99, DEF_SPEEDO_TYPE);
        if(json["useSpeedo"]) {    // transition old boolean "useSpeedo" setting
            char temp[4] = { '0', 0, 0, 0 };
            CopyCheckValidNumParm(json["useSpeedo"], temp, sizeof(temp), 0, 1, 0);
            if(temp[0] == '0') strcpy(settings.speedoType, "99");
        }
        
        wd |= CopyCheckValidNumParm(json["speedoBright"], settings.speedoBright, sizeof(settings.speedoBright), 0, 15, DEF_BRIGHT_SPEEDO);
        wd |= CopyCheckValidNumParmF(json["speedoFact"], settings.speedoFact, sizeof(settings.speedoFact), 0.5, 5.0, DEF_SPEEDO_FACT);
        #ifdef TC_HAVEGPS
        wd |= CopyCheckValidNumParm(json["useGPSSpeed"], settings.useGPSSpeed, sizeof(settings.useGPSSpeed), 0, 1, DEF_USE_GPS_SPEED);
        #endif
        #ifdef TC_HAVETEMP
        wd |= CopyCheckValidNumParm(json["dispTemp"], settings.dispTemp, sizeof(settings.dispTemp), 0, 1, DEF_DISP_TEMP);
        wd |= CopyCheckValidNumParm(json["tempBright"], settings.tempBright, sizeof(settings.tempBright), 0, 15, DEF_TEMP_BRIGHT);
        wd |= CopyCheckValidNumParm(json["tempOffNM"], settings.tempOffNM, sizeof(settings.tempOffNM), 0, 1, DEF_TEMP_OFF_NM);
        #endif
        #endif // HAVESPEEDO
        
        #ifdef FAKE_POWER_ON
        wd |= CopyCheckValidNumParm(json["fakePwrOn"], settings.fakePwrOn, sizeof(settings.fakePwrOn), 0, 1, DEF_FAKE_PWR);
        #endif

        #ifdef EXTERNAL_TIMETRAVEL_IN
        wd |= CopyCheckValidNumParm(json["ettDelay"], settings.ettDelay, sizeof(settings.ettDelay), 0, ETT_MAX_DEL, DEF_ETT_DELAY);
        //wd |= CopyCheckValidNumParm(json["ettLong"], settings.ettLong, sizeof(settings.ettLong), 0, 1, DEF_ETT_LONG);
        #endif
        
        #ifdef EXTERNAL_TIMETRAVEL_OUT
        wd |= CopyCheckValidNumParm(json["useETTO"], settings.useETTO, sizeof(settings.useETTO), 0, 1, DEF_USE_ETTO);
        wd |= CopyCheckValidNumParm(json["noETTOLead"], settings.noETTOLead, sizeof(settings.noETTOLead), 0, 1, DEF_NO_ETTO_LEAD);
        #endif
        #ifdef TC_HAVEGPS
        wd |= CopyCheckValidNumParm(json["quickGPS"], settings.quickGPS, sizeof(settings.quickGPS), 0, 1, DEF_QUICK_GPS);
        #endif
        wd |= CopyCheckValidNumParm(json["playTTsnds"], settings.playTTsnds, sizeof(settings.playTTsnds), 0, 1, DEF_PLAY_TT_SND);

        #ifdef TC_HAVEMQTT
        wd |= CopyCheckValidNumParm(json["useMQTT"], settings.useMQTT, sizeof(settings.useMQTT), 0, 1, 0);
        if(json["mqttServer"]) {
            memset(settings.mqttServer, 0, sizeof(settings.mqttServer));
            strncpy(settings.mqttServer, json["mqttServer"], sizeof(settings.mqttServer) - 1);
        } else wd = true;
        if(json["mqttUser"]) {
            memset(settings.mqttUser, 0, sizeof(settings.mqttUser));
            strncpy(settings.mqttUser, json["mqttUser"], sizeof(settings.mqttUser) - 1);
        } else wd = true;
        if(json["mqttTopic"]) {
            memset(settings.mqttTopic, 0, sizeof(settings.mqttTopic));
            strncpy(settings.mqttTopic, json["mqttTopic"], sizeof(settings.mqttTopic) - 1);
        } else wd = true;
        #ifdef EXTERNAL_TIMETRAVEL_OUT
        wd |= CopyCheckValidNumParm(json["pubMQTT"], settings.pubMQTT, sizeof(settings.pubMQTT), 0, 1, 0);
        #endif
        #endif

        wd |= CopyCheckValidNumParm(json["shuffle"], settings.shuffle, sizeof(settings.shuffle), 0, 1, DEF_SHUFFLE);

        wd |= CopyCheckValidNumParm(json["CfgOnSD"], settings.CfgOnSD, sizeof(settings.CfgOnSD), 0, 1, DEF_CFG_ON_SD);
        //wd |= CopyCheckValidNumParm(json["sdFreq"], settings.sdFreq, sizeof(settings.sdFreq), 0, 1, DEF_SD_FREQ);

    } else {

        wd = true;

    }

    return wd;
}

void write_settings()
{
    const char *funcName = "write_settings";
    DynamicJsonDocument json(JSON_SIZE);
    //StaticJsonDocument<JSON_SIZE> json;

    if(!haveFS && !FlashROMode) {
        Serial.printf("%s: %s\n", funcName, fsNoAvail);
        return;
    }

    #ifdef TC_DBG
    Serial.printf("%s: Writing config file\n", funcName);
    #endif
    
    json["timeTrPers"] = (const char *)settings.timesPers;
    json["alarmRTC"] = (const char *)settings.alarmRTC;
    json["mode24"] = (const char *)settings.mode24;
    json["beep"] = (const char *)settings.beep;
    json["playIntro"] = (const char *)settings.playIntro;
    json["autoRotateTimes"] = (const char *)settings.autoRotateTimes;

    json["hostName"] = (const char *)settings.hostName;
    json["systemID"] = (const char *)settings.systemID;
    json["appw"] = (const char *)settings.appw;
    json["wifiConRetries"] = (const char *)settings.wifiConRetries;
    json["wifiConTimeout"] = (const char *)settings.wifiConTimeout;
    json["wifiOffDelay"] = (const char *)settings.wifiOffDelay;
    json["wifiAPOffDelay"] = (const char *)settings.wifiAPOffDelay;
    json["wifiPRetry"] = (const char *)settings.wifiPRetry;
    
    json["timeZone"] = (const char *)settings.timeZone;
    json["ntpServer"] = (const char *)settings.ntpServer;

    json["timeZoneDest"] = (const char *)settings.timeZoneDest;
    json["timeZoneDep"] = (const char *)settings.timeZoneDep;
    json["timeZoneNDest"] = (const char *)settings.timeZoneNDest;
    json["timeZoneNDep"] = (const char *)settings.timeZoneNDep;
    
    json["destTimeBright"] = (const char *)settings.destTimeBright;
    json["presTimeBright"] = (const char *)settings.presTimeBright;
    json["lastTimeBright"] = (const char *)settings.lastTimeBright;

    json["dtNmOff"] = (const char *)settings.dtNmOff;
    json["ptNmOff"] = (const char *)settings.ptNmOff;
    json["ltNmOff"] = (const char *)settings.ltNmOff;
    
    json["autoNMPreset"] = (const char *)settings.autoNMPreset;
    json["autoNMOn"] = (const char *)settings.autoNMOn;
    json["autoNMOff"] = (const char *)settings.autoNMOff;
    #ifdef TC_HAVELIGHT
    json["useLight"] = (const char *)settings.useLight;
    json["luxLimit"] = (const char *)settings.luxLimit;
    #endif

    #ifdef TC_HAVETEMP
    json["tempUnit"] = (const char *)settings.tempUnit;
    json["tempOffs"] = (const char *)settings.tempOffs;
    #endif

    #ifdef TC_HAVESPEEDO    
    json["speedoType"] = (const char *)settings.speedoType;
    json["speedoBright"] = (const char *)settings.speedoBright;
    json["speedoFact"] = (const char *)settings.speedoFact;
    #ifdef TC_HAVEGPS
    json["useGPSSpeed"] = (const char *)settings.useGPSSpeed;
    #endif
    #ifdef TC_HAVETEMP
    json["dispTemp"] = (const char *)settings.dispTemp;
    json["tempBright"] = (const char *)settings.tempBright;
    json["tempOffNM"] = (const char *)settings.tempOffNM;
    #endif
    #endif // HAVESPEEDO
    
    #ifdef FAKE_POWER_ON
    json["fakePwrOn"] = (const char *)settings.fakePwrOn;
    #endif

    #ifdef EXTERNAL_TIMETRAVEL_IN
    json["ettDelay"] = (const char *)settings.ettDelay;
    //json["ettLong"] = (const char *)settings.ettLong;
    #endif

    #ifdef EXTERNAL_TIMETRAVEL_OUT
    json["useETTO"] = (const char *)settings.useETTO;
    json["noETTOLead"] = (const char *)settings.noETTOLead;
    #endif
    #ifdef TC_HAVEGPS
    json["quickGPS"] = (const char *)settings.quickGPS;
    #endif
    json["playTTsnds"] = (const char *)settings.playTTsnds;

    #ifdef TC_HAVEMQTT
    json["useMQTT"] = (const char *)settings.useMQTT;
    json["mqttServer"] = (const char *)settings.mqttServer;
    json["mqttUser"] = (const char *)settings.mqttUser;
    json["mqttTopic"] = (const char *)settings.mqttTopic;
    #ifdef EXTERNAL_TIMETRAVEL_OUT
    json["pubMQTT"] = (const char *)settings.pubMQTT;
    #endif
    #endif

    json["shuffle"] = (const char *)settings.shuffle;

    json["CfgOnSD"] = (const char *)settings.CfgOnSD;
    //json["sdFreq"] = (const char *)settings.sdFreq;

    writeJSONCfgFile(json, cfgName, FlashROMode, funcName);
}

bool checkConfigExists()
{
    return FlashROMode ? SD.exists(cfgName) : (haveFS && SPIFFS.exists(cfgName));
}

/*
 *  Helpers for parm copying & checking
 */

static bool CopyCheckValidNumParm(const char *json, char *text, uint8_t psize, int lowerLim, int upperLim, int setDefault)
{
    if(!json) return true;

    memset(text, 0, psize);
    strncpy(text, json, psize-1);
    return checkValidNumParm(text, lowerLim, upperLim, setDefault);
}

static bool CopyCheckValidNumParmF(const char *json, char *text, uint8_t psize, float lowerLim, float upperLim, float setDefault)
{
    if(!json) return true;

    memset(text, 0, psize);
    strncpy(text, json, psize-1);
    return checkValidNumParmF(text, lowerLim, upperLim, setDefault);
}

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

    // Re-do to get rid of formatting errors (eg "000")
    sprintf(text, "%d", i);

    return false;
}

static bool checkValidNumParmF(char *text, float lowerLim, float upperLim, float setDefault)
{
    int i, len = strlen(text);
    float f;

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

    f = atof(text);

    if(f < lowerLim) {
        sprintf(text, "%1.1f", lowerLim);
        return true;
    }
    if(f > upperLim) {
        sprintf(text, "%1.1f", upperLim);
        return true;
    }

    // Re-do to get rid of formatting errors (eg "0.")
    sprintf(text, "%1.1f", f);

    return false;
}

static bool openCfgFileRead(const char *fn, File& f)
{
    bool haveConfigFile = false;
    
    if(configOnSD) {
        if(SD.exists(fn)) {
            haveConfigFile = (f = SD.open(fn, "r"));
        }
    }

    if(!haveConfigFile && haveFS) {
        if(SPIFFS.exists(fn)) {
            haveConfigFile = (f = SPIFFS.open(fn, "r"));
        }
    }

    return haveConfigFile;
}

/*
 *  Load/save the Alarm settings
 */

bool loadAlarm()
{
    const char *funcName = "loadAlarm";
    bool writedefault = true;
    bool haveConfigFile = false;
    File configFile;

    if(!haveFS && !configOnSD) {
        Serial.printf("%s: %s\n", funcName, fsNoAvail);
        return false;
    }

    #ifdef TC_DBG
    Serial.printf("%s: Loading from %s\n", funcName, configOnSD ? "SD" : "flash FS");
    #endif

    if(openCfgFileRead(almCfgName, configFile)) {

        StaticJsonDocument<512> json;
        
        if(!readJSONCfgFile(json, configFile, funcName)) {
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
        #ifdef TC_DBG
        Serial.printf("%s: %s\n", funcName, badConfig);
        #endif
        alarmHour = alarmMinute = 255;
        alarmOnOff = false;
        alarmWeekday = 0;
        saveAlarm();
    }

    return true;
}

void saveAlarm()
{
    const char *funcName = "saveAlarm";
    char aooBuf[8];
    char hourBuf[8];
    char minBuf[8];
    StaticJsonDocument<512> json;

    if(!haveFS && !configOnSD) {
        Serial.printf("%s: %s\n", funcName, fsNoAvail);
        return;
    }

    sprintf(aooBuf, "%d", (alarmWeekday * 16) + (alarmOnOff ? 1 : 0));
    json["alarmonoff"] = (const char *)aooBuf;

    sprintf(hourBuf, "%d", alarmHour);
    sprintf(minBuf, "%d", alarmMinute);

    json["alarmhour"] = (const char *)hourBuf;
    json["alarmmin"] = (const char *)minBuf;

    writeJSONCfgFile(json, almCfgName, configOnSD, funcName);
}

/*
 *  Load/save the Yearly/Monthly Reminder settings
 */

bool loadReminder()
{
    const char *funcName = "loadReminder";
    bool writedefault = false;
    File configFile;

    if(!haveFS && !configOnSD) {
        Serial.printf("%s: %s\n", funcName, fsNoAvail);
        return false;
    }

    #ifdef TC_DBG
    Serial.printf("%s: Loading from %s\n", funcName, configOnSD ? "SD" : "flash FS");
    #endif

    if(openCfgFileRead(remCfgName, configFile)) {

        StaticJsonDocument<512> json;
        
        if(!readJSONCfgFile(json, configFile, funcName)) {
            if(json["month"] && json["hour"] && json["min"]) {
                remMonth  = atoi(json["month"]);
                remDay  = atoi(json["day"]);
                remHour = atoi(json["hour"]);
                remMin  = atoi(json["min"]);
                if(remMonth > 12 ||               // month can be zero (=monthly reminder)
                   remDay   > 31 || remDay < 1 ||
                   remHour  > 23 || 
                   remMin   > 59)
                    writedefault = true;
            }
        } 
        configFile.close();

        if(writedefault) {
            #ifdef TC_DBG
            Serial.printf("%s: %s\n", funcName, badConfig);
            #endif
            remMonth = remDay = remHour = remMin = 0;
            deleteReminder();
        }
        
    }

    return true;
}

void saveReminder()
{
    const char *funcName = "saveReminder";
    char monBuf[8];
    char dayBuf[8];
    char hourBuf[8];
    char minBuf[8];
    StaticJsonDocument<512> json;

    if(!haveFS && !configOnSD) {
        Serial.printf("%s: %s\n", funcName, fsNoAvail);
        return;
    }

    if(!remMonth && !remDay) {
        deleteReminder();
        return;
    }

    sprintf(monBuf, "%d", remMonth);
    sprintf(dayBuf, "%d", remDay);
    sprintf(hourBuf, "%d", remHour);
    sprintf(minBuf, "%d", remMin);
    
    json["month"] = (const char *)monBuf;
    json["day"] = (const char *)dayBuf;
    json["hour"] = (const char *)hourBuf;
    json["min"] = (const char *)minBuf;

    writeJSONCfgFile(json, remCfgName, configOnSD, funcName);
}

static void deleteReminder()
{
    if(configOnSD) {
        SD.remove(remCfgName);
    } else if(haveFS) {
        SPIFFS.remove(remCfgName);
    }
}

/*
 *  Load/save carMode
 */

static void loadCarMode()
{
    bool haveConfigFile = false;
    File configFile;

    if(!haveFS && !configOnSD)
        return;

    if(openCfgFileRead(cmCfgName, configFile)) {
        StaticJsonDocument<512> json;
        if(!readJSONCfgFile(json, configFile, "loadCarMode")) {
            if(json["CarMode"]) {
                carMode = (atoi(json["CarMode"]) > 0);
            }
        } 
        configFile.close();
    }
}

void saveCarMode()
{
    char buf[2];
    StaticJsonDocument<512> json;

    if(!haveFS && !configOnSD)
        return;

    buf[0] = carMode ? '1' : '0';
    buf[1] = 0;
    json["CarMode"] = (const char *)buf;

    writeJSONCfgFile(json, cmCfgName, configOnSD, "saveCarMode");
}

/*
 *  Load/save the Volume
 */

bool loadCurVolume()
{
    const char *funcName = "loadCurVolume";
    char temp[6];
    bool writedefault = true;
    bool haveConfigFile = false;
    File configFile;

    curVolume = DEFAULT_VOLUME;

    if(!haveFS && !configOnSD) {
        Serial.printf("%s: %s\n", funcName, fsNoAvail);
        return false;
    }

    #ifdef TC_DBG
    Serial.printf("%s: Loading from %s\n", funcName, configOnSD ? "SD" : "flash FS");
    #endif

    if(openCfgFileRead(volCfgName, configFile)) {
        StaticJsonDocument<512> json;
        if(!readJSONCfgFile(json, configFile, funcName)) {
            if(!CopyCheckValidNumParm(json["volume"], temp, sizeof(temp), 0, 255, 255)) {
                uint8_t ncv = atoi(temp);
                if((ncv >= 0 && ncv <= 19) || ncv == 255) {
                    curVolume = ncv;
                    writedefault = false;
                } 
            }
        } 
        configFile.close();
    }

    if(writedefault) {
        #ifdef TC_DBG
        Serial.printf("%s: %s\n", funcName, badConfig);
        #endif
        saveCurVolume();
    }

    return true;
}

void saveCurVolume()
{
    const char *funcName = "saveCurVolume";
    char buf[6];
    StaticJsonDocument<512> json;

    if(!haveFS && !configOnSD) {
        Serial.printf("%s: %s\n", funcName, fsNoAvail);
        return;
    }

    sprintf(buf, "%d", curVolume);
    json["volume"] = (const char *)buf;

    writeJSONCfgFile(json, volCfgName, configOnSD, funcName);
}

/*
 * Save/load stale present time
 */

#ifdef HAVE_STALE_PRESENT
void loadStaleTime(void *target, bool& currentOn)
{
    bool haveConfigFile = false;
    uint8_t loadBuf[1+(2*sizeof(dateStruct))+1];
    uint16_t sum = 0;
    
    if(!haveFS && !configOnSD)
        return;

    if(configOnSD) {
        haveConfigFile = readFileFromSD(stCfgName, loadBuf, sizeof(loadBuf));
    }
    if(!haveConfigFile && haveFS) {
        haveConfigFile = readFileFromFS(stCfgName, loadBuf, sizeof(loadBuf));
    }

    if(!haveConfigFile) return;

    for(uint8_t i = 0; i < 1+(2*sizeof(dateStruct)); i++) {
        sum += (loadBuf[i] ^ 0x55);
    }
    if(((sum & 0xff) == loadBuf[1+(2*sizeof(dateStruct))])) {
        currentOn = loadBuf[0] ? true : false;
        memcpy(target, (void *)&loadBuf[1], 2*sizeof(dateStruct));
    }
}

void saveStaleTime(void *source, bool currentOn)
{
    uint8_t savBuf[1+(2*sizeof(dateStruct))+1];
    uint16_t sum = 0;

    savBuf[0] = currentOn;
    memcpy((void *)&savBuf[1], source, 2*sizeof(dateStruct));
    for(uint8_t i = 0; i < 1+(2*sizeof(dateStruct)); i++) {
        sum += (savBuf[i] ^ 0x55);
    }
    savBuf[1+(2*sizeof(dateStruct))] = sum & 0xff;
    
    if(configOnSD) {
        writeFileToSD(stCfgName, savBuf, sizeof(savBuf));
    } else if(haveFS) {
        writeFileToFS(stCfgName, savBuf, sizeof(savBuf));
    }
}
#endif

/* Copy alarm/volume settings from/to SD if user
 * changed "save to SD"-option in CP
 */

void copySettings()
{   
    if(!haveSD || !haveFS) 
        return;

    configOnSD = !configOnSD;

    if(configOnSD || !FlashROMode) {
        #ifdef TC_DBG
        Serial.println(F("copySettings: Copying secondary settings to other medium"));
        #endif
        saveCurVolume();
        saveAlarm();
        saveReminder();
        saveCarMode();
    }

    configOnSD = !configOnSD;
}

/*
 * Load/save Music Folder Number
 */

bool loadMusFoldNum()
{
    bool writedefault = true;
    char temp[4];

    if(!haveSD)
        return false;

    if(SD.exists(musCfgName)) {

        File configFile = SD.open(musCfgName, "r");
        if(configFile) {
            StaticJsonDocument<512> json;
            if(!readJSONCfgFile(json, configFile, "loadMusFoldNum")) {
                if(!CopyCheckValidNumParm(json["folder"], temp, sizeof(temp), 0, 9, 0)) {
                    musFolderNum = atoi(temp);
                    writedefault = false;
                }
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
    const char *funcName = "saveMusFoldNum";
    StaticJsonDocument<512> json;
    char buf[4];

    if(!haveSD)
        return;

    sprintf(buf, "%1d", musFolderNum);
    json["folder"] = (const char *)buf;

    writeJSONCfgFile(json, musCfgName, true, funcName);
}

/*
 * Load/save/delete settings for static IP configuration
 */

bool loadIpSettings()
{
    bool invalid = false;
    bool haveConfig = false;

    if(!haveFS && !FlashROMode)
        return false;

    if( (!FlashROMode && SPIFFS.exists(ipCfgName)) ||
        (FlashROMode && SD.exists(ipCfgName)) ) {

        File configFile = FlashROMode ? SD.open(ipCfgName, "r") : SPIFFS.open(ipCfgName, "r");

        if(configFile) {

            StaticJsonDocument<512> json;
            DeserializationError error = readJSONCfgFile(json, configFile, "loadIpSettings");

            if(!error) {

                invalid |= CopyIPParm(json["IpAddress"], ipsettings.ip, sizeof(ipsettings.ip));
                invalid |= CopyIPParm(json["Gateway"], ipsettings.gateway, sizeof(ipsettings.gateway));
                invalid |= CopyIPParm(json["Netmask"], ipsettings.netmask, sizeof(ipsettings.netmask));
                invalid |= CopyIPParm(json["DNS"], ipsettings.dns, sizeof(ipsettings.dns));
                
                haveConfig = !invalid;

            } else {

                invalid = true;

            }

            configFile.close();

        }

    }

    if(invalid) {

        // config file is invalid - delete it

        #ifdef TC_DBG
        Serial.println(F("loadIpSettings: IP settings invalid; deleting file"));
        #endif
        
        deleteIpSettings();

        memset(ipsettings.ip, 0, sizeof(ipsettings.ip));
        memset(ipsettings.gateway, 0, sizeof(ipsettings.gateway));
        memset(ipsettings.netmask, 0, sizeof(ipsettings.netmask));
        memset(ipsettings.dns, 0, sizeof(ipsettings.dns));

    }

    return haveConfig;
}

static bool CopyIPParm(const char *json, char *text, uint8_t psize)
{
    if(!json) return true;

    if(strlen(json) == 0)
        return true;

    memset(text, 0, psize);
    strncpy(text, json, psize-1);
    return false;
}

void writeIpSettings()
{
    StaticJsonDocument<512> json;

    if(!haveFS && !FlashROMode)
        return;

    if(strlen(ipsettings.ip) == 0)
        return;
    
    json["IpAddress"] = (const char *)ipsettings.ip;
    json["Gateway"]   = (const char *)ipsettings.gateway;
    json["Netmask"]   = (const char *)ipsettings.netmask;
    json["DNS"]       = (const char *)ipsettings.dns;

    writeJSONCfgFile(json, ipCfgName, FlashROMode, "writeIpSettings");
}

void deleteIpSettings()
{
    #ifdef TC_DBG
    Serial.println(F("deleteIpSettings: Deleting ip config"));
    #endif

    if(FlashROMode) {
        SD.remove(ipCfgName);
    } else if(haveFS) {
        SPIFFS.remove(ipCfgName);
    }
}

/*
 * Audio file installer
 *
 * Copies our default audio files from SD to flash FS.
 * The is restricted to the original default audio
 * files that came with the software. If you want to
 * customize your sounds, put them on a FAT32 formatted
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
      4178, 4178, 4178, 4178, 4178, 4178, 3760, 3760, 4596, 3760, // DTMF
      65230, 71500, 60633, 10478,           // alarm, alarmoff, alarmon, baddate
      15184, 22983, 33364, 51701, 88190,    // ee1, ee2, ee3, ee4, ee5
      SND_ENTER_LEN, 125804, 33853, 47228,  // enter, intro, nmoff, nmon
      16747, 151719, 3790, SND_STARTUP_LEN, // ping, reminder, shutdown, startup, 
      84894, 38899, 135447, 113713          // timer, timetravel, travelstart, travelstart2
    };

    if(!haveSD)
        return false;

    // If identifier missing, quit now
    if(!(SD.exists(IDFN))) {
        #ifdef TC_DBG
        Serial.println("SD: ID file not present");
        #endif
        return false;
    }

    for(i = 0; i < 10; i++) {
        dtmf_buf[6] = i + '0';
        if(!(SD.exists(dtmf_buf))) {
            #ifdef TC_DBG
            Serial.printf("missing: %s\n", dtmf_buf);
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
            Serial.printf("missing: %s\n", audioFiles[i]);
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

    return true;
}

bool copy_audio_files()
{
    char dtmf_buf[16] = "/Dtmf-0.mp3\0";
    int i, haveErr = 0;

    if(!allowCPA) {
        return false;
    }

    start_file_copy();

    /* Copy DTMF files */
    for(i = 0; i < 10; i++) {
        dtmf_buf[6] = i + '0';
        open_and_copy(dtmf_buf, haveErr);
        if(haveErr) break;
    }

    if(!haveErr) {
        for(i = 0; i < NUM_AUDIOFILES; i++) {
            open_and_copy(audioFiles[i], haveErr);
            if(haveErr) break;
        }
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
    const char *funcName = "copy_audio_files";
    File sFile, dFile;

    if((sFile = SD.open(fn, FILE_READ))) {
        #ifdef TC_DBG
        Serial.printf("%s: Opened source file: %s\n", funcName, fn);
        #endif
        if((dFile = SPIFFS.open(fn, FILE_WRITE))) {
            #ifdef TC_DBG
            Serial.printf("%s: Opened destination file: %s\n", funcName, fn);
            #endif
            if(!filecopy(sFile, dFile)) {
                haveErr++;
            }
            dFile.close();
        } else {
            Serial.printf("%s: Error opening destination file: %s\n", funcName, fn);
            haveErr++;
        }
        sFile.close();
    } else {
        Serial.printf("%s: %s not found\n", funcName, fn);
        haveErr++;
    }
}

static bool filecopy(File source, File dest)
{
    uint8_t buffer[1024];
    size_t bytesr, bytesw;

    while((bytesr = source.read(buffer, 1024))) {
        if((bytesw = dest.write(buffer, bytesr)) != bytesr) {
            Serial.println(F("filecopy: Write error"));
            return false;
        }
        file_copy_progress();
    }

    return true;
}

bool audio_files_present()
{
    File file;
    size_t ts;
    
    if(FlashROMode || !haveFS)
        return true;

    if(!SPIFFS.exists(audioFiles[SND_ENTER_IDX]))
        return false;
      
    if(!(file = SPIFFS.open(audioFiles[SND_ENTER_IDX])))
        return false;
      
    ts = file.size();
    file.close();

    if(ts != SND_ENTER_LEN)
        return false;

    return true;
}

void delete_ID_file()
{
    if(!haveSD)
        return;

    #ifdef TC_DBG
    Serial.printf("Deleting ID file %s\n", IDFN);
    #endif
        
    if(SD.exists(IDFN)) {
        SD.remove(IDFN);
    }
}

/*
 * Various helpers
 */

void formatFlashFS()
{
    #ifdef TC_DBG
    Serial.println(F("Formatting flash FS"));
    #endif
    SPIFFS.format();
}

// Re-write IP/alarm/reminder/vol settings
// Used during audio file installation when flash FS needs
// to be re-formatted.
// Is never called in FlashROmode
void rewriteSecondarySettings()
{
    bool oldconfigOnSD = configOnSD;
    
    #ifdef TC_DBG
    Serial.println("Re-writing secondary settings");
    #endif
    
    writeIpSettings();

    // Create current secondary settings on flash FS
    // regardless of SD-option
    configOnSD = false;

    saveCurVolume();
    saveAlarm();
    saveReminder();
    saveCarMode();
    
    configOnSD = oldconfigOnSD;

    // Music Folder Number is always on SD only
}

static DeserializationError readJSONCfgFile(JsonDocument& json, File& configFile, const char *funcName)
{
    char *buf = NULL;
    size_t bufSize = configFile.size();
    DeserializationError ret;

    if(!(buf = (char *)malloc(bufSize + 1))) {
        Serial.printf("%s: Buffer allocation failed (%d)\n", funcName, bufSize);
        return DeserializationError::NoMemory;
    }

    memset(buf, 0, bufSize + 1);

    configFile.read((uint8_t *)buf, bufSize);

    #ifdef TC_DBG
    Serial.println((const char *)buf);
    #endif
    
    ret = deserializeJson(json, buf);

    free(buf);

    return ret;
}

static bool writeJSONCfgFile(const JsonDocument& json, const char *fn, bool useSD, const char *funcName)
{
    char *buf;
    size_t bufSize = measureJson(json);
    bool success = false;

    if(!(buf = (char *)malloc(bufSize + 1))) {
        Serial.printf("%s: Buffer allocation failed (%d)\n", funcName, bufSize);
        return false;
    }

    memset(buf, 0, bufSize + 1);
    serializeJson(json, buf, bufSize);

    #ifdef TC_DBG
    Serial.printf("Writing %s to %s, %d bytes\n", fn, useSD ? "SD" : "FS", bufSize);
    Serial.println((const char *)buf);
    #endif

    if(useSD) {
        success = writeFileToSD(fn, (uint8_t *)buf, (int)bufSize);
    } else {
        success = writeFileToFS(fn, (uint8_t *)buf, (int)bufSize);
    }

    free(buf);

    if(!success) {
        Serial.printf("%s: %s\n", funcName, failFileWrite);
    }

    return success;
}

bool readFileFromSD(const char *fn, uint8_t *buf, int len)
{
    size_t bytesr;
    
    if(!haveSD)
        return false;

    File myFile = SD.open(fn, FILE_READ);
    if(myFile) {
        bytesr = myFile.read(buf, len);
        myFile.close();
        return (bytesr == len);
    } else
        return false;
}

bool writeFileToSD(const char *fn, uint8_t *buf, int len)
{
    size_t bytesw;
    
    if(!haveSD)
        return false;

    File myFile = SD.open(fn, FILE_WRITE);
    if(myFile) {
        bytesw = myFile.write(buf, len);
        myFile.close();
        return (bytesw == len);
    } else
        return false;
}

bool readFileFromFS(const char *fn, uint8_t *buf, int len)
{
    size_t bytesr;
    
    if(!haveFS)
        return false;

    if(!SPIFFS.exists(fn))
        return false;

    File myFile = SPIFFS.open(fn, FILE_READ);
    if(myFile) {
        bytesr = myFile.read(buf, len);
        myFile.close();
        return (bytesr == len);
    } else
        return false;
}

bool writeFileToFS(const char *fn, uint8_t *buf, int len)
{
    size_t bytesw;
    
    if(!haveFS)
        return false;

    File myFile = SPIFFS.open(fn, FILE_WRITE);
    if(myFile) {
        bytesw = myFile.write(buf, len);
        myFile.close();
        return (bytesw == len);
    } else
        return false;
}
