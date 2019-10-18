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

void amMod()
{
	uint8_t am_depth = amDepth_Str.toInt();
	int intModFreq = internalModulationFreq_Str.toInt();
	uint8_t decimation_value = ceil((float)intModFreq / 1000);
	uint16_t readValue = 0;
	int delay_ns_amount = 0;

	// Initial DAC Value. In other words, value for max amplitude while depth is 100. 
	// DACValue is retrieved from amplitude calibration.
	int valueOfDAC = DACValue;

	int amArray[maxSamplesNum] = { 0 };
	float sample = 0;
	float max_voltage = 1;
	float min_voltage = 0.01;
	float center_voltage;
	float lower_bound;
	float upper_bound;
	float log_lower_bound;
	float log_upper_bound;
	float min_log_lower_bound;
	float max_log_upper_bound;
	int waveform[maxSamplesNum] = { 0 };

	attachInterrupt(0, modulation_paused, CHANGE);
	attachInterrupt(19, modulation_paused, CHANGE);

	// If AM Depth is more than 98%, Voltage value is going be less then 0.01
	// When AM Depth is 100%, lower_bound value is 0 thus log will be infinite.
	if (am_depth > 98) { am_depth = 98; }

	center_voltage = (max_voltage - min_voltage) / 2;
	lower_bound = center_voltage - (center_voltage * am_depth / 100);
	upper_bound = center_voltage + (center_voltage * am_depth / 100);
	log_lower_bound = 10 + 20 * log10(lower_bound);
	log_upper_bound = 10 + 20 * log10(upper_bound);
	min_log_lower_bound = 10 + 20 * log10(min_voltage);
	max_log_upper_bound = 10 + 20 * log10(max_voltage);

	if (modSource == Internal)
	{
		switch (waveformType)
		{
		case Sine:	 for (int i = 0; i < maxSamplesNum; i++) { waveform[i] = sine_waveform[i]; } break;
		case Triangle: for (int i = 0; i < maxSamplesNum; i++) { waveform[i] = triangular_waveform[i]; } break;
		case Ramp:	 for (int i = 0; i < maxSamplesNum; i++) { waveform[i] = ramp_waveform[i]; } break;
		case Square:	 for (int i = 0; i < maxSamplesNum; i++) { waveform[i] = square_waveform[i]; } break;
		}

		for (int i = 0; i < maxSamplesNum; i++)
		{
			sample = myMap((float)waveform[i], 0, 4095, lower_bound, upper_bound); // Voltage calculated.
			sample = 10 + 20 * log10(sample); // Convert voltages to dB
			amArray[i] = (max_log_upper_bound - sample) * 46.5454; // Slope of RFSA2113 is 46.5454
		}

		delay_ns_amount = (1e9 / (intModFreq * ceil((float)maxSamplesNum / decimation_value))) - default_delay_in_loop_AM;

		// "delayMicroseconds" function works very accurately in the range 3 microseconds and up.
		// Arduino cannot assure that delayMicroseconds will perform precisely for smaller delay-times. 
		if (delay_ns_amount < 0) { delay_ns_amount = 0; }

		if (isDebugEnabled)
		{
			Serial.print("AM Mod Started with Internal ");
			Serial.print(intModFreq);
			Serial.print("Hz and with ");
			Serial.print(am_depth);
			Serial.println("% AM Depth");
		}

		is_modulation_paused = false;
		SPI.beginTransaction(SPISettings(31e6, MSBFIRST, SPI_MODE1));

		while (1)
		{
			for (int i = 0; i < maxSamplesNum; i = i + decimation_value)
			{
				setDAC(valueOfDAC + amArray[i], DAC3_LE);
				delayNanoseconds(delay_ns_amount);

				if (nextFreq) { sweepERASynth(); }

				if (is_modulation_paused)
				{
					detachInterrupt(0);
					detachInterrupt(19);
					SPI.endTransaction();
					return;
				}

			}
		}

	}

	if (modSource == External)
	{
		if (isDebugEnabled)
		{
			Serial.println("AM Mod Started with External Modulation Input");
		}

		is_modulation_paused = false;
		SPI.beginTransaction(SPISettings(31e6, MSBFIRST, SPI_MODE1));

		uint16_t sampleArray[4096];

		for (uint16_t i = 0; i < 4096; i++)
		{
			sample = myMap((float)i, 0, 4095, lower_bound, upper_bound); // Voltage is calculated.        
			sample = 10 + 20 * log10(sample); // Convert voltages to dB.
			sample = (max_log_upper_bound - sample) * 46.5454; // Slope of RFSA2113 is 46.5454.
			sampleArray[i] = valueOfDAC + (int)sample;
		}
		
		while (1)
		{
			while ((ADC->ADC_ISR & 0x01) == 0x00); // Wait for conversion on A7 pin
			uint16_t adc = ADC->ADC_CDR[0];

			setDAC(sampleArray[adc], DAC3_LE);

			if (nextFreq) { sweepERASynth(); }
			if (is_modulation_paused)
			{
				detachInterrupt(0);
				detachInterrupt(19);
				SPI.endTransaction();
				return;
			}
		}
	}

	if (modSource == Microphone)
	{
		if (isDebugEnabled) { Serial.println("AM Mod Started with Microphone Modulation Input"); }

		is_modulation_paused = false;
		SPI.beginTransaction(SPISettings(31e6, MSBFIRST, SPI_MODE1));

		uint16_t sampleArray[4096];

		for (uint16_t i = 0; i < 4096; i++)
		{
			sample = myMap((float)i, 0, 4095, lower_bound, upper_bound); // Voltage is calculated
			sample = 10 + 20 * log10(sample); // Convert voltages to dB. 
			sample = (max_log_upper_bound - sample) * 46.5454; // Slope of RFSA2113 is 46.5454      
			sampleArray[i] = valueOfDAC + (int)sample;
		}

		while (1)
		{
			while ((ADC->ADC_ISR & 0x02) == 0x00); // Wait for conversion on A6 pin
			uint16_t adc = ADC->ADC_CDR[1];
			setDAC(sampleArray[adc], DAC3_LE);

			if (nextFreq) { sweepERASynth(); }

			if (is_modulation_paused)
			{
				detachInterrupt(0);
				detachInterrupt(19);
				SPI.endTransaction();
				return;
			}

		}
	}
}