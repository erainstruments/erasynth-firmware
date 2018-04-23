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

String tempRead()
{
	int tempValue = 0;

	Wire.beginTransmission(64);	// Transmit to slave device #64(adress: 0x40)
	Wire.write(byte(0xE3));		// Sends one byte 0xE3: Measure Temperature, Hold Master Mode
	Wire.endTransmission();		// Stop transmission
	Wire.requestFrom(64, 2);    // Request 2 bytes from slave device #64(adress: 0x40)

	if (2 <= Wire.available()) 
	{ 
		// If 2 bytes were received
		tempValue = Wire.read();	// Receive high byte
		tempValue = tempValue << 8; // Shift high byte to be high 8 bits
		tempValue |= Wire.read();	// Receive low byte as lower 8 bits
	}

	temperature_Str = String(((tempValue * 175.2) / 65536) - 46.85, 1);
	return temperature_Str;
}