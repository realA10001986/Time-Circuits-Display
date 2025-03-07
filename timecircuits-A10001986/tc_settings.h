/*
 * -------------------------------------------------------------------
 * CircuitSetup.us Time Circuits Display
 * (C) 2021-2022 John deGlavina https://circuitsetup.us
 * (C) 2022-2025 Thomas Winischhofer (A10001986)
 * https://github.com/realA10001986/Time-Circuits-Display
 * https://tcd.out-a-ti.me
 *
 * Settings & file handling
 *
 * -------------------------------------------------------------------
 * License: MIT NON-AI
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
 * In addition, the following restrictions apply:
 * 
 * 1. The Software and any modifications made to it may not be used 
 * for the purpose of training or improving machine learning algorithms, 
 * including but not limited to artificial intelligence, natural 
 * language processing, or data mining. This condition applies to any 
 * derivatives, modifications, or updates based on the Software code. 
 * Any usage of the Software in an AI-training dataset is considered a 
 * breach of this License.
 *
 * 2. The Software may not be included in any dataset used for 
 * training or improving machine learning algorithms, including but 
 * not limited to artificial intelligence, natural language processing, 
 * or data mining.
 *
 * 3. Any person or organization found to be in violation of these 
 * restrictions will be subject to legal action and may be held liable 
 * for any damages resulting from such use.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY 
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _TC_SETTINGS_H
#define _TC_SETTINGS_H

extern bool haveFS;
extern bool haveSD;
extern bool FlashROMode;

extern bool haveAudioFiles;

extern uint8_t musFolderNum;

#define MS(s) XMS(s)
#define XMS(s) #s

// Default settings
#define DEF_PLAY_INTRO      1     // 0: Skip intro; 1: Play intro
#define DEF_MODE24          0     // 0: 12 hour clock; 1: 24 hour clock
#define DEF_BEEP            0     // 0: annoying beep(tm) off, 1: on, 2: on (30 secs), 3: on (60 secs)
#define DEF_AUTOROTTIMES    1     // Time cycling interval. 0: off, 1: 5min, 2: 10min, 3: 15min, 4: 30min, 5: 60min
#define DEF_ALARM_RTC       1     // 0: Alarm is presentTime-based; 1: Alarm is RTC-based
#define DEF_HOSTNAME        "timecircuits"
#define DEF_WIFI_RETRY      3     // 1-10; Default: 3 retries
#define DEF_WIFI_TIMEOUT    7     // 7-25; Default: 7 seconds
#define DEF_WIFI_OFFDELAY   0     // 0/10-99; Default 0 = Never power down WiFi in STA-mode
#define DEF_WIFI_APOFFDELAY 0     // 0/10-99; Default 0 = Never power down WiFi in AP-mode
#define DEF_WIFI_PRETRY     1     // 1: Nightly, periodic WiFi reconnection attempts for time sync; 0: off
#define DEF_NTP_SERVER      "pool.ntp.org"
#define DEF_TIMEZONE        ""    // Default: UTC; Posix format
#define DEF_BRIGHT_DEST     10    // 1-15; Default: medium brightness
#define DEF_BRIGHT_PRES     10
#define DEF_BRIGHT_DEPA     10
#define DEF_AUTONM_PRESET   10    // Default: AutoNM disabled
#define DEF_AUTONM_ON       0     // Default: Both 0
#define DEF_AUTONM_OFF      0
#define DEF_DT_OFF          1     // 1: Dest. time off in night mode; 0: dimmed
#define DEF_PT_OFF          0     // 1: Present time off in night mode; 0: dimmed
#define DEF_LT_OFF          1     // 1: Last dep. time off in night mode; 0: dimmed
#define DEF_FAKE_PWR        0     // 0: Do not use external fake "power" switch, 1: Do
#define DEF_ETT_DELAY       0     // in ms; Default 0: ETT immediately
#define DEF_ETT_LONG        1     // [removed] 0: Ext. TT short (reentry), 1: long
#define DEF_SPEEDO_TYPE     99    // Default display type: None
#define DEF_SPEEDO_ACCELFIG 0     // Accel figures: 0: Movie (approximated), 1: Real-life
#define DEF_SPEEDO_FACT     2.0   // Real-life acceleration factor (1.0 actual DeLorean figures; >1.0 faster, <1.0 slower)
#define DEF_BRIGHT_SPEEDO   15    // Default: Max. brightness for speed
#define DEF_USE_GPS_SPEED   0     // 0: Do not show GPS speed on speedo display; 1: Do
#define DEF_SPD_UPD_RATE    1     // 0: 1Hz, 1: 2Hz (default), 2: 4Hz, 3: 5Hz
#define DEF_DISP_TEMP       1     // 1: Display temperature (if available) on speedo; 0: do not (speedo off)
#define DEF_TEMP_BRIGHT     3     // Default temperature brightness
#define DEF_TEMP_OFF_NM     1     // Default: 1: temperature off in night mode; 0: dimmed
#define DEF_TEMP_UNIT       0     // Default: 0: temperature unit Fahrenheit; 1: Celsius
#define DEF_TEMP_OFFS       0.0   // Default: temperature offset 0.0
#define DEF_USE_LIGHT       0     // Default: No i2c light sensor
#define DEF_LUX_LIMIT       3     // Default Lux threshold for night mode
#define DEF_USE_ETTO        0     // Default: 0: No external props (wired); 1: Wired props present
#define DEF_NO_ETTO_LEAD    0     // Default: 0: ETTO with ETTO_LEAD lead time; 1: without
#define DEF_QUICK_GPS       0     // Default: 0: Do not provide GPS speed for BTTF props; 1: Do
#define DEF_PLAY_TT_SND     1     // Default: 1: Play time travel sounds (0: Do not; for use with external equipment)
#define DEF_USE_LINEOUT     0     // Default: 0: Play all sound through built-in speaker; 1: Use Line out for music & time travel sound 
#define DEF_SHUFFLE         0     // Music Player: 0: Do not shuffle by default, 1: Do
#define DEF_CFG_ON_SD       1     // Default: 1: Save secondary settings on SD, 0: Do not (use internal Flash)
#define DEF_TIMES_PERS      0     // Default: 0: Timetravels not persistent; 1: TT persistent
#define DEF_SD_FREQ         0     // SD/SPI frequency: Default 16MHz

struct Settings {
    char playIntro[4]       = MS(DEF_PLAY_INTRO);
    char mode24[4]          = MS(DEF_MODE24);
    char beep[4]            = MS(DEF_BEEP);
    char autoRotateTimes[4] = MS(DEF_AUTOROTTIMES);
    char alarmRTC[4]        = MS(DEF_ALARM_RTC);
    char hostName[32]       = DEF_HOSTNAME;
    char systemID[8]        = "";
    char appw[10]           = "";
    char wifiConRetries[4]  = MS(DEF_WIFI_RETRY);
    char wifiConTimeout[4]  = MS(DEF_WIFI_TIMEOUT);
    char wifiOffDelay[4]    = MS(DEF_WIFI_OFFDELAY);
    char wifiAPOffDelay[4]  = MS(DEF_WIFI_APOFFDELAY);
    char wifiPRetry[4]      = MS(DEF_WIFI_PRETRY);
    char ntpServer[64]      = DEF_NTP_SERVER;
    char timeZone[64]       = DEF_TIMEZONE;
    char timeZoneDest[64]   = "";
    char timeZoneDep[64]    = "";
    char timeZoneNDest[16]  = "";
    char timeZoneNDep[16]   = "";
    char destTimeBright[4]  = MS(DEF_BRIGHT_DEST);
    char presTimeBright[4]  = MS(DEF_BRIGHT_PRES);
    char lastTimeBright[4]  = MS(DEF_BRIGHT_DEPA);
    char autoNMPreset[4]    = MS(DEF_AUTONM_PRESET);
    char autoNMOn[4]        = MS(DEF_AUTONM_ON);
    char autoNMOff[4]       = MS(DEF_AUTONM_OFF);
    char dtNmOff[4]         = MS(DEF_DT_OFF);
    char ptNmOff[4]         = MS(DEF_PT_OFF);
    char ltNmOff[4]         = MS(DEF_LT_OFF);
#ifdef FAKE_POWER_ON
    char fakePwrOn[4]       = MS(DEF_FAKE_PWR);
#endif
#ifdef EXTERNAL_TIMETRAVEL_IN
    char ettDelay[8]        = MS(DEF_ETT_DELAY);
    char ettLong[4]         = MS(DEF_ETT_LONG);
#endif
#ifdef TC_HAVETEMP
    char tempUnit[4]        = MS(DEF_TEMP_UNIT);
    char tempOffs[6]        = MS(DEF_TEMP_OFFS);
#endif
#ifdef TC_HAVELIGHT
    char useLight[4]        = MS(DEF_USE_LIGHT);
    char luxLimit[8]        = MS(DEF_LUX_LIMIT);
#endif
#ifdef TC_HAVESPEEDO
    char speedoType[4]      = MS(DEF_SPEEDO_TYPE);
    char speedoBright[4]    = MS(DEF_BRIGHT_SPEEDO);
    char speedoAF[4]        = MS(DEF_SPEEDO_ACCELFIG);
    char speedoFact[6]      = MS(DEF_SPEEDO_FACT);
#ifdef TC_HAVEGPS
    char useGPSSpeed[4]     = MS(DEF_USE_GPS_SPEED);
    char spdUpdRate[4]      = MS(DEF_SPD_UPD_RATE);
#endif
#ifdef TC_HAVETEMP
    char dispTemp[4]        = MS(DEF_DISP_TEMP);
    char tempBright[4]      = MS(DEF_TEMP_BRIGHT);
    char tempOffNM[4]       = MS(DEF_TEMP_OFF_NM);
#endif
#endif // HAVESPEEDO 
#ifdef EXTERNAL_TIMETRAVEL_OUT
    char useETTO[4]         = MS(DEF_USE_ETTO);
    char noETTOLead[4]      = MS(DEF_NO_ETTO_LEAD);
#endif
#ifdef TC_HAVEGPS
    char quickGPS[4]        = MS(DEF_QUICK_GPS);
#endif
    char playTTsnds[4]      = MS(DEF_PLAY_TT_SND);

    char shuffle[4]         = MS(DEF_SHUFFLE);
    char CfgOnSD[4]         = MS(DEF_CFG_ON_SD);
    char timesPers[4]       = MS(DEF_TIMES_PERS);
    char sdFreq[4]          = MS(DEF_SD_FREQ);
#ifdef TC_HAVEMQTT  
    char useMQTT[4]         = "0";
    char mqttServer[80]     = "";  // ip or domain [:port]  
    char mqttUser[128]      = "";  // user[:pass] (UTF8)
    char mqttTopic[512]     = "";  // topic (UTF8)
    char pubMQTT[4]         = "0"; // publish to broker (timetravel)
#endif    
};

// Maximum delay for incoming tt trigger
#define ETT_MAX_DEL 60000

struct IPSettings {
    char ip[20]       = "";
    char gateway[20]  = "";
    char netmask[20]  = "";
    char dns[20]      = "";
};

extern struct Settings settings;
extern struct IPSettings ipsettings;

void settings_setup();

void unmount_fs();

void write_settings();
bool checkConfigExists();

void saveBrightness(bool useCache = true);

void saveAutoInterval(bool useCache = true);

bool loadAlarm();
void saveAlarm();

bool loadReminder();
void saveReminder();

void saveCarMode();

bool loadCurVolume();
void saveCurVolume(bool useCache = true);

bool loadMusFoldNum();
void saveMusFoldNum();

#ifdef HAVE_STALE_PRESENT
void loadStaleTime(void *target, bool& currentOn);
void saveStaleTime(void *source, bool currentOn);
#endif

#ifdef TC_HAVELINEOUT
void loadLineOut();
void saveLineOut();
#endif

#ifdef TC_HAVE_REMOTE
void saveRemoteAllowed();
#endif

bool loadIpSettings();
void writeIpSettings();
void deleteIpSettings();

void copySettings();

bool check_if_default_audio_present();
void doCopyAudioFiles();
bool copy_audio_files(bool& delIDfile);

bool check_allow_CPA();
void delete_ID_file();

void waitForEnterRelease();

void formatFlashFS();

void rewriteSecondarySettings();

bool readFileFromSD(const char *fn, uint8_t *buf, int len);
bool writeFileToSD(const char *fn, uint8_t *buf, int len);
bool readFileFromFS(const char *fn, uint8_t *buf, int len);
bool writeFileToFS(const char *fn, uint8_t *buf, int len);

#include <FS.h>
bool   openACFile(File& file);
size_t writeACFile(File& file, uint8_t *buf, size_t len);
void   closeACFile(File& file);
void   removeACFile();

#endif
