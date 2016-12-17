boolean connectMQTT() {
  if (mqttClient.connected()) {
    return true;
  }

  Serial.print("Connecting to MQTT server ");
  Serial.print(mqttServer);
  Serial.print(" as ");
  Serial.println(host);

  if (mqttClient.connect(host)) {
    Serial.println("Connected to MQTT broker");
    if (mqttClient.subscribe((char*)subTopic.c_str())) {
      Serial.println("Subsribed to topic.");
    } else {
      Serial.println("NOT subsribed to topic!");
    }
    return true;
  }
  else {
    Serial.println("MQTT connect failed! ");
    return false;
  }
}

void disconnectMQTT() {
  mqttClient.disconnect();
}

void mqtt_handler() {
  if (toPub == 1) {
    Debugln("DEBUG: Publishing state via MWTT");
    if (pubState()) {
      toPub = 0;
    }
  }
  mqttClient.loop();
  delay(100); //let things happen in background
}

void mqtt_arrived(char* subTopic, byte* payload, unsigned int length) { // handle messages arrived
  int i = 0;
  Serial.print("MQTT message arrived:  topic: " + String(subTopic));
  // create character buffer with ending null terminator (string)

  for (i = 0; i < length; i++) {
    buf[i] = payload[i];
  }
  buf[i] = '\0';
  String msgString = String(buf);
  Serial.println(" message: " + msgString);
//  if (msgString == "R13_ON") {
//    detachInterrupt(AC_ZERO_CROSS);
//    Serial.print("Light is ");
//    Serial.println(digitalRead(OUTPIN_TRIAC));
//    Serial.print("Switching Triac to ");
//    Serial.println("high");
//    delay(7);//to solve on off bug solved 
//    digitalWrite(OUTPIN_TRIAC, HIGH);
//  } else if (msgString == "R13_OFF") {
//    detachInterrupt(AC_ZERO_CROSS);
//    Serial.print("Light is ");
//    Serial.println(digitalRead(OUTPIN_TRIAC));
//    Serial.print("Switching Triac to ");
//    Serial.println("low");
//    delay(7);//to solve on off bug solved 
//    digitalWrite(OUTPIN_TRIAC, LOW);
//  }
//  else if (msgString == "R14_ON") {
//    Serial.print("Led is ");
//    Serial.println(digitalRead(OUTPIN_SSR));
//    Serial.print("Switching SSR to ");
//    Serial.println("high");
//    digitalWrite(OUTPIN_SSR, HIGH);
//  } else if (msgString == "R14_OFF") {
//    Serial.print("Led is ");
//    Serial.println(digitalRead(OUTPIN_SSR));
//    Serial.print("Switching SSR to ");
//    Serial.println("low");
//    digitalWrite(OUTPIN_SSR, LOW);
//  }

//---------------------------------------------------------------------------"R13_ON")
if ((msgString == "R13_ON"))
  {   
      state_dimmer_mode  = 0;
      Serial.print("Light is ");
      Serial.println(digitalRead(OUTPIN_TRIAC));      
      Serial.print("Switching light to "); 
      Serial.println("high");
      if(switch_status3==1)
      { 
      state_13=0;
      }
      else
      { 
      state_13=1;
      
      }
      send_status_13=1;
      
  } 
    else if ((msgString == "R13_OFF")){
      state_dimmer_mode  = 0;
      Serial.print("Light is ");
      Serial.println(digitalRead(OUTPIN_TRIAC));    
      Serial.print("Switching light to "); 
      Serial.println("low");
       if(switch_status3==0)
      { 
      state_13=0;
      }
      else
      { 
      state_13=1;   
      }
      send_status_13=1;
      send_status_13=1;
      //digitalWrite(OUTPIN, 0); 
  }

//-------------------------------------------------------------------------------"R14_ON")

if ((msgString == "R14_ON"))
  {
      Serial.print("Light is ");
      Serial.println(digitalRead(OUTPIN_SSR));      
      Serial.print("Switching light to "); 
      Serial.println("high");
      if(switch_status4==1)
      { 
      state_14=1;
      }
      else
      { 
      state_14=0;
      
      }
      send_status_14=0;
      
  } 
    else if ((msgString == "R14_OFF")){
      Serial.print("Light is ");
      Serial.println(digitalRead(OUTPIN_SSR));    
      Serial.print("Switching light to "); 
      Serial.println("low");
       if(switch_status4==0)
      { 
      state_14=1;
      }
      else
      { 
      state_14=0;   
      }
      send_status_14=0;
      send_status_14=0;
      //digitalWrite(OUTPIN, 0); 
    }
  else if (msgString != "")//48xxx
  { 
    //String dim_str = getValue(msgString,'8', 0);
    //String dim_val_str = getValue(msgString,'8', 1);
    String dim_val_str = msgString;
    String dim_str ="4";//debug 
    if(dim_str =="4")
    {
     int dim_val = dim_val_str.toInt();
     if(dim_val>=0 && dim_val <=100)
     {
      dimming =127-dim_val;
      delay(5); 
      if(dimming>=120)
      {
       state_dimmer_mode  = 0;
       detachInterrupt(AC_ZERO_CROSS);
       Serial.print("Light is ");
       Serial.println(digitalRead(OUTPIN_TRIAC));    
       Serial.print("Switching light to "); 
       Serial.println("low");
       if(switch_status3==0)
      { 
      state_13=0;
      }
      else
      { 
      state_13=1;   
      }
      send_status_13=1;
      send_status_13=1;
      //digitalWrite(OUTPIN, 0); 
      }
      else if(dimming<=10)
      {
        state_dimmer_mode  = 0;
        detachInterrupt(AC_ZERO_CROSS);
        Serial.print("Light is ");
        Serial.println(digitalRead(OUTPIN_TRIAC));      
        Serial.print("Switching light to "); 
        Serial.println("high");
      if(switch_status3==1)
      { 
      state_13=0;
      }
      else
      { 
      state_13=1;
      
      }
      send_status_13=1;
      }
      else
      {
        state_dimmer_mode  = 1;
        attachInterrupt(AC_ZERO_CROSS, zero_crosss_int, RISING); 
      }   
      Serial.println(dim_val);
     }
    else
     {
     }
    } 
    }
    
    
  
}

boolean pubState() { //Publish the current state of the light
  if (!connectMQTT()) {
    delay(100);
    if (!connectMQTT) {
      Serial.println("Could not connect MQTT.");
      Serial.println("Publish state NOK");
      return false;
    }
  }
  if (mqttClient.connected()) {
    //String state = (digitalRead(OUTPIN))?"1":"0";
    Serial.println("To publish state " + state );
    if (mqttClient.publish((char*)pubTopic.c_str(), (char*) state.c_str())) {
      Serial.println("Publish state OK");
      return true;
    } else {
      Serial.println("Publish state NOK");
      return false;
    }
  } else {
    Serial.println("Publish state NOK");
    Serial.println("No MQTT connection.");
  }
}

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {
    0, -1
  };
  int maxIndex = data.length() - 1;
  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

