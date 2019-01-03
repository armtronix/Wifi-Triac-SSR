
// -- BEGIN mainpage.html
static const char PROGMEM INDEX_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name = "viewport" content = "width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0">
<title>ARMtronix Technologies LLP</title>
<style>
body { background: #ffffff repeat; font-family: Arial, Helvetica, Sans-Serif; Color:  #ff0000; }
#container { width: 80%; max-width: 450px; margin: auto; }
.bulb { display: block; clear: none; width: 32px; height: 32px; padding-bottom: 0.5em; background-attachment: fixed; background-position: center; background-repeat: no-repeat; }
.bulbOn { background: transparent url('http://sasakaranovic.com/iotdimmer/bulbON.png') top left no-repeat; float: right; }
.bulbOff{ background: transparent url('http://sasakaranovic.com/iotdimmer/bulbOFF.png') top left no-repeat; float: left; }
.image {font-size: 0; text-align: center; width: 450px;  height: 150px;}
img {display: inline-block; vertical-align: middle; max-height: 100%; max-width: 100%;}
.trick {display: inline-block; vertical-align: middle; height: 150px;}

h1 {  display: block; font-size: 2em; margin-top: 0.67em; margin-bottom: 0.67em; margin-left: 0; margin-right: 0; font-weight: bold; text-align: center; }
.slidecontainer {width: 100%; }
.slider {width: 100%; margin: 0 0 3em 0; }
  .buttonOff { background-color: #ff0000;margin-top: 0.67em; margin-bottom: 0.67em }
  .buttonOn{ background-color: #4CAF50;margin-top: 0.67em; margin-bottom: 0.67em }
  a { 
  background-color: #212121;
    border: none;
    color: white;
    padding: 15px 32px;
  margin-right: 1em;
    text-align: center;
    text-decoration: none;
    display: inline-block;
    font-size: 16px;
  border-radius: 4px;
  }
</style>
<script>
var websock;
function start() {
websock = new WebSocket('ws://' + window.location.hostname + ':81/');
websock.onopen = function(evt) { console.log('websock open'); };
websock.onclose = function(evt) { console.log('websock close'); };
websock.onerror = function(evt) { console.log(evt); };
websock.onmessage = function(evt) {
console.log(evt);
var ch1 = document.getElementById('ch1');
var ch2 = document.getElementById('ch2');
var ch3 = document.getElementById('ch3');
var ch_values = JSON.parse(evt.data);
ch1.value = ch_values.ch1;
ch2.value = ch_values.ch2;
ch3.value = ch_values.ch3;
};
}
function setCh1(val) {
var dat = 'ch1:'+ val
websock.send(dat);
}
function setCh2(val) {
var dat = 'ch2:'+ val
websock.send(dat);
}
function updateSlider(e) {
var dat = e.id +':'+ e.value
websock.send(dat);
}
</script>
</head>
<body onload="javascript:start();">
<div id="container">
<div class="image">
<div class="trick"></div>
  <img src='http://armtronix.in/img/logo.png'/>
</div>
<h1>Wifi Dimmer & ON/OFF Board</h1>
<div class="slidecontainer">
<div class="bulb bulbOn"></div>
<div class="bulb bulbOff"></div>
<input id="ch1" type="range" min="0" max="255" value="0" class="slider" onchange="updateSlider(this)">
</div>
<div class="slidecontainer"style="display:none; visibility: hidden">
<div class="bulb bulbOn"></div>
<div class="bulb bulbOff"></div>
<input id="ch2" type="range" min="1" max="255" value="0" class="slider" onchange="updateSlider(this)">
</div>
<div class="slidecontainer" style="display:none; visibility: hidden">
<div class="bulb bulbOn"></div>
<div class="bulb bulbOff"></div>
<input id="ch3" type="range" min="1" max="255" value="0" class="slider" onchange="updateSlider(this)">
</div>
<div class="slidecontainer" style="text-align: center">
<a href="#" class="buttonOff" onClick="setCh1(0)">Turn Off</a> <a href="#" onClick="setCh1(90)">Set to 50%</a> <a href="#" class="buttonOn"  onClick="setCh1(200)">Turn On</a>
</div>
<div class="slidecontainer" style="text-align: center">
<a href="#" class="buttonOff" onClick="setCh2(0)">Turn Off SSR</a> <a href="#" class="buttonOn"  onClick="setCh2(100)">Turn On SSR</a>
</div>
</div>
</body>
</html>
)rawliteral";
// ------------------------------------ END Index.html -----------------------------------------------------------------------------------------------------------------------------------
//url('https://litumiot.com/wp-content/uploads/2016/10/page-center-background-image-iot-2.jpg')

// ------------------------------------  MAIN.html -----------------------------------------------------------------------------------------------------------------------------------

static const char PROGMEM MAIN_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name = "viewport" content = "width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0">
<title>ARMtronix Technologies LLP</title>
<style>
body { background: #ffffff  repeat; font-family: Arial, Helvetica, Sans-Serif; Color:  #f44336; }
#container { width: 80%; max-width: 450px; margin: auto; }
.image {font-size: 0; text-align: center; width: 450px;  height: 150px;}
img {display: inline-block; vertical-align: middle; max-height: 100%; max-width: 100%;}
.trick {display: inline-block; vertical-align: middle; height: 150px;}
h1 {  display: block; font-size: 2em; margin-top: 0.67em; margin-bottom: 0.67em; margin-left: 0; margin-right: 0; font-weight: bold; text-align: center; color: #f44336; }
  a { 
  background-color: #ff0000;
    border: none;
    color: white;
    padding: 15px 32px;
  margin-top: 0.67em; 
  margin-bottom: 0.67em;
  margin-right: 0em;
  margin-left: 0em; 
    text-align: center;
    text-decoration: none;
    display: block;
    font-size: 18px;
    font-weight: bold;
  border-radius: 4px;
  }
</style>
</head>
<body>
<div id="container">
<div class="image">
<div class="trick"></div>
  <img src='http://armtronix.in/img/logo.png'/>
</div>
<h1>Wifi Dimmer & ON/OFF Board</h1>
<a href=\gpio>Control Outputs</a><br />
<a href=\cleareeprom>Clear settings an boot into Config mode</a><br />
</div>
</body>
</html>
)rawliteral";
// ------------------------------------ END MAIN.html -----------------------------------------------------------------------------------------------------------------------------------

static const char PROGMEM CONFIG_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name = "viewport" content = "width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0">
<title>ARMtronix Technologies LLP</title>
</head>
<body>
<div id="container">
<h1>Wifi Dimmer & ON/OFF Board</h1>
<form method='get' action='a'>
<label>SSID: </label><input type="text" name='ssid' length=32><label> Pass: </label><input name='pass' type='password' length=64></br>
<label>IOT Mode: </label><input type='radio' name='iot' value='0'>HTTP <input type='radio' name='iot' value='1' checked> MQTT</br>
<label>MQTT Broker IP/DNS: </label><input type="text" name='host' length=15></br>
<label>MQTT Publish topic: </label><input type="text" name='pubtop' length=64></br>
<label>MQTT Subscribe topic: </label><input type="text" name='subtop' length=64></br>
        <div id="formbox">
<input type='submit'>
        </div>
</form>
</div>
</body>
</html>
)rawliteral";















void initWiFi(){
  Serial.println();
  Serial.println();
  Serial.println("Startup");
  
  // test esid 
  WiFi.disconnect();
  delay(100);
  WiFi.mode(WIFI_STA);
  Serial.print("Connecting to WiFi ");
  Serial.println(esid);
  Debugln(epass);
  WiFi.begin((char*)esid.c_str(), (char*)epass.c_str());
  if ( testWifi() == 20 ) { 
      launchWeb(0);
      return;
  }
  Serial.println("Opening AP");
  setupAP();   
}

int testWifi(void) {
  int c = 0;
  Debugln("Wifi test...");  
  while ( c < 30 ) {
    if (WiFi.status() == WL_CONNECTED) { 
      attachInterrupt(AC_ZERO_CROSS, zcDetectISR, RISING);    // Attach an Interupt to Pin 2 (interupt 0) for Zero Cross Detection
      hw_timer_init(NMI_SOURCE, 0);
      hw_timer_set_func(dimTimerISR);
  
      return(20); } 
    delay(500);
    Serial.print(".");    
    c++;
  }
  Serial.println("WiFi Connect timed out!");
  return(10);
} 


void setupAP(void) {
  
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0){
    Serial.println("no networks found");
    st ="<b>No networks found:</b>";
  } else {
    Serial.print(n);
    Serial.println(" Networks found");
    st = "<ul>";
    for (int i = 0; i < n; ++i)
     {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" (OPEN)":"*");
      
      // Print to web SSID and RSSI for each network found
      st += "<li>";
      st +=i + 1;
      st += ": ";
      st += WiFi.SSID(i);
      st += " (";
      st += WiFi.RSSI(i);
      st += ")";
      st += (WiFi.encryptionType(i) == ENC_TYPE_NONE)?" (OPEN)":"*";
      st += "</li>";
      //delay(10);
     }
    st += "</ul>";
  }
  Serial.println(""); 
  WiFi.disconnect();
  delay(100);
  WiFi.mode(WIFI_AP);

  
  WiFi.softAP(host);
  WiFi.begin(host); // not sure if need but works
  Serial.print("Access point started with name ");
  Serial.println(host);
  inApMode=1;
  launchWeb(1);
}

