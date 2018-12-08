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
		debugPrintln(">A" + server.arg("amplitude"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("frequency") != "") 
	{
		debugPrintln(">F" + server.arg("frequency"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("rfoutput") != "") 
	{
		debugPrintln(">P0" + server.arg("rfoutput"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("modulation_type") != "") 
	{
		debugPrintln(">M0" + server.arg("modulation_type"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("modulation_source") != "") 
	{
		debugPrintln(">M1" + server.arg("modulation_source"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("modulation_signal_waveform") != "") 
	{
		debugPrintln(">M2" + server.arg("modulation_signal_waveform"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("modulation_freq") != "") 
	{
		debugPrintln(">M3" + server.arg("modulation_freq"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("modulation_fm_deviation") != "") 
	{
		debugPrintln(">M4" + server.arg("modulation_fm_deviation"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("modulation_am_depth") != "") 
	{
		debugPrintln(">M5" + server.arg("modulation_am_depth"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("modulation_pulse_period") != "") 
	{
		debugPrintln(">M6" + server.arg("modulation_pulse_period"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("modulation_pulse_width") != "") 
	{
		debugPrintln(">M7" + server.arg("modulation_pulse_width"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("modulation_on_off") != "")
	{
		debugPrintln(">MS" + server.arg("modulation_on_off"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("sweep_start_stop") != "") 
	{
		debugPrintln(">SS" + server.arg("sweep_start_stop"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("sweep_start") != "") 
	{
		debugPrintln(">S1" + server.arg("sweep_start"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("sweep_stop") != "") 
	{
		debugPrintln(">S2" + server.arg("sweep_stop"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("sweep_step") != "") {
		debugPrintln(">S3" + server.arg("sweep_step"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("sweep_dwell") != "") 
	{
		debugPrintln(">S4" + server.arg("sweep_dwell"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("sweep_trigger") != "") 
	{
		debugPrintln(">S0" + server.arg("sweep_trigger"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("reference_int_ext") != "") 
	{
		debugPrintln(">P1" + server.arg("reference_int_ext"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("reference_tcxo_ocxo") != "")
	{
		debugPrintln(">P5" + server.arg("reference_tcxo_ocxo"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("wifi_mode") != "")
	{
		debugPrintln(">PEW" + server.arg("wifi_mode"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("wifi_sta_ssid") != "") 
	{
		debugPrintln(">PES0" + server.arg("wifi_sta_ssid"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("wifi_sta_password") != "") 
	{
		debugPrintln(">PEP0" + server.arg("wifi_sta_password"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("wifi_ap_ssid") != "") 
	{
		debugPrintln(">PES1" + server.arg("wifi_ap_ssid"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("wifi_ap_password") != "") 
	{
		debugPrintln(">PEP1" + server.arg("wifi_ap_password"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("wifi_ip_address") != "") 
	{
		debugPrintln(">PEI" + server.arg("wifi_ip_address"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("wifi_gateway_address") != "") 
	{
		debugPrintln(">PEG" + server.arg("wifi_gateway_address"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("wifi_subnet_address") != "") 
	{
		debugPrintln(">PEN" + server.arg("wifi_subnet_address"));
		server.send(200, "text/plain", "");
	}
	else if (server.arg("preset") != "")
	{
		debugPrintln(">PP");
		server.send(200, "text/plain", "");
	}
	else if (server.arg("readAll") != "") 
	{
		debugPrintln(">RA");
		readBackAll_Str = Serial.readString();
		server.send(200, "application/json", readBackAll_Str);
	}
	else if (server.arg("readDiagnostic") != "") 
	{
		RSSI_Str = WiFi.RSSI();
		debugPrintln(">RD");
		delay(15);
		debugPrint(RSSI_Str);
		readBackDiagnostic_Str = Serial.readString();
		server.send(200, "application/json", readBackDiagnostic_Str);
	}
	else if (server.arg("readTemperature") != "") 
	{
		debugPrintln(">RT");
		server.send(200, "text/plain", Serial.readString());
	}
	else if (server.arg("readLockLMX2") != "") 
	{
		debugPrintln(">R2");
		server.send(200, "text/plain", Serial.readString());
	}
	else if (server.arg("readLockLMX1") != "") 
	{
		debugPrintln(">R1");
		server.send(200, "text/plain", Serial.readString());
	}
	else if (server.arg("readLockXtal") != "")
	{
		debugPrintln(">R0");
		server.send(200, "text/plain", Serial.readString());
	}
	else if (server.arg("readCurrent") != "") 
	{
		debugPrintln(">RC");
		server.send(200, "text/plain", Serial.readString());
	}
	else if (server.arg("readVoltage") != "")
	{
		debugPrintln(">RV");
		server.send(200, "text/plain", Serial.readString());
	}
	else if (server.arg("readRSSI") != "") 
	{
		RSSI_Str = WiFi.RSSI();
		debugPrintln(RSSI_Str);
		server.send(200, "text/plain", RSSI_Str);
	}
	else if (server.arg("readEmbeddedVersion") != "") 
	{
		debugPrintln(">RE");
		server.send(200, "text/plain", Serial.readString());
	}
}
