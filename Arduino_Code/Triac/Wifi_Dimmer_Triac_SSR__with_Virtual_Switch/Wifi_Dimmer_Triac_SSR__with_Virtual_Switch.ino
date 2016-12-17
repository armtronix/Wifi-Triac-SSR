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


#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
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
#define OUTPIN_TRIAC 13 //output pin of Triac
#define INPIN 0  // input pin (push button)
#define OUTPIN_SSR 14  //output pin of SSR
#define SWITCH_INPIN3 4  // input pin (Physical Switch)   //Intially while booting maintain this gpio2 to high to low by placing the physical switch in off condition //trigger relay1
#define SWITCH_INPIN4 5  // input pin (Physical Switch)   //trigger relay2
#define AC_ZERO_CROSS  12   // input to Opto Triac pin 


#define RESTARTDELAY 3 //minimal time in sec for button press to reset
#define HUMANPRESSDELAY 50 // the delay in ms untill the press should be handled as a normal push by human. Button debounce. !!! Needs to be less than RESTARTDELAY & RESETDELAY!!!
#define RESETDELAY 20 //Minimal time in sec for button press to reset all settings and boot to config mode
  


//##### Object instances ##### 
MDNSResponder mdns;
ESP8266WebServer server(80);
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

int freqStep = 375;//75*5 as prescalar is 16 for 80MHZ
volatile int i=0;
volatile int dimming =0;  
volatile boolean zero_cross=0;

unsigned long count = 0; //Button press time counter
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

String javaScript,XML;
int switch_status3, switch_status4; //Physical state of the switch
int state_13, state_14, state_dimmer_mode;
int send_status_13, send_status_14;

//-------------- void's -------------------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(10);
  // prepare GPIO2
  pinMode(OUTPIN_TRIAC, OUTPUT);
  pinMode(OUTPIN_SSR, OUTPUT);
  pinMode(AC_ZERO_CROSS, INPUT);
  pinMode(INPIN, INPUT_PULLUP);

    //define manual switch
   pinMode(SWITCH_INPIN3, INPUT_PULLUP);
   pinMode(SWITCH_INPIN4, INPUT_PULLUP);  

   
  //digitalWrite(OUTLED, HIGH);
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

void InitInterrupt(timercallback handler,int Step )
{ 
  timer1_disable();
  timer1_isr_init();
  timer1_attachInterrupt(handler);
  timer1_enable(TIM_DIV16, TIM_EDGE, TIM_LOOP);
  timer1_write(Step);//max 8388607 //75*5
}

void  ICACHE_RAM_ATTR do_on_delay()
{
  //digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));
  
  if(zero_cross == true) 
  {              
    if(i>=dimming) 
    {  
                       
     digitalWrite(OUTPIN_TRIAC, HIGH); // turn on light       
     i=0;  // reset time step counter                         
     zero_cross = false; //reset zero cross detection
     delayMicroseconds(10);         // triac On propogation delay
     digitalWrite(OUTPIN_TRIAC, LOW);    // triac Off
    } 
    else 
    {
     i++; // increment time step counter 
     digitalWrite(OUTPIN_TRIAC, LOW);    // triac Off        
                 
    }                                
  }    
}

void zero_crosss_int()  // function to be fired at the zero crossing to dim the light
{
  
 zero_cross = true; 
 i=0;
  //InitInterrupt(do_something);
 digitalWrite(OUTPIN_TRIAC, LOW);  
 InitInterrupt(do_on_delay,freqStep);
 
}

