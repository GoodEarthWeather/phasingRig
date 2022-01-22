/*
 * main.h
 *
 *  Created on: May 16, 2018
 *      Author: dmcneill
 */

/*
 * Change BTN_SB_SELECT to BTN_DIGIT_SELECT - this makes the sideband select button now be the digit select button for tuning
 */

#ifndef MYINCLUDE_MAIN_H_
#define MYINCLUDE_MAIN_H_

#include "driverlib.h"

void initClocks(void);
void initGPIO(void);
void moveFreqCursor(void);
void updateDisplay(void);
void selectBand(void);
void selectFilter(void);
void selectSideband(void);
void selectAudioState(uint8_t);
void setBatVoltText(void);
void getBatteryVoltage(void);
void initADC(void);
void selectMenuFunction(void);

//  SI5351 Declarations
void si5351_start(void);
void si5351_set_RX_freq(unsigned long);
void si5351_set_TX_freq(unsigned long);
void si5351_RXTX_enable(void);
void initsi5351(void);

/////////////////////
//Defines for Si5351
/////////////////////
#define SI5351_ADDRESS 0x60

//Set of Si5351A register addresses
#define DEVICE_STATUS            0
#define CLK_ENABLE_CONTROL       3
#define PLLX_SRC                15
#define CLK0_CONTROL            16
#define CLK1_CONTROL            17
#define CLK2_CONTROL            18
#define SYNTH_PLL_A             26
#define SYNTH_PLL_B             34
#define SYNTH_MS_0              42
#define SYNTH_MS_1              50
#define SYNTH_MS_2              58
#define PLL_RESET              177
#define XTAL_LOAD_CAP          183


#define RESET_PLL 1
#define NO_RESET_PLL 0

#define ENABLED 1
#define DISABLED 0

// Define all I/O
#define ENCODER_A GPIO_PORT_P4, GPIO_PIN5
#define ENCODER_B GPIO_PORT_P2, GPIO_PIN2
#define ENCODER_SWITCH GPIO_PORT_P4, GPIO_PIN6

//#define LED1 GPIO_PORT_P1, GPIO_PIN1
#define VBAT_SNS GPIO_PORT_P1, GPIO_PIN1

#define BTN_CWSPEED GPIO_PORT_P5, GPIO_PIN7
#define BTN_RIT GPIO_PORT_P3, GPIO_PIN3
#define BTN_DIGIT_SELECT GPIO_PORT_P2, GPIO_PIN3
#define BTN_FILTER_SELECT GPIO_PORT_P3, GPIO_PIN4
#define BTN_BAND_SELECT GPIO_PORT_P3, GPIO_PIN1
#define BTN_SPOT GPIO_PORT_P2, GPIO_PIN4
#define BTN_TUNE GPIO_PORT_P3, GPIO_PIN7
#define BTN_MENU GPIO_PORT_P4, GPIO_PIN0
#define DIT_KEY GPIO_PORT_P4, GPIO_PIN1
#define DAH_KEY GPIO_PORT_P4, GPIO_PIN2
#define STRAIGHT_KEY GPIO_PORT_P2, GPIO_PIN7
#define TR_SWITCH GPIO_PORT_P3, GPIO_PIN5
#define CW_OUT GPIO_PORT_P3, GPIO_PIN2
#define TR_MUTE GPIO_PORT_P3, GPIO_PIN6
#define BAND_40M_SELECT GPIO_PORT_P6, GPIO_PIN1
#define BAND_30M_SELECT GPIO_PORT_P6, GPIO_PIN2
#define BAND_20M_SELECT GPIO_PORT_P4, GPIO_PIN7
#define BAND_17M_SELECT GPIO_PORT_P5, GPIO_PIN0
#define BAND_10M_SELECT GPIO_PORT_P5, GPIO_PIN1
#define FILTER_SELECT GPIO_PORT_P5, GPIO_PIN4
#define SIDEBAND_SELECT GPIO_PORT_P1, GPIO_PIN0


// define frequency bands
#define BAND_40M_LOWER 7000000
#define BAND_40M_UPPER 7300000
#define BAND_30M_LOWER 10100000
#define BAND_30M_UPPER 10150000
#define BAND_20M_LOWER 14000000
#define BAND_20M_UPPER 14350000
#define BAND_17M_LOWER 18068000
#define BAND_17M_UPPER 18168000
#define BAND_10M_LOWER 28000000
#define BAND_10M_UPPER 29700000
#define BAND_6M_LOWER 50000000
#define BAND_6M_UPPER 54000000

// define names for button presses
#define BTN_PRESSED_NONE 0x0
#define BTN_PRESSED_CWSPEED 0x1
#define BTN_PRESSED_RIT 0x2
#define BTN_PRESSED_DIGIT_SELECT 0x3
#define BTN_PRESSED_FILTER_SELECT 0x4
#define BTN_PRESSED_BAND_SELECT 0x5
#define BTN_PRESSED_SPOT 0x6
#define BTN_PRESSED_TUNE 0x7
#define BTN_PRESSED_MENU 0x8
#define BTN_PRESSED_ENCODER 0x9

// define bands
#define BAND_40M 0x0
#define BAND_30M 0x1
#define BAND_20M 0x2
#define BAND_17M 0x3
#define BAND_10M 0x4
#define BAND_6M 0x5

// define menu functions
#define MENU_FUNCTION_SIDEBAND 0x0
#define MENU_FUNCTION_BATVOLTAGE 0x1
#define MENU_FUNCTION_CWSPEED 0x2
#define MENU_FUNCTION_RIT 0x3
#define MENU_FUNCTION_MUTE 0x4

#define UPPER_SIDEBAND 0x0
#define LOWER_SIDEBAND 0x1
#define SSB_FILTER 0x0
#define CW_FILTER 0x1

#define MUTE 0x1
#define UNMUTE 0x0

#define TX_KEY_UP 0x0
#define TX_KEY_DOWN 0x1

#endif /* MYINCLUDE_MAIN_H_ */
