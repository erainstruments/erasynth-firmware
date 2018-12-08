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

void pulseMod()
{
	pulsePeriod = pulsePeriod_Str.toInt(); // - default_delay_in_Pulse_Mod - default_delay_in_Pulse_Mod - 30;
	pulseWidth = pulseWidth_Str.toInt();   // - default_delay_in_Pulse_Mod;

	if (modSource == Internal)
	{
		if (pulsePeriod < pulseWidth) { pulsePeriod = pulseWidth + default_delay_in_Pulse_Mod; }

		if ((pulsePeriod - pulseWidth) < default_delay_in_Pulse_Mod)  { pulsePeriod += (default_delay_in_Pulse_Mod - (pulsePeriod - pulseWidth)); }

    //
    // while loop add exstra delay so we need to shorten the off time with calibration value
    //
		uint32_t offTime = pulsePeriod - pulseWidth - 25;
		
		command(">M6" + String(pulsePeriod));
		command(">M7" + String(pulseWidth));

		if (isDebugEnabled)
		{
			Serial.println("Pulse Mod Started with Internal Source");
			Serial.print("Pulse Width: ");  Serial.print(pulseWidth);  Serial.println("us");
			Serial.print("Pulse Period: "); Serial.print(pulsePeriod); Serial.println("us");
			Serial.print("Off Time : ");    Serial.print(offTime);     Serial.println("us");
		}
		
		while (isPulseActive)
		{	
			// ON
			soft_spiWrite_DAC1(DACValue);
			soft_spiWrite_DAC2(DACValue);
			soft_spiWrite_DAC3(DACValue);

			//setDAC(DACValue, DAC1_LE);
			//setDAC(DACValue, DAC2_LE);
			//setDAC(DACValue, DAC3_LE);
			delay_micro(pulseWidth);
			
			// OFF
			soft_spiWrite_DAC1(max_DAC_Value);
			soft_spiWrite_DAC2(max_DAC_Value);
			soft_spiWrite_DAC3(max_DAC_Value);

			//setDAC(max_DAC_Value, DAC1_LE);
			//setDAC(max_DAC_Value, DAC2_LE);
			//setDAC(max_DAC_Value, DAC3_LE);
			delay_micro(offTime);
			
			if (nextFreq) { sweepERASynth(); }

			serialEvent();
			serialEvent1();

			if (stringComplete)
			{
				stringComplete = false;
				command(cmdString);
				cmdString = "";
				isCmdExist = false;
				return;
			}

			if (string1Complete)
			{
				string1Complete = false;
				command(cmd1String);
				cmd1String = "";
				isCmd1Exist = false;
				return;				
			}
		}
	}

	if (modSource == External)
	{
		if (isDebugEnabled) { Serial.println("Pulse Mod Started with External Source"); }
		attachInterrupt(Trig_Inp, pulse_rising, RISING);
	}
}
