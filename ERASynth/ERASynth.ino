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

#include <SPI.h>
#include <DueTimer.h>
#include <Wire.h>
#include "definitions.h"

const int LED = 3; //(LED is active Low)
const int Wi_Fi_Flash = 25;
const int Wi_Fi_PD = 29;
const int Wi_Fi_RST = 30;
const int TCXO_En = 49; 
const int OCXO_En = 36;
const int SW1 = 43;
const int NB_FM_En = 35;
const int ADF4002_LE = 31;
const int ADF4002_LD = 46;
const int LMX1_MUXOUT = 47;
const int LMX2_MUXOUT = 50;
const int SW2 = 41;
const int SW3 = 40;
const int WB_FM_En = 51;
const int DC_6GHz_En = 38;
const int X6_15GHz_En = 34;
const int SW4 = 37;
const int DAC0_LE = 44;
const int DAC1_LE = 45;
const int DAC2_LE = 42;
const int DAC3_LE = 39;
const int DAC4_LE = 53;
const int Ext_Mod = 61; // AD7
const int Mic_Mod = 60; // AD6
const int Trig_Inp = 48;
const int AD0_current = 54;
const int AD1_voltage = 55;
const int FRAM_Hold = 22;
const int FRAM_WP = 23;
const int FRAM_CS = 24;
const int SPI_MISO = 74;
const int DDS_RESET = 33;
const int DDS_IO_UPD = 32;
const int PIN_ENB = 12;
int previousBand = MBand;
int freqBand = MBand;

// Default delay in while(1) loop in modulation
int default_delay_in_loop_NBFM = 4350;
int default_delay_in_loop_AM = 4350;
int default_delay_in_loop_WBFM = 4350;
int default_delay_in_Pulse_Mod = 30; //in us

float amplitude = 0;
float lastAmplitude = 0;

uint8_t modType = WBFM_Mod;
uint8_t modSource = Internal;
uint8_t waveformType = Sine;
uint8_t ERASynthModel = 1;
uint8_t LMX2_PFD_DLY_SEL = 2;
uint8_t previous_LMX2_PFD_DLY_SEL = 0;

uint16_t DACValue = 1500;
uint16_t modIndex = 0;
uint16_t serialNumber = 0;
uint16_t DDSPowerLevel = 400;

uint32_t dwellTime = 100;  //ms
uint32_t pulsePeriod = 2000; //ms
uint32_t pulseWidth = 1000; //ms

uint64_t sweepIndex = 0;
uint64_t BandPoint = 4500e6;
uint64_t lastFrequency = 3225123456;
uint64_t frequency = 3225123456;
uint64_t startFrequency = 4000e6;
uint64_t stopFrequency = 4000e6;
uint64_t stepFrequency = 100e6;
uint64_t number_of_sweep_points = 0;

boolean isUploadCodeModeActive = false;
boolean isCmdExist = false;
boolean isCmd1Exist = false;
boolean isFreqBandChanged = false;
boolean is_modulation_stopped = true;
boolean is_modulation_paused = true;
boolean sweepTriggerMode = false;
boolean isDebugEnabled = true;
boolean pulse_condition = true;
boolean is_sweep_stopped = true;
boolean isLowPowerModeActive = false;
boolean isPulseActive = false;
boolean isLowPhaseNoiseActive = false;

int calculated_waveform[maxSamplesNum] = { 0 };

double ODIV = 1;    // Used in WBFM Modulation, so it is global
int ledState = LOW;  // LedState used to set the LED
long previousMillis = 0;    // Will store last time LED was updated
unsigned long currentMillis = 0;

// Following variables is a long because of the time, measured in miliseconds,
// They will become a big number quickly thus can not be stored in int.
long blinkInterval = 400;           // Interval at which to blink (milliseconds)
long initESP8266Interval = 5000;    // Interval at which to init ESP8266 (milliseconds)

