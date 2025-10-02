/**
 * WiFiManager.h
 *
 * WiFiManager, a library for the ESP32/Arduino platform
 *
 * @author Creator tzapu
 * @author tablatronix
 * @version 2.0.15+A10001986
 * @license MIT
 *
 * Adapted by Thomas Winischhofer (A10001986)
 */


#ifndef WiFiManager_h
#define WiFiManager_h

#define WM_NODEBUG
#define _A10001986_NO_COUNTRY
//#define _A10001986_DBG

// #define WM_MDNS            // includes MDNS

#define WM_G(string_literal)  (String(FPSTR(string_literal)).c_str())

#include <WiFi.h>
#include <esp_wifi.h>
#include <Update.h>

#define WIFI_getChipId() (uint32_t)ESP.getEfuseMac()
#define WM_WIFIOPEN   WIFI_AUTH_OPEN

#ifndef WEBSERVER_H
#include <WebServer.h>
#endif

#ifdef WM_MDNS
#include <ESPmDNS.h>
#endif

#include <DNSServer.h>
#include <memory>

// prep string concat vars
#define WM_STRING2(x) #x
#define WM_STRING(x) WM_STRING2(x)

// menu ids
#define WM_MENU_WIFI        0
#define WM_MENU_WIFINOSCAN  1
#define WM_MENU_PARAM       2
#define WM_MENU_ERASE       3
#define WM_MENU_UPDATE      4
#define WM_MENU_SEP         5
#define WM_MENU_CUSTOM      6
#define WM_MENU_END        -1
#define WM_MENU_MAX         WM_MENU_CUSTOM

// params will autoincrement and realloc by this amount when max is reached
// can (and should) be overruled by allocParms()/allocWiFiParms()
#ifndef WIFI_MANAGER_MAX_PARAMS
#define WIFI_MANAGER_MAX_PARAMS 5
#endif

// Label placement for parameters
#define WFM_NO_LABEL      0
#define WFM_LABEL_BEFORE  1
#define WFM_LABEL_AFTER   2
#define WFM_LABEL_DEFAULT WFM_LABEL_BEFORE

// HTML id:s of "static IP" parameters on "WiFi Configuration" page
#define WMS_ip    "ip"
#define WMS_gw    "gw"
#define WMS_sn    "sn"
#define WMS_dns   "dns"

// Parm handed to GPCallback
#define WM_LP_NONE          0   // No special reason (just do over-due stuff)
#define WM_LP_PREHTTPSEND   1   // pre-HTTPSend() (problem with mp3 in AP mode)
#define WM_LP_POSTHTTPSEND  2   // post-HTTPSend() (just do over-due stuff, ...)

#define WM_WIFI_SCAN_BUSY -133

#if defined(ESP_ARDUINO_VERSION) && defined(ESP_ARDUINO_VERSION_VAL)
    #if ESP_ARDUINO_VERSION < ESP_ARDUINO_VERSION_VAL(2,0,0)
        #define WM_NOCOUNTRY
    #endif

    // This is broken, #define *defines* something, always.
    // So #ifdef is true - always.
    // However, I have been running this since 2022 with this broken
    // check under ESP32/arduino 2.0.5 - 2.0.17 without issues, so leave
    // it in. Does not seem to cause harm.
    //#define WM_ARDUINOVERCHECK_204 ESP_ARDUINO_VERSION <= ESP_ARDUINO_VERSION_VAL(2, 0, 5)
    //#ifdef WM_ARDUINOVERCHECK_204
    // The way to do it would be:
    //#if ESP_ARDUINO_VERSION > ESP_ARDUINO_VERSION_VAL(2,0,4)
        #define WM_DISCONWORKAROUND
    //#endif
#else
    #define WM_NOCOUNTRY
#endif

// Set default debug level
#ifndef WM_DEBUG_LEVEL
#define WM_DEBUG_LEVEL DEBUG_NOTIFY
#endif

    // override debug level OFF
#ifdef WM_NODEBUG
#undef WM_DEBUG_LEVEL
#endif

