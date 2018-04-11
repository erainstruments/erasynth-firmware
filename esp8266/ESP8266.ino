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
#include <FS.h>

#define DBG_OUTPUT_PORT Serial

boolean isCmdExist = false;
String cmdString = "";

IPAddress ip(192, 168, 50, 53);
IPAddress gateway(192, 168, 50, 254);
IPAddress subnet(255, 255, 255, 0);

String typed_ip = "192.168.050.053";
String typed_gateway = "192, 168, 50, 254";
String typed_subnet = "255, 255, 255, 0";

String AP_ssid = "ERASynth";
String AP_password = "erainstruments";

String STA_ssid = "";
String STA_password = "";

char ssid[40] = "";
char password[40] = "";

String ESP8266FirmwareVersion_Str = "v1.0.2";

ESP8266WebServer server(80);

String formatBytes(size_t bytes)
{
	// Format bytes
	if (bytes < 1024) { return String(bytes) + "B"; }
	else if (bytes < (1024 * 1024)) { return String(bytes / 1024.0) + "KB"; }
	else if (bytes < (1024 * 1024 * 1024)) { return String(bytes / 1024.0 / 1024.0) + "MB"; }
	else { return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB"; }
}

void setup() 
{
	DBG_OUTPUT_PORT.begin(115200);
	DBG_OUTPUT_PORT.print("\n");
	DBG_OUTPUT_PORT.setDebugOutput(true);
	DBG_OUTPUT_PORT.setTimeout(100);
	SPIFFS.begin();
	delay(10);

	// Start connection in AP Mode
	command("<W1"); 


	Dir dir = SPIFFS.openDir("/");

	while (dir.next()) 
	{
		String fileName = dir.fileName();
		size_t fileSize = dir.fileSize();
		DBG_OUTPUT_PORT.printf("FS File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
	}

	DBG_OUTPUT_PORT.printf("\n");

	// Initialise server
	serverInit();
}

void loop() 
{
	server.handleClient();
	serialEvent();
}
