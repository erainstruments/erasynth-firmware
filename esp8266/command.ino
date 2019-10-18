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

void command(String commandBuffer) 
{
	char commandID = (char)commandBuffer[1];
	String commandInString = "";
	uint8_t counter = 0;

	for (int i = 2; i <= commandBuffer.length() - 1; i++) { commandInString += (char)commandBuffer[i]; }

	switch (commandID) 
	{
	case 'E': debugPrintln(ESP8266FirmwareVersion_Str); break; //ESP8266 Firmware Version Read
	case 'R': debugPrintln(String(WiFi.RSSI())); break; //RSSI Read
	case 'S': 

		//Change SSID
		if (commandBuffer[2] == '0') // Station Mode SSID
		{
			commandInString.remove(0, 1);

			// Removes zeros in front of the string
			while (commandInString.indexOf('0') == 0) { commandInString.remove(0, 1); }
			STA_ssid = commandInString;
		}
		else if (commandBuffer[2] == '1') // AP Mode SSID
		{
			commandInString.remove(0, 1);

			// Removes zeros in front of the string
			while (commandInString.indexOf('0') == 0) { commandInString.remove(0, 1); }
			AP_ssid = commandInString;
		}
		break;

	case 'P': 
		
		//Change Password
		if (commandBuffer[2] == '0') 
		{
			// Station Mode Password
			commandInString.remove(0, 1);

			// Removes zeros in front of the string
			while (commandInString.indexOf('0') == 0) { commandInString.remove(0, 1); }
			STA_password = commandInString;
		}
		else if (commandBuffer[2] == '1') 
		{
			// AP Mode Password
			commandInString.remove(0, 1);

			// Removes zeros in front of the string
			while (commandInString.indexOf('0') == 0) { commandInString.remove(0, 1); }
			AP_password = commandInString;
		}
		break;

	case 'I': 
		
		//Change IP address
		typed_ip = commandInString;
		ip[0] = typed_ip.substring(0, 3).toInt();
		ip[1] = typed_ip.substring(4, 7).toInt();
		ip[2] = typed_ip.substring(8, 11).toInt();
		ip[3] = typed_ip.substring(12, 15).toInt();
		break;

	case 'G': 
		
		//Change Gateway address
		typed_gateway = commandInString;
		gateway[0] = typed_gateway.substring(0, 3).toInt();
		gateway[1] = typed_gateway.substring(4, 7).toInt();
		gateway[2] = typed_gateway.substring(8, 11).toInt();
		gateway[3] = typed_gateway.substring(12, 15).toInt();
		break;

	case 'N': 
		
		//Change Subnet address
		typed_subnet = commandInString;
		subnet[0] = typed_subnet.substring(0, 3).toInt();
		subnet[1] = typed_subnet.substring(4, 7).toInt();
		subnet[2] = typed_subnet.substring(8, 11).toInt();
		subnet[3] = typed_subnet.substring(12, 15).toInt();
		break;

	case 'W': 
		
		//Wifi Mode
		if (commandBuffer[2] == '0')  { wifiMode = 0; setWifi(); }
		else if (commandBuffer[2] == '1') { wifiMode = 1; setWifi(); }
    else if (commandBuffer[2] == '2') { wifiMode = 2; setWifi(); }

		break;

   case 'A':

    isInitiated = true;
    Serial.println(">XI");
  
   break;
	}
 
}
