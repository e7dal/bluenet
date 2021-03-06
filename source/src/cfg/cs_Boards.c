/*
 * Author: Crownstone Team
 * Copyright: Crownstone (https://crownstone.rocks)
 * Date: Aug 20, 2018
 * Triple-license: LGPLv3+, Apache License, and/or MIT
 */

/* Hardware board configuration.
 *
 * This file stores information that is different per type of hardware. Not every type of hardware is available to 
 * customers. It contains the following information:
 *   - pin assignment
 *   - existence of e.g. dimming/switch hardware (a Guidestone or dongle does not have relays or IGBTs)
 *   - circuit parameters (e.g. the measurement circuit might have different values)
 *   - calibration values (e.g. the threshold for triggering a high temperature warning depends on board layout, or
 *     the value with which we want to trigger a tap-to-toggle action depends on the antenna)
 *
 * For information on how to add a new board see:
 *    - https://github.com/crownstone/bluenet/blob/master/docs/ADD_BOARD.md
 */

#include "cfg/cs_Boards.h"
#include "cfg/cs_DeviceTypes.h"
#include "nrf_error.h"
#include "nrf52.h"
#include "cfg/cs_AutoConfig.h"

/* ********************************************************************************************************************
 * Crownstone Built-in Zero
 * *******************************************************************************************************************/

void asACR01B1D(boards_config_t* p_config) {
	p_config->pinGpioPwm                         = 8;
	p_config->pinGpioEnablePwm                   = 11; // Something unused
	p_config->pinGpioRelayOn                     = 6;
	p_config->pinGpioRelayOff                    = 7;
	p_config->pinAinCurrentGainLow               = 2;
	p_config->pinAinVoltage                      = 1;
//	p_config->pinAinZeroRef                      = -1; // Non existing
	p_config->pinAinPwmTemp                      = 3;
	p_config->pinGpioRx                          = 20;
	p_config->pinGpioTx                          = 19;
	p_config->pinLedRed                          = 10;
	p_config->pinLedGreen                        = 9;

	p_config->flags.hasRelay                     = true;
	p_config->flags.pwmInverted                  = false;
	p_config->flags.hasSerial                    = false;
//	p_config->flags.hasSerial                    = true;
	p_config->flags.hasLed                       = true;
	p_config->flags.ledInverted                  = false;
	p_config->flags.hasAdcZeroRef                = false;
	p_config->flags.pwmTempInverted              = false;

	p_config->deviceType                         = DEVICE_CROWNSTONE_BUILTIN;

	p_config->voltageMultiplier                  = 0.2f;
	p_config->currentMultiplier                  = 0.0044f;
	p_config->voltageZero                        = 1993;
	p_config->currentZero                        = 1980;
	p_config->powerZero                          = 3500;
	p_config->voltageRange                       = 1200; // 0V - 1.2V
	p_config->currentRange                       = 1200; // 0V - 1.2V

	p_config->pwmTempVoltageThreshold            = 0.76; // About 1.5kOhm --> 90-100C
	p_config->pwmTempVoltageThresholdDown        = 0.41; // About 0.7kOhm --> 70-95C

	p_config->minTxPower                         = -20; // higher tx power for builtins

	p_config->scanIntervalUs                     = 200 * 1000;
	p_config->scanWindowUs                       = 150 * 1000;
	p_config->tapToToggleDefaultRssiThreshold    = -35;
}



/* ********************************************************************************************************************
 * Crownstone Built-in One
 * *******************************************************************************************************************/

