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

void rfOnOff(int value)
{
	if (value == 0) //RF OFF
	{
		Serial.println("RF OFF");

		digitalWrite(DC_6GHz_En, LOW);  //DC-6GHz Amp. is Disabled.
		digitalWrite(X6_15GHz_En, LOW); //6-15GHz Amp. is Disabled.
		digitalWrite(SW3, LOW);			//30MHz-6GHz path is selected to turn off LF path amplifier

		digitalWrite(DDS_IO_UPD, LOW);
		spiWrite(DDS_CFR1_Pwr_Down, 5, DDS_LE); // Power down DDS
		digitalWrite(DDS_IO_UPD, HIGH);

		spiWrite_LMX(&LMX1_R0_mute, LMX1_LE);
		spiWrite_LMX(&LMX2_R0_mute, LMX2_LE);
	}
	else //RF ON 
	{
		Serial.println("RF ON");
		spiWrite_LMX(&LMX1_R0, LMX1_LE); // turn on LMX1
		delay(10);
		spiWrite_LMX(&LMX2_R0, LMX2_LE); // turn on LMX2
		delay(10);
		previousBand = OFFBand;
		setFreqBand(lastFrequency);
		setFreqParam(lastFrequency);
		setAmplitude();
	}
}