class WiFiManagerParameter {
  public:
    /**
        Create custom parameters that can be added to the WiFiManager setup web page
        @id is used for HTTP queries and must not contain spaces nor other special characters
    */
    WiFiManagerParameter();
    WiFiManagerParameter(const char *custom);
    WiFiManagerParameter(const char *(*CustomHTMLGenerator)(const char *));
    WiFiManagerParameter(const char *id, const char *label);
    WiFiManagerParameter(const char *id, const char *label, const char *defaultValue, int length);
    WiFiManagerParameter(const char *id, const char *label, const char *defaultValue, int length, const char *custom);
    WiFiManagerParameter(const char *id, const char *label, const char *defaultValue, int length, const char *custom, int labelPlacement);
    ~WiFiManagerParameter();

    const char *getID() const;
    const char *getValue() const;
    const char *getLabel() const;
    int         getValueLength() const;
    int         getLabelPlacement() const;
    virtual const char *getCustomHTML() const;
    void        setValue(const char *defaultValue, int length);

  protected:
    void init(const char *id, const char *label, const char *defaultValue, int length, const char *custom, int labelPlacement);

  private:
    WiFiManagerParameter& operator=(const WiFiManagerParameter&);
    const char *_id;
    const char *_label;
    union {
        char       *_value;
        const char *(*_customHTMLGenerator)(const char *);
    };
    int         _length;
    int         _labelPlacement;
  protected:
    const char *_customHTML;
    friend class WiFiManager;
};


class WiFiManager
{
    /////////////////////////////////////////////////////////////////////////////
    //                                 Public                                  //
    /////////////////////////////////////////////////////////////////////////////

  public:
    WiFiManager(Print& consolePort);
    WiFiManager();
    ~WiFiManager();
    void WiFiManagerInit();

    // auto connect to saved wifi, or custom, and start config portal on failures
    bool          autoConnect();
    bool          autoConnect(char const *apName, char const *apPassword = NULL);

    // manually start the config portal (AP mode)
    bool          startConfigPortal(); // auto generates apname
    bool          startConfigPortal(char const *apName, char const *apPassword = NULL);

    // manually stop the config portal if started manually, stop immediatly
    bool          stopConfigPortal();

    // manually start the web portal (STA/connected) (=same as Config Portal in AP mode)
    void          startWebPortal();

    // manually stop the web portal if started manually (used for when connected)
    void          stopWebPortal();

    // Run webserver processing. Param: Do handle webserver, or skip
    bool          process(bool handleWeb = true);

    // get the AP name of the config portal, so it can be used in the callback
    String        getConfigPortalSSID();
    int           getRSSIasQuality(int RSSI);

    // reboot esp32
    void          reboot();

    // disconnect wifi, without persistent saving or erasing
    bool          disconnect();

    // erase esp (identical, opt has been removed)
    bool          erase();
    bool          erase(bool opt);

    // allocate numParms entries in params array (overrules WIFI_MANAGER_MAX_PARAMS)
    void          allocParms(int numParms);

    // adds a custom parameter, returns false on failure
    bool          addParameter(WiFiManagerParameter *p);

    // returns the list of Parameters
    WiFiManagerParameter** getParameters();

    // returns the Parameters Count
    int           getParametersCount();

    // same as above for WiFi Configuration page
    void          allocWiFiParms(int numParms);
    bool          addWiFiParameter(WiFiManagerParameter *p);
    WiFiManagerParameter** getWiFiParameters();
    int           getWiFiParametersCount();

    // SET CALLBACKS

    // called after AP mode and config portal has started
    void          setAPCallback(void(*func)(WiFiManager*));

    // called after wifi hw init, but before connection attempts
    void          setPreConnectCallback(void(*func)());

    // called after webserver has started
    void          setWebServerCallback(void(*func)());

    // called when wifi settings have been changed
    void          setSaveConfigCallback(void(*func)(bool, String&));

    // called when saving params-in-wifi or params before anything else happens (eg wifi)
    void          setPreSaveConfigCallback(void(*func)());

    // called when saving params before anything else happens
    void          setPreSaveParamsCallback(void(*func)());

    // called when saving either params-in-wifi or params page
    void          setSaveParamsCallback(void(*func)());

    // called just before/after doing OTA update
    void          setPreOtaUpdateCallback(void(*func)());
		void          setPostOtaUpdateCallback(void(*func)(bool));