void asACR01B10D(boards_config_t* p_config) {
	p_config->pinGpioPwm                         = 8; // 18 for v3
//	p_config->pinGpioPwm                         = 15;
	p_config->pinGpioEnablePwm                   = 10; // Only for v25, but on v20 this pin is not connected.
	p_config->pinGpioRelayOn                     = 14; // 15 for v3
	p_config->pinGpioRelayOff                    = 13; // 16 for v3
	p_config->pinAinCurrentGainHigh              = 4; // highest gain   6 for v3
	p_config->pinAinCurrentGainMed               = 5; //                4 for v3
	p_config->pinAinCurrentGainLow               = 6; // lowest gain,   5 for v3
	p_config->pinAinVoltage                      = 1;
//	p_config->pinAinVoltage                      = 3; // actually the 14V pin
	p_config->pinAinZeroRef                      = 0;
	p_config->pinAinPwmTemp                      = 2;
	p_config->pinGpioRx                          = 20;
	p_config->pinGpioTx                          = 19;
	p_config->pinLedRed                          = 6; // Not there
	p_config->pinLedGreen                        = 7; // Not there

	p_config->flags.hasRelay                     = true;
	p_config->flags.pwmInverted                  = false;
	p_config->flags.hasSerial                    = false;
//	p_config->flags.hasSerial                    = true;
	p_config->flags.hasLed                       = false;
	p_config->flags.ledInverted                  = false;
	p_config->flags.hasAdcZeroRef                = true;
//	p_config->flags.hasAdcZeroRef                = false; // Non-differential measurements
	p_config->flags.pwmTempInverted              = true;

	p_config->deviceType                         = DEVICE_CROWNSTONE_BUILTIN_ONE;

	p_config->voltageMultiplier                  = -0.253f; // for range -1800 - 1800 mV
	p_config->currentMultiplier                  = 0.0071f; // for range -600 - 600 mV on pin 6
//	p_config->voltageMultiplier                  = 0.0f;
//	p_config->currentMultiplier                  = 0.0f;
	p_config->voltageZero                        = 500; // for range -1800 - 1800 mV
	p_config->currentZero                        = -125; // for range -600 - 600 mV on pin 6
	p_config->powerZero                          = 0;
	p_config->voltageRange                       = 1800;
	p_config->currentRange                       = 600;

	// See https://en.wikipedia.org/wiki/Thermistor#B_or_%CE%B2_parameter_equation B=3380, T0=25, R0=10000
	// Python: temp=82; r=10000*math.exp(3380*(1/(temp+273.15)-1/(25+273.15))); 3.3/(16000+r)*r
	//
	// Here R_temp is not 16k, but 18k. Hence, 76 degrees = 0.31645 and 82 degrees = 0.27265 V.
	//
//	p_config->pwmTempVoltageThreshold            = 0.7;  // About 50 degrees C
//	p_config->pwmTempVoltageThreshold            = 0.4;  // About 70 degrees C
	p_config->pwmTempVoltageThreshold            = 0.35; // About 76 degrees C
//	p_config->pwmTempVoltageThresholdDown        = 0.5;  // About 60 degrees C
//	p_config->pwmTempVoltageThresholdDown        = 0.25; // About 90 degrees C
	p_config->pwmTempVoltageThresholdDown        = 0.3; // About 82 degrees C

	p_config->minTxPower                         = -20; // higher tx power for builtins

//	// For NTC voltage advertising.
//	p_config->pinAinCurrentGainLow               = 2; // actually the IGBT NTC pin, make sure to disable differential measurements.
//	p_config->pinAinPwmTemp                      = 0; // actually zero ref: 1.2V, so always above the threshold voltage, so always below temperature threshold.
//	p_config->flags.hasAdcZeroRef                = false; // Non-differential measurements
//	p_config->currentRange                       = 3000; // 0-3V

	p_config->scanIntervalUs                     = 2000 * 1000;
	p_config->scanWindowUs                       = 2000 * 1000;
	p_config->tapToToggleDefaultRssiThreshold    = -35;
}

/* ********************************************************************************************************************
 * Crownstone Plug Zero
 * *******************************************************************************************************************/

void asACR01B2C(boards_config_t* p_config) {
	p_config->pinGpioPwm                         = 8;
	p_config->pinGpioEnablePwm                   = 11; // Something unused
	p_config->pinGpioRelayOn                     = 6;
	p_config->pinGpioRelayOff                    = 7;
	p_config->pinAinCurrentGainLow               = 2;
	p_config->pinAinVoltage                      = 1;
//	p_config->pinAinZeroRef                      = -1; // Non existing
	p_config->pinAinPwmTemp                      = 3;
	p_config->pinGpioRx                          = 20;
	p_config->pinGpioTx                          = 19;
	p_config->pinLedRed                          = 10;
	p_config->pinLedGreen                        = 9;

	p_config->flags.hasRelay                     = true;
	p_config->flags.pwmInverted                  = false;
	p_config->flags.hasSerial                    = false;
	p_config->flags.hasLed                       = true;
	p_config->flags.ledInverted                  = false;
	p_config->flags.hasAdcZeroRef                = false;
	p_config->flags.pwmTempInverted              = false;

	p_config->deviceType                         = DEVICE_CROWNSTONE_PLUG;

	p_config->voltageMultiplier                  = 0.2f;
	p_config->currentMultiplier                  = 0.0045f;
	p_config->voltageZero                        = 2003; // 2010 seems better?
	p_config->currentZero                        = 1997; // 1991 seems better?
	p_config->powerZero                          = 1500;
	p_config->voltageRange                       = 1200; // 0V - 1.2V
	p_config->currentRange                       = 1200; // 0V - 1.2V

	p_config->pwmTempVoltageThreshold            = 0.76; // About 1.5kOhm --> 90-100C
	p_config->pwmTempVoltageThresholdDown        = 0.41; // About 0.7kOhm --> 70-95C

	p_config->minTxPower                         = -20;

	p_config->scanIntervalUs                     = 200 * 1000;
	p_config->scanWindowUs                       = 150 * 1000;
	p_config->tapToToggleDefaultRssiThreshold    = -35;
}

