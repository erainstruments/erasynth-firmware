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

void setAmplitude()
{
	uint16_t LMX2_Amp_Index = 0;
	uint16_t LMX2_Pwr_Level = 0;
	uint16_t index_freq		= 0;
	uint16_t index_amp_low	= 0;
	uint16_t index_amp_high = 0;
	float amplitude_frac	= 0.0;
	uint16_t DACValue_low	= 0;
	uint16_t DACValue_high	= 0;


	// Find first index of Calibration_LUT_n
	if (lastFrequency > 30e6) 
	{
		index_freq = (uint16_t)round((lastFrequency / 1e7) - 2);

		if (index_freq == 1) 
		{
			//30e6 belongs to DC-30MHz band. So changed with the 40e6 calibration value 
			index_freq = index_freq + 1; 
		}

		if (ERASynth != 0 && lastFrequency > 4500e6 && lastFrequency < 4505e6) 
		{
			//4500e6 < freq < 4505e6 belongs to Upper band. So changed with the next calibration value
			index_freq = index_freq + 1; 
		}

		if (ERASynth == 2 && lastFrequency > 15000e6) 
		{
			if (index_freq == 1498) 
			{
				// 15000e6 < freq < 15005e6 belongs to doubler path of LMX2595. 
				// So changed with right calibration value 
				index_freq = 0; 
			}
			else 
			{
				index_freq = index_freq - 1499;
			}
		}

		// Find LMX2 output power level for >30MHz  
		LMX2_Amp_Index = (uint16_t)round((lastFrequency / 1e7) - 3);

		if (ERASynth != 0 && lastFrequency > 4500e6 && lastFrequency < 4505e6) 
		{
			// 4500e6<freq<4505e6 belongs to Upper band. 
			// So changed with the next calibration value
			LMX2_Amp_Index = LMX2_Amp_Index + 1; 
		}

		if (ERASynth == 0 && lastFrequency > 2800e6) 
		{ 
			// If freq > 2800MHz in ERASynth
			LMX2_Pwr_Level = 63;
		}
		else if (ERASynth == 2 && lastFrequency > 15000e6) 
		{ 
			// If freq > 15000MHz in ERASynth++
			if (lastFrequency < 19000e6) 
			{
				LMX2_Pwr_Level = 63;
			}
			else
			{
				LMX2_Pwr_Level = 20;
			}
		}
		else
		{
			LMX2_Pwr_Level = LMX2_calibration[LMX2_Amp_Index];
		}


		LMX2_R44_update = (LMX2_Pwr_Level << 8) + 2883712;

	}
	else
	{ 
		// Below 30MHz
		// First of all, mute the output    
		setDDSPowerLevel(0);

		// Find first index of Calibration_LUT_n below 30MHz
		if (lastFrequency >= 25e6) { index_freq = 1; }
		else { index_freq = 0; }

		//unleash the power 30MHz>freq>1MHz     
		if (lastFrequency >= 1e6) { setDDSPowerLevel(1023); } 
		else 
		{
			// Limit the amplitude below 1MHz
			setDDSPowerLevel(400); //limit the power <1MHz 11dBm typ.
			amplitude = amplitude + 8; //Amplitude value is different below 1MHz.
			if (amplitude > 20) { amplitude = 20; }
		}
	}

	//Calculate the interpolation points
	index_amp_low = floor(20 - amplitude);
	index_amp_high = ceil(20 - amplitude);
	amplitude_frac = amplitude - floor(amplitude);


	if (lastFrequency <= 15000e6) 
	{
		if (amplitude >= -20) 
		{
			DACValue_low = pgm_read_word(&Calibration_LUT_1[index_freq][index_amp_high]);
			DACValue_high = pgm_read_word(&Calibration_LUT_1[index_freq][index_amp_low]);
		}
		else
		{
			LMX2_R44_update = 0x2C0080; //Below -20dBm calibration is done with the mininum LMX2 output power
			index_amp_low = index_amp_low - 40;
			index_amp_high = index_amp_high - 40;
			DACValue_low = pgm_read_word(&Calibration_LUT_2[index_freq][index_amp_high]);
			DACValue_high = pgm_read_word(&Calibration_LUT_2[index_freq][index_amp_low]);
		}
	}

	#if ERASynth == 2 
	if (lastFrequency > 15000e6) 
	{
		if (amplitude >= -20) 
		{
			DACValue_low = pgm_read_word(&Calibration_LUT_3[index_freq][index_amp_high]);
			DACValue_high = pgm_read_word(&Calibration_LUT_3[index_freq][index_amp_low]);
		}
		else 
		{
			LMX2_R44_update		= 0x2C0080; //Below -20dBm calibration is done with the mininum LMX2 output power
			index_amp_low		= index_amp_low - 40;
			index_amp_high		= index_amp_high - 40;
			DACValue_low		= pgm_read_word(&Calibration_LUT_4[index_freq][index_amp_high]);
			DACValue_high		= pgm_read_word(&Calibration_LUT_4[index_freq][index_amp_low]);
		}
	}
	#endif

	DACValue = DACValue_low + (DACValue_high - DACValue_low) * amplitude_frac;

	// Recall the right amplitude value below 1MHz. Because amplitude is changed above
	if (lastFrequency < 1e6) { amplitude = lastAmplitude; }

	// Set the calculated amplitude variables.   
	spiWrite_LMX(&LMX2_R44_update, LMX2_LE); // calibration value of LMX2 output power
											 // calibration value of three DACs
	setDAC(DACValue, DAC1_LE);
	setDAC(DACValue, DAC2_LE);
	setDAC(DACValue, DAC3_LE);
}