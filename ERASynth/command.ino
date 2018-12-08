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
	char commandID = 0;
	String commandInString = "";
	String DAC_ValueInString = "";
	String Amp_ValueInString = "";
	uint16_t DAC_Value = 0;
	uint16_t Amp_Value = 0;
	uint16_t Depth_Value = 0;
	float floatAmpValue = 0;
	uint16_t cur = 0;
	uint16_t volt = 0;
	uint32_t LMX_RXX = 0;

	commandID = (char)commandBuffer[1];

	switch (commandID) 
	{
	case 'F': //Frequency Set
		for (int i = 2; i <= commandBuffer.length() - 1; i++) 
		{
			if (isDigit(commandBuffer[i])) { commandInString += (char)commandBuffer[i]; }
		}

		if (commandInString.length() > _freqSize) { Serial.println("Limits exceeded"); break; }

		if (commandInString.length() <= 8) { frequency = (long long)commandInString.toInt(); }
		else 
		{
			String freqInString1 = "";
			String freqInString2 = "";
			for (int i = 0; i < 8; i++) { freqInString1 += (char)commandInString[i]; }
			for (int i = 8; i < commandInString.length(); i++) { freqInString2 += (char)commandInString[i]; }

			frequency = (long long)((long long)freqInString1.toInt() * pow(10, commandInString.length() - 8)) + freqInString2.toInt();
		}

		if (frequency < 250e3) { frequency = 250e3; commandInString = "250000"; }

		if (ERASynth == 0)
		{
			if (frequency > 6e9) { frequency = 6e9; commandInString = "6000000000"; }
		}
		else if (ERASynth == 1)
		{
			if (frequency > 15e9) { frequency = 15e9; commandInString = "15000000000"; }
		}
		else if (ERASynth == 2)
		{
			if (frequency > 20e9) { frequency = 20e9; commandInString = "20000000000"; }
		}

		if (commandInString != frequency_Str) { frequency_Str = commandInString; setFRAM(_frequency, frequency_Str); }

		Serial.print("Frequency: "); 
		Serial.print(frequency_Str); 
		Serial.println("Hz");

		lastFrequency = frequency;

		if (rfOnOff_Str == "1")
		{
			setMinAmplitude();
			setFreqBand(frequency);
			setFreqParam(frequency);
			setAmplitude();
		}
		break;

	case 'A': 
		//DAC Set  
		Amp_ValueInString = "";

		if (commandBuffer[2] == 'L') // LMX2 Power Level
		{
			for (int i = 3; i <= commandBuffer.length() - 1; i++) 
			{
				if (isDigit(commandBuffer[i])) { Amp_ValueInString += (char)commandBuffer[i]; }
			}
			Amp_Value = Amp_ValueInString.toInt();
			if (Amp_Value < 0) Amp_Value = 0;
			if (Amp_Value > 63) Amp_Value = 63;

			LMX2_R44_update = (Amp_Value << 8) + 2883712;
			spiWrite_LMX(&LMX2_R44_update, LMX2_LE);
		}
		else if (commandBuffer[2] == 'D') // DDS Power Level
		{
			for (int i = 3; i <= commandBuffer.length() - 1; i++) 
			{
				if (isDigit(commandBuffer[i])) { Amp_ValueInString += (char)commandBuffer[i]; }
			}

			DDSPowerLevel = Amp_ValueInString.toInt();
			if (DDSPowerLevel < 0) DDSPowerLevel = 0;
			if (DDSPowerLevel > 1023) DDSPowerLevel = 1023;

			DDSPowerLevel_Str = String(DDSPowerLevel);
			setDDSPowerLevel(DDSPowerLevel); 
		}
		else 
		{
			for (int i = 2; i <= commandBuffer.length() - 1; i++) 
			{
				if (isDigit(commandBuffer[i]) || commandBuffer[i] == '-' || commandBuffer[i] == '.') { Amp_ValueInString += (char)commandBuffer[i]; }
			}

			floatAmpValue = Amp_ValueInString.toFloat();
			if (floatAmpValue < -60) floatAmpValue = -60;
			if (floatAmpValue > 20) floatAmpValue = 20;

			if (String(floatAmpValue) != amplitude_Str) 
			{
				amplitude_Str = String(floatAmpValue);
				setFRAM(_amplitude, amplitude_Str);
			}

			Serial.print("Amplitude: "); 
			Serial.print(amplitude_Str); 
			Serial.println("dBm");

			amplitude = floatAmpValue;
			lastAmplitude = amplitude;
			setAmplitude();
		}
		break;

	case 'L': //LMX Register Set  
		LMX_RXX = 0;
		
		for (int i = 3; i <= commandBuffer.length() - 1; i++) 
		{
			if (isDigit(commandBuffer[i])) { commandInString += (char)commandBuffer[i]; }
		}

		LMX_RXX = commandInString.toInt();

		if (commandBuffer[2] == '1') { spiWrite_LMX(&LMX_RXX, LMX1_LE); }//LMX1
		else if (commandBuffer[2] == '2') { spiWrite_LMX(&LMX_RXX, LMX2_LE); }//LMX2
		
		Serial.print("LMX_RXX: "); Serial.println(LMX_RXX, HEX);
		break;

	case 'D': //DAC Set
		
		DAC_ValueInString = "";
		
		for (int i = 3; i <= commandBuffer.length() - 1; i++) 
		{
			if (isDigit(commandBuffer[i])) { DAC_ValueInString += (char)commandBuffer[i]; }
		}

		DAC_Value = DAC_ValueInString.toInt();
		
		if (DAC_Value < 0) DAC_Value = 0;
		if (DAC_Value > 4095) DAC_Value = 4095;

		DACValue = DAC_Value;

		Serial.print("DAC Value: "); 
		Serial.println(DAC_Value);
		
		if (commandBuffer[2] == '1')
		{
			setDAC(DAC_Value, DAC1_LE);
		}
		else if (commandBuffer[2] == '2')
		{
			setDAC(DAC_Value, DAC2_LE);
		}
		else if (commandBuffer[2] == '3')
		{
			setDAC(DAC_Value, DAC3_LE);
		}
		else if (commandBuffer[2] == 'A')
		{
			setDAC(DAC_Value, DAC1_LE);
			setDAC(DAC_Value, DAC2_LE);
			setDAC(DAC_Value, DAC3_LE);
		}
		break;

	case 'S':
		
		for (int i = 3; i <= commandBuffer.length() - 1; i++) 
		{
			if (isDigit(commandBuffer[i]))  { commandInString += (char)commandBuffer[i]; }
		}

		if (commandInString.length() > _freqSize) { Serial.println("Limits exceeded"); break; }

		if (commandInString.length() <= 8) 
		{
			frequency = (long long)commandInString.toInt();
		}
		else
		{
			String freqInString1 = "";
			String freqInString2 = "";
			for (int i = 0; i < 8; i++) { freqInString1 += (char)commandInString[i]; }
			for (int i = 8; i < commandInString.length(); i++) { freqInString2 += (char)commandInString[i]; }
			frequency = (long long)((long long)freqInString1.toInt() * pow(10, commandInString.length() - 8)) + freqInString2.toInt();
		}
		if (commandBuffer[2] == '0')
		{ 
			//Sweep Trigger Mode (0=Free Run; 1=External)
			if (commandBuffer[3] == '0') { sweepTriggerMode = false; }
			else if (commandBuffer[3] == '1') { sweepTriggerMode = true; }

			if (String(commandBuffer[3]) != sweepTriggerMode_Str) 
			{
				sweepTriggerMode_Str = String(commandBuffer[3]);
				setFRAM(_sweepTriggerMode, sweepTriggerMode_Str);
			}
			if (isDebugEnabled) { Serial.print("Sweep Mode Free-Run[0]/External[1]: "); Serial.println(sweepTriggerMode_Str); }
		}
		else if (commandBuffer[2] == '1') 
		{
			//Start Frequency in Hz
			startFrequency = frequency;

			if (commandInString != startFrequency_Str) 
			{
				startFrequency_Str = commandInString;
				setFRAM(_startFrequency, startFrequency_Str);
			}
			if (isDebugEnabled) { Serial.print("Start Frequency: "); Serial.print(startFrequency_Str); Serial.println("Hz"); }
		}
		else if (commandBuffer[2] == '2') 
		{
			//Stop Frequency in Hz
			stopFrequency = frequency;

			if (commandInString != stopFrequency_Str) 
			{
				stopFrequency_Str = commandInString;
				setFRAM(_stopFrequency, stopFrequency_Str);
			}
			if (isDebugEnabled) { Serial.print("Stop Frequency: "); Serial.print(stopFrequency_Str); Serial.println("Hz"); }
		}
		else if (commandBuffer[2] == '3') 
		{
			//Step Frequency in Hz
			stepFrequency = frequency;

			if (commandInString != stepFrequency_Str) 
			{
				stepFrequency_Str = commandInString;
				setFRAM(_stepFrequency, stepFrequency_Str);
			}
			if (isDebugEnabled) { Serial.print("Step Frequency: "); Serial.print(stepFrequency_Str); Serial.println("Hz"); }
		}
		else if (commandBuffer[2] == '4') 
		{
			//Dwell time in ms
			dwellTime = (uint32_t)frequency;

			if (String(dwellTime) != dwellTime_Str) 
			{
				dwellTime_Str = String(dwellTime);
				setFRAM(_dwellTime, dwellTime_Str);
			}
			if (isDebugEnabled) { Serial.print("Dwell Time: "); Serial.print(dwellTime_Str); Serial.println("ms"); }
		}
		else if (commandBuffer[2] == 'S')
		{ 
			//Sweep Start-Stop (0=Stop; 1=Start)
			if (String(commandBuffer[3]) != sweepOnOff_Str) 
			{
				sweepOnOff_Str = String(commandBuffer[3]);
				setFRAM(_sweepOnOff, sweepOnOff_Str);
			}

			if (commandBuffer[3] == '1')
			{
				is_sweep_stopped = false;
				if (isDebugEnabled) { Serial.println("Frequency Sweep: Started"); }

				number_of_sweep_points = ((stopFrequency - startFrequency) / stepFrequency) + 1;

				if (stopFrequency > 30e6 || startFrequency > 30e6) 
				{
					// Turn on LMX2 incase present path was LF Path
					spiWrite_LMX(&LMX2_R0, LMX2_LE); 
					delay(10);
				}

				sweepIndex = 0;
				sweepERASynth();

				if (!sweepTriggerMode) 
				{
					Timer4.attachInterrupt(sweepRoutine).setPeriod(dwellTime * 1000).start();  // attaches sweepERASynth() as a timer function called every dwellTime milliseconds:
				}
				else 
				{
					attachInterrupt(Trig_Inp, sweepRoutine, RISING);
				}
			}
			else if (commandBuffer[3] == '0')
			{
				is_sweep_stopped = true;
				nextFreq = false;
				Timer4.stop(); detachInterrupt(Trig_Inp);
				if (isDebugEnabled) { Serial.println("Frequency Sweep: Stopped"); }
				command(">F" + frequency_Str); // Recall last frequency
			}
		}
		break;

	case 'P': //Path Selections

		if (commandBuffer[2] == '0') 
		{
			// First rf status must be set. It is checked in the function below
			if (String(commandBuffer[3]) != rfOnOff_Str) 
			{
				rfOnOff_Str = String(commandBuffer[3]);
				setFRAM(_rfOnOff, rfOnOff_Str);
			}

			//RF ON-OFF (0=RF OFF; 1=RF ON)  
			rfOnOff(commandBuffer[3] - 48);
		}
		else if (commandBuffer[2] == '1') 
		{
			//Reference (0=Int; 1=Ext)
			digitalWrite(SW1, commandBuffer[3] - 48); 

			if (String(commandBuffer[3]) != referenceIntOrExt_Str) 
			{
				referenceIntOrExt_Str = String(commandBuffer[3]);
				setFRAM(_referenceIntOrExt, referenceIntOrExt_Str);
			}
			if (isDebugEnabled) { Serial.print("Reference Int[0]/Ext[1]: "); Serial.println(referenceIntOrExt_Str); }
		}
		else if (commandBuffer[2] == '2') 
		{
			//LMX1 REF path (0=LMX1 Bypassed; 1=LMX1 path is selected)
			digitalWrite(SW2, commandBuffer[3] - 48); 
		}
		else if (commandBuffer[2] == '3') 
		{
			//Low Freq Path Select (0=30MHz-6GHz; 1=LF-30MHz)
			digitalWrite(SW3, commandBuffer[3] - 48); 
		}
		else if (commandBuffer[2] == '4') 
		{
			//High Freq Path Select (0=LF-6GHz; 1=6GHz-15GHz)
			digitalWrite(SW4, commandBuffer[3] - 48); 
		}
		else if (commandBuffer[2] == '5') 
		{
			//TCXO-OCXO select (0=TCXO is ON; OCXO is OFF; 1=OCXO is ON; TCXO is OFF)
			digitalWrite(TCXO_En, !(commandBuffer[3] - 48)); 
			digitalWrite(OCXO_En, commandBuffer[3] - 48);

			if (String(commandBuffer[3]) != referenceTcxoOrOcxo_Str)
			{
				referenceTcxoOrOcxo_Str = String(commandBuffer[3]);
				setFRAM(_referenceTcxoOrOcxo, referenceTcxoOrOcxo_Str);
			}

			if (isDebugEnabled) { Serial.print("Reference Tcxo[0]/Ocxo[1]: "); Serial.println(referenceTcxoOrOcxo_Str); }
		}
		else if (commandBuffer[2] == '6') 
		{
			//DC_6GHz Amp. (0=OFF; 1=ON) 
			digitalWrite(DC_6GHz_En, commandBuffer[3] - 48); 
		}
		else if (commandBuffer[2] == '7') 
		{
			//6_15GHz Amp. (0=OFF; 1=ON)
			digitalWrite(X6_15GHz_En, commandBuffer[3] - 48); 
		}
		else if (commandBuffer[2] == '8') 
		{
			if (String(commandBuffer[3]) != rememberLastStates_Str) 
			{
				rememberLastStates_Str = String(commandBuffer[3]);
				setFRAM(_rememberLastStates, rememberLastStates_Str);
			}
			if (isDebugEnabled) { Serial.print("Remember Last States No[0]/Yes[1]: "); Serial.println(rememberLastStates_Str); }
		}
		else if (commandBuffer[2] == 'A') 
		{
			// Low Power Mode (0=OFF; 1=ON)
			isLowPowerModeActive = commandBuffer[3] - 48; 

			if (String(commandBuffer[3]) != isLowPowerModeActive_Str) 
			{
				isLowPowerModeActive_Str = String(commandBuffer[3]);
				setFRAM(_isLowPowerModeActive, isLowPowerModeActive_Str);
			}

			if (isDebugEnabled) { Serial.print("Low Power Mode Off[0]/On[1]: "); Serial.println(isLowPowerModeActive_Str); }
		}
		else if (commandBuffer[2] == 'D') 
		{
			// Debug Messages (0=OFF; 1=ON)
			isDebugEnabled = commandBuffer[3] - 48;
			Serial.print("Debug Messages Dis[0]/Enb[1]: "); Serial.println(String(isDebugEnabled));
		}
		else if (commandBuffer[2] == 'M') 
		{
			// ERASynth Model (0=ERASynth; 1=ERASynth+; 2=ERASynth++)
			ERASynthModel = commandBuffer[3] - 48; 

			if (String(commandBuffer[3]) != ERASynthModel_Str)
			{
				ERASynthModel_Str = String(commandBuffer[3]);
				setFRAM(_ERASynthModel, ERASynthModel_Str);
			}

			if (isDebugEnabled) { Serial.print("ERASynth Model: "); Serial.println(ERASynthModel_Str); }
		}
		else if (commandBuffer[2] == 'P')
		{ 
			// PRESET
			if (isDebugEnabled) { Serial.println("PRESET is activated"); }
			preset_ERASynth();
		}
		else if (commandBuffer[2] == 'R')
		{ 
			// Factory RESET
			if (isDebugEnabled) { Serial.println("Factory Reset is activated"); }
			facReset();
		}
		else if (commandBuffer[2] == 'E')
		{ 
			// ESP8266 Settings
			for (int i = 4; i <= commandBuffer.length() - 1; i++) { commandInString += (char)commandBuffer[i]; }

			if (commandBuffer[3] == '0') // ESP8266 ON/OFF (0=OFF; 1=ON)
			{
				digitalWrite(Wi_Fi_PD, commandBuffer[4] - 48);
				if (String(commandBuffer[4]) != esp8266OnOff_Str)
				{
					esp8266OnOff_Str = String(commandBuffer[4]);
					setFRAM(_esp8266OnOff, esp8266OnOff_Str);
				}
				if (isDebugEnabled) { Serial.print("ESP8266 Off[0]/On[1]: "); Serial.println(esp8266OnOff_Str); }
			}
			else if (commandBuffer[3] == 'W') // WiFi Mode  (0=STA; 1=AP)
			{
				if (String(commandBuffer[4]) != wifiMode_Str) 
				{
					wifiMode_Str = String(commandBuffer[4]);
					setFRAM(_wifiMode, wifiMode_Str);
				}

				if (isDebugEnabled) { Serial.print("WiFi Mode Sta[0]/AP[1]: "); Serial.println(wifiMode_Str); }
				
				if (wifiMode_Str == "0") { Serial1.println("<W0"); }
				else if (wifiMode_Str == "1") { Serial1.println("<W1"); }
			}
			else if (commandBuffer[3] == 'S' && commandBuffer[4] == '0') // Station Mode SSID
			{
				commandInString.remove(0, 1);
				if (commandInString != staModeSSID_Str)
				{
					staModeSSID_Str = commandInString;
					setFRAM(_staModeSSID, staModeSSID_Str);
				}
				if (isDebugEnabled) { Serial.print("Station Mode SSID: "); Serial.println(staModeSSID_Str); }
				Serial1.println("<S0" + staModeSSID_Str);
			}
			else if (commandBuffer[3] == 'S' && commandBuffer[4] == '1') // AP Mode SSID
			{
				commandInString.remove(0, 1);
				if (commandInString != apModeSSID_Str)
				{
					apModeSSID_Str = commandInString;
					setFRAM(_apModeSSID, apModeSSID_Str);
				}
				if (isDebugEnabled) { Serial.print("AP Mode SSID: "); Serial.println(apModeSSID_Str); }
				Serial1.println("<S1" + apModeSSID_Str);
			}
			else if (commandBuffer[3] == 'P' && commandBuffer[4] == '0') // Station Mode Password
			{
				commandInString.remove(0, 1);
				if (commandInString != staModePassword_Str) 
				{
					staModePassword_Str = commandInString; 
					setFRAM(_staModePassword, staModePassword_Str); 
				}
				if (isDebugEnabled) { Serial.print("Station Mode Password: "); Serial.println(staModePassword_Str); }
				Serial1.println("<P0" + staModePassword_Str);
			}
			else if (commandBuffer[3] == 'P' && commandBuffer[4] == '1') // AP Mode Password
			{
				commandInString.remove(0, 1);
				if (commandInString != apModePassword_Str)
				{
					apModePassword_Str = commandInString;
					setFRAM(_apModePassword, apModePassword_Str);
				}
				if (isDebugEnabled) { Serial.print("AP Mode Password: "); Serial.println(apModePassword_Str); }
				Serial1.println("<P1" + apModePassword_Str);
			}
			else if (commandBuffer[3] == 'I') // IP Address
			{
				if (commandInString != ipAddress_Str) 
				{
					ipAddress_Str = commandInString;
					setFRAM(_ipAddress, ipAddress_Str);
				}
				if (isDebugEnabled) { Serial.print("IP Address: "); Serial.println(ipAddress_Str); }
				Serial1.println("<I" + ipAddress_Str);
			}
			else if (commandBuffer[3] == 'G') // Gateway Address
			{
				if (commandInString != gatewayAddress_Str) 
				{
					gatewayAddress_Str = commandInString;
					setFRAM(_gatewayAddress, gatewayAddress_Str);
				}
				if (isDebugEnabled) { Serial.print("Gateway Address: "); Serial.println(gatewayAddress_Str); }
				Serial1.println("<G" + gatewayAddress_Str);
			}
			else if (commandBuffer[3] == 'N') // Subnet Address
			{
				if (commandInString != subnetAddress_Str)
				{
					subnetAddress_Str = commandInString;
					setFRAM(_subnetAddress, subnetAddress_Str);
				}
				if (isDebugEnabled) { Serial.print("Subnet Address: "); Serial.println(subnetAddress_Str); }
				Serial1.println("<N" + subnetAddress_Str);
			}

		}
		break;

	case 'M': 
		
		//FM Modulation with default settings
		if (commandBuffer[2] == '0')
		{ 
			//Modulation Type
			//0=NBFM_Mod; 1=WBFM_Mod; 2=AM_Mod; 3=Pulse_Mod
			modType = commandBuffer[3] - 48; 

			if (String(modType) != modulationType_Str)
			{
				modulationType_Str = String(modType);
				setFRAM(_modulationType, modulationType_Str);
			}

			if (isDebugEnabled)
			{
				Serial.print("Modulation Type: ");
				if (modType == 0) { Serial.println("NBFM"); }
				else if (modType == 1) { Serial.println("WBFM"); }
				else if (modType == 2) { Serial.println("AM"); }
				else if (modType == 3) { Serial.println("Pulse"); }
				else { Serial.println("Unknown"); }
			}
		}
		else if (commandBuffer[2] == '1') 
		{ 
			//Modulation Source 0=Internal; 1=External; 2=Microphone
			modSource = commandBuffer[3] - 48;

			if (String(modSource) != modulationSource_Str) 
			{
				modulationSource_Str = String(modSource);
				setFRAM(_modulationSource, modulationSource_Str);
			}

			if (isDebugEnabled) 
			{
				Serial.print("Modulation Source: ");
				if (modSource == External) { Serial.println("External"); ADC->ADC_CHER = 0x01; } // Enable ADC on pin A7
				else if (modSource == Internal) { Serial.println("Internal"); }
				else if (modSource == Microphone) { Serial.println("Microphone"); ADC->ADC_CHER = 0x02; } // Enable ADC on pin A6
			}
		}
		else if (commandBuffer[2] == '2') 
		{ 
			//Signal Waveform (0=Sine; 1=Triangle, 2=Ramp, 3=Square)
			waveformType = commandBuffer[3] - 48;

			if (String(waveformType) != waveformType_Str) 
			{
				waveformType_Str = String(waveformType);
				setFRAM(_waveformType, waveformType_Str);
			}

			if (isDebugEnabled) 
			{
				Serial.print("Waveform Type: ");
				if (waveformType == Sine) { Serial.println("Sine"); }
				else if (waveformType == Triangle) { Serial.println("Triangle"); }
				else if (waveformType == Ramp) { Serial.println("Ramp"); }
				else if (waveformType == Square) { Serial.println("Square"); }
			}
		}
		else if (commandBuffer[2] == '3')
		{ 
			//Internal Modulation Frequency (Hz)
			for (int i = 3; i <= commandBuffer.length() - 1; i++) 
			{
				if (isDigit(commandBuffer[i])) { commandInString += (char)commandBuffer[i]; }
			}

			if (commandInString.length() > _internalModulationFreq[1]) { Serial.println("Limits exceeded"); break; }

			if (commandInString != internalModulationFreq_Str) 
			{
				internalModulationFreq_Str = commandInString;
				setFRAM(_internalModulationFreq, internalModulationFreq_Str);
			}

			if (isDebugEnabled) { Serial.print("Internal Modulation Frequency: "); Serial.print(internalModulationFreq_Str); Serial.println("Hz"); }
		}
		else if (commandBuffer[2] == '4')
		{ 
			//FM Deviation Frequency (Hz)
			for (int i = 3; i <= commandBuffer.length() - 1; i++) 
			{
				if (isDigit(commandBuffer[i])) { commandInString += (char)commandBuffer[i]; }
			}

			if (commandInString.length() > _fmDeviation[1]) { Serial.println("Limits exceeded"); break; }

			if (commandInString != fmDeviation_Str)
			{
				fmDeviation_Str = commandInString;
				setFRAM(_fmDeviation, fmDeviation_Str);
			}

			if (isDebugEnabled) { Serial.print("FM Deviation: "); Serial.print(fmDeviation_Str); Serial.println("Hz"); }
		}
		else if (commandBuffer[2] == '5')
		{ 
			//AM Depth (%)
			for (int i = 3; i <= commandBuffer.length() - 1; i++) 
			{
				if (isDigit(commandBuffer[i]) || commandBuffer[i] == '.') { commandInString += (char)commandBuffer[i]; }
			}

			if (commandInString.length() > _amDepth[1]) { Serial.println("Limits exceeded"); break; }

			Depth_Value = commandInString.toInt();

			if (Depth_Value > 100) { Depth_Value = 100; }
			
			if (String(Depth_Value) != amDepth_Str) 
			{
				amDepth_Str = String(Depth_Value);
				setFRAM(_amDepth, amDepth_Str);
			}


			if (isDebugEnabled) { Serial.print("AM Depth: "); Serial.print(amDepth_Str); Serial.println("%"); }
		}
		else if (commandBuffer[2] == '6') 
		{ 
			//Pulse Period (us) 
			for (int i = 3; i <= commandBuffer.length() - 1; i++) 
			{
				if (isDigit(commandBuffer[i])) { commandInString += (char)commandBuffer[i]; }
			}

			if (commandInString.length() > _pulsePeriod[1]) { Serial.println("Limits exceeded"); break; }

			if (commandInString.toInt() < 2 * default_delay_in_Pulse_Mod) { commandInString = String(2 * default_delay_in_Pulse_Mod); }

			if (commandInString != pulsePeriod_Str) 
			{
				pulsePeriod_Str = commandInString;
				setFRAM(_pulsePeriod, pulsePeriod_Str);
			}

			if (isDebugEnabled) { Serial.print("Pulse Period: "); Serial.print(pulsePeriod_Str); Serial.println("us"); }
		}
		else if (commandBuffer[2] == '7') 
		{ 
			//Pulse Width (us)
			for (int i = 3; i <= commandBuffer.length() - 1; i++) 
			{
				if (isDigit(commandBuffer[i])) { commandInString += (char)commandBuffer[i]; }
			}

			if (commandInString.length() > _pulseWidth[1]) { Serial.println("Limits exceeded"); break; }

			if (commandInString.toInt() < default_delay_in_Pulse_Mod) { commandInString = String(default_delay_in_Pulse_Mod); }

			if (commandInString != pulseWidth_Str) 
			{
				pulseWidth_Str = commandInString;
				setFRAM(_pulseWidth, pulseWidth_Str);
			}

			if (isDebugEnabled) { Serial.print("Pulse Width: "); Serial.print(pulseWidth_Str); Serial.println("us"); }
		}
		else if (commandBuffer[2] == 'S') 
		{ 
			//Modulation Start-Stop (0=Stop; 1=Start)
			if (String(commandBuffer[3]) != modulationOnOff_Str) 
			{
				modulationOnOff_Str = String(commandBuffer[3]);
				setFRAM(_modulationOnOff, modulationOnOff_Str);
			}

			if (commandBuffer[3] == '1')
			{
				is_modulation_stopped = false;
				is_modulation_paused = false;
				stopAllMod();

				switch (modType) 
				{
					case NBFM_Mod:
						digitalWrite(NB_FM_En, HIGH); //NB_FM Enabled.
						nbFmMod();
						break;
					case WBFM_Mod:
						delay(1); // it is required when changing the frequency while modulation is ON
						spiWrite_LMX(&WBFM_LMX2_R12, LMX2_LE);
						spiWrite_LMX(&WBFM_LMX2_R11, LMX2_LE);
						spiWrite_LMX(&WBFM_LMX2_R34_update, LMX2_LE);
						spiWrite_LMX(&WBFM_LMX2_R36_update, LMX2_LE);
						spiWrite_LMX(&WBFM_LMX2_R14, LMX2_LE);
						digitalWrite(WB_FM_En, HIGH); //WB_FM Enabled.
						wbFmMod();
						break;
					case AM_Mod:
						amMod();
						break;
					case Pulse_Mod:
						isPulseActive = true;
						
						break;
				}
				delay(10); // it is required while serial activity
				return;
			}
			else if (commandBuffer[3] == '0') 
			{
				is_modulation_stopped = true;
				switch (modType)
				{
					case NBFM_Mod:
						
						digitalWrite(NB_FM_En, LOW); //NB_FM Disabled. Normal Operation
						if (isDebugEnabled) { Serial.println("NBFM Modulation: Stopped"); }
						break;

					case WBFM_Mod:
						
						digitalWrite(WB_FM_En, LOW); //WB_FM Disabled. Normal Operation
						spiWrite_LMX(&LMX2_register[112 - 12], LMX2_LE);
						spiWrite_LMX(&LMX2_register[112 - 11], LMX2_LE);
						spiWrite_LMX(&LMX2_register[112 - 34], LMX2_LE);
						spiWrite_LMX(&LMX2_R36_update, LMX2_LE);
						spiWrite_LMX(&LMX2_register[112 - 14], LMX2_LE);
						rfOnOff(rfOnOff_Str.toInt());
						if (isDebugEnabled) { Serial.println("WBFM Modulation: Stopped"); }
						break;

					case AM_Mod:
						
						if (isDebugEnabled) { Serial.println("AM Modulation: Stopped"); }
						setAmplitude(); //restore the right amplitude value after AM modulation
						break;

					case Pulse_Mod:
						
						if (isDebugEnabled) { Serial.println("Pulse Modulation: Stopped"); }
						detachInterrupt(Trig_Inp);
						Timer2.stop();
						isPulseActive = false;
						rfOnOff(rfOnOff_Str.toInt());
						break;
				}
			}
		}
		break;

	case 'R': //READ BACK
		
		if (commandBuffer[2] == 'A') //Read All
		{
			String vals[28][2];

			vals[0][0] = "rfoutput";
			vals[0][1] = rfOnOff_Str;

			vals[1][0] = "frequency";
			vals[1][1] = frequency_Str;

			vals[2][0] = "amplitude";
			vals[2][1] = amplitude_Str;

			vals[3][0] = "modulation_on_off";
			vals[3][1] = modulationOnOff_Str;

			vals[4][0] = "modulation_type";
			vals[4][1] = modulationType_Str;

			vals[5][0] = "modulation_source";
			vals[5][1] = modulationSource_Str;

			vals[6][0] = "modulation_signal_waveform";
			vals[6][1] = waveformType_Str;

			vals[7][0] = "modulation_freq";
			vals[7][1] = internalModulationFreq_Str;

			vals[8][0] = "modulation_fm_deviation";
			vals[8][1] = fmDeviation_Str;

			vals[9][0] = "modulation_am_depth";
			vals[9][1] = amDepth_Str;

			vals[10][0] = "modulation_pulse_period";
			vals[10][1] = pulsePeriod_Str;

			vals[11][0] = "modulation_pulse_width";
			vals[11][1] = pulseWidth_Str;

			vals[12][0] = "sweep_start_stop";
			vals[12][1] = sweepOnOff_Str;

			vals[13][0] = "sweep_start";
			vals[13][1] = startFrequency_Str;

			vals[14][0] = "sweep_stop";
			vals[14][1] = stopFrequency_Str;

			vals[15][0] = "sweep_step";
			vals[15][1] = stepFrequency_Str;

			vals[16][0] = "sweep_dwell";
			vals[16][1] = dwellTime_Str;

			vals[17][0] = "sweep_trigger";
			vals[17][1] = sweepTriggerMode_Str;

			vals[18][0] = "reference_int_ext";
			vals[18][1] = referenceIntOrExt_Str;

			vals[19][0] = "reference_tcxo_ocxo";
			vals[19][1] = referenceTcxoOrOcxo_Str;

			vals[20][0] = "wifi_mode";
			vals[20][1] = wifiMode_Str;

			vals[21][0] = "wifi_sta_ssid";
			vals[21][1] = staModeSSID_Str;

			vals[22][0] = "wifi_sta_password";
			vals[22][1] = staModePassword_Str;

			vals[23][0] = "wifi_ap_ssid";
			vals[23][1] = apModeSSID_Str;

			vals[24][0] = "wifi_ap_password";
			vals[24][1] = apModePassword_Str;

			vals[25][0] = "wifi_ip_address";
			vals[25][1] = ipAddress_Str;

			vals[26][0] = "wifi_gateway_address";
			vals[26][1] = gatewayAddress_Str;

			vals[27][0] = "wifi_subnet_address";
			vals[27][1] = subnetAddress_Str;

			String read = getJSON(vals, 28);

			Serial1.print(read);
			Serial.println(read);
		}
		else if (commandBuffer[2] == 'D') //Read Diagnostic
		{
			if (is_modulation_stopped || (!is_modulation_stopped && modType == Pulse_Mod)) 
			{
				delay(10);
				RSSI_Str = Serial1.readString();
				RSSI_Str.remove(0, 1);
			}
			else
			{
				RSSI_Str = Serial1.readString();
			}

			ADC->ADC_CHER = 0xC0; // Enable ADC on pin A0 and A1
			while ((ADC->ADC_ISR & 0x80) == 0x00); // Wait for conversion on pin A0
			cur = ADC->ADC_CDR[7];
			while ((ADC->ADC_ISR & 0x40) == 0x00); // Wait for conversion on pin A1
			volt = ADC->ADC_CDR[6];

			String vals[11][2];

			vals[0][0] = "temperature";
			vals[0][1] = tempRead();

			vals[1][0] = "lock_xtal";
			vals[1][1] = String(digitalRead(ADF4002_LD));

			vals[2][0] = "lock_lmx1";
			vals[2][1] = String(digitalRead(LMX1_MUXOUT));

			vals[3][0] = "lock_lmx2";
			vals[3][1] = String(digitalRead(LMX2_MUXOUT));

			vals[4][0] = "current";
			vals[4][1] = String(((cur * 3.25) / 4096) / 1.5, 2);

			vals[5][0] = "voltage";
			vals[5][1] = String((volt * 3.25) / 4096 * 5.02, 2);

			vals[6][0] = "rssi";
			vals[6][1] = RSSI_Str;

			vals[7][0] = "em";
			vals[7][1] = embeddedVersion_Str;

			vals[8][0] = "ESP8266em";
			vals[8][1] = ESP8266FirmwareVersion_Str;

			vals[9][0] = "serial_number";
			vals[9][1] = serialNumber_Str;

			vals[10][0] = "model";
			vals[10][1] = ERASynthModel_Str;

			String read = getJSON(vals, 11);

			Serial1.print(read);
			Serial.println(read);
		}
		else if (commandBuffer[2] == 'C') 
		{	
			//Analog Readings current
			ADC->ADC_CHER = 0x80;					// Enable ADC on pin A0
			while ((ADC->ADC_ISR & 0x80) == 0x00);  // Wait for conversion on A0 pin
			cur = ADC->ADC_CDR[7];
			Serial.println(String(((cur * 3.25) / 4096) / 1.5, 2));
		}
		else if (commandBuffer[2] == 'V') 
		{			
			//Analog Readings voltage
			ADC->ADC_CHER = 0x40;					// Enable ADC on pin A1
			while ((ADC->ADC_ISR & 0x40) == 0x00);  // Wait for conversion on A1 pin
			volt = ADC->ADC_CDR[6];
			Serial.println(String((volt * 3.25) / 4096 * 5.02, 2));
		}
		else if (commandBuffer[2] == '0') 
		{
			//Reference Lock Status
			Serial.println(String(digitalRead(ADF4002_LD)));
		}
		else if (commandBuffer[2] == '1') 
		{
			//LMX1 Lock Status
			Serial.println(String(digitalRead(LMX1_MUXOUT)));
		}
		else if (commandBuffer[2] == '2') 
		{
			//LMX2 Lock Status
			Serial.println(String(digitalRead(LMX2_MUXOUT)));
		}
		else if (commandBuffer[2] == 'R') 
		{
			//Read RSSI from ESP8266
			Serial1.println("<R");
		}
		else if (commandBuffer[2] == 'T') 
		{
			//Temperature Reading
			Serial.println(tempRead());
		}
		else if (commandBuffer[2] == 'E') 
		{
			//Read Embedded Version
			Serial1.println(embeddedVersion_Str); 
			Serial.println(embeddedVersion_Str);
		}
		else if (commandBuffer[2] == '8') 
		{
			//Read ESP8266 Embedded Version
			Serial1.println("<E");
		}
		else if (commandBuffer[2] == 'S')
		{
			//Read Serial Number 
			Serial.println(serialNumber_Str);
		}
		else if (commandBuffer[2] == 'M') 
		{
			//Read ERASynth Model
			if (ERASynthModel_Str == "0") { Serial.println("ERASynth"); }
			else if (ERASynthModel_Str == "1") { Serial.println("ERASynth+"); }
			else if (ERASynthModel_Str == "2") { Serial.println("ERASynth++"); }
		}
		else if (commandBuffer[2] == 'W') 
		{
			//Read ESP8266 ON/OFF (0=OFF; 1=ON) 
			Serial.println(esp8266OnOff_Str);
		}
		break;

	case 'U': 
		
		//ESP8266 UART (UploadCode) Mode will be activated      
		isUploadCodeModeActive = true;
		digitalWrite(Wi_Fi_Flash, LOW);//UART (UploadCode)
		delay(100);
		digitalWrite(Wi_Fi_RST, LOW);
		delay(100);
		digitalWrite(Wi_Fi_RST, HIGH);
		delay(100);
		digitalWrite(Wi_Fi_Flash, HIGH); //The ESP8266 is now in flash mode. It is able to upload the sketch.
		Serial.println("The ESP8266 can be programmed! Please close the COM Port");
		delay(100);
		Serial.begin(57600); //initialize Serial Comm.(RX0, TX0)

		stopAllMod(); //Turn off Modulations
		rfOnOff(0); //Turn off ERASynth
		break;

	case 'X': 

		if (commandBuffer[2] == 'I') 
		{
			isInitESP8266Done = true;
			Serial.println("ESP is initiated...");
		}
		else 
		{
			//ESP8266 Reset will be activated
			digitalWrite(Wi_Fi_RST, LOW);
			delay(100);
			digitalWrite(Wi_Fi_RST, HIGH);
			delay(500);
			Serial1.println("<A");
			Serial.println("ESP8266 RESET is done!");
		}
		
		break;

	default:;
	}
}