void asACR01B2G(boards_config_t* p_config) {
	p_config->pinGpioPwm                         = 8;
	p_config->pinGpioEnablePwm                   = 11; // Something unused
	p_config->pinGpioRelayOn                     = 6;
	p_config->pinGpioRelayOff                    = 7;
	p_config->pinAinCurrentGainLow               = 2;
	p_config->pinAinVoltage                      = 1;
	p_config->pinAinZeroRef	                     = 0;
	p_config->pinAinPwmTemp                      = 3;
	p_config->pinGpioRx                          = 20;
	p_config->pinGpioTx                          = 19;
	p_config->pinLedRed                          = 10;
	p_config->pinLedGreen                        = 9;

	p_config->flags.hasRelay                     = true;
	p_config->flags.pwmInverted                  = false;
	p_config->flags.hasSerial                    = false;
	p_config->flags.hasLed                       = true;
	p_config->flags.ledInverted                  = false;
	p_config->flags.hasAdcZeroRef                = true;
//	p_config->flags.hasAdcZeroRef                = false; // Non-differential measurements
	p_config->flags.pwmTempInverted              = true;

	p_config->deviceType                         = DEVICE_CROWNSTONE_PLUG;

	p_config->voltageMultiplier                  = 0.171f;  // Calibrated by average of 10 production crownstones

	p_config->currentMultiplier                  = 0.00385f; // Calibrated by average of 10 production crownstones

	p_config->voltageZero                        = -99;     // Calibrated by noisy data from 1 crownstone
	p_config->currentZero                        = -270;    // Calibrated by noisy data from 1 crownstone

	p_config->powerZero                          = 9000;   // Calibrated by average of 10 production crownstones

	p_config->voltageRange                       = 1200; // 0V - 1.2V, or -1.2V - 1.2V around zeroRef pin // voltage ranges between 0.54 and 2.75, ref = 1.65
//	p_config->currentRange                       = 1800; // 0V - 1.8V, or -1.8V - 1.8V around zeroRef pin // Able to measure up to about 20A.
//	p_config->currentRange                       = 1200; // 0V - 1.2V, or -1.2V - 1.2V around zeroRef pin // Able to measure up to about 13A.
	p_config->currentRange                       = 600;  // 0V - 0.6V, or -0.6V - 0.6V around zeroRef pin // Able to measure up to about 6A.
//	p_config->currentRange                       = 1800; // Range used when not doing differential measurements.

	p_config->pwmTempVoltageThreshold            = 0.7;  // About 50 degrees C
	p_config->pwmTempVoltageThresholdDown        = 0.25; // About 90 degrees C

	p_config->minTxPower                         = -20;

	p_config->scanIntervalUs                     = 200 * 1000;
	p_config->scanWindowUs                       = 150 * 1000; // This board cannot provide enough power for 100% scanning.
	p_config->tapToToggleDefaultRssiThreshold    = -35;
}

/* ********************************************************************************************************************
 * Crownstone Plug One
 * *******************************************************************************************************************/

