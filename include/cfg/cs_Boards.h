/*
 * Author: Anne van Rossum
 * Copyright: Distributed Organisms B.V. (DoBots)
 * Date: 4 Nov., 2014
 * License: LGPLv3+, Apache, and/or MIT, your choice
 */

/**
 * Convention:
 *   * use TABS for identation, but use SPACES for column-wise representations!
 */
#pragma once

//! Current accepted HARDWARE_BOARD types

#define PCA10001             0
#define NRF6310              1
#define VIRTUALMEMO          2
#define CROWNSTONE           3
#define CROWNSTONE2          4
#define CROWNSTONE3          5
#define CROWNSTONE4          6
#define CROWNSTONE5          7
#define NORDIC_BEACON        8
#define DOBEACON             9
#define DOBEACON2            10
#define PLUGIN_FLEXPRINT_01  11
#define GUIDESTONE           12

// NORDIC DEV BOARDS
#define PCA10000             30
#define PCA10031             31

#define PCA10036             40
#define PCA10040             41

// CROWNSTONE PLUGS
#define ACR01B1A             1000
#define ACR01B1B             1001
#define ACR01B1C             1002
#define ACR01B1D             1003

// CROWNSTONE BUILTINS
#define ACR01B2A             1500
#define ACR01B2B             1501
#define ACR01B2C             1502

#ifndef HARDWARE_BOARD
#error "Add HARDWARE_BOARD=... to CMakeBuild.config"
#endif

#if(HARDWARE_BOARD==CROWNSTONE)

#define PIN_GPIO_SWITCH      3                   //! this is p0.03 or gpio 3
#define PIN_GPIO_RELAY_ON    0                   //! something unused
#define PIN_GPIO_RELAY_OFF   0                   //! something unused
//#define PIN_AIN_ADC          6                   //! ain6 is p0.05 or gpio 5
//#define PIN_AIN_LPCOMP       5                   //! ain5 is p0.04 or gpio 4
//#define PIN_AIN_LPCOMP_REF   0                   //! ref0 is p0.00 or gpio 0
#define PIN_AIN_CURRENT      6                   //! ain6 is gpio 5
#define PIN_AIN_VOLTAGE      5                   //! ain5 is gpio 4, actually also current sense
#define PIN_GPIO_RX          6                   //! this is p0.06 or gpio 6
#define PIN_GPIO_TX          1                   //! this is p0.01 or gpio 1

//! resistance of the shunt in milli ohm
#define SHUNT_VALUE                 120
//! amplification of the voltage over the shunt, to the adc input of the chip
#define VOLTAGE_AMPLIFICATION       1
#endif


#if(HARDWARE_BOARD==CROWNSTONE2)
//! v0.86
#define PIN_GPIO_SWITCH      3                   //! this is p0.03 or gpio 3
#define PIN_GPIO_RELAY_ON    5                   //! something unused
#define PIN_GPIO_RELAY_OFF   0                   //! something unused
//#define PIN_AIN_ADC          2                   //! ain2 is p0.01 or gpio 1
//#define PIN_AIN_LPCOMP       2                   //! ain2 is p0.01 or gpio 1
//#define PIN_AIN_LPCOMP_REF   0                   //! ref0 is p0.00 or gpio 0
#define PIN_AIN_CURRENT      2                   //! ain2 is gpio 1
#define PIN_AIN_VOLTAGE      6                   //! ain6 is gpio 5, not actually working
#define PIN_GPIO_RX          4                   //! this is p0.04 or gpio 4
#define PIN_GPIO_TX          2                   //! this is p0.02 or gpio 2

#define HAS_RELAY true
//! resistance of the shunt in milli ohm
#define SHUNT_VALUE                 20
//! amplification of the voltage over the shunt, to the adc input of the chip
#define VOLTAGE_AMPLIFICATION       5
#endif


