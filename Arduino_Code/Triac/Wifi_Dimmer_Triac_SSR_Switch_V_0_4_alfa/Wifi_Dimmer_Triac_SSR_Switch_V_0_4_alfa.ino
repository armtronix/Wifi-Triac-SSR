/*
 *  This sketch is running a web server for configuring WiFI if can't connect or for controlling of one GPIO to switch a light/LED
 *  Also it supports to change the state of the light via MQTT message and gives back the state after change.
 *  The push button has to switch to ground. It has following functions: Normal press less than 1 sec but more than 50ms-> Switch light. Restart press: 3 sec -> Restart the module. Reset press: 20 sec -> Clear the settings in EEPROM
 *  While a WiFi config is not set or can't connect:
 *    http://server_ip will give a config page with 
 *  While a WiFi config is set:
 *    http://server_ip/gpio -> Will display the GIPIO state and a switch form for it
 *    http://server_ip/gpio?state_led=0 -> Will change the GPIO14 to Low (triggering the SSR) 
 *    http://server_ip/gpio?state_led=1 -> Will change the GPIO14 to High (triggering the SSR)
 *    http://server_ip/gpio?state_sw=0 -> Will change the GPIO13 to Low (triggering the TRIAC) 
 *    http://server_ip/gpio?state_sw=1 -> Will change the GPIO13 to High ( triggering the TRIAC) 
 *    http://server_ip/gpio?state_dimmer=value -> value has to be a number between 0-90 example  http://server_ip/gpio?state_dimmer=80 (triggering the TRIAC)
 *    http://server_ip/cleareeprom -> Will reset the WiFi setting and rest to configure mode as AP
 *  server_ip is the IP address of the ESP8266 module, will be 
 *  printed to Serial when the module is connected. (most likly it will be 192.168.4.1)
 * To force AP config mode, press button 20 Secs!
 *  For several snippets used, the credit goes to:
 *  - https://github.com/esp8266
 *  - https://github.com/chriscook8/esp-arduino-apboot
 *  - https://github.com/knolleary/pubsubclient
 *  - https://github.com/vicatcu/pubsubclient <- Currently this needs to be used instead of the origin
 *  - https://gist.github.com/igrr/7f7e7973366fc01d6393
 *  - http://www.esp8266.com/viewforum.php?f=25
 *  - http://www.esp8266.com/viewtopic.php?f=29&t=2745
 *  Dimmer Code with timer from https://github.com/nassir-malik/IOT-Light-Dimmer
 *  - And the whole Arduino and ESP8266 comunity
 */

#define DEBUG
//#define WEBOTA
//debug added for information, change this according your needs

#ifdef DEBUG
  #define Debug(x)    Serial.print(x)
  #define Debugln(x)  Serial.println(x)
  #define Debugf(...) Serial.printf(__VA_ARGS__)
  #define Debugflush  Serial.flush
#else
  #define Debug(x)    {}
  #define Debugln(x)  {}
  #define Debugf(...) {}
  #define Debugflush  {}
#endif

#include "hw_timer.h"  
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <Hash.h>
//#include <EEPROM.h>
#include <Ticker.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "FS.h"

extern "C" {
  #include "user_interface.h" //Needed for the reset command
}

//***** Settings declare ********************************************************************************************************* 
String hostName ="Armtronix"; //The MQTT ID -> MAC adress will be added to make it kind of unique
int iotMode=0; //IOT mode: 0 = Web control, 1 = MQTT (No const since it can change during runtime)
//select GPIO's

const byte OUTPIN_TRIAC = 13; //output pin of Triac
const byte  AC_ZERO_CROSS = 12;   // input to Opto Triac pin   
#define INPIN 0  // input pin (push button)
#define OUTPIN_SSR 14  //output pin of SSR
#define RESTARTDELAY 3 //minimal time in sec for button press to reset
#define HUMANPRESSDELAY 50 // the delay in ms untill the press should be handled as a normal push by human. Button debounce. !!! Needs to be less than RESTARTDELAY & RESETDELAY!!!
#define RESETDELAY 20 //Minimal time in sec for button press to reset all settings and boot to config mode
#define INPIN_REGULATOR 4  // input pin (push button)