/*
 * Make sure that pin 19 is floating (not listed here). It is a short to another pin. The pin for the dimmer is
 * accidentally connected to a non-connected pin (A18). It has been patched to pin LED red on pin 8.
 *
 * Warning!! The pinAin values are NOT the PX.X numbers! Those are the AINX pins.
 *
 * Temperature is set according to:
 *   T_0    = 25 (room temperature)
 *   R_0    = 10000 (from datasheet, 10k at 25 degrees Celcius)
 *   B_ntc  = 3434 K (from datasheet of NCP15XH103J03RC at 25/85 degrees Celcius)
 *   R_22   = 15000
 *   R_ntc  = R_0 * exp(B_ntc * (1/(T+273.15) + 1/(T_0+273.15)))
 *   V_temp = 3.3 * R_ntc/(R_22+R_ntc)
 *  
 * We want to trigger between 76 < T < 82 degrees Celcius.
 *
 * |  T | R_ntc | V_temp |
 * | -- | ----  | ------ |
 * | 25 | 10000 | 1.32   | 
 * | 76 |  1859 | 0.3639 |
 * | 82 |  1575 | 0.3135 | 
 *
 * Hence, 0.3135 < V_temp < 0.3639 in an inverted fashion (higher voltage means cooler).
 *
 * The voltage range with R10 = 36.5k and R6 = 10M (C9 neglectable) is 0.0365/(0.0365+10)*680 = 2.47V. Thus it is 
 * 1.24V each way. A range of 1300 would be fine. Then 1240 would correspond to 240V RMS. Hence, the voltage 
 * multiplier (from ADC values to volts RMS) is 0.19355.
 */
void asACR01B11A(boards_config_t* p_config) {
	p_config->pinGpioPwm                         = 8;
	p_config->pinGpioEnablePwm                   = 11; // something unused
	p_config->pinGpioRelayOn                     = 15;
	p_config->pinGpioRelayOff                    = 13;

	p_config->pinAinCurrentGainLow               = 0; // GPIO 0.02
	p_config->pinAinCurrentGainMed               = 1; // GPIO 0.03
//	p_config->pinAinVoltageGainLow               = 7; // GPIO 0.31
//	p_config->pinAinVoltageGainHigh              = 5; // GPIO 0.29

	
	p_config->pinAinVoltage                      = 7; // default

	p_config->pinAinCurrentGainHigh              = 5; // Actually voltage high gain.
	
	p_config->pinAinZeroRef	                     = 3; // GPIO 0.05
	p_config->pinAinPwmTemp                      = 2; // GPIO 0.04

	p_config->pinGpioRx                          = 22; 
	p_config->pinGpioTx                          = 20;
	
	p_config->pinGpio1                           = 24;
	p_config->pinGpio2                           = 32;
	p_config->pinGpio3                           = 34;
	p_config->pinGpio4                           = 36;

	p_config->pinLedRed                          = 8;   // not placed, now for patch to PWM
	p_config->pinLedGreen                        = 41;  // not placed

	p_config->flags.hasRelay                     = true;
	p_config->flags.pwmInverted                  = true;
	p_config->flags.hasSerial                    = true;
	p_config->flags.hasLed                       = false; // not placed
	p_config->flags.ledInverted                  = false; // not placed
	p_config->flags.hasAdcZeroRef                = true;
	p_config->flags.pwmTempInverted              = true; 

//	p_config->deviceType                         = DEVICE_CROWNSTONE_PLUG_ONE;
	p_config->deviceType                         = DEVICE_CROWNSTONE_BUILTIN_ONE;

	p_config->voltageMultiplier                  = 0.19355f;     // unknown
	p_config->currentMultiplier                  = 0.00385f;     // unknown
	p_config->voltageZero                        = 0;            // unknown
	p_config->currentZero                        = -270;         // unknown
	p_config->powerZero                          = 9000;         // unknown

	p_config->voltageRange                       = 1800;
	p_config->currentRange                       = 600;          // unknown

	p_config->pwmTempVoltageThreshold            = 0.3639;
	p_config->pwmTempVoltageThresholdDown        = 0.3135;

	p_config->minTxPower                         = -20;          // unknown

	p_config->scanIntervalUs                     = 200 * 1000;   // unknown 
	p_config->scanWindowUs                       = 150 * 1000;   // unknown
	p_config->tapToToggleDefaultRssiThreshold    = -35;          // unknown
}

/* ********************************************************************************************************************
 * Dev Boards
 * *******************************************************************************************************************/