#if(HARDWARE_BOARD==CROWNSTONE3)
//! v0.90
#define PIN_GPIO_SWITCH      3                   //! this is p0.03 or gpio 3
#define PIN_GPIO_RELAY_ON    5                   //! something unused
#define PIN_GPIO_RELAY_OFF   0                   //! something unused
//#define PIN_AIN_ADC          2                   //! ain2 is p0.01 or gpio 1
//#define PIN_AIN_LPCOMP       2                   //! ain2 is p0.01 or gpio 1
//#define PIN_AIN_LPCOMP_REF   0                   //! ref0 is p0.00 or gpio 0
#define PIN_AIN_CURRENT      2                   //! ain2 is gpio 1
#define PIN_AIN_VOLTAGE      7                   //! ain7 is gpio 6
#define PIN_GPIO_RX          2                   //! this is p0.02 or gpio 2
#define PIN_GPIO_TX          4                   //! this is p0.04 or gpio 4

#define HAS_RELAY true
//! Switch pin should be low to switch lights on
#define SWITCH_INVERSED
//! resistance of the shunt in milli ohm
#define SHUNT_VALUE                 1
//! amplification of the voltage over the shunt, to the adc input of the chip
#define VOLTAGE_AMPLIFICATION       80
#endif


#if(HARDWARE_BOARD==CROWNSTONE4)
//! v0.92
#define PIN_GPIO_SWITCH      4                   //! this is p0.04 or gpio 4
#define PIN_GPIO_RELAY_ON    5                   //! something unused
#define PIN_GPIO_RELAY_OFF   0                   //! something unused
//#define PIN_AIN_ADC          2                   //! ain2 is p0.01 or gpio 1
//#define PIN_AIN_LPCOMP       2                   //! ain2 is p0.01 or gpio 1
//#define PIN_AIN_LPCOMP_REF   0                   //! ref0 is p0.00 or gpio 0
#define PIN_AIN_CURRENT      2                   //! ain2 is p0.01 or gpio 1
#define PIN_AIN_VOLTAGE      7                   //! ain7 is p0.06 or gpio 6
#define PIN_GPIO_RX          2                   //! this is p0.02 or gpio 2
#define PIN_GPIO_TX          3                   //! this is p0.03 or gpio 3

#define HAS_RELAY true
//! Switch pin should be low to switch lights on
#define SWITCH_INVERSED
//! resistance of the shunt in milli ohm
#define SHUNT_VALUE                 1
//! amplification of the voltage over the shunt, to the adc input of the chip
#define VOLTAGE_AMPLIFICATION       80
#endif

#if(HARDWARE_BOARD==CROWNSTONE5)
//! plugin quant
#define PIN_GPIO_SWITCH      12                  //! p0.12
#define PIN_GPIO_RELAY_ON    10                  //! p0.10
#define PIN_GPIO_RELAY_OFF   11                  //! p0.11
#define PIN_AIN_CURRENT      7                   //! ain7 is p0.06
#define PIN_AIN_VOLTAGE      6                   //! ain6 is p0.05
#define PIN_GPIO_RX          15                  //! p0.15
#define PIN_GPIO_TX          16                  //! p0.16
#define PIN_LED_RED          8
#define PIN_LED_GREEN        9

#define HAS_RELAY true
//! Switch pin should be low to switch lights on
#define SWITCH_INVERSED
//! resistance of the shunt in milli ohm
#define SHUNT_VALUE                 1
//! amplification of the voltage over the shunt, to the adc input of the chip
#define VOLTAGE_AMPLIFICATION       80
#endif

#if(HARDWARE_BOARD==DOBEACON)
//! doBeacon v0.7
#define PIN_GPIO_SWITCH      4                   //! this is p0.04 or gpio 4
#define PIN_GPIO_RELAY_ON    5                   //! something unused
#define PIN_GPIO_RELAY_OFF   0                   //! something unused
//#define PIN_AIN_ADC          2                   //! ain2 is p0.01 or gpio 1
//#define PIN_AIN_LPCOMP       2                   //! ain2 is p0.01 or gpio 1
//#define PIN_AIN_LPCOMP_REF   0                   //! ref0 is p0.00 or gpio 0
#define PIN_AIN_CURRENT      2                   //! ain2 is p0.01 or gpio 1
#define PIN_AIN_VOLTAGE      7                   //! ain7 is p0.06 or gpio 6
#define PIN_GPIO_RX          2                   //! this is p0.02 or gpio 2
#define PIN_GPIO_TX          3                   //! this is p0.03 or gpio 3

