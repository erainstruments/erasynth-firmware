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

void nbFmMod()
{
	int intModFreq				  = internalModulationFreq_Str.toInt();
	int fm_dev					  = fmDeviation_Str.toInt();
	int delay_ns_amount			  = 0;
	double Kvco					  = 2.25; // Hz/[0..4095] @100MHz
	double current_freq_100MHz	  = lastFrequency / 100e6;
	double modBW = (2 * fm_dev) / (Kvco * current_freq_100MHz); // in DAC steps 0..4095
	if (modBW > 4096) { modBW = 4096; }
	double denum				  = 4096 / modBW;
	int calValueForDAC_internal   = 80;
	int calValueForDAC_external   = 50;
	int calValueForDAC_microphone = 0;
	int sum_external			  = 0;
	uint8_t decimation_value	  = ceil((float)intModFreq / 1000);
	uint16_t readValue			  = 0;
	int waveform[maxSamplesNum]   = { 0 };

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

		delay_ns_amount = (1e9 / (intModFreq * ceil((float)maxSamplesNum / decimation_value))) - default_delay_in_loop_NBFM;

		if (delay_ns_amount < 0) { delay_ns_amount = 0; }

		if (isDebugEnabled) 
		{
			Serial.print("NBFM Mod Started with internal "); 
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
				setDAC(calculated_waveform[i], DAC0_LE);
				delayNanoseconds(delay_ns_amount);
				if (nextFreq) { sweepERASynth(); }
				
				if (is_modulation_paused) 
				{
					SPI.endTransaction();
					detachInterrupt(0);
					detachInterrupt(19);
					return;
				}
			}
		}
	}

	if (modSource == External) 
	{
		if (isDebugEnabled) { Serial.println("NBFM Mod Started with External Modulation Input"); }
		sum_external = 2048 + calValueForDAC_external - (modBW / 2);
		is_modulation_paused = false;
		SPI.beginTransaction(SPISettings(31e6, MSBFIRST, SPI_MODE1));
		
		while (1) 
		{
			while ((ADC->ADC_ISR & 0x01) == 0x00); // Wait for conversion on A7 pin
			setDAC((int)(ADC->ADC_CDR[0] / (int)denum) + sum_external, DAC0_LE);
			
			if (nextFreq) { sweepERASynth(); }
			
			if (is_modulation_paused) 
			{
				SPI.endTransaction();
				detachInterrupt(0);
				detachInterrupt(19);
				return;
			}
		}
	}

	if (modSource == Microphone) 
	{
		if (isDebugEnabled) { Serial.println("NBFM Mod Started with Microphone Input"); }
		sum_external = 2048 + calValueForDAC_microphone - (modBW / 2);
		is_modulation_paused = false;
		SPI.beginTransaction(SPISettings(31e6, MSBFIRST, SPI_MODE1));
		
		while (1) 
		{
			while ((ADC->ADC_ISR & 0x02) == 0x00); // Wait for conversion on A6 pin
			setDAC((int)(ADC->ADC_CDR[1] / (int)denum) + sum_external, DAC0_LE);

			if (nextFreq) { sweepERASynth(); }
			
			if (is_modulation_paused) 
			{
				SPI.endTransaction();
				detachInterrupt(0);
				detachInterrupt(19);
				return;
			}
		}
	}
}