//##### Object instances ##### 
MDNSResponder mdns;
ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
WiFiClient wifiClient;
PubSubClient mqttClient;
Ticker btn_timer;
Ticker otaTickLoop;

//##### Flags ##### They are needed because the loop needs to continue and cant wait for long tasks!
int rstNeed=0;   // Restart needed to apply new settings
int toPub=0; // determine if state should be published.
int configToClear=0; // determine if config should be cleared.
int otaFlag=0;
boolean inApMode=0;
//##### Global vars ##### 
int webtypeGlob;
int otaCount=300; //imeout in sec for OTA mode
int current; //Current state of the button

unsigned long count = 0; //Button press time counter
unsigned long count_regulator = 0; //Button press time counter
String st; //WiFi Stations HTML list
String state; //State of light
char buf[40]; //For MQTT data recieve
char* host; //The DNS hostname
//To be read from Config file
String esid="";
String epass = "";
String pubTopic;
String subTopic;
String mqttServer = "";
const char* otaServerIndex = "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";

// Dimmer://
int button_press_flag =1;
byte fade = 0;
byte statem = 1;
byte tarBrightness = 0;
byte curBrightness = 0;
byte zcState = 0; // 0 = ready; 1 = processing;
int max_brightness =255;

//-------------- void's -------------------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  pinMode(OUTPIN_TRIAC, OUTPUT);
  pinMode(OUTPIN_SSR, OUTPUT);
  pinMode(AC_ZERO_CROSS, INPUT_PULLUP);
  pinMode(INPIN, INPUT_PULLUP);
  pinMode(INPIN_REGULATOR, INPUT_PULLUP);
  
  btn_timer.attach(0.05, btn_handle);
  
  Debugln("DEBUG: Entering loadConfig()");
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
  }
  
  uint8_t mac[6];
  WiFi.macAddress(mac);
  hostName += "-";
  hostName += macToStr(mac);
  String hostTemp=hostName;
  hostTemp.replace(":","-");
  host = (char*) hostTemp.c_str();
  loadConfig();
  //loadConfigOld();
  Debugln("DEBUG: loadConfig() passed");
  
  // Connect to WiFi network
  Debugln("DEBUG: Entering initWiFi()");
  initWiFi();
  Debugln("DEBUG: initWiFi() passed");
  Debug("iotMode:");
  Debugln(iotMode);
  Debug("webtypeGlob:");
  Debugln(webtypeGlob);
  Debug("otaFlag:");
  Debugln(otaFlag);
  Debugln("DEBUG: Starting the main loop");
}


void dimTimerISR() 
{
    if (fade == 1) {
      if (curBrightness > tarBrightness || (statem == 0 && curBrightness > 0)) {
        --curBrightness;
      }
      else if (curBrightness < tarBrightness && statem == 1 && curBrightness < max_brightness) {
        ++curBrightness;
      }
    }
    else {
      if (statem == 1) {
        curBrightness = tarBrightness;
      }
      else {
        curBrightness = 0;
      }
    }
    
    if (curBrightness == 0) {
      statem = 1;
      digitalWrite(OUTPIN_TRIAC, 0);
    }
    else if (curBrightness == max_brightness) {
      statem = 1;
      digitalWrite(OUTPIN_TRIAC, 1);
    }
    else {
      digitalWrite(OUTPIN_TRIAC, 1); //naren commented
      
    }
    //Serial.println(curBrightness);
    zcState = 0;
}

void zcDetectISR() 
{
  if (zcState == 0)
  {
    zcState = 1;
  
    if (curBrightness < max_brightness && curBrightness > 0) 
    {
      digitalWrite(OUTPIN_TRIAC, 0);
      
      int dimDelay = 35* (max_brightness - curBrightness);  //8050
      hw_timer_arm(dimDelay);
    }
  }
}