#define HAS_RELAY true
//! Switch pin should be low to switch lights on
#define SWITCH_INVERSED
//! resistance of the shunt in milli ohm
#define SHUNT_VALUE                 1
//! amplification of the voltage over the shunt, to the adc input of the chip
#define VOLTAGE_AMPLIFICATION       80
#endif

#if(HARDWARE_BOARD==DOBEACON2)
// v0.92
//#define PIN_GPIO_SWITCH      4                   // this is p0.04 or gpio 4
//#define PIN_GPIO_RELAY_ON    0                   // something unused
//#define PIN_GPIO_RELAY_OFF   0                   // something unused
//#define PIN_AIN_ADC          2                   // ain2 is p0.01 or gpio 1
//#define PIN_AIN_LPCOMP       2                   // ain2 is p0.01 or gpio 1
//#define PIN_AIN_LPCOMP_REF   0                   // ref0 is p0.00 or gpio 0
//#define PIN_AIN_CURRENT      0                   // ain2 is p0.01 or gpio 1
//#define PIN_AIN_VOLTAGE      0                   // ain7 is p0.06 or gpio 6
#define PIN_GPIO_RX          2                   // this is p0.02 or gpio 2
#define PIN_GPIO_TX          3                   // this is p0.03 or gpio 3

#define PIN_GPIO_LED_1       6					 // this is p0.07, GREEN
#define PIN_GPIO_LED_2       7					 // this is p0.07, RED
#define HAS_LEDS             1

// Switch pin should be low to switch lights on
//#define SWITCH_INVERSED
// resistance of the shunt in milli ohm
//#define SHUNT_VALUE                 1
// amplification of the voltage over the shunt, to the adc input of the chip
//#define VOLTAGE_AMPLIFICATION       80
#endif


#if(HARDWARE_BOARD==NRF6310)

#define PIN_GPIO_LED0        8                   //! this is p1.0 or gpio 8
#define PIN_GPIO_LED1        9                   //! this is p1.1 or gpio 9
#define PIN_GPIO_LED2        10
#define PIN_GPIO_LED3        11
#define PIN_GPIO_LED4        12
#define PIN_GPIO_LED5        13
#define PIN_GPIO_LED6        14
#define PIN_GPIO_LED7        15

#define PIN_GPIO_SWITCH      PIN_GPIO_LED1       //! show switch as LED
#define PIN_GPIO_RELAY_ON    0                   //! something unused
#define PIN_GPIO_RELAY_OFF   0                   //! something unused
//#define PIN_AIN_ADC          2                   //! ain2 is p0.1 or gpio 1
//#define PIN_AIN_LPCOMP       3                   //! ain3 is p0.2 or gpio 2
//#define PIN_AIN_LPCOMP_REF   0                   //! ref0 is p0.00 or gpio 0
#define PIN_AIN_CURRENT      2                   //! ain2 is p0.01 or gpio 1
#define PIN_AIN_VOLTAGE      3                   //! ain3 is p0.02 or gpio 2
#define PIN_GPIO_RX          16
#define PIN_GPIO_TX          17

//! resistance of the shunt in milli ohm
#define SHUNT_VALUE                 1
//! amplification of the voltage over the shunt, to the adc input of the chip
#define VOLTAGE_AMPLIFICATION       1
#endif


#if(HARDWARE_BOARD==PCA10001)

#define PIN_GPIO_LED0        18                  //! led
#define PIN_GPIO_LED1        19                  //! led

