/**
 * wm_strings_en.h
 *
 * Based on:
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

#ifndef _WM_STRINGS_EN_H_
#define _WM_STRINGS_EN_H_

#include "wm_consts_en.h"

const char HTTP_HEAD_START[]       PROGMEM = "<!DOCTYPE html>"
    "<html lang='en'><head>"
    "<meta name='format-detection' content='telephone=no'>"
    "<meta charset='UTF-8'>"
    "<meta name='viewport' content='width=device-width,initial-scale=1,user-scalable=no'/>"
    "<title>{v}</title>";

const char HTTP_SCRIPT[]           PROGMEM = "<script>"
    "function wlp(){return window.location.pathname}"
    "function getn(x){return document.getElementsByTagName(x)}"
    "function ge(x){return document.getElementById(x)}"
    "function gecl(x){return document.getElementsByClassName(x)}"
    "function f(){x=ge('p');x.type==='password'?x.type='text':x.type='password';}"
    "</script>";

const char HTTP_HEAD_END[]         PROGMEM = "</head><body><div class='wrap'>";

const char HTTP_ROOT_MAIN[]        PROGMEM = "<h1>{t}</h1><h3>{v}</h3>";

const char * const HTTP_PORTAL_MENU[] PROGMEM = {
    "<form action='/wifi' method='get' onsubmit='d=ge(\"wbut\");if(d){d.disabled=true;d.innerHTML=\"Please wait\"}'><button id='wbut'>WiFi Configuration</button></form>\n",
    "<form action='/param' method='get'><button>Settings</button></form>\n",
    "<form action='/update' method='get'><button>Update</button></form>\n",
    "<hr>", // sep
    ""      // custom, if _customMenuHTML is NULL
};

const char HTTP_FORM_START[]       PROGMEM = "<form method='POST' action='{v}'>";
const char HTTP_FORM_LABEL[]       PROGMEM = "<label for='{i}'>{t}</label>";
const char HTTP_FORM_PARAM_HEAD[]  PROGMEM = "<hr>";
const char HTTP_FORM_PARAM[]       PROGMEM = "<input id='{i}' name='{n}' maxlength='{l}' value='{v}' {c}>";
const char HTTP_FORM_END[]         PROGMEM = "<button type='submit'>Save</button></form>";

const char HTTP_FORM_WIFI[]        PROGMEM = "<div class='sects'><div class='headl'>WiFi connection: Network selection</div><label for='s'>Network name (SSID)</label><input id='s' name='s' maxlength='32' autocorrect='off' autocapitalize='none' placeholder='{V}' oninput='f=ge(\"fg\");h=ge(\"p\");h.disabled=false;if(!this.value.length&&this.placeholder.length){if(f&&!h.value.length){f.style.display=\"\"}h.placeholder=h.getAttribute(\"data-ph\")||\"********\";}else{if(f){f.style.display=\"none\"}h.placeholder=\"\"}'><br/><label for='p'>Password</label><input id='p' name='p' maxlength='64' type='password' placeholder='{p}' data-ph='{p}' oninput='f=ge(\"fg\");if(f){s=ge(\"s\");if(!s.value.length&&s.placeholder.length){if(this.value.length){f.style.display=\"none\"}else{f.style.display=\"\"}}}'><label><input type='checkbox' onclick='f()' style='margin:0px 5px 10px 0px'>Show password when typing</label>";
const char HTTP_FORM_WIFI_END[]    PROGMEM = "</div>";
const char HTTP_WIFI_ITEM[]        PROGMEM = "<div><a href='#p' onclick='return {t}(this)' data-ssid='{V}'>{v}</a>{c}<div role='img' aria-label='{r}dBm' title='{r}dBm' class='q q-{q} {i}'></div></div>";
const char HTTP_FORM_SECT_HEAD[]   PROGMEM = "<div class='sects'><div class='headl'>WiFi connection: Static IP settings</div>";
const char HTTP_FORM_SECT_FOOT[]   PROGMEM = "</div>";
const char HTTP_FORM_WIFI_PH[]     PROGMEM = "placeholder='Leave this section empty to use DHCP'";
const char HTTP_MSG_NONETWORKS[]   PROGMEM = "<div class='msg'>No networks found. Click 'WiFi Scan' to re-scan.</div>";
const char HTTP_MSG_SCANFAIL[]     PROGMEM = "<div class='msg D'>Scan failed. Click 'WiFi Scan' to retry.</div>";
const char HTTP_MSG_NOSCAN[]       PROGMEM = "<div class='msg'>Device busy, WiFi scan prohibited.<br/>Click 'WiFi Scan' after animation sequence has finished.</div>";
const char HTTP_SCAN_LINK[]        PROGMEM = "<form action='/wifi?refresh=1' method='POST' onsubmit='if(confirm(\"This will reload the page, changes are not saved. Proceed?\")){d=ge(\"wrefr\");if(d){d.disabled=true;d.innerHTML=\"Please wait\"}return true;}return false;'><button id='wrefr' name='refresh' value='1'>WiFi Scan</button></form>";
const char HTTP_ERASE_BUTTON[]     PROGMEM = "<div id='fg' class='c' style='border:2px solid #dc3630;border-radius:7px'><label><input id='fgn' name='fgn' type='checkbox' style='margin-top:0'>Forget saved WiFi network</label></div>";
const char HTTP_SHOWALL[]          PROGMEM = "<div class='c'><button class='s' form='saform' type='submit'>Show all</button></div>";
const char HTTP_SHOWALL_FORM[]     PROGMEM = "<form id='saform' action='/wifi?showall=1' method='POST'></form>";

const char HTTP_PARAMSAVED[]       PROGMEM = "<div class='msg S'>Settings saved. Rebooting.<br/>";
const char HTTP_SAVED_NORMAL[]     PROGMEM = "Trying to connect to network.<br />In case of error, device boots in AP mode.";
const char HTTP_SAVED_CARMODE[]    PROGMEM = "Device is run in <strong>car mode<strong> and will <strong>not</strong><br/>connect to saved WiFi network after reboot.";
const char HTTP_SAVED_ERASED[]     PROGMEM = "WiFi network credentials deleted.<br />Restarting in AP mode.<br/>";
const char HTTP_PARAMSAVED_END[]   PROGMEM = "</div>";

const char HTTP_UPDATE[]           PROGMEM = "Upload new firmware<br/><form method='POST' action='u' enctype='multipart/form-data' onsubmit=\"var aa=ge('uploadbin');if(aa){aa.disabled=true;aa.innerHTML='Please wait'}aa=ge('uacb');if(aa){aa.disabled=true}\" onchange=\"(function(el){ge('uploadbin').style.display=el.value==''?'none':'initial';})(this)\"><input type='file' name='update' accept='.bin,application/octet-stream'><button id='uploadbin' type='submit' class='h D'>Update</button></form>";
const char HTTP_UPLOADSND1[]       PROGMEM = "Upload sound pack (";
const char HTTP_UPLOADSND2[]       PROGMEM = ".bin)<br>and/or .mp3 file(s)<br><form method='POST' action='uac' enctype='multipart/form-data' onsubmit=\"var aa=ge('uacb');if(aa){aa.disabled=true;aa.innerHTML='Please wait'}aa=ge('uploadbin');if(aa){aa.disabled=true}\" onchange=\"(function(el){ge('uacb').style.display=el.value==''?'none':'initial';})(this)\"><input type='file' name='upac' multiple accept='.bin,application/octet-stream,.mp3,audio/mpeg'><button id='uacb' type='submit' class='h'>Upload</button></form>";
const char HTTP_UPDATE_FAIL1[]     PROGMEM = "<div class='msg D'><strong>Update failed.</strong><br/>";
const char HTTP_UPDATE_FAIL2[]     PROGMEM = "</div>";
const char HTTP_UPDATE_SUCCESS[]   PROGMEM = "<div class='msg S'><strong>Update successful.</strong><br/>Device rebooting.</div>";
const char HTTP_UPLOAD_SDMSG[]     PROGMEM = "<div class='msg' style='text-align:left'>In order to upload the sound-pack,<br/>please insert an SD card.</div>";

const char HTTP_BACKBTN[]          PROGMEM = "<hr><form action='/' method='get'><button>Back</button></form>";

const char HTTP_STATUS_ON[]        PROGMEM = "<div class='msg S'><strong>Connected</strong> to {v}<br/><em><small>with IP {i}</small></em></div>";
const char HTTP_STATUS_OFF[]       PROGMEM = "<div class='msg {c}'><strong>Not connected</strong> to {v}{r}<br/><em><small>Currently operating in {V}</small></em></div>";
const char HTTP_STATUS_APMODE[]    PROGMEM = "AP-mode";
const char HTTP_STATUS_CARMODE[]   PROGMEM = "car mode";
const char HTTP_STATUS_OFFNOAP[]   PROGMEM = "<br/>Network not found";    // WL_NO_SSID_AVAIL
const char HTTP_STATUS_OFFFAIL[]   PROGMEM = "<br/>Could not connect";    // WL_CONNECT_FAILED
const char HTTP_STATUS_DISCONN[]   PROGMEM = "<br/>Disconnected. Wrong Password?";    // WL_DISCONNECTED
const char HTTP_STATUS_NONE[]      PROGMEM = "<div class='msg'>No WiFi connection configured</div>";
const char HTTP_BR[]               PROGMEM = "<br/>";
const char HTTP_END[]              PROGMEM = "</div></body></html>";

const char HTTP_STYLE[]            PROGMEM = "<style>"
    ".c,body{text-align:center;font-family:-apple-system,BlinkMacSystemFont,system-ui,'Segoe UI',Roboto,'Helvetica Neue',Verdana,Helvetica}"
    "div,input,select{padding:5px;font-size:1em;margin:5px 0;box-sizing:border-box}"
    "input,button,select,.msg{border-radius:.3rem;width: 100%}"
    "input[type=radio],input[type=checkbox]{display:inline-block;margin-top:10px;margin-right:5px;width:auto;}"
    "button,input[type='button'],input[type='submit']{cursor:pointer;border:0;background-color:#225a98;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%}"
    "input[type='file']{border:1px solid #225a98}"
    ".h{display:none}"
    ".wrap{text-align:left;display:inline-block;min-width:260px;max-width:500px}"
    ".headl{margin:0 0 7px 0;padding:0}"
    ".sects{background-color:#eee;border-radius:7px;margin-bottom:20px;padding-bottom:7px;padding-top:7px}"
    // links
    "a{color:#000;font-weight:bold;text-decoration:none}"
    "a:hover{color:#225a98;text-decoration:underline}"
    // msg callouts
    ".msg{padding:20px;margin:20px 0;border:1px solid #ccc;border-radius:20px;border-left-width:15px;border-left-color:#777;background:linear-gradient(320deg,rgb(255,255,255) 0%,rgb(235,234,233) 100%)}"
    ".msg.P{border-left-color:#225a98}"
    ".msg.D{border-left-color:#dc3630}"
    ".msg.S{border-left-color:#609b71}"
    // buttons
    "button{transition:0s opacity;transition-delay:3s;transition-duration:0s;cursor:pointer}"
    "button.D{background-color:#dc3630;border-color:#dc3630}"
    "button:active{opacity:50% !important;cursor:wait;transition-delay:0s}"
    ":disabled {opacity:0.5;}"
    "</style>";

// quality icons plus some specific JS
const char HTTP_STYLE_QI[]         PROGMEM = "<style>"
    "button.s{width:initial;line-height:1.3em;margin:0}"
    ".q{height:16px;margin:0;padding:0 5px;text-align:right;min-width:38px;float:right}"
    ".q.q-0:after{background-position-x:0}"
    ".q.q-1:after{background-position-x:0}"
    ".q.q-2:after{background-position-x:-21px}"
    ".q.q-3:after{background-position-x:-42px}"
    ".q.q-4:after{background-position-x:-63px}"
    ".q.l:before{background-position-x:-84px}"
    ".ql .q{float:left}"
    ".q:after,.q:before{content:'';width:21px;height:16px;display:inline-block;background-repeat:no-repeat;background-position:21px 0;"
    "background-image:url('data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAGIAAAAQCAMAAADakVr2AAAA81BMVEUAAADHycnHyckAAADHyckAAAAAAADHycnHyckAAAAAAADHyckAAAAAAAAAAAAAAADHycnHyckAAADHyckAAAAAAADHyckAAAAAAADHyckAAADHyckAAADHycnHyckAAADHyckAAADHyckAAADHyckAAAAAAAAAAAAAAAAAAADHyckAAADHycnHyckAAADHycnHyckAAADHyckAAAAAAADHycnHycnHyckAAADHycnHyckAAAAAAADHyckAAAAAAADHyckAAAAAAAAAAADHyckAAADHycnHyckAAAAAAADHycnHycnHycnHyckAAAAAAADHyclrQTMdAAAAT3RSTlMAKG7W0JlsZrukGaP7zciQRr+8OiSxn0ApHh0WDOvft6melZSRYyET9eKtqJuOXU1LLhrr0cfCi1g/IwkG99qsp4+HcWBFBdR8ODO0WVdVJL4uIQAAAbdJREFUOMullOlWwjAQRqctXSi0IItUQNmFulRZ3EBFRdx18v5P47TRUimLwP2RfLmnJ8lMzwl47IixyO7NbiQm7nCxmtSVodFtdXvDUR5mky6zCQfpVWU7ixNKbZhBlT7fq6vJdFI93qN89LRMXgWljIh2whm/jp17m7IM07xFGNs68ZenGmMRQfivfLMkRCPny5yJeGsBxxpmvZq2GFNpEsSLo2NRoPRILSjPkndcWsp9JaFYXL6nEBsUM7504lgCj2aPjtPdbbQk3YgOIvgtr7ROYa7MGcjp5VwpZIzxX0mLnypS+NpGk+fCNu1zVr2on9Ec6yyQgygimrWHhEnzYZHL6z9yAL+4f+UBeabNYoKX+hTvpqUwkTJiVPeSHqW4QBJYA3hBnvvnIt36U/woAKjnQkDqzsjJB2ReUqjHl8plE6AhZeZLooIt225h9nddqDOXagcCFGvoIjdnyeIy2UUOgN+W5/IzY2UIUEGUDiWqfS0poUfXfxc0kUZV60MA3VRobBj7a8lb9GgF1KA9gDD71+tKO3SEnsUShNhAjlIeLxPzFcd4BqbIbyDDFBMoh+1mkvMNs1qA66I7EMYAAAAASUVORK5CYII=');}"
    // icons @2x media query (32px rescaled)
    "@media (-webkit-min-device-pixel-ratio:2),(min-resolution:192dpi){.q:before,.q:after{"
    "background-image:url('data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAMQAAAAgCAMAAAB6rSfNAAABJlBMVEUAAAAAAAAAAAAAAADHyckAAADHyckAAADHycnHyckAAADHyckAAADHycnHyckAAAAAAAAAAAAAAADHyckAAADHyckAAAAAAAAAAADHycnHyckAAADHyckAAADHycnHycnHyckAAAAAAADHyckAAAAAAAAAAAAAAADHyckAAAAAAAAAAAAAAADHycnHyckAAADHyckAAADHyckAAADHyckAAADHyckAAADHyckAAAAAAADHyckAAADHycnHycnHycnHyckAAADHycnHycnHycnHyckAAADHycnHycnHycnHycnHycnHyckAAADHycnHycnHycnHyckAAADHyckAAAAAAAAAAADHyckAAADHyckAAAAAAAAAAADHycmXmZmQkpIAAADHycms9KC6AAAAYHRSTlMA3u8J/LxKI+rfBHFFJg359Oa1sHFXOTInHdvTy5eWd1BIE4J3Uywd1rGhTT/3ko+JWDYvF+rSyq2siW1nYUAK49m3tKNoX0Y7Lyry7sysppwhDwalkoJdxnx7beK+jWod6B+WAAADSklEQVRYw9WV21raQBCANyAECFTOQSByEEREUVREEEVEOSlqFbGe2tn3f4nGsJQsSQhNP/jof7PJzn8zu7MziOKpfZr3Re02mz3qW+fOa1Rw7qY5+P6cLTHFIlPKCv4dMzJALXB/gyl2716+Lco0F/puoHCXm4/o7/i+votVWHlwLcJkhVVQYVUIodk5useanKzN28z0QZNyBs3IxviUtnzrG7HYxrrveAWP2J+vORiffCQrDFKpgZCNjG+mMlslhfGQ26u4rF6/WR9sJHDhmp/J1mGIaTsnewPmxHaRBD5mqKkAHnIcV8asURI8m5dZgCGRtNJM9EiwiXR4xRL2c/XwwRa5/vmYbyDB7KibwbpGSbEVi0XgESGPJU5lV+7qHHRlJXA2NK7+xXwM8XwoqTB/PoPEu7bZHBp9RFFlpAKskt9rLBIeNYs1rhHelR6j/TI22jySDi42s8nRZsbvZValZ8t4UxnK/FWVriEzzUxGQCRF5wAi8m0rxpeT1TpZ0ScYBwyauR5QRNKUmQDw6pllgAJdS2JU4NmKuPCju2gM13YYK7ggVX0XM2YGGVDw4ZSbVa++WU4higpAmawCHbnSGEtPYsyo2dcYYA6DJkG8MlaqNABGvr9mJ4PpkrN2XK5Oi/ORwWQT+7kxMzM6XG8qcciymwl/lowwT8iYSRDtJPkwyXs3mUzckaz3nJK5FECGzASZYamkrPe8e0AibcCUJWFSSyJO+vYEG2QoGTJz6h1+QMaXAVPnJtDBCr45UOpd8Yhb4mrIDAK4g0pzswjgNGISeMtXW5YQP3pywWrvSutTfL8RjTby8drwBYRfiKAwHbk9b6nk3UubtcwcE1IzM/WmIZNgAQqT0uBuMeHmFU3FbwKC+82xOFM/CZedav0dpAlLtXXP4aJM/SQCeIJrpEEBJqguyNRPooW/COc5Lr+lbJnKplh/9vv3ItJnYVGmfjl1d7GNtJj2BcZtpMmmG4pO0lo8AMHFmfoPu/X5Z4bVPgNoCk5LEhHMlvQCTWUS/yOzJeHYcZon95bILANFT1ViPWJ6k/1tiUyeoS6CV5V+gEgE0SyhOY1NkNhZdnMqIZDgl92czjaIZJffnIpDcK9um5ffHPMbdfGaiuWnXuoAAAAASUVORK5CYII=');"
    "background-size:98px 16px;}}"
    "</style>"
    "<script>function c(l){ge('s').value=l.getAttribute('data-ssid')||l.innerText||l.textContent;"
    "p=l.nextElementSibling.classList.contains('l');pp=ge('p');"
    "pp.disabled=!p;pp.placeholder='';f=ge('fg');if(f){f.style.display='none'}if(p){pp.focus()}return false;}"
    "function d(l){return false}"
    "</script>";

const char A_paramsave[]          PROGMEM = "paramsave";
const char A_wifisave[]           PROGMEM = "wifisave";

const char S_titlewifi[]          PROGMEM = "WiFi Configuration";
const char S_titleparam[]         PROGMEM = "Settings";
const char S_titleupd[]           PROGMEM = "Upload";

const char S_passph[]             PROGMEM = "********";
const char S_staticip[]           PROGMEM = "Static IP";
const char S_staticgw[]           PROGMEM = "Static gateway";
const char S_staticdns[]          PROGMEM = "Static DNS";
const char S_subnet[]             PROGMEM = "Subnet mask";

const char S_brand[]              PROGMEM = "WiFiManager";

const char S_GET[]                PROGMEM = "GET";
const char S_POST[]               PROGMEM = "POST";
const char S_NA[]                 PROGMEM = "Unknown";

const char S_notfound[]           PROGMEM = "404 File not found\n\n";

#endif  // _WM_STRINGS_EN_H_