    // called post-erase before reboot
    void          setEraseCallback(void(*func)(bool));

    // add stuff to the main menu; second one to give WM the length for buf sizing
  	void          setMenuOutCallback(void(*func)(String &page));
  	void          setMenuOutLenCallback(int(*func)());

  	//  app-specific replacement for delay()
  	void          setDelayReplacement(void(*func)(unsigned int));

  	// called to give app chance to update stuff when WM op might take a while
  	// WM_LP_xxx given as arg
  	void          setGPCallback(void(*func)(int));

  	// Pre-scan, allows app to forbid scan (use cache, or display no list)
  	void          setPreWiFiScanCallback(bool(*func)());

    //sets timeout for which to attempt connecting, useful if you get a lot of failed connects
    void          setConnectTimeout(unsigned long seconds);

    // sets number of retries for autoconnect, force retry after wait failure exit
    void          setConnectRetries(uint8_t numRetries); // default 1

    //sets timeout for which to attempt connecting on saves, useful if there are bugs in esp waitforconnectloop
    void          setSaveConnectTimeout(unsigned long seconds);

    // lets you disable automatically connecting after save from webportal
    void          setSaveConnect(bool connect = true);

    // toggle debug output
    void          setDebugOutput(bool debug);
    void          setDebugOutput(bool debug, String prefix); // log line prefix, default "*wm:"

    //set min quality percentage to include in scan, defaults to 8% if not specified
    void          setMinimumSignalQuality(int quality = 8);