//#define PIN_GPIO_LED_CON     PIN_GPIO_LED1       //! shows connection state on the evaluation board
#define PIN_GPIO_SWITCH      PIN_GPIO_LED0       //! show switch as LED
#define PIN_GPIO_RELAY_ON    0                   //! something unused
#define PIN_GPIO_RELAY_OFF   0                   //! something unused
//#define PIN_AIN_ADC          2                   //! ain2 is p0.01 or gpio 1
//#define PIN_AIN_LPCOMP       3                   //! ain3 is p0.02 or gpio 2
//#define PIN_AIN_LPCOMP_REF   0                   //! ref0 is p0.00 or gpio 0
#define PIN_AIN_CURRENT      2                   //! ain2 is p0.01 or gpio 1
#define PIN_AIN_VOLTAGE      3                   //! ain3 is p0.02 or gpio 2
#define PIN_GPIO_RX          11                  //! this is p0.11 or gpio 11
#define PIN_GPIO_TX          9                   //! this is p0.09 or gpio 9

#define PIN_AIN_SENSOR       4
#define BUTTON_0             16
#define BUTTON_1             17

//! resistance of the shunt in milli ohm
#define SHUNT_VALUE                 1
//! amplification of the voltage over the shunt, to the adc input of the chip
#define VOLTAGE_AMPLIFICATION       1
#endif


#if(HARDWARE_BOARD==PCA10000) || (HARDWARE_BOARD==PCA10031)

#define LED_RGB_RED          21
#define LED_RGB_GREEN        22
#define LED_RGB_BLUE         23

//#define PIN_GPIO_LED_CON     PIN_GPIO_GREEN       //! shows connection state on the evaluation board
#define PIN_GPIO_SWITCH      LED_RGB_BLUE        //! show switch as LED
#define PIN_GPIO_RELAY_ON    0                   //! something unused
#define PIN_GPIO_RELAY_OFF   0                   //! something unused
//#define PIN_AIN_ADC          2                   //! something unused
//#define PIN_AIN_LPCOMP       2                   //! something unused
//#define PIN_AIN_LPCOMP_REF   0                   //! something unused
#define PIN_AIN_CURRENT      2                   //! something unused
#define PIN_AIN_VOLTAGE      2                   //! something unused
#define PIN_GPIO_RX          11                  //! this is p0.11 or gpio 11
#define PIN_GPIO_TX          9                   //! this is p0.09 or gpio 9

//! Switch pin should be low to switch lights on
#define SWITCH_INVERSED
//! resistance of the shunt in milli ohm
#define SHUNT_VALUE                 1
//! amplification of the voltage over the shunt, to the adc input of the chip
#define VOLTAGE_AMPLIFICATION       1
#endif


#if(HARDWARE_BOARD==PCA10036 || HARDWARE_BOARD==PCA10040)
#define LED_1                17
#define LED_2                18
#define LED_3                19
#define LED_4                20

#define PIN_GPIO_SWITCH      LED_1               //! show switch as LED
#define PIN_GPIO_RELAY_ON    11                  //! something unused
#define PIN_GPIO_RELAY_OFF   12                  //! something unused
#define PIN_AIN_CURRENT      0                   //! something unused - gpio 2
#define PIN_AIN_VOLTAGE      1                   //! something unused - gpio 3
#define PIN_GPIO_RX          8                   //! gpio 8
#define PIN_GPIO_TX          6                   //! gpio 6
//#define PIN_GPIO_TX          31                   //! gpio 31

//! Switch pin should be low to switch lights on
#define SWITCH_INVERSED
//! resistance of the shunt in milli ohm
#define SHUNT_VALUE                 1
//! amplification of the voltage over the shunt, to the adc input of the chip
#define VOLTAGE_AMPLIFICATION       1

#define HAS_RELAY true
#endif


#if(HARDWARE_BOARD==VIRTUALMEMO)

#define PIN_GPIO_LED0        7                   //! this is p0.07 or gpio 7
#define PIN_GPIO_LED1        8                   //! this is p0.08 or gpio 8
#define PIN_GPIO_LED2        9
#define PIN_GPIO_LED3        10
#define PIN_GPIO_LED4        11
#define PIN_GPIO_LED5        12
#define PIN_GPIO_LED6        13
#define PIN_GPIO_LED7        14

#define PIN_GPIO_RX          15
#define PIN_GPIO_TX          16

#endif


#if(HARDWARE_BOARD==NORDIC_BEACON)

