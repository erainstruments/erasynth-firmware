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

void handleCommand() 
{
	String readBackAll_Str = "";
	String readBackDiagnostic_Str = "";
	String RSSI_Str = "";

	// Clean the existing buffer
	Serial.readString(); 

	if (server.arg("amplitude") != "") 
	{
		Serial.println(">A" + server.arg("amplitude"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("frequency") != "") 
	{
		Serial.println(">F" + server.arg("frequency"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("rfoutput") != "") 
	{
		Serial.println(">P0" + server.arg("rfoutput"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("modulation_type") != "") 
	{
		Serial.println(">M0" + server.arg("modulation_type"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("modulation_source") != "") 
	{
		Serial.println(">M1" + server.arg("modulation_source"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("modulation_signal_waveform") != "") 
	{
		Serial.println(">M2" + server.arg("modulation_signal_waveform"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("modulation_freq") != "") 
	{
		Serial.println(">M3" + server.arg("modulation_freq"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("modulation_fm_deviation") != "") 
	{
		Serial.println(">M4" + server.arg("modulation_fm_deviation"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("modulation_am_depth") != "") 
	{
		Serial.println(">M5" + server.arg("modulation_am_depth"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("modulation_pulse_period") != "") 
	{
		Serial.println(">M6" + server.arg("modulation_pulse_period"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("modulation_pulse_width") != "") 
	{
		Serial.println(">M7" + server.arg("modulation_pulse_width"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("modulation_on_off") != "")
	{
		Serial.println(">MS" + server.arg("modulation_on_off"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("sweep_start_stop") != "") 
	{
		Serial.println(">SS" + server.arg("sweep_start_stop"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("sweep_start") != "") 
	{
		Serial.println(">S1" + server.arg("sweep_start"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("sweep_stop") != "") 
	{
		Serial.println(">S2" + server.arg("sweep_stop"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("sweep_step") != "") {
		Serial.println(">S3" + server.arg("sweep_step"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("sweep_dwell") != "") 
	{
		Serial.println(">S4" + server.arg("sweep_dwell"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("sweep_trigger") != "") 
	{
		Serial.println(">S0" + server.arg("sweep_trigger"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("reference_int_ext") != "") 
	{
		Serial.println(">P1" + server.arg("reference_int_ext"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("reference_tcxo_ocxo") != "")
	{
		Serial.println(">P5" + server.arg("reference_tcxo_ocxo"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("wifi_mode") != "")
	{
		Serial.println(">PEW" + server.arg("wifi_mode"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("wifi_sta_ssid") != "") 
	{
		Serial.println(">PES0" + server.arg("wifi_sta_ssid"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("wifi_sta_password") != "") 
	{
		Serial.println(">PEP0" + server.arg("wifi_sta_password"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("wifi_ap_ssid") != "") 
	{
		Serial.println(">PES1" + server.arg("wifi_ap_ssid"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("wifi_ap_password") != "") 
	{
		Serial.println(">PEP1" + server.arg("wifi_ap_password"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("wifi_ip_address") != "") 
	{
		Serial.println(">PEI" + server.arg("wifi_ip_address"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("wifi_gateway_address") != "") 
	{
		Serial.println(">PEG" + server.arg("wifi_gateway_address"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("wifi_subnet_address") != "") 
	{
		Serial.println(">PEN" + server.arg("wifi_subnet_address"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("preset") != "")
	{
		Serial.println(">PP");
		server.send(200, "text/plain", "");
	}
	else if (server.arg("readAll") != "") 
	{
		Serial.println(">RA");
		readBackAll_Str = Serial.readString();
		server.send(200, "application/json", readBackAll_Str);
	}
	else if (server.arg("readDiagnostic") != "") 
	{
		RSSI_Str = WiFi.RSSI();
		Serial.println(">RD");
		delay(15);
		Serial.print(RSSI_Str);
		readBackDiagnostic_Str = Serial.readString();
		server.send(200, "application/json", readBackDiagnostic_Str);
	}
	else if (server.arg("readTemperature") != "") 
	{
		Serial.println(">RT");
		server.send(200, "text/plain", Serial.readString());
	}
	else if (server.arg("readLockLMX2") != "") 
	{
		Serial.println(">R2");
		server.send(200, "text/plain", Serial.readString());
	}
	else if (server.arg("readLockLMX1") != "") 
	{
		Serial.println(">R1");
		server.send(200, "text/plain", Serial.readString());
	}
	else if (server.arg("readLockXtal") != "")
	{
		Serial.println(">R0");
		server.send(200, "text/plain", Serial.readString());
	}
	else if (server.arg("readCurrent") != "") 
	{
		Serial.println(">RC");
		server.send(200, "text/plain", Serial.readString());
	}
	else if (server.arg("readVoltage") != "")
	{
		Serial.println(">RV");
		server.send(200, "text/plain", Serial.readString());
	}
	else if (server.arg("readRSSI") != "") 
	{
		RSSI_Str = WiFi.RSSI();
		Serial.println(RSSI_Str);
		server.send(200, "text/plain", RSSI_Str);
	}
	else if (server.arg("readEmbeddedVersion") != "") 
	{
		Serial.println(">RE");
		server.send(200, "text/plain", Serial.readString());
	}
}
