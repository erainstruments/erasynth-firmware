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
	pulsePeriod = pulsePeriod_Str.toInt() - default_delay_in_Pulse_Mod - default_delay_in_Pulse_Mod - 30;
	pulseWidth = pulseWidth_Str.toInt() - default_delay_in_Pulse_Mod;

	if (modSource == Internal)
	{
		if (isDebugEnabled)
		{
			Serial.println("Pulse Mod Started with Internal Source");
			Serial.print("Pulse Width: ");
			Serial.print(pulseWidth_Str);
			Serial.println("us");
			Serial.print("Pulse Period: ");
			Serial.print(pulsePeriod_Str);
			Serial.println("us");
		}

		pulse_condition = true;
		pulse_changed();
		return;
	}

	if (modSource == External)
	{
		if (isDebugEnabled) { Serial.println("Pulse Mod Started with External Source"); }
		attachInterrupt(Trig_Inp, pulse_rising, RISING);
	}
}