void launchWeb(int webtype) {
    Serial.println("");
    Serial.println("WiFi connected");    
    //Start the web server or MQTT
    if(otaFlag==1 && !inApMode){
      Serial.println("Starting OTA mode.");    
      Serial.printf("Sketch size: %u\n", ESP.getSketchSize());
      Serial.printf("Free size: %u\n", ESP.getFreeSketchSpace());
      MDNS.begin(host);
      server.on("/", HTTP_GET, [](){
        server.sendHeader("Connection", "close");
        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.send(200, "text/html", otaServerIndex);
      });
      server.on("/update", HTTP_POST, [](){
        server.sendHeader("Connection", "close");
        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.send(200, "text/plain", (Update.hasError())?"FAIL":"OK");
        setOtaFlag(0); 
        ESP.restart();
      },[](){
        HTTPUpload& upload = server.upload();
        if(upload.status == UPLOAD_FILE_START){
          //Serial.setDebugOutput(true);
          WiFiUDP::stopAll();
          Serial.printf("Update: %s\n", upload.filename.c_str());
          otaCount=300;
          uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
          if(!Update.begin(maxSketchSpace)){//start with max available size
            Update.printError(Serial);
          }
        } else if(upload.status == UPLOAD_FILE_WRITE){
          if(Update.write(upload.buf, upload.currentSize) != upload.currentSize){
            Update.printError(Serial);
          }
        } else if(upload.status == UPLOAD_FILE_END){
          if(Update.end(true)){ //true to set the size to the current progress
            Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
          } else {
            Update.printError(Serial);
          }
          Serial.setDebugOutput(false);
        }
        yield();
      });
      server.begin();
      
      Serial.printf("Ready! Open http://%s.local in your browser\n", host);
      MDNS.addService("http", "tcp", 80);
      MDNS.addService("ws", "tcp", 81);  //added by naren on 02/01/2019
//      otaTickLoop.attach(1, otaCountown);
    } else { 
      //setOtaFlag(1); 
      if (webtype==1 || iotMode==0){ //in config mode or WebControle
          if (webtype==1) {           
            webtypeGlob == 1;
            Serial.println(WiFi.softAPIP());
            server.on("/", webHandleConfig);
            server.on("/a", webHandleConfigSave);
            server.on("/gpio", webHandleGpio);//naren          
          } else {
             webSocket.begin();  //added by naren on 02/01/2019
             webSocket.onEvent(webSocketEvent);  //added by naren on 02/01/2019
             Serial.println("setting up MDNS responder now!");
            //setup DNS since we are a client in WiFi net
            if (!MDNS.begin(host)) {
              Serial.println("Error setting up MDNS responder!");
            } else {
              Serial.println("mDNS responder started");
              MDNS.addService("http", "tcp", 80);
            }          
            Serial.println(WiFi.localIP());
            server.on("/", webHandleRoot);  
            server.on("/cleareeprom", webHandleClearRom);
            server.on("/gpio", webHandleGpio);
          }
          //server.onNotFound(webHandleRoot);
          server.begin();
          Serial.println("Web server started");   
          webtypeGlob=webtype; //Store global to use in loop()
        } else if(webtype!=1 && iotMode==1){ // in MQTT and not in config mode     
          //mqttClient.setBrokerDomain((char*) mqttServer.c_str());//naren
          //mqttClient.setPort(1883);//naren
          mqttClient.setServer((char*) mqttServer.c_str(),1883);
          mqttClient.setCallback(mqtt_arrived);
          mqttClient.setClient(wifiClient);
          if (WiFi.status() == WL_CONNECTED){
            if (!connectMQTT()){
               // delay(2000);
                if (!connectMQTT()){                            
                  Serial.println("Could not connect MQTT.");
                  Serial.println("Starting web server instead.");
                  iotMode=0;
                  launchWeb(0);
                  webtypeGlob=webtype;
                }
              }                    
            }
      }
    }
}

