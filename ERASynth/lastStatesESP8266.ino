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

void setLastStatesOfESP8266()
{
	esp8266OnOff_Str		= getFRAM(_esp8266OnOff);
	
	if (esp8266OnOff_Str == "0") { digitalWrite(Wi_Fi_PD, 0); return; }
	
	staModeSSID_Str			= getFRAM(_staModeSSID);
	staModePassword_Str		= getFRAM(_staModePassword);
	apModeSSID_Str			= getFRAM(_apModeSSID);
	apModePassword_Str		= getFRAM(_apModePassword);
	ipAddress_Str			= getFRAM(_ipAddress);
	gatewayAddress_Str		= getFRAM(_gatewayAddress);
	subnetAddress_Str		= getFRAM(_subnetAddress);
	wifiMode_Str			= getFRAM(_wifiMode);

	command(">PE0" + esp8266OnOff_Str);

	// Removes zeros in front of the string
	while (staModeSSID_Str.indexOf('0') == 0) { staModeSSID_Str.remove(0, 1); }
	while (staModePassword_Str.indexOf('0') == 0) { staModePassword_Str.remove(0, 1); }
	while (apModeSSID_Str.indexOf('0') == 0) { apModeSSID_Str.remove(0, 1); }
	while (apModePassword_Str.indexOf('0') == 0) { apModePassword_Str.remove(0, 1); }

	command(">PES0" + staModeSSID_Str);
	command(">PEP0" + staModePassword_Str); 
	command(">PES1" + apModeSSID_Str); 
	command(">PEP1" + apModePassword_Str);	
	command(">PEI" + ipAddress_Str);	
	command(">PEG" + gatewayAddress_Str);	
	command(">PEN" + subnetAddress_Str);	
	command(">PEW" + wifiMode_Str);
}