void asPca10040(boards_config_t* p_config) {
	p_config->pinGpioPwm                         = 17;
	p_config->pinGpioEnablePwm                   = 22; // Something unused
	p_config->pinGpioRelayOn                     = 11; // something unused
	p_config->pinGpioRelayOff                    = 12; // something unused
	p_config->pinAinCurrentGainLow               = 1; // gpio3
	p_config->pinAinVoltage                      = 2; // gpio4
	p_config->pinAinZeroRef	                     = 4; // gpio28
	p_config->pinAinPwmTemp                      = 0; // gpio2
	p_config->pinGpioRx                          = 8; // for UART commands
	p_config->pinGpioTx                          = 6; // for UART output
	p_config->pinLedRed                          = 19;
	p_config->pinLedGreen                        = 20;

	p_config->flags.hasRelay                     = false;
//	p_config->flags.hasRelay                     = true;
	p_config->flags.pwmInverted                  = true;
	p_config->flags.hasSerial                    = true;
	p_config->flags.hasLed                       = true;
	p_config->flags.ledInverted                  = true;
	p_config->flags.hasAdcZeroRef                = false;
	p_config->flags.pwmTempInverted              = false;

	p_config->deviceType                         = DEVICE_CROWNSTONE_PLUG;

	p_config->voltageMultiplier                  = 0.0; // set to 0 to disable sampling checks
	p_config->currentMultiplier                  = 0.0; // set to 0 to disable sampling checks
	p_config->voltageZero                        = 1000; // something
	p_config->currentZero                        = 1000; // something
//	p_config->voltageMultiplier                  = 1.0;
//	p_config->currentMultiplier                  = 1.0;
//	p_config->voltageZero                        = 0;
//	p_config->currentZero                        = 0;
	p_config->powerZero                          = 0; // something
	p_config->voltageRange                       = 3600; // 0V - 3.6V
	p_config->currentRange                       = 3600; // 0V - 3.6V

	p_config->pwmTempVoltageThreshold            = 2.0; // something
	p_config->pwmTempVoltageThresholdDown        = 1.0; // something

	p_config->minTxPower                         = -40;

	p_config->scanIntervalUs                     = 2000 * 1000;
	p_config->scanWindowUs                       = 2000 * 1000;
	p_config->tapToToggleDefaultRssiThreshold    = -35;
}

void asUsbDongle(boards_config_t* p_config) {
	asPca10040(p_config);
	p_config->deviceType                         = DEVICE_CROWNSTONE_USB;
}

void asGuidestone(boards_config_t* p_config) {
	// Guidestone has pads for pin 9, 10, 25, 26, 27, SWDIO, SWDCLK, GND, VDD

//	p_config->pinGpioPwm           = ; // unused
//	p_config->pinGpioRelayOn       = ; // unused
//	p_config->pinGpioRelayOff      = ; // unused
//	p_config->pinAinCurrent        = ; // unused
//	p_config->pinAinVoltage        = ; // unused
	p_config->pinGpioRx            = 25;
	p_config->pinGpioTx            = 26;
//	p_config->pinLedRed            = ; // unused
//	p_config->pinLedGreen          = ; // unused

	p_config->flags.hasRelay       = false;
	p_config->flags.pwmInverted    = false;
	p_config->flags.hasSerial      = false;
	p_config->flags.hasLed         = false;

	p_config->deviceType           = DEVICE_GUIDESTONE;

//	p_config->voltageMultiplier   = ; // unused
//	p_config->currentMultiplier   = ; // unused
//	p_config->voltageZero         = ; // unused
//	p_config->currentZero         = ; // unused
//	p_config->powerZero           = ; // unused

	p_config->minTxPower          = -20;

	p_config->scanIntervalUs      = 2000 * 1000;
	p_config->scanWindowUs        = 2000 * 1000;
}

uint32_t configure_board(boards_config_t* p_config) {

	uint32_t hardwareBoard = NRF_UICR->CUSTOMER[UICR_BOARD_INDEX];
	if (hardwareBoard == 0xFFFFFFFF) {
		hardwareBoard = g_DEFAULT_HARDWARE_BOARD;
	}

	switch(hardwareBoard) {
	case ACR01B1A:
	case ACR01B1B:
	case ACR01B1C:
	case ACR01B1D:
	case ACR01B1E:
		asACR01B1D(p_config);
		break;

	case ACR01B10B:
	case ACR01B10D:
		asACR01B10D(p_config);
		break;

	case ACR01B2A:
	case ACR01B2B:
	case ACR01B2C:
		asACR01B2C(p_config);
		break;

	case ACR01B2E:
	case ACR01B2G:
		asACR01B2G(p_config);
		break;
	case ACR01B11A:
		asACR01B11A(p_config);
		break;
	case GUIDESTONE:
		asGuidestone(p_config);
		break;

	case PCA10036:
	case PCA10040:
	case PCA10100:
		asPca10040(p_config);
		break;
	case CS_USB_DONGLE:
		asUsbDongle(p_config);
		break;

	default:
		// undefined board layout !!!
		asACR01B2C(p_config);
		return NRF_ERROR_INVALID_PARAM;
	}
	p_config->hardwareBoard = hardwareBoard;

	return NRF_SUCCESS;

}