void btn_handle()
{
if(count_regulator<=9)
{
  count_regulator=0;
  tarBrightness =count_regulator;
}

if(!digitalRead(INPIN_REGULATOR))
{
  if(button_press_flag ==1)  
{
button_press_flag =0;
if(count_regulator<=9)
{
tarBrightness =count_regulator+10;
count_regulator=count_regulator+10;
}
else
{
count_regulator=count_regulator+20;
tarBrightness =count_regulator;
}
if(count_regulator<=200)
     {
      Serial.print("Reg VAL:");
      Serial.println(count_regulator);
      Serial.print("Brig VAL:");
      Serial.println(curBrightness);
      Serial.println(button_press_flag);
     }
     else
     {
      count_regulator=0;
     }
}
}
else
{
  button_press_flag =1;
  if (count_regulator > 1 && count_regulator < HUMANPRESSDELAY/5) 
    { 
    if(count_regulator<=200)
     {
      //Serial.println(count_regulator);
     }
     else
     {
      count_regulator=0;
     }
    }
}
  if(!digitalRead(INPIN))
  {
    ++count; // one count is 50ms
    
  } 
  else 
  {
    if (count > 1 && count < HUMANPRESSDELAY/5) 
    {
      //push between 50 ms and 1 sec      
      Serial.print("button pressed "); 
      Serial.print(count*0.05); 
      Serial.println(" Sec."); 
    
      Serial.print("Light is ");
      Serial.println(digitalRead(OUTPIN_SSR));
      
      Serial.print("Switching light to "); 
      Serial.println(!digitalRead(OUTPIN_SSR));
      digitalWrite(OUTPIN_SSR, !digitalRead(OUTPIN_SSR)); 
      state = digitalRead(OUTPIN_SSR);
      if(iotMode==1 && mqttClient.connected())
      {
        toPub=1;        
        Debugln("DEBUG: toPub set to 1");
      }
    } else if (count > (RESTARTDELAY/0.05) && count <= (RESETDELAY/0.05))
    { 
      //pressed 3 secs (60*0.05s)
      Serial.print("button pressed "); 
      Serial.print(count*0.05); 
      Serial.println(" Sec. Restarting!"); 
      setOtaFlag(!otaFlag);      
      ESP.reset();
    } else if (count > (RESETDELAY/0.05)){ //pressed 20 secs
      Serial.print("button pressed "); 
      Serial.print(count*0.05); 
      Serial.println(" Sec."); 
      Serial.println(" Clear settings and resetting!");       
      configToClear=1;
      }
    count=0; //reset since we are at high
  }
}



//-------------------------------- Main loop ---------------------------
void loop() {
   webSocket.loop();
  //Debugln("DEBUG: loop() begin");
  if(configToClear==1){
    //Debugln("DEBUG: loop() clear config flag set!");
    clearConfig()? Serial.println("Config cleared!") : Serial.println("Config could not be cleared");
   // delay(1000);
    ESP.reset();
  }
  //Debugln("DEBUG: config reset check passed");  
  if (WiFi.status() == WL_CONNECTED && otaFlag){
    if(otaCount<=1) {
      Serial.println("OTA mode time out. Reset!"); 
      setOtaFlag(0);
      ESP.reset();
     // delay(100);
    }
    server.handleClient();
   // delay(1);
  } else if (WiFi.status() == WL_CONNECTED || webtypeGlob == 1){
    //Debugln("DEBUG: loop() wifi connected & webServer ");
    if (iotMode==0 || webtypeGlob == 1){
      //Debugln("DEBUG: loop() Web mode requesthandling ");
      server.handleClient();
     // delay(1);
       if(esid != "" && WiFi.status() != WL_CONNECTED) //wifi reconnect part
      {
        Scan_Wifi_Networks();
      }
    } else if (iotMode==1 && webtypeGlob != 1 && otaFlag !=1){
          //Debugln("DEBUG: loop() MQTT mode requesthandling ");
          if (!connectMQTT()){
            //  delay(200);          
          }                    
          if (mqttClient.connected()){            
              //Debugln("mqtt handler");
              mqtt_handler();
          } else {
              Debugln("mqtt Not connected!");
          }
    }
  } else{
    Debugln("DEBUG: loop - WiFi not connected");  
    delay(1000);
    initWiFi(); //Try to connect again
  }
    //Debugln("DEBUG: loop() end");
}
