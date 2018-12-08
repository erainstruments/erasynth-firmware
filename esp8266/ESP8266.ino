/*
*
* Copyright (c) 2018 ERA Instruments (http://erainstruments.com/)

* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.

* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266NetBIOS.h>
#include <FS.h>
#include <Ticker.h>

boolean isCmdExist = false;
String cmdString = "";

IPAddress ip(192, 168, 1, 100);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

String typed_ip = "192.168.1.100";
String typed_gateway = "192.168.1.1";
String typed_subnet = "255, 255, 255, 0";

String AP_ssid = "ERASynth";
String AP_password  = "erainstruments";

String STA_ssid = "ERASynth";
String STA_password = "erainstruments";

char ssid[100] = "";
char password[100] = "";

String ESP8266FirmwareVersion_Str = "v1.0.4";

ESP8266WebServer server(80);

Ticker checker;
bool inProgress = false;
// STA = 0  AP = 1  STA+AP = 2
int wifiMode = 1;
bool isChecked = true;
bool isInitiated = false;
bool isSent = false;

void setup() 
{
  Serial.begin(115200);
  debugPrint("\n");
  Serial.setDebugOutput(true);
  Serial.setTimeout(100);
  SPIFFS.begin();

  serialEvent();

  debugPrintln("========================================");
  debugPrintln("ERASynth ESP8266 has started");
  debugPrintln("========================================");
  debugPrintln("Device File Content : ");

  Dir dir = SPIFFS.openDir("/");
  while (dir.next()) 
  {
    String fileName = dir.fileName();
    size_t fileSize = dir.fileSize();
    debugPrint("FS File : "); debugPrint(fileName.c_str()); debugPrint("  Size : "); debugPrintln(formatBytes(fileSize).c_str());
  }

  debugPrintln("DNS is initiated");
  if (!MDNS.begin("erasynth"))
  {
    debugPrintln("Error setting up DNS!!");
    while (1) { delay(1000); debugPrint("."); }
  }
  debugPrintln("DNS started");

  // Add service to MDNS-SD
  MDNS.addService("http", "tcp", 80);
  NBNS.begin("ERASYNTH");

  serverInit();
  debugPrintln("HTTP server started");

  // Start in AP Mode
  setWifi();
  
  // Set timer to check connection
  checker.attach(10, checkConn);
}

void loop() 
{
  server.handleClient();
  serialEvent();
  
  if((wifiMode == 0 || wifiMode == 2) && !isChecked && WiFi.status() != WL_CONNECTED)
  { 
    int counter = 0;
    STA_ssid.toCharArray(ssid, STA_ssid.length() + 1);
    STA_password.toCharArray(password, STA_password.length() + 1);

    delay(150);

    int numberOfNetworks = WiFi.scanNetworks();
    bool isNetworkFound = false;
  
    if (numberOfNetworks == 0) 
    { 
      debugPrintln("No networks found!!! Nothing to connect"); 
    }
    else
    {
      debugPrintln("Listing available networks...");
      for (int i = 0; i < numberOfNetworks; i++) 
      { 
        debugPrint(String(i) + "-)");
        debugPrint(String(WiFi.SSID(i).c_str()));
        debugPrint("   CH : ");
        debugPrint(String(WiFi.channel(i)));
        debugPrintln("  (" + String(WiFi.RSSI(i)) + "dBm)");
        if (STA_ssid == WiFi.SSID(i))
        { 
          isNetworkFound = true; debugPrintln("We have a match... Starting the connection"); 
        } 
      }      
    }

    if(isNetworkFound)
    {
      wifiMode = 0; 
      setWifi();
      
//      debugPrintln("Connecting to " + String(ssid));
//      //if (String(WiFi.SSID()) != String(ssid))  {  }
//      WiFi.begin(ssid, password);
//      
//      while (WiFi.status() != WL_CONNECTED) 
//      {
//        delay(500);
//        debugPrint(".");
//        if(++counter){ break; }
//      }
//      
//      delay(100);
//      
//      debugPrint("Connected! IP address: ");  debugPrintln(String(WiFi.localIP()));
//      debugPrint("Server MAC address: ");     debugPrintln(String(WiFi.macAddress()));
//      debugPrint("Signal strength (RSSI): "); debugPrint(String(WiFi.RSSI()));
//      debugPrintln(" dBm");
      //serverInit(); 
    }
   isChecked = true;
  }
}

void setWifi()
{
  int counter = 0;
  inProgress = true;
  debugPrintln("Wifi Connection is initilazing...");

  // 
  // In AP Mode Output power must be max 10 dBm.
  //
  if(wifiMode == 1){ WiFi.setOutputPower(10); }
  else { WiFi.setOutputPower(20.5); }
  
  WiFi.disconnect();
  WiFi.softAPdisconnect();
  
  if(wifiMode == 0) 
  {
    WiFi.mode(WIFI_STA);
    debugPrintln("Station mode is running...");
    
    STA_ssid.toCharArray(ssid, STA_ssid.length() + 1);
    STA_password.toCharArray(password, STA_password.length() + 1);

    debugPrint("SSID: ");     debugPrintln(ssid);
    debugPrint("Password: "); debugPrintln(password);
    debugPrint("IP: ");       debugPrintln(String(ip));
    debugPrint("Gateway: ");  debugPrintln(String(gateway));
    debugPrint("Subnet: ");   debugPrintln(String(subnet));

    WiFi.config(ip, gateway, subnet);
    delay(150);

    int numberOfNetworks = WiFi.scanNetworks();
    bool isNetworkFound = false;
  
    if (numberOfNetworks == 0) 
    { 
      debugPrintln("No networks found!!! Nothing to connect"); 
    }
    else
    {
      debugPrintln("Listing available networks...");
      for (int i = 0; i < numberOfNetworks; i++) 
      { 
        debugPrint(String(i) + "-)");
        debugPrint(String(WiFi.SSID(i).c_str()));
        debugPrint("   CH : ");
        debugPrint(String(WiFi.channel(i)));
        debugPrintln("  (" + String(WiFi.RSSI(i)) + "dBm)");
        if (STA_ssid == WiFi.SSID(i)) { isNetworkFound = true; debugPrintln("We have a match... Starting the connection"); } 
      }      
    }

    if(isNetworkFound)
    {
      debugPrintln("Connecting to " + String(ssid));
      if (String(WiFi.SSID()) != String(ssid))  { WiFi.begin(ssid, password); }
       
      while (WiFi.status() != WL_CONNECTED) 
      {
        delay(500);
        debugPrint(".");
        if (++counter == 50) 
        { 
          debugPrint(ssid); 
          debugPrint(" is available but "); 
          debugPrintln("password may be wrong");  
          debugPrintln("Access Point mode is enabling...");  
          wifiMode = 2;
          setWifi();
        }
      }
      
      delay(100);
      
      debugPrint("Connected! IP address: ");  debugPrintln(String(WiFi.localIP()));
      debugPrint("Server MAC address: ");     debugPrintln(String(WiFi.macAddress()));
      debugPrint("Signal strength (RSSI): "); debugPrint(String(WiFi.RSSI()));
      debugPrintln(" dBm");
      serverInit(); 
    }
    else
    {
      debugPrintln("No Connection for Network. Access Point mode is also enabling...");
      wifiMode = 2;
      setWifi();
    }
  
  }
  else if(wifiMode == 1)
  { 
    WiFi.mode(WIFI_AP); 
    debugPrintln("Access mode is running...");
    AP_ssid.toCharArray(ssid, AP_ssid.length() + 1);
    AP_password.toCharArray(password, AP_password.length() + 1);

    debugPrint("SSID: ");     debugPrintln(ssid);
    debugPrint("Password: "); debugPrintln(password);
    debugPrint("IP: ");       debugPrintln(String(ip));
    debugPrint("Gateway: ");  debugPrintln(String(gateway));
    debugPrint("Subnet: ");   debugPrintln(String(subnet));

    WiFi.softAPConfig(ip, gateway, subnet);
    WiFi.softAP(ssid, password);
    delay(100);
    
    debugPrint("Server IP address: ");  debugPrintln(String(WiFi.softAPIP()));
    debugPrint("Server MAC address: "); debugPrintln(String(WiFi.softAPmacAddress()));
    serverInit();
  }
  else if(wifiMode == 2)
  { 
    WiFi.mode(WIFI_AP_STA); 
    debugPrintln("Station and Access mode both is running...");

    debugPrint("IP: ");       debugPrintln(String(ip));
    debugPrint("Gateway: ");  debugPrintln(String(gateway));
    debugPrint("Subnet: ");   debugPrintln(String(subnet));

    WiFi.config(ip, gateway, subnet);
    WiFi.softAPConfig(ip, gateway, subnet);
    
    AP_ssid.toCharArray(ssid, AP_ssid.length() + 1);
    AP_password.toCharArray(password, AP_password.length() + 1);
    WiFi.softAP(ssid, password);
    
    debugPrint("AP SSID: ");     debugPrintln(ssid);
    debugPrint("AP Password: "); debugPrintln(password);
    
    delay(150);

    STA_ssid.toCharArray(ssid, STA_ssid.length() + 1);
    STA_password.toCharArray(password, STA_password.length() + 1);

    debugPrint("SSID: ");     debugPrintln(ssid);
    debugPrint("Password: "); debugPrintln(password);

    int numberOfNetworks = WiFi.scanNetworks();
    bool isNetworkFound = false;
  
    if (findSSID()) 
    {
      debugPrintln("Connecting to " + String(ssid));
      if (String(WiFi.SSID()) != String(ssid))  { wifiMode = 0; setWifi(); }
    }
        
    serverInit();
  }
  inProgress = false;
  counter = 0;
}

void checkConn()
{
  if(!inProgress) 
  { 
    checker.detach(); 
    isChecked = false;
    checker.attach(3, checkConn); 
  }
}