boolean isInitESP8266Done = false;
boolean stringComplete = false;
boolean string1Complete = false;
boolean serial1ActivityExist = false;
boolean serialActivityExist = false;
boolean nextFreq = false;
boolean isPulseRising = false;
boolean is_pulse_changed = false;

String ESP8266FirmwareVersion_Str = "";
String embeddedVersion_Str = "v1.0.16";
String cmdString = "";
String cmd1String = "";
String frequency_Str = "";
String amplitude_Str = "";
String startFrequency_Str = "";
String stopFrequency_Str = "";
String stepFrequency_Str = "";
String dwellTime_Str = "";
String rfOnOff_Str = "";
String esp8266OnOff_Str = "";
String isLowPowerModeActive_Str = "";
String referenceIntOrExt_Str = "";
String referenceTcxoOrOcxo_Str = "";
String modulationType_Str = "";
String modulationSource_Str = "";
String internalModulationFreq_Str = "";
String fmDeviation_Str = "";
String amDepth_Str = "";
String pulsePeriod_Str = "";
String pulseWidth_Str = "";
String modulationOnOff_Str = "";
String sweepOnOff_Str = "";
String rememberLastStates_Str = "";
String sweepTriggerMode_Str = "";
String waveformType_Str = "";
String wifiMode_Str = "";
String staModeSSID_Str = "";
String apModeSSID_Str = "";
String staModePassword_Str = "";
String apModePassword_Str = "";
String ipAddress_Str = "222.222.222.222";
String gatewayAddress_Str = "222.222.222.222";
String subnetAddress_Str = "222.222.222.222";
String temperature_Str = "";
String RSSI_Str = "";
String ERASynthModel_Str = ""; // 0:ERASynth, 1:ERASynth+, 2:ERASynth++
String serialNumber_Str = "";
String DDSPowerLevel_Str = "";
String phaseNoise_Str = "";

uint32_t frequencyValues[7];

// [0] PLL_NUM
// [1] PLL_DENUM
// [2] PLL_INT
// [3] CHDIV
// [4] ODIV
// [5] VCO FREQ (HIGH)
// [6] VCO FREQ (LOW)					