void webHandleConfig(){
  IPAddress ip = WiFi.softAPIP();
  String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
  String s;
 
  s = "Configuration of " + hostName + " at ";
  s += ipStr;
  s += "<p><a href=\"/gpio\">Control GPIO</a><br />";
  s += st;
  s += "<form method='get' action='a'>";
  s += "<label>SSID: </label><input name='ssid' length=32><label> Pass: </label><input name='pass' type='password' length=64></br>";
  s += "The following is not ready yet!</br>";
  s += "<label>IOT mode: </label><input type='radio' name='iot' value='0'> HTTP<input type='radio' name='iot' value='1' checked> MQTT</br>";
  s += "<label>MQTT Broker IP/DNS: </label><input name='host' length=15></br>";
  s += "<label>MQTT Publish topic: </label><input name='pubtop' length=64></br>";
  s += "<label>MQTT Subscribe topic: </label><input name='subtop' length=64></br>";
  s += "<input type='submit'></form></p>";
  s += "\r\n\r\n";
  Serial.println("Sending 200");  
 // server.send(200, "text/html", s); 
  server.send_P(200, "text/html", CONFIG_HTML);
}

void webHandleConfigSave(){
  // /a?ssid=blahhhh&pass=poooo
  String s;
  s = "<p>Settings saved to eeprom and reset to boot into new settings</p>\r\n\r\n";
  server.send(200, "text/html", s); 
  Serial.println("clearing EEPROM.");
  clearConfig();
  String qsid; 
  qsid = server.arg("ssid");
  qsid.replace("%2F","/");
  Serial.println("Got SSID: " + qsid);
  esid = (char*) qsid.c_str();
  
  String qpass;
  qpass = server.arg("pass");
  qpass.replace("%2F","/");
  Serial.println("Got pass: " + qpass);
  epass = (char*) qpass.c_str();

  String qiot;
  qiot= server.arg("iot");
  Serial.println("Got iot mode: " + qiot);
  qiot=="0"? iotMode = 0 : iotMode = 1 ;
  
  String qsubTop;
  qsubTop = server.arg("subtop");
  qsubTop.replace("%2F","/");
  Serial.println("Got subtop: " + qsubTop);
  subTopic = (char*) qsubTop.c_str();
  
  String qpubTop;
  qpubTop = server.arg("pubtop");
  qpubTop.replace("%2F","/");
  Serial.println("Got pubtop: " + qpubTop);
  pubTopic = (char*) qpubTop.c_str();
  
  mqttServer = (char*) server.arg("host").c_str();
  Serial.print("Got mqtt Server: ");
  Serial.println(mqttServer);

  Serial.print("Settings written ");
  saveConfig()? Serial.println("sucessfully.") : Serial.println("not succesfully!");;
  Serial.println("Restarting!"); 
  //delay(1000);
  ESP.reset();
}

