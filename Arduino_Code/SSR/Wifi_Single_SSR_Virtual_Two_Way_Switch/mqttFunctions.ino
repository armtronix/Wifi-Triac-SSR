boolean connectMQTT(){
  if (mqttClient.connected()){
    return true;
  }  
  
  Serial.print("Connecting to MQTT server ");
  Serial.print(mqttServer);
  Serial.print(" as ");
  Serial.println(host);
  
  if (mqttClient.connect(host)) {
    Serial.println("Connected to MQTT broker");
    if(mqttClient.subscribe((char*)subTopic.c_str())){
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

void disconnectMQTT(){
  mqttClient.disconnect();
}

void mqtt_handler(){
  if (toPub==1){
    Debugln("DEBUG: Publishing state via MWTT");
    if(pubState()){
     toPub=0; 
    }
  }
  mqttClient.loop();
  delay(100); //let things happen in background
}

void mqtt_arrived(char* subTopic, byte* payload, unsigned int length) { // handle messages arrived 
  int i = 0;
  Serial.print("MQTT message arrived:  topic: " + String(subTopic));
    // create character buffer with ending null terminator (string)
    
  for(i=0; i<length; i++) {    
    buf[i] = payload[i];
  }
  buf[i] = '\0';
  String msgString = String(buf);
  Serial.println(" message: " + msgString);
  if ((msgString == "R13_ON"))
  {
      Serial.print("Light is ");
      Serial.println(digitalRead(OUTPIN));      
      Serial.print("Switching light to "); 
      Serial.println("high");
      if(switch_status==1)
      { 
      state_sw=0;
      }
      else
      { 
      state_sw=1;
      
      }
      send_status=1;
      
  } 
    else if ((msgString == "R13_OFF")){
      Serial.print("Light is ");
      Serial.println(digitalRead(OUTPIN));    
      Serial.print("Switching light to "); 
      Serial.println("low");
       if(switch_status==0)
      { 
      state_sw=0;
      }
      else
      { 
      state_sw=1;   
      }
      send_status=1;
      send_status=1;
      //digitalWrite(OUTPIN, 0); 
  }
  if (msgString == "Led_on"){
      Serial.print("Led is ");
      Serial.println(digitalRead(OUTLED));      
      Serial.print("Switching LED to "); 
      Serial.println("high");
      digitalWrite(OUTLED, 1); 
  } else if (msgString == "Led_off"){
      Serial.print("Led is ");
      Serial.println(digitalRead(OUTLED));    
      Serial.print("Switching LED to "); 
      Serial.println("low");
      digitalWrite(OUTLED, 0); 
  } 
  else if (msgString == "Status")
  {
       send_status=1;
  }
  else if (msgString == "Sensor_Enable"){
      Serial.print("Sensor Enabled");
      attachInterrupt(PIR_INPIN, pir_sensor_int, CHANGE);       
  } 
   else if (msgString == "Sensor_Disable"){
      Serial.print("Sensor Disable");
      detachInterrupt(PIR_INPIN);    
  }        
}

boolean pubState(){ //Publish the current state of the light    
  if (!connectMQTT()){
      delay(100);
      if (!connectMQTT){                            
        Serial.println("Could not connect MQTT.");
        Serial.println("Publish state NOK");
        return false;
      }
    }
    if (mqttClient.connected()){      
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
     delay(10);   
}
