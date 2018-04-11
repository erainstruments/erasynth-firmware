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
	case 'E': DBG_OUTPUT_PORT.println(ESP8266FirmwareVersion_Str); break; //ESP8266 Firmware Version Read
	case 'R': DBG_OUTPUT_PORT.println(WiFi.RSSI()); break; //RSSI Read
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
		if (commandBuffer[2] == '0') 
		{
			// Mode=Station
			WiFi.setOutputPower(20.5); // max: +20.5dBm min: 0dBm 
			WiFi.disconnect();
			WiFi.softAPdisconnect();
			WiFi.mode(WIFI_STA);

			STA_ssid.toCharArray(ssid, STA_ssid.length() + 1);
			STA_password.toCharArray(password, STA_password.length() + 1);

			DBG_OUTPUT_PORT.print("SSID: ");
			DBG_OUTPUT_PORT.println(ssid);
			DBG_OUTPUT_PORT.print("Password: ");
			DBG_OUTPUT_PORT.println(password);
			DBG_OUTPUT_PORT.print("IP: ");
			DBG_OUTPUT_PORT.println(ip);
			DBG_OUTPUT_PORT.print("Gateway: ");
			DBG_OUTPUT_PORT.println(gateway);
			DBG_OUTPUT_PORT.print("Subnet: ");
			DBG_OUTPUT_PORT.println(subnet);

			WiFi.config(ip, gateway, subnet);

			Serial.println("Network scan initiated");
			Serial.println("-----------------------");
			if (findSSID())
			{
				DBG_OUTPUT_PORT.printf("Connecting to %s\n", ssid);
				if (String(WiFi.SSID()) != String(ssid)) 
				{
					WiFi.begin(ssid, password);
				}
				
				WiFi.mode(WIFI_STA);
				while (WiFi.status() != WL_CONNECTED) 
				{
					delay(500);
					DBG_OUTPUT_PORT.print(".");
					if (++counter > 50)
					{
						DBG_OUTPUT_PORT.print(ssid);
						DBG_OUTPUT_PORT.print(" is available but ");
						DBG_OUTPUT_PORT.println("password may be wrong");
						DBG_OUTPUT_PORT.println("Switched to AP mode");
						command("<W1");
						return;
					}
				}
				
				delay(100);
				DBG_OUTPUT_PORT.println("");
				DBG_OUTPUT_PORT.print("Connected! IP address: ");
				DBG_OUTPUT_PORT.println(WiFi.localIP());
				DBG_OUTPUT_PORT.print("Server MAC address: ");
				DBG_OUTPUT_PORT.println(WiFi.macAddress());
				DBG_OUTPUT_PORT.print("Signal strength (RSSI): ");
				DBG_OUTPUT_PORT.print(WiFi.RSSI());
				DBG_OUTPUT_PORT.println(" dBm");
				serverInit();
			}
			else
			{
				DBG_OUTPUT_PORT.print(ssid);
				DBG_OUTPUT_PORT.println(" is not available");
				DBG_OUTPUT_PORT.println("Switched to AP mode");
				command("<W1");
			}
		}
		else if (commandBuffer[2] == '1') // Mode=Soft AP
		{
			WiFi.disconnect();
			WiFi.softAPdisconnect();
			WiFi.mode(WIFI_AP);

			AP_ssid.toCharArray(ssid, AP_ssid.length() + 1);
			AP_password.toCharArray(password, AP_password.length() + 1);

			DBG_OUTPUT_PORT.print("SSID: ");
			DBG_OUTPUT_PORT.println(ssid);
			DBG_OUTPUT_PORT.print("Password: ");
			DBG_OUTPUT_PORT.println(password);
			DBG_OUTPUT_PORT.print("IP: ");
			DBG_OUTPUT_PORT.println(ip);
			DBG_OUTPUT_PORT.print("Gateway: ");
			DBG_OUTPUT_PORT.println(gateway);
			DBG_OUTPUT_PORT.print("Subnet: ");
			DBG_OUTPUT_PORT.println(subnet);

			WiFi.softAPConfig(ip, gateway, subnet);
			WiFi.softAP(ssid, password);
			WiFi.mode(WIFI_AP);
			WiFi.setOutputPower(10); // max: +20.5dBm min: 0dBm 
			delay(100);
			DBG_OUTPUT_PORT.println();
			DBG_OUTPUT_PORT.print("Server IP address: ");
			DBG_OUTPUT_PORT.println(WiFi.softAPIP());
			DBG_OUTPUT_PORT.print("Server MAC address: ");
			DBG_OUTPUT_PORT.println(WiFi.softAPmacAddress());
			serverInit();
		}

		break;
	}
}