void setup()
{
	// digitalWrite function is little bit slow.
	// We need a quick start up on this pin. (Pin 29) High PD6
	REG_PIOD_ODSR |= 1 << 6;

	pinMode(LED, OUTPUT);

	// ESP8266 configurations
	pinMode(Wi_Fi_PD, OUTPUT);
	pinMode(Wi_Fi_Flash, OUTPUT);
	pinMode(Wi_Fi_RST, OUTPUT);

	digitalWrite(Wi_Fi_Flash, HIGH);
	digitalWrite(Wi_Fi_PD, HIGH);  // Power-up Wi-Fi 
	digitalWrite(Wi_Fi_RST, HIGH); // Normal operation

	//TCXO Power-up
	pinMode(TCXO_En, OUTPUT);
	digitalWrite(TCXO_En, HIGH);

	//OCXO Power-down  
	pinMode(OCXO_En, OUTPUT);
	digitalWrite(OCXO_En, LOW);

	//Internal REF selected
	pinMode(SW1, OUTPUT);
	digitalWrite(SW1, LOW);

	//NB_FM Disabled. Normal Operation
	pinMode(NB_FM_En, OUTPUT);
	digitalWrite(NB_FM_En, LOW);

	pinMode(ADF4002_LE, OUTPUT);
	digitalWrite(ADF4002_LE, HIGH);

	pinMode(ADF4002_LD, INPUT);
	pinMode(LMX1_MUXOUT, INPUT);
	pinMode(LMX2_MUXOUT, INPUT);

	//LMX1 path is selected, Low Power mode is OFF
	pinMode(SW2, OUTPUT);
	digitalWrite(SW2, HIGH);

	//30MHz-6GHz path is selected
	pinMode(SW3, OUTPUT);
	digitalWrite(SW3, LOW);

	//WB_FM Disabled. Normal Operation
	pinMode(WB_FM_En, OUTPUT);
	digitalWrite(WB_FM_En, LOW);

	//DC-6GHz Amp. is Enabled.
	pinMode(DC_6GHz_En, OUTPUT);
	digitalWrite(DC_6GHz_En, HIGH);

	//6-15GHz Amp. is Disabled.
	pinMode(X6_15GHz_En, OUTPUT);
	digitalWrite(X6_15GHz_En, LOW);

	//LF-6GHz path is selected
	pinMode(SW4, OUTPUT);
	digitalWrite(SW4, LOW);

	pinMode(DDS_RESET, OUTPUT);
	digitalWrite(DDS_RESET, LOW);

	pinMode(DDS_IO_UPD, OUTPUT);
	digitalWrite(DDS_IO_UPD, LOW);

	//Do Not Hold
	pinMode(FRAM_Hold, OUTPUT);
	digitalWrite(FRAM_Hold, HIGH);

	//Do Not Protect
	pinMode(FRAM_WP, OUTPUT);
	digitalWrite(FRAM_WP, HIGH);

	//FRAM Chip Select is High
	pinMode(FRAM_CS, OUTPUT);
	digitalWrite(FRAM_CS, HIGH);

	pinMode(DAC0_LE, OUTPUT);
	digitalWrite(DAC0_LE, HIGH);

	pinMode(DAC1_LE, OUTPUT);
	digitalWrite(DAC1_LE, HIGH);

	pinMode(DAC2_LE, OUTPUT);
	digitalWrite(DAC2_LE, HIGH);

	pinMode(DAC3_LE, OUTPUT);
	digitalWrite(DAC3_LE, HIGH);

	pinMode(DAC4_LE, OUTPUT);
	digitalWrite(DAC4_LE, HIGH);

	pinMode(PIN_ENB, OUTPUT);
	digitalWrite(PIN_ENB, LOW);

	pinMode(SPI_MISO, INPUT);
	pinMode(Ext_Mod, INPUT);
	pinMode(Mic_Mod, INPUT);
	pinMode(Trig_Inp, INPUT);

	// Initialize SPI
	SPI.begin(4);
	SPI.begin(10);
	SPI.begin(52);
	SPI.begin();

	// Join I2C bus
	Wire.begin();

	// Initialize Serial Comm.(RX0, TX0)
	Serial.begin(115200);
	Serial.setTimeout(10);

	// Initialize Serial1 Comm.(RX1, TX1)
	Serial1.begin(115200);
	Serial1.setTimeout(10);

	analogWriteResolution(12); // Sets Resolution for DACs (12 bit --> 0-4095)
	analogReadResolution(12);  // Sets Resolution for ADCs (12 bit --> 0-4095)

	ADC->ADC_MR |= 0x80; // ADC in free running mode.
	ADC->ADC_CR = 2;     // Starts ADC conversion.

						 // Attaches ledFade() as a timer function called every x microseconds:
	Timer0.attachInterrupt(ledBlink).setPeriod(blinkInterval * 1000).start();

	// (ms)Delay a moment for start-up
	delay(1000);

	// Fix Attanuation for protecting Amplifiers from over drive
	setDAC(max_DAC_Value, DAC1_LE);
	setDAC(max_DAC_Value, DAC2_LE);
	setDAC(max_DAC_Value, DAC3_LE);
	setDAC(0, DAC0_LE);
	setDAC(0, DAC4_LE);

	// Reset DDS
	digitalWrite(DDS_RESET, LOW);
	digitalWrite(DDS_RESET, HIGH);
	digitalWrite(DDS_RESET, LOW);

	// Send SPI initial bytes for ADF4002 (REF)
	soft_spiWrite(init_ADF4002_Initialization, 3, ADF4002_LE);
	soft_spiWrite(init_ADF4002_Function, 3, ADF4002_LE);
	soft_spiWrite(init_ADF4002_R_Counter, 3, ADF4002_LE);
	soft_spiWrite(init_ADF4002_N_Counter, 3, ADF4002_LE);

	delay(100);

	// Send SPI initial bytes for DDS
	digitalWrite(DDS_IO_UPD, LOW);
	spiWrite(init_DDS_CFR1, 5, DDS_LE);
	spiWrite(init_DDS_CFR2, 3, DDS_LE);
	spiWrite(init_DDS_DAC, 5, DDS_LE);
	spiWrite(init_DDS_FTW, 5, DDS_LE);
	delay(1);
	digitalWrite(DDS_IO_UPD, HIGH);

	// Send SPI initial bytes for LMX1 (Tunable REF)
	spiWrite_LMX(&LMX1_R0_reset, LMX1_LE);
	spiWrite_LMX(&LMX1_R0, LMX1_LE);
	for (int i = 0; i < 113; i++)
	{
		spiWrite_LMX(&LMX1_register[i], LMX1_LE);
	}
	delay(100);
	spiWrite_LMX(&LMX1_R0, LMX1_LE);

	delay(100);

	// Send SPI initial bytes for LMX2 (Main PLL),
	spiWrite_LMX(&LMX2_R0_reset, LMX2_LE);
	spiWrite_LMX(&LMX2_R0, LMX2_LE);
	for (int i = 0; i < 113; i++)
	{
		spiWrite_LMX(&LMX2_register[i], LMX2_LE);
	}
	delay(100);
	spiWrite_LMX(&LMX2_R0, LMX2_LE);


	checkVersion();
	   
	if (getFRAM(_isUploadCodeModeActive) == "1")
	{
		isUploadCodeModeActive = true;
		setFRAM(_isUploadCodeModeActive, String(0));
	}

	Serial.print("Upload code mode is: ");
	Serial.println(isUploadCodeModeActive);
	Serial.println();
	Serial.println("---------------------------------------------------------------------");
	Serial.print("Embedded Version: ");
	Serial.println(embeddedVersion_Str);

	ERASynthModel_Str = String(ERASynth);

	Serial.print("ERASynth Model: ");
	if (ERASynth == 0) { Serial.println("ERASynth");   BandPoint = 6000e6; }
	else if (ERASynth == 1) { Serial.println("ERASynth+");  BandPoint = 4500e6; }
	else if (ERASynth == 2) { Serial.println("ERASynth++"); BandPoint = 4500e6; }

	serialNumber_Str = getFRAM(_serialNumber);
	
	Serial.print("Serial Number: ");
	Serial.println(serialNumber_Str);

	// Remember the last states through reading FRAM 
	rememberLastStates_Str = getFRAM(_rememberLastStates);

	if (rememberLastStates_Str == "1")
	{
		frequency_Str = getFRAM(_frequency);
		amplitude_Str = getFRAM(_amplitude);
		startFrequency_Str = getFRAM(_startFrequency);
		stopFrequency_Str = getFRAM(_stopFrequency);
		stepFrequency_Str = getFRAM(_stepFrequency);
		dwellTime_Str = getFRAM(_dwellTime);
		sweepTriggerMode_Str = getFRAM(_sweepTriggerMode);
		sweepOnOff_Str = getFRAM(_sweepOnOff);
		rfOnOff_Str = getFRAM(_rfOnOff);
		referenceIntOrExt_Str = getFRAM(_referenceIntOrExt);
		referenceTcxoOrOcxo_Str = getFRAM(_referenceTcxoOrOcxo);
		modulationType_Str = getFRAM(_modulationType);
		modulationSource_Str = getFRAM(_modulationSource);
		waveformType_Str = getFRAM(_waveformType);
		internalModulationFreq_Str = getFRAM(_internalModulationFreq);
		fmDeviation_Str = getFRAM(_fmDeviation);
		amDepth_Str = getFRAM(_amDepth);
		pulsePeriod_Str = getFRAM(_pulsePeriod);
		pulseWidth_Str = getFRAM(_pulseWidth);
		modulationOnOff_Str = getFRAM(_modulationOnOff);
		phaseNoise_Str = getFRAM(_phaseNoise);

		// Removes zeros in front of the string
		while (frequency_Str.indexOf('0') == 0) { frequency_Str.remove(0, 1); }
		while (amplitude_Str.indexOf('0') == 0) { amplitude_Str.remove(0, 1); }
		while (startFrequency_Str.indexOf('0') == 0) { startFrequency_Str.remove(0, 1); }
		while (stopFrequency_Str.indexOf('0') == 0) { stopFrequency_Str.remove(0, 1); }
		while (stepFrequency_Str.indexOf('0') == 0) { stepFrequency_Str.remove(0, 1); }
		while (dwellTime_Str.indexOf('0') == 0) { dwellTime_Str.remove(0, 1); }
		while (fmDeviation_Str.indexOf('0') == 0) { fmDeviation_Str.remove(0, 1); }
		while (amDepth_Str.indexOf('0') == 0) { amDepth_Str.remove(0, 1); }
		while (pulsePeriod_Str.indexOf('0') == 0) { pulsePeriod_Str.remove(0, 1); }
		while (pulseWidth_Str.indexOf('0') == 0) { pulseWidth_Str.remove(0, 1); }
		while (internalModulationFreq_Str.indexOf('0') == 0) { internalModulationFreq_Str.remove(0, 1); }

		// Set Phase Noise Mode first, so frequency calculations will be according to it.
		command(">P9" + phaseNoise_Str);
		command(">F" + frequency_Str);
		command(">A" + amplitude_Str);
		command(">S1" + startFrequency_Str);
		command(">S2" + stopFrequency_Str);
		command(">S3" + stepFrequency_Str);
		command(">S4" + dwellTime_Str);
		command(">S0" + sweepTriggerMode_Str);
		if (sweepOnOff_Str == "1") { command(">SS" + sweepOnOff_Str); }
		command(">P0" + rfOnOff_Str);
		command(">P1" + referenceIntOrExt_Str);
		command(">P5" + referenceTcxoOrOcxo_Str);
		command(">M0" + modulationType_Str);
		command(">M1" + modulationSource_Str);
		command(">M2" + waveformType_Str);
		command(">M3" + internalModulationFreq_Str);
		command(">M4" + fmDeviation_Str);
		command(">M5" + amDepth_Str);
		command(">M6" + pulsePeriod_Str);
		command(">M7" + pulseWidth_Str);
	}

	Serial1.println("<A");
}

void loop()
{
	if (isInitESP8266Done && !isUploadCodeModeActive)
	{
		Serial.println("---------------------------------------------------------------------");
		// Read the buffer to clear it so we can get pure data
		Serial1.readString();
		Serial1.println("<E");
		ESP8266FirmwareVersion_Str = Serial1.readStringUntil('\r');
		Serial.print("ESP8266 Embedded Version: ");
		Serial.println(ESP8266FirmwareVersion_Str);
		setLastStatesOfESP8266();
		isInitESP8266Done = false;
		command(">MS" + modulationOnOff_Str);
	}

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

	if (nextFreq) { sweepERASynth(); }

	if (!is_modulation_stopped && modType == Pulse_Mod && modSource == Internal && isPulseActive)
	{
		pulseMod();
	}	

	if (!is_modulation_stopped && modType == Pulse_Mod && modSource == External)
	{
		if (isPulseRising) { attachInterrupt(Trig_Inp, pulse_rising, RISING); }
		else { attachInterrupt(Trig_Inp, pulse_falling, FALLING); }
	}

	if (serial1ActivityExist || serialActivityExist)
	{
		serialActivityExist = false;
		serial1ActivityExist = false;
		if (!is_modulation_stopped) { command(">MS1"); }
	}
}
