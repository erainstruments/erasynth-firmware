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

void serverInit()
{
	server.on("/", HTTP_POST, handleCommand);

	// Called when the url is not defined here
	// Use it to load content from SPIFFS
	server.onNotFound([]() { if (!handleFileRead(server.uri())) { server.send(404, "text/plain", "FileNotFound"); } });
	
	// Start the server
	server.begin();
}

void serialEvent()
{
    char rx_byte = 0;
  while (Serial.available() > 0) 
  {
    rx_byte = Serial.read();

    if (rx_byte == '<' || isCmdExist) 
    {
      isCmdExist = true;
      if (rx_byte != '\r') cmdString += rx_byte;
    }
    
    if (rx_byte == '\r' && isCmdExist) 
    {
      command(cmdString);
      cmdString = "";
      isCmdExist = false;
    }
  }
}


bool findSSID()
{
	int numberOfNetworks = WiFi.scanNetworks();
	bool isNetworkFound = false;
	
	if (numberOfNetworks == 0)
	{
		debugPrintln("No Networks Found");
	}
	else
	{
		for (int i = 0; i < numberOfNetworks; i++) { if (STA_ssid == WiFi.SSID(i)) { isNetworkFound = true; } }
	}
	return isNetworkFound;
}

bool handleFileRead(String path) 
{
	debugPrintln("handleFileRead: " + path);
	if (path.endsWith("/")) { path += "index.html"; }

	String contentType = getContentType(path);
	String pathWithGz = path + ".gz";
	
	if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) 
	{
		if (SPIFFS.exists(pathWithGz)) { path += ".gz"; }
			
		File file = SPIFFS.open(path, "r");
		debugPrintln("!!!Stream has started: " + path);
		size_t sent = server.streamFile(file, contentType);
		file.close();
		debugPrintln("!!!Stream has Finished!!!");
		return true;
	}
	return false;
}

void handleFileList() 
{
	if (!server.hasArg("dir")) { server.send(500, "text/plain", "BAD ARGS"); return; }

	String path = server.arg("dir");
	debugPrintln("handleFileList: " + path);
	Dir dir = SPIFFS.openDir(path);
	path = String();

	String output = "[";
	while (dir.next()) 
	{
		File entry = dir.openFile("r");
		if (output != "[") output += ',';
		bool isDir = false;
		output += "{\"type\":\"";
		output += (isDir) ? "dir" : "file";
		output += "\",\"name\":\"";
		output += String(entry.name()).substring(1);
		output += "\"}";
		entry.close();
	}

	output += "]";
	server.send(200, "text/json", output);
}

String getContentType(String filename)
{
	if (server.hasArg("download")) return "application/octet-stream";
	else if (filename.endsWith(".htm")) return "text/html";
	else if (filename.endsWith(".html")) return "text/html";
	else if (filename.endsWith(".css")) return "text/css";
	else if (filename.endsWith(".js")) return "application/javascript";
	else if (filename.endsWith(".png")) return "image/png";
	else if (filename.endsWith(".gif")) return "image/gif";
	else if (filename.endsWith(".jpg")) return "image/jpeg";
	else if (filename.endsWith(".ico")) return "image/x-icon";
	else if (filename.endsWith(".xml")) return "text/xml";
	else if (filename.endsWith(".pdf")) return "application/x-pdf";
	else if (filename.endsWith(".zip")) return "application/x-zip";
	else if (filename.endsWith(".gz")) return "application/x-gzip";
	return "text/plain";
}

String formatBytes(size_t bytes)
{
  // Format bytes
  if (bytes < 1024) { return String(bytes) + "B"; }
  else if (bytes < (1024 * 1024)) { return String(bytes / 1024.0) + "KB"; }
  else if (bytes < (1024 * 1024 * 1024)) { return String(bytes / 1024.0 / 1024.0) + "MB"; }
  else { return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB"; }
}

void debugPrint(String input)
{
  if(isInitiated){ Serial.print(input); }
}

void debugPrintln(String input)
{
  if(isInitiated){ Serial.println(input); }
}
