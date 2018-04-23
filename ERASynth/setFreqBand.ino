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

void setFreqBand(uint64_t freq)
{
	if (freq > BandPoint) { freqBand = UBand; }
	else if (freq > 30e6) { freqBand = MBand; }
	else { freqBand = LBand; }

	if (previousBand == freqBand) { return; }
	else if (freqBand == UBand)
	{
		//6000 to 20000 MHz
		if (previousBand == LBand && is_sweep_stopped)
		{
			// Turn on LMX2 incase previous path was LF Path
			spiWrite_LMX(&LMX2_R0, LMX2_LE); 
			delay(10);
		}

		digitalWrite(DDS_IO_UPD, LOW);
		spiWrite(DDS_CFR1_Pwr_Down, 5, DDS_LE); // Power down DDS
		digitalWrite(DDS_IO_UPD, HIGH);

		digitalWrite(SW3, LOW);				// 30MHz-6GHz path is selected
		digitalWrite(DC_6GHz_En, LOW);		// DC-6GHz Amp. is Disabled.
		digitalWrite(X6_15GHz_En, HIGH);	// 6-15GHz Amp. is Enabled.
		digitalWrite(SW4, HIGH);			// 6GHz-15GHz path is selected
	}
	else if (freqBand == MBand) 
	{ 
		//30 to 6000 MHz
		if (previousBand == LBand && is_sweep_stopped) 
		{
			// Turn on LMX2 incase previous path was LF Path
			spiWrite_LMX(&LMX2_R0, LMX2_LE); 
			delay(10);
		}

		digitalWrite(DDS_IO_UPD, LOW);
		spiWrite(DDS_CFR1_Pwr_Down, 5, DDS_LE); // Power down DDS
		digitalWrite(DDS_IO_UPD, HIGH);

		digitalWrite(SW3, LOW);			// 30MHz-6GHz path is selected.
		digitalWrite(DC_6GHz_En, HIGH); // DC-6GHz Amp. is enabled.
		digitalWrite(X6_15GHz_En, LOW); // 6-15GHz Amp. is disabled.
		digitalWrite(SW4, LOW);			// LF-6GHz path is selected
	}
	else if (freqBand == LBand) 
	{ 
		//LF to 30 MHz
		if (is_sweep_stopped) 
		{ 
			// if ERASynth is not in sweep mode
			// mute LMX2, it is unused
			spiWrite_LMX(&LMX2_R0_mute, LMX2_LE); 
		}

		digitalWrite(DDS_IO_UPD, LOW);
		spiWrite(init_DDS_CFR1, 5, DDS_LE); // Power-up DDS
		digitalWrite(DDS_IO_UPD, HIGH);

		digitalWrite(SW3, HIGH);			// LF-30MHz path is selected
		digitalWrite(DC_6GHz_En, HIGH);		// DC-6GHz Amp. is enabled.
		digitalWrite(X6_15GHz_En, LOW);		// 6-15GHz Amp. is disabled.
		digitalWrite(SW4, LOW);				// LF-6GHz path is selected
	}
	previousBand = freqBand;
}