void webHandleRoot(){
  String s;
  s = "<p>Hello from ESP8266";
  s += "</p>";
  s += "<a href=\"/gpio\">Controle GPIO</a><br />";
  s += "<a href=\"/cleareeprom\">Clear settings an boot into Config mode</a><br />";
  s += "\r\n\r\n";
  //Serial.println("Sending 200");  
  //server.send(200, "text/html", s); 
  server.send_P(200, "text/html",MAIN_HTML);
}

void webHandleClearRom(){
  String s;
  s = "<p>Clearing the config and reset to configure new wifi<p>";
  s += "</html>\r\n\r\n";
  Serial.println("Sending 200"); 
  server.send(200, "text/html", s); 
  Serial.println("clearing config");
  clearConfig();
  delay(10);
  Serial.println("Done, restarting!");
  ESP.wdtDisable();
  ESP.restart();
}


void webHandleGpio(){
  String s;
   // Set GPIO according to the request
    if (server.arg("state_sw")=="1" || server.arg("state_sw")=="0" ) {
      int state_sw = server.arg("state_sw").toInt();
      if(state_sw ==1)
      {
        tarBrightness =100;
      }
      else if (state_sw ==0)
      {
         tarBrightness =0;
      }
      Serial.print("Light switched via web request to  ");      
      Serial.println(state_sw);      
    }
     if (server.arg("state_led")=="1" || server.arg("state_led")=="0" ) {
      int state_led = server.arg("state_led").toInt();
      digitalWrite(OUTPIN_SSR, state_led);
      Serial.print("Light switched via web request to  ");      
      Serial.println(state_led);      
    }
   if (server.arg("state_dimmer") !="") {
      attachInterrupt(AC_ZERO_CROSS, zcDetectISR, RISING); 
      int state_dimmer = server.arg("state_dimmer").toInt();
      tarBrightness =server.arg("state_dimmer").toInt();
      //digitalWrite(BUILTIN_LED, state_led);
     // Serial.print("Light switched via web request to  ");      
      Serial.println(state_dimmer);   
    }
     if(server.arg("reboot")=="1")
    {
     ESP.reset(); 
    }
     
//    s = "TRIAC is now ";
//    s += (digitalRead(OUTPIN_TRIAC))?"ON":"OFF";
//    s += "<p>Change to <form action='gpio'><input type='radio' name='state_sw' value='1' ";
//    s += (digitalRead(OUTPIN_TRIAC))?"checked":"";
//    s += ">TRIAC_ON<input type='radio' name='state_sw' value='0' ";
//    s += (digitalRead(OUTPIN_TRIAC))?"":"checked";
//    s += ">TRIAC_OFF <input type='submit' value='Submit'></form></p>";  
//
//    s += "SSR is now ";
//    s += (digitalRead(OUTPIN_SSR))?"ON":"OFF";
//    s += "<p>Change to <form action='gpio'><input type='radio' name='state_led' value='1' ";
//    s += (digitalRead(OUTPIN_SSR))?"checked":"";
//    s += ">SSR_ON <input type='radio' name='state_led' value='0' ";
//    s += (digitalRead(OUTPIN_SSR))?"":"checked";
//    s += ">SSR_OFF <input type='submit' value='Submit'></form></p>"; 
//    
//    s += "<p>Change to <form action='gpio' name='state' method='GET' oninput='showValue(state_dimmer.value)'><input type='range' name='state_dimmer' id='state_dimmer' min='0' max='90' step='5' value='0'  >" ; 
//    s += "</form></p>";
//    s += "<script type='text/javascript'>";
//    s += "function showValue(dimming)";
//    s += "{  document.querySelector('#state_dimmer').value=dimming ;";
//    s += "   document.forms['state'].submit();";
//    s += "}";
//    s += "</script>";
//    s +="<p><a href=\"gpio?reboot=1\">Reboot</a></p>";
    //server.send(200, "text/html", "<html><head><script>var connection = new WebSocket('ws://'+location.hostname+':81/', ['arduino']);connection.onopen = function () {  connection.send('Connect ' + new Date()); }; connection.onerror = function (error) {    console.log('WebSocket Error ', error);};connection.onmessage = function (e) {  console.log('Server: ', e.data);};function sendRGB() {  var r = parseInt(document.getElementById('r').value).toString(16);  var g = parseInt(document.getElementById('g').value).toString(16);  var b = parseInt(document.getElementById('b').value).toString(16);  if(r.length < 2) { r = '0' + r; }   if(g.length < 2) { g = '0' + g; }   if(b.length < 2) { b = '0' + b; }   var rgb = '#'+r+g+b;    console.log('RGB: ' + rgb); connection.send(rgb); }</script></head><body>LED Control:<br/><br/>R: <input id=\"r\" type=\"range\" min=\"0\" max=\"255\" step=\"1\" oninput=\"sendRGB();\" /><br/>G: <input id=\"g\" type=\"range\" min=\"0\" max=\"255\" step=\"1\" oninput=\"sendRGB();\" /><br/>B: <input id=\"b\" type=\"range\" min=\"0\" max=\"255\" step=\"1\" oninput=\"sendRGB();\" /><br/></body></html>");
    //server.send(200, "text/html", s);
     server.send_P(200, "text/html", INDEX_HTML);    
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
 
    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED: {
            IPAddress ip = webSocket.remoteIP(num);
            //Serial.println("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
 
            // send message to client
            webSocket.sendTXT(num, "Connected");
        }
            break;
        case WStype_TEXT:
            Serial.printf("[%u] get Text: %s\n", num, payload);
             Serial.write((const char *)payload);
            if(payload[2]=='0')
            {
              //Serial.println("%s",payload);
             // tarBrightness =payload;
            }
            if(payload[2]=='1')
            {   String dum1=(const char *)payload;
                //Serial.write((const char *)payload);
                Serial.println(dum1.substring(4));
                tarBrightness =dum1.substring(4).toInt();
            }
           if(payload[2]=='2')
            {
              String dum2=(const char *)payload;
              if(dum2.substring(4).toInt()==100)
              digitalWrite(OUTPIN_SSR, HIGH);
              else if(dum2.substring(4).toInt()==0)
              digitalWrite(OUTPIN_SSR, LOW);
            }
            break;
    }
 
}

