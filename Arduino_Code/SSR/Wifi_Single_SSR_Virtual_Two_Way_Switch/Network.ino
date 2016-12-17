void Scan_Wifi_Networks()
{

  int n = WiFi.scanNetworks();


  if (n == 0)
  {
   // Serial.println("no networks found");
  }
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i)
    {
      if (esid == WiFi.SSID(i))

      {
        Serial.print("Old Network Found ");
        Do_Connect();
        //Fl_MyNetwork = true;
      }
      else
      {
        Serial.print("Old Network Not Found");
      }
     
    }
  }
  Serial.println("");
}


void Do_Connect()                  // Try to connect to the Found WIFI Network!
{

ESP.wdtDisable();
  ESP.restart(); 
  
}

