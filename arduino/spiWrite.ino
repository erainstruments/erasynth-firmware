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


// Hard LE SPI (4,10,52 Pins) Function
void spiWrite_LMX(uint32_t* data_u32, int pin_LE)
{
	SPI.beginTransaction(pin_LE, SPISettings(31e6, MSBFIRST, SPI_MODE0));
	SPI.transfer(pin_LE, ((uint8_t*)data_u32)[2], SPI_CONTINUE);
	SPI.transfer(pin_LE, ((uint8_t*)data_u32)[1], SPI_CONTINUE);
	SPI.transfer(pin_LE, ((uint8_t*)data_u32)[0]);
	SPI.endTransaction();
}

// Hard LE SPI (4,10,52 Pins) Function //DDS
void spiWrite(byte *spiArray, int spiArraySize, int pin_LE)
{
	SPI.beginTransaction(pin_LE, SPISettings(31e6, MSBFIRST, SPI_MODE0));
	for (uint8_t i = 0; i < spiArraySize; i++) { SPI.transfer(pin_LE, spiArray[i]); }
	SPI.endTransaction();
}

//Soft LE SPI (Any Pin) Function //ADF4002
void soft_spiWrite(byte *array, int array_size, int pin_LE)
{
	SPI.beginTransaction(SPISettings(31e6, MSBFIRST, SPI_MODE0));
	digitalWrite(pin_LE, LOW);
	SPI.transfer(array, array_size);
	digitalWrite(pin_LE, HIGH);
	SPI.endTransaction();
}

//Soft LE SPI (Any Pin) Function for AD5621 It uses MODE1
void soft_spiWrite_DAC(byte *array, int array_size, int pin_LE)
{
	SPI.beginTransaction(SPISettings(31e6, MSBFIRST, SPI_MODE1));
	digitalWrite(pin_LE, LOW);
	SPI.transfer(array, array_size);
	digitalWrite(pin_LE, HIGH);
	SPI.endTransaction();
}

//Soft LE SPI (Any Pin) Function for AD5621 It uses MODE1
void soft_spiWrite_DAC0(byte *array, int array_size)
{
	REG_PIOC_ODSR &= ~(1 << 19); //Low PC19 (pin44)
	SPI.transfer(array, array_size);
	REG_PIOC_ODSR |= 1 << 19; //High PC19 (Pin44) 
}

//Soft LE SPI (Any Pin) Function for AD5621 It uses MODE1
void soft_spiWrite_DAC4(byte *array, int array_size)
{
	REG_PIOB_ODSR &= ~(1 << 14); //Low PB14 (pin53)
	SPI.transfer(array, array_size);
	REG_PIOB_ODSR |= 1 << 14; //High PB14 (Pin53) 
}

//Soft LE SPI (Any Pin) Function for AD5621 It uses MODE1
void soft_spiWrite_DAC3(byte *array, int array_size)
{
	REG_PIOC_ODSR &= ~(1 << 7); //Low PC7 (pin39)
	SPI.transfer(array, array_size);
	REG_PIOC_ODSR |= 1 << 7; //High PC7 (Pin39) 
}

//FRAM SPI Write Function
void spiWriteFRAM(uint16_t address, char *spiArray, uint8_t spiArraySize)
{
	byte MSB_Addr = (byte)(address >> 8);
	byte LSB_Addr = (byte)address;
	SPI.beginTransaction(FRAM_CS, SPISettings(31e6, MSBFIRST, SPI_MODE0));
	REG_PIOA_ODSR &= ~(1 << 15); //Low PA15 (Pin24)
	SPI.transfer(FRAM_CS, WREN);
	delayMicroseconds(5);
	REG_PIOA_ODSR |= 1 << 15; //High PA15 (Pin24)
	REG_PIOA_ODSR &= ~(1 << 15); //Low PA15 (Pin24)
	SPI.transfer(FRAM_CS, WRITE, SPI_CONTINUE);
	SPI.transfer(FRAM_CS, MSB_Addr, SPI_CONTINUE);
	SPI.transfer(FRAM_CS, LSB_Addr, SPI_CONTINUE);
	SPI.transfer(spiArray, spiArraySize);
	REG_PIOA_ODSR |= 1 << 15; //High PA15 (Pin24)
	SPI.endTransaction();
}

//FRAM SPI Read Function
void spiReadFRAM(uint16_t address, char *spiArray, uint8_t spiArraySize)
{
	byte MSB_Addr = (byte)(address >> 8);
	byte LSB_Addr = (byte)address;
	SPI.beginTransaction(FRAM_CS, SPISettings(31e6, MSBFIRST, SPI_MODE0));
	REG_PIOA_ODSR &= ~(1 << 15); //Low PA15 (Pin24)
	SPI.transfer(FRAM_CS, READ, SPI_CONTINUE);
	SPI.transfer(FRAM_CS, MSB_Addr, SPI_CONTINUE);
	SPI.transfer(FRAM_CS, LSB_Addr, SPI_CONTINUE);

	for (uint8_t i = 0; i < spiArraySize; i++)
	{
		spiArray[i] = SPI.transfer(FRAM_CS, 0xFF);
		if (spiArray[i] == 0) { spiArray[i] = '0'; }
	}

	REG_PIOA_ODSR |= 1 << 15; //High PA15 (Pin24)
	SPI.endTransaction();
}