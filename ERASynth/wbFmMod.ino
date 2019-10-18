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


void wbFmMod()
{
	int intModFreq = internalModulationFreq_Str.toInt();
	int fm_dev = fmDeviation_Str.toInt();
	int delay_ns_amount = 0;
	double Kvco = 78750; // Hz/[0..4095] @7500MHz-15000MHz 
	double Kvco1 = 0;
	double Kvco2 = 0;
	int VCO_freq = (lastFrequency / 1e6) * ODIV; // in MHz
	int VCO_freq1 = 0;
	int VCO_freq2 = 0;
	float corr_factor = 0.75;

	if (7500 <= VCO_freq && VCO_freq < 8600)
	{
		Kvco1 = 59; VCO_freq1 = 7500;
		Kvco2 = 89; VCO_freq2 = 8600;
	}
	else if (8600 <= VCO_freq && VCO_freq < 9800)
	{
		Kvco1 = 59; VCO_freq1 = 8600;
		Kvco2 = 89; VCO_freq2 = 9800;
	}
	else if (9800 <= VCO_freq && VCO_freq < 10800)
	{
		Kvco1 = 59; VCO_freq1 = 9800;
		Kvco2 = 89; VCO_freq2 = 10800;
	}
	else if (10800 <= VCO_freq && VCO_freq < 12000)
	{
		Kvco1 = 59; VCO_freq1 = 10800;
		Kvco2 = 89; VCO_freq2 = 12000;
	}
	else if (12000 <= VCO_freq && VCO_freq < 12900)
	{
		Kvco1 = 59; VCO_freq1 = 12000;
		Kvco2 = 89; VCO_freq2 = 12900;
	}
	else if (12900 <= VCO_freq && VCO_freq < 13900)
	{
		Kvco1 = 59; VCO_freq1 = 12900;
		Kvco2 = 89; VCO_freq2 = 13900;
	}
	else if (13900 <= VCO_freq && VCO_freq <= 15000)
	{
		Kvco1 = 59; VCO_freq1 = 13900;
		Kvco2 = 89; VCO_freq2 = 15000;
	}

	Kvco = Kvco1 + (Kvco2 - Kvco1)*(VCO_freq - VCO_freq1) / (VCO_freq2 - VCO_freq1); // In MHz/V
	Kvco = Kvco * 1e6 * 3.25 / 4096; // In Hz/[0..4095]

	double modBW = ((2 * fm_dev) / (Kvco / ODIV)) * corr_factor; // In DAC steps 0..4095
	if (modBW > 4096) { modBW = 4096; }
	double denum = 4096 / modBW;
	int calValueForDAC_internal = 0;
	int calValueForDAC_external = 0;
	int calValueForDAC_microphone = 0;
	int sum_external = 0;
	uint8_t decimation_value = ceil((float)intModFreq / 1000);
	uint16_t readValue = 0;
	int waveform[maxSamplesNum] = { 0 };

	attachInterrupt(0, modulation_paused, CHANGE);
	attachInterrupt(19, modulation_paused, CHANGE);

	if (modSource == Internal)
	{
		switch (waveformType)
		{
			case Sine:		for (int i = 0; i < maxSamplesNum; i++) { waveform[i] = sine_waveform[i]; }			break;					
			case Triangle:	for (int i = 0; i < maxSamplesNum; i++) { waveform[i] = triangular_waveform[i]; }	break;		
			case Ramp:		for (int i = 0; i < maxSamplesNum; i++) { waveform[i] = ramp_waveform[i]; }			break;			
			case Square:	for (int i = 0; i < maxSamplesNum; i++) { waveform[i] = square_waveform[i]; }		break;			
		}

		for (int i = 0; i < maxSamplesNum; i++) 
		{
			calculated_waveform[i] = ((waveform[i] / denum) + ((2048 + calValueForDAC_internal) - (modBW / 2)));
		}

		delay_ns_amount = (1e9 / (intModFreq * ceil((float)maxSamplesNum / decimation_value))) - default_delay_in_loop_WBFM;

		if (delay_ns_amount < 0) { delay_ns_amount = 0; }

		if (isDebugEnabled) 
		{
			Serial.print("WBFM Mod Started with internal "); 
			Serial.print(intModFreq); 
			Serial.print("Hz and with "); 
			Serial.print(fm_dev); 
			Serial.println("Hz FM Deviation");
		}

		is_modulation_paused = false;
		SPI.beginTransaction(SPISettings(31e6, MSBFIRST, SPI_MODE1));

		while (1) 
		{
			for (int i = 0; i < maxSamplesNum; i = i + decimation_value) 
			{
				setDAC(calculated_waveform[i], DAC4_LE);
				delayNanoseconds(delay_ns_amount);
				
				if (nextFreq) { sweepERASynth(); }
				
				if (is_modulation_paused) 
				{
					SPI.endTransaction();
					detachInterrupt(0);
					detachInterrupt(19);
					digitalWrite(WB_FM_En, LOW); //WB_FM Disabled. Normal Operation
					spiWrite_LMX(&LMX2_register[112 - 12], LMX2_LE);
					spiWrite_LMX(&LMX2_register[112 - 11], LMX2_LE);
					spiWrite_LMX(&LMX2_register[112 - 34], LMX2_LE);
					spiWrite_LMX(&LMX2_R36_update, LMX2_LE);
					spiWrite_LMX(&LMX2_register[112 - 14], LMX2_LE);
					spiWrite_LMX(&LMX2_R0, LMX2_LE);
					return;
				}
			}
		} 
	}

	if (modSource == External) 
	{
		if (isDebugEnabled) { Serial.println("WBFM Mod Started with External Modulation Input"); }
		sum_external = 2048 + calValueForDAC_external - (modBW / 2);
		is_modulation_paused = false;
		SPI.beginTransaction(SPISettings(31e6, MSBFIRST, SPI_MODE1));
		
		uint16_t sampleArray[4096];

		for (uint16_t i = 0; i < 4096; i++)
		{
			sampleArray[i] = (int)(i / (int)denum) + sum_external;
		}

		while (1) 
		{
			while ((ADC->ADC_ISR & 0x01) == 0x00); // Wait for conversion on A7 pin
			uint16_t adc = ADC->ADC_CDR[0];
			setDAC(sampleArray[adc], DAC4_LE);

			if (nextFreq) { sweepERASynth(); }

			if (is_modulation_paused) 
			{
				SPI.endTransaction();
				detachInterrupt(0);
				detachInterrupt(19);
				digitalWrite(WB_FM_En, LOW); //WB_FM Disabled. Normal Operation
				spiWrite_LMX(&LMX2_register[112 - 12], LMX2_LE);
				spiWrite_LMX(&LMX2_register[112 - 11], LMX2_LE);
				spiWrite_LMX(&LMX2_register[112 - 34], LMX2_LE);
				spiWrite_LMX(&LMX2_R36_update, LMX2_LE);
				spiWrite_LMX(&LMX2_register[112 - 14], LMX2_LE);
				spiWrite_LMX(&LMX2_R0, LMX2_LE);
				return;
			}
		}
	}

	if (modSource == Microphone) 
	{
		if (isDebugEnabled) { Serial.println("WBFM Mod Started with Microphone Input"); }
		sum_external = 2048 + calValueForDAC_microphone - (modBW / 2);
		is_modulation_paused = false;
		SPI.beginTransaction(SPISettings(31e6, MSBFIRST, SPI_MODE1));
		
		uint16_t sampleArray[4096];

		for (uint16_t i = 0; i < 4096; i++)
		{
			sampleArray[i] = (int)(i / (int)denum) + sum_external;
		}

		while (1) 
		{
			while ((ADC->ADC_ISR & 0x02) == 0x00); // Wait for conversion on A6 pin
			uint16_t adc = ADC->ADC_CDR[1];
			setDAC(sampleArray[adc], DAC4_LE);

			if (nextFreq) { sweepERASynth(); }

			if (is_modulation_paused) 
			{
				SPI.endTransaction();
				detachInterrupt(0);
				detachInterrupt(19);
				digitalWrite(WB_FM_En, LOW); //WB_FM Disabled. Normal Operation
				spiWrite_LMX(&LMX2_register[112 - 12], LMX2_LE);
				spiWrite_LMX(&LMX2_register[112 - 11], LMX2_LE);
				spiWrite_LMX(&LMX2_register[112 - 34], LMX2_LE);
				spiWrite_LMX(&LMX2_R36_update, LMX2_LE);
				spiWrite_LMX(&LMX2_register[112 - 14], LMX2_LE);
				spiWrite_LMX(&LMX2_R0, LMX2_LE);
				return;
			}
		}
	}
}