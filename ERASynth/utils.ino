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

static inline void delayNanoseconds(uint32_t nsec) 
{
	/*
	* Based on Paul Stoffregen's implementation
	* for Teensy 3.0 (http://www.pjrc.com/)
	*/

	if (nsec == 0) return;
	
	// upper limit for delay here is 2^32 / 100 =  42 million nanoseconds (42 milliseconds). 
	// If you need a longer delay you'll have to use delayMicroseconds or delay.
	uint32_t n = (nsec * 100) / 3571; 
	asm volatile(
		"L_%=_delayNanos:"       "\n\t"
		"subs   %0, #1"                 "\n\t"
		"bne    L_%=_delayNanos" "\n"
		: "+r" (n) :
		);
}

void setDAC(int value, int pin_LE)
{
	if (value < 0) { value = 0; }
	else if (value > 4095) { value = 4095; }

	byte DAC[2] = { 0 };

	value = value << 2;

	DAC[1] = (byte)value; //LSB of DAC_Value
	DAC[0] = (byte)(value >> 8); //MSB of DAC_Value

	if (pin_LE == DAC0_LE) { soft_spiWrite_DAC0(DAC, 2); }
	else if (pin_LE == DAC4_LE) { soft_spiWrite_DAC4(DAC, 2); }
	else if (pin_LE == DAC3_LE) { soft_spiWrite_DAC3(DAC, 2); }
	else { soft_spiWrite_DAC(DAC, 2, pin_LE); }
}

void sweepRoutine()
{
	nextFreq = true;
}

void modulation_paused()
{
	is_modulation_paused = true;
}

void pulse_changed()
{
	if (!pulse_condition) 
	{
		setDAC(DACValue, DAC1_LE);
		setDAC(DACValue, DAC2_LE);
		setDAC(DACValue, DAC3_LE);
	}
	else 
	{
		setDAC(max_DAC_Value, DAC1_LE);
		setDAC(max_DAC_Value, DAC2_LE);
		setDAC(max_DAC_Value, DAC3_LE);
	}

	pulse_condition = !pulse_condition;
	is_pulse_changed = true;
}

void pulse_falling()
{
	setDAC(max_DAC_Value, DAC1_LE);
	setDAC(max_DAC_Value, DAC2_LE);
	setDAC(max_DAC_Value, DAC3_LE);

	isPulseRising = true;
}

void pulse_rising()
{
	setDAC(DACValue, DAC1_LE);
	setDAC(DACValue, DAC2_LE);
	setDAC(DACValue, DAC3_LE);

	isPulseRising = false;
}

void setMinAmplitude()
{
	//Fix Attanuation for protecting Amplifiers from over drive
	setDAC(max_DAC_Value, DAC1_LE); 
	setDAC(max_DAC_Value, DAC2_LE);
	setDAC(max_DAC_Value, DAC3_LE);
}

void facReset()
{
	command(">F1000000000");
	command(">A0");
	command(">S11000000000");
	command(">S26000000000");
	command(">S31000000000");
	command(">S41000");
	command(">S00");
	command(">SS0");
	command(">P01");
	command(">P10");
	command(">P50");
	command(">P81");
	command(">PA0");
	command(">PE01");
	command(">PEW1");
	command(">PES0ERA");
	command(">PEP0era19050");
	command(">PES1ERASynth");
	command(">PEP1erainstruments");
	command(">PEI192.168.001.150");
	command(">PEG192.168.001.001");
	command(">PEN255.255.255.000");
	command(">MS0");
	command(">M00");
	command(">M10");
	command(">M20");
	command(">M31000");
	command(">M45000");
	command(">M520");
	command(">M62000");
	command(">M71000");
}

void ledBlink()
{
	if (ledState == LOW) { ledState = HIGH; }
	else { ledState = LOW; }
	digitalWrite(LED, ledState);
}

void preset_ERASynth()
{
	command(">F1000000000");
	command(">A0");
	command(">S11000000000");
	command(">S26000000000");
	command(">S31000000000");
	command(">S41000");
	command(">S00");
	command(">SS0");
	command(">P01");
	command(">P10");
	command(">P50");
	command(">PA0");
	command(">M00");
	command(">M10");
	command(">M20");
	command(">M31000");
	command(">M45000");
	command(">M520");
	command(">M62000");
	command(">M71000");
	command(">MS0");
}