#define LED_RGB_RED          16                  //! this is p0.16 or gpio 16
#define LED_RGB_GREEN        12                  //! this is p0.12 or gpio 12
#define LED_RGB_BLUE         15                  //! this is p0.15 or gpio 15

#define PIN_GPIO_RX          9                   //! this is p0.09 or gpio 9
#define PIN_GPIO_TX          1                   //! this is p0.01 or gpio 1

//#define PIN_GPIO_LED_CON     LED_RGB_GREEN       //! shows connection state on the evaluation board
#define PIN_GPIO_SWITCH      LED_RGB_BLUE        //! show switch as LED
#define PIN_GPIO_RELAY_ON    0                   //! something unused
#define PIN_GPIO_RELAY_OFF   0                   //! something unused

#define PIN_GPIO_BUTTON      20                  //! this is p0.20 or gpio 20

#define SWITCH_INVERSED

#endif

#if (HARDWARE_BOARD >= 1000) && (HARDWARE_BOARD <= 2000)

#define RED_LED 			10					//! this is p0.10 or gpio 10
#define GREEN_LED 			9					//! this is p0.09 or gpio 9

#define PIN_GPIO_RX         20                  //! this is p0.20 or gpio 20
#define PIN_GPIO_TX         19                  //! this is p0.19 or gpio 19

#define PIN_GPIO_SWITCH      8	        		//! this is p0.08 or gpio 8
//#define PIN_GPIO_SWITCH      RED_LED	        //! show switch as LED

#define PIN_GPIO_RELAY_ON    6          		//! this is p0.06 or gpio 6
#define PIN_GPIO_RELAY_OFF   7          		//! this is p0.07 or gpio 7
//#define PIN_GPIO_RELAY_ON    RED_LED            //! something unused
//#define PIN_GPIO_RELAY_OFF   GREEN_LED          //! something unused

#define PIN_AIN_CURRENT      2                   //! ain2 is p0.04 or gpio 4
#define PIN_AIN_VOLTAGE      1                   //! ain1 is p0.03 or gpio 3

//#define SWITCH_INVERSED

#define HAS_RELAY true
//! resistance of the shunt in milli ohm
#define SHUNT_VALUE			 1
//! amplification of the voltage over the shunt, to the adc input of the chip
#define VOLTAGE_AMPLIFICATION       80

#endif

#if(HARDWARE_BOARD==PLUGIN_FLEXPRINT_01)

#define PIN_GPIO_RX          6                  //! something unused
#define PIN_GPIO_TX          7                  //! something unused

#define PIN_GPIO_SWITCH      15					//! this is p0.15 or gpio 15

#define PIN_GPIO_RELAY_ON    16					//! this is p0.16 or gpio 16
#define PIN_GPIO_RELAY_OFF   17					//! this is p0.17 or gpio 17

#define PIN_AIN_CURRENT      3					//! ain3 is p0.05 or gpio 5
#define PIN_AIN_VOLTAGE      2                  //! ain2 is p0.04 or gpio 4
#define PIN_AIN_TEMP	 	 1					//! ain1 is p0.03 or gpio 3

#define SWITCH_INVERSED

#define HAS_RELAY true
//! resistance of the shunt in milli ohm
#define SHUNT_VALUE			 1
//! amplification of the voltage over the shunt, to the adc input of the chip
#define VOLTAGE_AMPLIFICATION       80

#endif

#if(HARDWARE_BOARD==GUIDESTONE)
// doesn't have anything !!!
// not even SERIAL!!!
#define HAS_SERIAL false
//#define PIN_GPIO_RX          8                   //! gpio 8
//#define PIN_GPIO_TX          6                   //! gpio 6
#endif

#ifndef HAS_RELAY
#define HAS_RELAY false
#endif

#ifndef HAS_SERIAL
#define HAS_SERIAL true
#endif

//! Sanity checks

#if(HAS_SERIAL)
#ifndef PIN_GPIO_RX
#error "For UART, PIN_RX must be defined"
#endif

#ifndef PIN_GPIO_TX
#error "For UART, PIN_TX must be defined"
#endif
#endif

#ifndef HARDWARE_VERSION
#error "You have to specify the hardware version of your chip"
#endif