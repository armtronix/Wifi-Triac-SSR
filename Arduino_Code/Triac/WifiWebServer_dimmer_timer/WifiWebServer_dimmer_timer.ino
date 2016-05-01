

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Arduino.h>

const char *ssid = "Your SSID";
const char *password = "Your Passwrd";

ESP8266WebServer server ( 80 );

volatile int i=0;
volatile int dimming =0;  
volatile boolean zero_cross=0;
int AC_LOAD = 13;    // Output to Opto Triac pin
int AC_LOAD_SSR = 14;    // Output to SSR pin
int AC_ZERO_CROSS = 12;    // Output to Opto Triac pin   

int freqStep = 375;//75*5 as prescalar is 16 for 80MHZ

void handleRoot() {
   String s;
  s = "ARMTRONIX";
  s += "<p><a href=\"/gpio\">Control GPIO</a><br />";
  server.send ( 200, "text/html", s );
  
}

void handleNotFound() {
  
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }

  server.send ( 404, "text/plain", message );
  
}

void setup ( void ) {
  
  pinMode(AC_LOAD_SSR, OUTPUT);
  pinMode(AC_ZERO_CROSS, INPUT);
  pinMode(AC_LOAD, OUTPUT);
  Serial.begin ( 115200 );
  WiFi.begin ( ssid, password );
  Serial.println ( "" );
  //InitInterrupt(do_something);
  // Wait for connection
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  Serial.println ( "" );
  Serial.print ( "Connected to " );
  Serial.println ( ssid );
  Serial.print ( "IP address: " );
  Serial.println ( WiFi.localIP() );

  if ( MDNS.begin ( "esp8266" ) ) {
    Serial.println ( "MDNS responder started" );
  }

  server.on ( "/", handleRoot );
  server.on("/gpio", webHandleGpio);
 // server.on ( "/test.svg", drawGraph );
//  server.on ( "/inline", []() {
 //   server.send ( 200, "text/plain", "this works as well" );
//  } );
  server.onNotFound ( handleNotFound );
  server.begin();
  Serial.println ( "HTTP server started" );
}

void loop ( void ) {
  server.handleClient();
}


void InitInterrupt(timercallback handler,int Step )
    { 
        timer1_disable();
        timer1_isr_init();
        timer1_attachInterrupt(handler);
        timer1_enable(TIM_DIV16, TIM_EDGE, TIM_LOOP);
        timer1_write(Step);//max8388607//75*5
    }

void  ICACHE_RAM_ATTR do_on_delay()
{
  //digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));
  
  if(zero_cross == true) {              
    if(i>=dimming) {  
                       
     digitalWrite(AC_LOAD, HIGH); // turn on light       
      i=0;  // reset time step counter                         
      zero_cross = false; //reset zero cross detection
      delayMicroseconds(10);         // triac On propogation delay
      digitalWrite(AC_LOAD, LOW);    // triac Off
    } 
    else {
      i++; // increment time step counter 
      digitalWrite(AC_LOAD, LOW);    // triac Off        
                 
    }                                
  }    
}


void zero_crosss_int()  // function to be fired at the zero crossing to dim the light
{
  
  zero_cross = true; 
  i=0;
  //InitInterrupt(do_something);
  digitalWrite(AC_LOAD, LOW);  
 InitInterrupt(do_on_delay,freqStep);
 
}