    //sets a custom ip /gateway /subnet configuration
    void          setAPStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn);

    //sets config for a static IP
    void          setSTAStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn);

    //sets config for a static IP with DNS
    void          setSTAStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn, IPAddress dns);

    //add custom html at inside <head> for all pages
    void          setCustomHeadElement(const char* html);

    //if this is set, customise style
    void          setCustomMenuHTML(const char* html);

    //if this is true, remove duplicated Access Points - defaut true
    void          setRemoveDuplicateAPs(bool removeDuplicates);

    //setter for ESP wifi.persistent so we can remember it and restore user preference, as WIFi._persistent is protected
    void          setRestorePersistent(bool persistent);

    //if true, always show static net inputs, IP, subnet, gateway, else only show if set via setSTAStaticIPConfig
    void          setShowStaticFields(bool alwaysShow);

    //if true, always show static dns, esle only show if set via setSTAStaticIPConfig
    void          setShowDnsFields(bool alwaysShow);

    // if true (default) then start the config portal from autoConnect if connection failed
    void          setEnableConfigPortal(bool enable);

    // set a custom hostname, sets sta and ap dhcp client id for esp32, and sta for esp8266
    bool          setHostname(const char * hostname);

    // set ap channel
    void          setWiFiAPChannel(int32_t channel);

    // set ap hidden
    void          setWiFiAPHidden(bool hidden); // default false

    // clean connect, always disconnect before connecting
    void          setCleanConnect(bool enable); // default false

    // set custom menu items and order
    void          setMenu(const int8_t *menu, uint8_t size);

    // set the webapp title, default WiFiManager
    void          setTitle(String title);

    // show audio upload on Update page
    void          showUploadContainer(bool enable, const char *contName);

    // Custom
    void          setCarMode(bool enable);

    // get last connection result, includes autoconnect and wifisave
    uint8_t       getLastConxResult();

    // get a status as string
    #ifdef WM_DEBUG_LEVEL
    String        getWLStatusString(uint8_t status);
    String        getWLStatusString();
    #endif

    // get wifi mode as string
    String        getModeString(uint8_t mode);

    // check if the module has a saved ap to connect to
    bool          getWiFiIsSaved();

    // helper to get saved password, if persistent get stored, else get current if connected
    String        getWiFiPass(bool persistent = true);

    // helper to get saved ssid, if persistent get stored, else get current if connected
    String        getWiFiSSID(bool persistent = true);

    // gets number of retries for autoconnect, force retry after wait failure exit
    uint8_t       getConnectRetries();

    // make some HTML templates available for app
    const char *  getHTTPSTART(int& titleStart);
    const char *  getHTTPSCRIPT();
    const char *  getHTTPSTYLE();

    // debug output the softap config
    #ifndef WM_NODEBUG
    void          debugSoftAPConfig();
    #endif

    // helper for html
    String        htmlEntities(String str, bool whitespace = false);
	  int           htmlEntitiesLen(String& str, bool whitespace = false);

    // set the country code for wifi settings, CN
    #ifndef _A10001986_NO_COUNTRY
    void          setCountry(String cc);
    #endif

    // get default ap esp uses , esp_chipid etc
    String        getDefaultAPName();

    // set port of webserver, 80
    void          setHttpPort(uint16_t port);

    // check if config portal is active (true)
    bool          getConfigPortalActive();

    // check if web portal is active (true)
    bool          getWebPortalActive();

    // to preload autoconnect for test fixtures or other uses that skip esp sta config
    bool          preloadWiFi(String ssid, String pass);

    // get hostname helper
    String        getWiFiHostname();

    std::unique_ptr<DNSServer> dnsServer;

    std::unique_ptr<WebServer> server;

    /////////////////////////////////////////////////////////////////////////////
    //                                 Private                                 //
    /////////////////////////////////////////////////////////////////////////////

  private:
    // vars
    int8_t *      _menuIdArr = NULL;

    // ip configs
    IPAddress     _ap_static_ip;
    IPAddress     _ap_static_gw;
    IPAddress     _ap_static_sn;
    IPAddress     _sta_static_ip;
    IPAddress     _sta_static_gw;
    IPAddress     _sta_static_sn;
    IPAddress     _sta_static_dns;

    unsigned long _webPortalAccessed      = 0; // ms last web access time
    uint8_t       _lastconxresult         = WL_IDLE_STATUS; // store last result when doing connect operations
    int           _numNetworks            = 0; // init index for numnetworks wifiscans
    int16_t       _numNetworksAsync       = 0;
    unsigned long _lastscan               = 0; // ms for timing wifi scans
    unsigned long _startscan              = 0; // ms for timing wifi scans
    unsigned long _startconn              = 0; // ms for timing wifi connects

    // defaults
    const uint8_t DNS_PORT                = 53;
    String        _apName                 = "no-net";
    String        _apPassword             = "";
    String        _ssid                   = ""; // var temp ssid
    String        _pass                   = ""; // var temp psk
    String        _defaultssid            = ""; // preload ssid
    String        _defaultpass            = ""; // preload pass

    // options flags
    unsigned long _connectTimeout         = 0; // ms stop trying to connect to ap if set
    unsigned long _saveTimeout            = 0; // ms stop trying to connect to ap on saves, in case bugs in esp waitforconnectresult

    WiFiMode_t    _usermode               = WIFI_STA; // Default user mode
    bool          _cleanConnect           = false; // disconnect before connect in connectwifi, increases stability on connects
    bool          _connectonsave          = true;  // connect to wifi when saving creds
    bool          _disableSTA             = false; // disable sta when starting ap, always
    bool          _disableSTAConn         = true;  // disable sta when starting ap, if sta is not connected ( stability )
    int32_t       _apChannel              = 0;     // default channel to use for ap, 0 for auto
    bool          _apHidden               = false; // store softap hidden value
    uint16_t      _httpPort               = 80;    // port for webserver
    uint8_t       _connectRetries         = 1;     // number of sta connect retries, force reconnect, wait loop (connectimeout) does not always work and first disconnect bails
    bool          _aggresiveReconn        = true;  // use an aggressive reconnect strategy, WILL delay conxs
                                                   // on some conn failure modes will add delays and many retries to work around esp and ap bugs, ie, anti de-auth protections
                                                   // https://github.com/tzapu/WiFiManager/issues/1067

    wifi_event_id_t wm_event_id           = 0;
    static uint8_t _lastconxresulttmp;            // tmp var for esp32 callback

    // parameter options
    int           _minimumQuality         = -1;    // filter wifiscan ap by this rssi
    int8_t        _staShowStaticFields    = 0;     // ternary 1=always show static ip fields, 0=only if set, -1=never(cannot change ips via web!)
    int8_t        _staShowDns             = 0;     // ternary 1=always show dns, 0=only if set, -1=never(cannot change dns via web!)
    bool          _removeDuplicateAPs     = true;  // remove dup aps from wifiscan
    bool          _userpersistent         = true;  // users preferred persistence to restore
    bool          _showBack               = false; // show back button
    bool          _enableConfigPortal     = true;  // for autoconnect - start config portal if autoconnect failed
    String        _hostname               = "";    // hostname for dhcp, and/or MDNS

    const char*   _customHeadElement      = ""; // store custom head element html from user isnide <head>
    const char*   _customMenuHTML         = ""; // store custom head element html from user inside <>
    String        _title;                       // app title -  default WiFiManager

    bool          _showUploadSnd          = false;  // Show upload audio on Update page
    String        _sndContName;                     // File name of BIN file to upload

    // internal options
    bool          _preloadwifiscan        = false; // was: true; // preload wifiscan if true
    unsigned int  _scancachetime          = 30000; // ms cache time for preload scans
    bool          _asyncScan              = true;  // perform wifi network scan async

    bool          _autoforcerescan        = false; // automatically force rescan if scan networks is 0, ignoring cache

    bool          _disableIpFields        = false; // modify function of setShow_X_Fields(false), forces ip fields off instead of default show if set, eg. _staShowStaticFields=-1

    bool          _carMode                = false; // Custom

    #ifndef _A10001986_NO_COUNTRY
    String        _wificountry            = "";  // country code, @todo define in strings lang
    #endif

    // wrapper functions for handling setting and unsetting persistent for now.
    bool          esp32persistent         = false;
    bool          _hasBegun               = false; // flag wm loaded,unloaded
    void          _begin();
    void          _end();
	  void          _delay(unsigned int mydel);

	  bool          CheckParmID(const char *id);

    void          setupConfigPortal();
    bool          shutdownConfigPortal();
    void          setupHostname();

    bool          startAP();
    void          setupDNSD();
    void          setupHTTPServer();
    void          setupMDNS();

    uint8_t       connectWifi(String ssid, String pass, bool connect = true);
    bool          setStaticConfig();
    bool          wifiConnectDefault();
    bool          wifiConnectNew(String ssid, String pass, bool connect = true);

    uint8_t       waitForConnectResult();
    uint8_t       waitForConnectResult(uint32_t timeout);
    void          updateConxResult(uint8_t status);

    // webserver handlers
	  unsigned int  getHTTPHeadLength(String title, bool includeQI = false);
	  void          getHTTPHeadNew(String& page, String title, bool includeQI = false);

	  unsigned int  getParamOutSize(WiFiManagerParameter** params,
                        int paramsCount, unsigned int& maxItemSize);
  	void          getParamOut(WiFiManagerParameter** params,
                        int paramsCount, String &page, unsigned int maxItemSize);
    void          doParamSave(WiFiManagerParameter** params, int paramsCount);

	  int           reportStatusLen();
    void          reportStatus(String &page, unsigned int estSize = 0);

    void          HTTPSend(const String &content);

	  // Root menu
	  int           getMenuOutLength();
    void          getMenuOut(String& page);
    unsigned int  calcRootLen(unsigned int& headSize, unsigned int& repSize);
    void          buildRootPage(String& page, unsigned int headSize, unsigned int repSize);
    void          handleRoot();

  	// WiFi page
  	int           getScanItemStart();
  	void          sortNetworks(int n, int *indices);
  	unsigned int  getScanItemsLen(int n, bool scanErr, int *indices, unsigned int& maxItemSize);
    String        getScanItemsOut(int n, bool scanErr, int *indices, unsigned int maxItemSize);
	  String        getIpForm(const char *id, const char *title, IPAddress& value, const char *ph = NULL);
    String        getStaticOut(unsigned int estPageSize = 0);
	  unsigned int  getStaticLen();
    void          buildWifiPage(bool scan, String& page);
	  void          handleWifi(bool scan);
    void          handleWifiSave();

  	// Param page
  	int	  	      calcParmPageSize(unsigned int& maxItemSize);
  	void          handleParam();
    void          handleParamSave();

  	// Erase page
  	void          handleErase();

  	// OTA Update page
  	void          handleUpdate();
  	void          handleUpdating();
  	void          handleUpdateDone();

  	// Other
  	void          handleNotFound();
    void          handleRequest();

    uint8_t       processConfigPortal(bool handleWeb);

    // wifi platform abstractions
    bool          WiFi_Mode(WiFiMode_t m);
    bool          WiFi_Mode(WiFiMode_t m, bool persistent);
    bool          WiFi_Disconnect();
    bool          WiFi_enableSTA(bool enable);
    bool          WiFi_enableSTA(bool enable, bool persistent);
    bool          WiFi_eraseConfig();
    uint8_t       WiFi_softap_num_stations();
    void          WiFi_installEventHandler();
    String        WiFi_SSID(bool persistent = true) const;
    String        WiFi_psk(bool persistent = true) const;

    int16_t       WiFi_scanNetworks();
    int16_t       WiFi_scanNetworks(bool force, bool async);
    int16_t       WiFi_scanNetworks(unsigned int cachetime, bool async);
    int16_t       WiFi_scanNetworks(unsigned int cachetime);
	  int16_t       WiFi_waitForScan();
	  void          WiFi_scanComplete(int16_t networksFound);

    #ifndef _A10001986_NO_COUNTRY
    bool          WiFiSetCountry();
    #endif

    void          WiFiEvent(WiFiEvent_t event, arduino_event_info_t info);

    // helpers
    String        toStringIp(IPAddress ip);
    bool          validApPassword();

    // flags
    bool          connect             = false;
    bool          configPortalActive  = false;
    bool          webPortalActive     = false;

    bool          storeSTAmode        = true; // option store persistent STA mode in connectwifi

    // WiFiManagerParameter
    int           _paramsCount         = 0;
    int           _max_params;
    WiFiManagerParameter** _params     = NULL;
    int           _wifiparamsCount     = 0;
    int           _max_wifi_params;
    WiFiManagerParameter** _wifiparams = NULL;

    // callbacks
    void (*_apcallback)(WiFiManager*);
    void (*_webservercallback)(void);
    void (*_savewificallback)(bool, String&);
    void (*_presavewificallback)(void);
    void (*_presaveparamscallback)(void);
    void (*_saveparamscallback)(void);
    void (*_preotaupdatecallback)(void);
    void (*_postotaupdatecallback)(bool);
	  void (*_erasecallback)(bool);
	  void (*_menuoutcallback)(String &page);
	  int  (*_menuoutlencallback)(void);
	  void (*_delayreplacement)(unsigned int);
	  void (*_gpcallback)(int);
	  bool (*_prewifiscancallback)(void);
	  void (*_preconnectcallback)(void);

    template <class T>
    auto optionalIPFromString(T *obj, const char *s) -> decltype(  obj->fromString(s)  ) {
        return  obj->fromString(s);
    }
    auto optionalIPFromString(...) -> bool {
        return false;
    }

    // debugging
    typedef enum {
        DEBUG_SILENT    = 0, // debug OFF but still compiled for runtime
        DEBUG_ERROR     = 1, // error only
        DEBUG_NOTIFY    = 2, // default stable,INFO
        DEBUG_VERBOSE   = 3, // move verbose info
        DEBUG_DEV       = 4, // development useful debugging info
        DEBUG_MAX       = 5  // MAX extra dev auditing, var dumps etc (MAX+1 will print timing,mem and frag info)
    } wm_debuglevel_t;

    bool _debug  = true;
    String _debugPrefix;	// = FPSTR(S_debugPrefix);

    wm_debuglevel_t debugLvlShow = DEBUG_VERBOSE; // at which level start showing [n] level tags

    #ifdef WM_DEBUG_LEVEL
    uint8_t _debugLevel = (uint8_t)WM_DEBUG_LEVEL;
    #else
    uint8_t _debugLevel = 0; // default debug level
    #endif

    #ifdef WM_DEBUG_PORT
    Print& _debugPort = WM_DEBUG_PORT;
    #else
    Print& _debugPort = Serial; // debug output stream ref
    #endif

    template <typename Generic>
    void        DEBUG_WM(Generic text);

    template <typename Generic>
    void        DEBUG_WM(wm_debuglevel_t level,Generic text);
    template <typename Generic, typename Genericb>
    void        DEBUG_WM(Generic text,Genericb textb);
    template <typename Generic, typename Genericb>
    void        DEBUG_WM(wm_debuglevel_t level, Generic text,Genericb textb);

};

#endif