void btn_handle()
{
  if(!digitalRead(INPIN)){
    ++count; // one count is 50ms
  } else {
    if (count > 1 && count < HUMANPRESSDELAY/5) { //push between 50 ms and 1 sec      
      Serial.print("button pressed "); 
      Serial.print(count*0.05); 
      Serial.println(" Sec."); 
    
      Serial.print("Light is ");
      Serial.println(digitalRead(OUTPIN_SSR));
      
      Serial.print("Switching light to "); 
      Serial.println(!digitalRead(OUTPIN_SSR));
      digitalWrite(OUTPIN_SSR, !digitalRead(OUTPIN_SSR)); 
      state = digitalRead(OUTPIN_SSR);
      if(iotMode==1 && mqttClient.connected()){
        toPub=1;        
        Debugln("DEBUG: toPub set to 1");
      }
    } else if (count > (RESTARTDELAY/0.05) && count <= (RESETDELAY/0.05)){ //pressed 3 secs (60*0.05s)
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


////*---------------------------------------------------------------------*gpio13/
if(switch_status3==(digitalRead(SWITCH_INPIN3)))// to read the status of physical switch
   {
        // send_status=0;
   }
   else
  {
    state_dimmer_mode=0;
    switch_status3=(digitalRead(SWITCH_INPIN3));
     send_status_13=1;
   }
if(send_status_13==1)
  {
     send_status_13=0;
     toPub = 1;   
  }
  else
  {   
     toPub = 0;
  }

if(!state_dimmer_mode)
{
  detachInterrupt(AC_ZERO_CROSS);
  delay(7);
if(((state_13)&&(switch_status3))||((!state_13)&&(!switch_status3)))  //exor logic
      {
      //digitalWrite(OUTLED, HIGH);
      digitalWrite(OUTPIN_TRIAC, HIGH);
     // toPub = 1;
       state="Light is ON";
      //Serial.print("Light switched via web request to  ");      
      //Serial.println(digitalWrite(OUTPIN, HIGH));      
      }
      else
      {
      digitalWrite(OUTPIN_TRIAC, LOW);
      //toPub = 1;
       state="Light is OFF";
      //Serial.print("Light switched via web request to  ");      
      //Serial.println(digitalWrite(OUTPIN, LOW)); 
      }
}
///*---------------------------------------------------------------------*gpio14/
//
if(switch_status4==(digitalRead(SWITCH_INPIN4)))// to read the status of physical switch
   {
        // send_status=0;
   }
   else
  {
    switch_status4=(digitalRead(SWITCH_INPIN4));
     send_status_14=1;
   }
if(send_status_14==1)
  {
     send_status_14=0;
     toPub = 1;   
  }
  else
  {   
     toPub = 0;
  }


if(((state_14)&&(switch_status4))||((!state_14)&&(!switch_status4)))  //exor logic
      {
      //digitalWrite(OUTLED, HIGH);
      digitalWrite(OUTPIN_SSR, HIGH);
     // toPub = 1;
       state="Light is ON";
      //Serial.print("Light switched via web request to  ");      
      //Serial.println(digitalWrite(OUTPIN, HIGH));      
      }
      else
      {
      digitalWrite(OUTPIN_SSR, LOW);
      //toPub = 1;
       state="Light is OFF";
      //Serial.print("Light switched via web request to  ");      
      //Serial.println(digitalWrite(OUTPIN, LOW)); 
      }
/*---------------------------------------------------------------------*/

  
  //Debugln("DEBUG: loop() begin");
  if(configToClear==1){
    //Debugln("DEBUG: loop() clear config flag set!");
    clearConfig()? Serial.println("Config cleared!") : Serial.println("Config could not be cleared");
    delay(1000);
    ESP.reset();
  }
  //Debugln("DEBUG: config reset check passed");  
  if (WiFi.status() == WL_CONNECTED && otaFlag){
    if(otaCount<=1) {
      Serial.println("OTA mode time out. Reset!"); 
      setOtaFlag(0);
      ESP.reset();
      delay(100);
    }
    server.handleClient();
    delay(1);
  } else if (WiFi.status() == WL_CONNECTED || webtypeGlob == 1){
    //Debugln("DEBUG: loop() wifi connected & webServer ");
    if (iotMode==0 || webtypeGlob == 1){
      //Debugln("DEBUG: loop() Web mode requesthandling ");
      server.handleClient();
      delay(1);
       if(esid != "" && WiFi.status() != WL_CONNECTED) //wifi reconnect part
      {
        Scan_Wifi_Networks();
      }
    } else if (iotMode==1 && webtypeGlob != 1 && otaFlag !=1){
          //Debugln("DEBUG: loop() MQTT mode requesthandling ");
          if (!connectMQTT()){
              delay(200);          
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