void webHandleGpio(){
  String s;
   // Set GPIO according to the request
    if (server.arg("state_sw")=="1" || server.arg("state_sw")=="0" ) {
      int state_sw = server.arg("state_sw").toInt();
      detachInterrupt(AC_ZERO_CROSS);
      digitalWrite(AC_LOAD, state_sw);
     
      Serial.print("Light switched via web request to  ");      
      Serial.println(state_sw);      
    }
     if (server.arg("state_led")=="1" || server.arg("state_led")=="0" ) {
      int state_led = server.arg("state_led").toInt();
      digitalWrite(AC_LOAD_SSR, state_led);
      Serial.print("Light switched via web request to  ");      
      Serial.println(state_led);      
    }
  //   if (server.arg("state_dimmer") =="10" || server.arg("state_dimmer")=="20" || server.arg("state_dimmer")=="30" || server.arg("state_dimmer")=="40" || server.arg("state_dimmer")=="90" ) {
   if (server.arg("state_dimmer") !="") {
      int state_dimmer = server.arg("state_dimmer").toInt();
      //digitalWrite(BUILTIN_LED, state_led);
     // Serial.print("Light switched via web request to  ");      
      Serial.println(state_dimmer); 
      dimming =127-state_dimmer;
      delay(10); 
      if(dimming>=120)
      {
      detachInterrupt(AC_ZERO_CROSS);
      digitalWrite(AC_LOAD, LOW);
      }
      else if(dimming<=10)
      {
      detachInterrupt(AC_ZERO_CROSS);
      digitalWrite(AC_LOAD, HIGH);
      }
      else
      {
      attachInterrupt(AC_ZERO_CROSS, zero_crosss_int, RISING); 
      }   
    }
     
    s = "TRIAC is now ";
    s += (digitalRead(AC_LOAD))?"ON":"OFF";
    s += "<p>Change to <form action='gpio'><input type='radio' name='state_sw' value='1' ";
    s += (digitalRead(AC_LOAD))?"checked":"";
    s += ">TRIAC_ON<input type='radio' name='state_sw' value='0' ";
    s += (digitalRead(AC_LOAD))?"":"checked";
    s += ">TRIAC_OFF <input type='submit' value='Submit'></form></p>";  

    s += "SSR is now ";
    s += (digitalRead(AC_LOAD_SSR))?"ON":"OFF";
    s += "<p>Change to <form action='gpio'><input type='radio' name='state_led' value='1' ";
    s += (digitalRead(AC_LOAD_SSR))?"checked":"";
    s += ">SSR_ON <input type='radio' name='state_led' value='0' ";
    s += (digitalRead(AC_LOAD_SSR))?"":"checked";
    s += ">SSR_OFF <input type='submit' value='Submit'></form></p>"; 

//    s += "LED is now ";/// enable for onboard led
//    s += (digitalRead(BUILTIN_LED))?"ON":"OFF";
//    s += "<p>Change to <form action='gpio'><input type='radio' name='state_led' value='1' ";
//    s += (digitalRead(BUILTIN_LED))?"checked":"";
//    s += ">LED_ON <input type='radio' name='state_led' value='0' ";
//    s += (digitalRead(BUILTIN_LED))?"":"checked";
//    s += ">LED_OFF <input type='submit' value='Submit'></form></p>"; 
    
    s += "<p>Change to <form name='state' action='gpio' method='get' autocomplete='on' ><input type='range' name='state_dimmer' id='dimInputId' min='0' max='90' step='10' value='0' oninput='showValue()'>" ; 
    s += "<output name='dimOutputName' id='dimOutputId'>0</output></form></p>";
    //s += "<input type='submit' value='Submit'></form></p>";//<span id='range'>0</span>
    s += "<script type='text/javascript'>";
    s += "function showValue()";//newValue
    s += "{";
    s += "   ";//document.getElementById('range').innerHTML=newValue;
    s += "   document.state.dimOutputId.value = document.state.dimInputId.value;";
    s += "   document.forms['state'].submit(); }";// document.forms['state'].submit(); 
    s += "</script>";

//    s += "<p>Change to <form name='state' action='gpio' method='get' autocomplete='on' ><input type='range' name='state_dimmer' id='dimInputId' min='0' max='90' step='10' value='0' oninput='showValue()'>" ; 
//    s += "<output name='dimOutputName' id='dimOutputId'>0</output></form></p>";
//    //s += "<input type='submit' value='Submit'></form></p>";//<span id='range'>0</span>
//    s += "<script type='text/javascript'>";
//    s += "function showValue()";//newValue
//    s += "{";
//    s += "   ";//document.getElementById('range').innerHTML=newValue;
//    s += "   document.state.dimOutputId.value = document.state.dimInputId.value;";
//    s += "   document.forms['state'].submit(); }";// document.forms['state'].submit(); 
//    s += "</script>";
    server.send(200, "text/html", s);
        

}