void serialEvent()
{
	char rx_byte = 0;
	while (Serial.available() > 0) 
	{
		rx_byte = Serial.read();

		if (rx_byte == '\r' || rx_byte == '\n') { serialActivityExist = true; }

		if (!isUploadCodeModeActive) 
		{
			if (rx_byte == '>' || isCmdExist) 
			{
				isCmdExist = true;
				if (rx_byte != '\r' && rx_byte != '\n') cmdString += rx_byte;
			}
			
			if (rx_byte == '\r' && isCmdExist) 
			{
				stringComplete = true;
			}
		}
		else
		{
			Serial1.print(rx_byte);
		}
	}
}

void serialEvent1()
{
	char rx_byte = 0;
	while (Serial1.available() > 0) 
	{
		rx_byte = Serial1.read();
		Serial.print(rx_byte);

		if (rx_byte == '\r' || rx_byte == '\n') { serial1ActivityExist = true; }

		if (!isUploadCodeModeActive) 
		{
			if (rx_byte == '>' || isCmd1Exist) 
			{
				isCmd1Exist = true;
				if (rx_byte != '\r' && rx_byte != '\n') cmd1String += rx_byte;
			}
			
			if (rx_byte == '\r' && isCmd1Exist) { string1Complete = true; }
		}
	}
}

void setDDSPowerLevel(uint16_t level)
{
	DDS_DAC_update[4] = (uint8_t)level; //Lower 8 bits
	DDS_DAC_update[3] = (uint8_t)(level >> 8) + 16; //Higher 2 bits
	digitalWrite(DDS_IO_UPD, LOW);
	spiWrite(DDS_DAC_update, 5, DDS_LE);
	digitalWrite(DDS_IO_UPD, HIGH);
}

void stopAllMod()
{
	//NB_FM and WB_FM Disabled. Normal Operation
	digitalWrite(NB_FM_En, LOW); 
	digitalWrite(WB_FM_En, LOW); 

	spiWrite_LMX(&LMX2_register[112 - 12], LMX2_LE);
	spiWrite_LMX(&LMX2_register[112 - 11], LMX2_LE);
	spiWrite_LMX(&LMX2_register[112 - 34], LMX2_LE);
	spiWrite_LMX(&LMX2_R36_update, LMX2_LE);
	spiWrite_LMX(&LMX2_register[112 - 14], LMX2_LE);
	
	detachInterrupt(Trig_Inp);
	Timer2.stop();

	rfOnOff(rfOnOff_Str.toInt());
}

void sweepERASynth()
{
	nextFreq = false;
	lastFrequency = startFrequency + stepFrequency * sweepIndex++;
	setMinAmplitude();
	setFreqBand(lastFrequency);
	setFreqParam(lastFrequency);
	setAmplitude();

	if (sweepIndex == number_of_sweep_points) { sweepIndex = 0; }
}

void setFRAM(uint16_t vars[], String str)
{
	int i = 0;
	char FRAM_char_array[65] = { 0 };
	for (i = 0; i < ((int)(vars[1] - str.length())); i++) { FRAM_char_array[i] = '0'; }
	str.toCharArray(&FRAM_char_array[i], str.length() + 1); 
	spiWriteFRAM(vars[0], FRAM_char_array, vars[1]); 
}

String getFRAM(uint16_t vars[])
{
	String str = "";
	char FRAM_char_array[65] = { 0 };
	spiReadFRAM(vars[0], FRAM_char_array, vars[1]);
	for (int i = 0; i < vars[1]; i++) { str += FRAM_char_array[i]; }
	return str;
}

void UINT32to4Bytes_DDS(uint8_t* p_bytes_u8, const uint32_t* p_data_u32)
{
	p_bytes_u8[4] = ((uint8_t*)p_data_u32)[0];
	p_bytes_u8[3] = ((uint8_t*)p_data_u32)[1];
	p_bytes_u8[2] = ((uint8_t*)p_data_u32)[2];
	p_bytes_u8[1] = ((uint8_t*)p_data_u32)[3];
}

void UINT32to3Bytes(uint8_t* p_bytes_u8, const uint32_t* p_data_u32)
{
	p_bytes_u8[2] = ((uint8_t*)p_data_u32)[0];
	p_bytes_u8[1] = ((uint8_t*)p_data_u32)[1];
	p_bytes_u8[0] = ((uint8_t*)p_data_u32)[2];
}

float myMap(float x, float in_min, float in_max, float out_min, float out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

String getJSON(String input[][2], uint8_t rows)
{
	String vals = "{";
	for (int i = 0; i < rows; i++)
	{
		String key = input[i][0];
		String val = input[i][1];

		vals += "\"" + key + "\":\"" + val + "\",";
	}
	vals.remove(vals.length() - 1, 1);
	vals += "}";
	return vals;
}