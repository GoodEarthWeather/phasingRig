#include "driverlib.h"
#include "lcdLib.h"
#include "main.h"
#include <stdlib.h>

#define	LOWNIB(x)	P2OUT = (P2OUT & 0xF0) + (x & 0x0F)

#define BUFFER_SIZE 12
static char buffer[BUFFER_SIZE];  /* must be static to be able to return it */
static char freqBuffer[16];
static char batVoltBuffer[8]; /* for holding battery voltage text */
static char cwSpeedBuffer[8]; /* for holding cw speed text */

void setCWSpeedText(void);
void setBatVoltText(void);


void lcdInit() {
	delay_ms(100);
	// Wait for 100ms after power is applied.

	setData(0x03);  // Start LCD (send 0x03)

	lcdTriggerEN(); // Send 0x03 3 times at 5ms then 100 us
	delay_ms(5);
	lcdTriggerEN();
	delay_ms(5);
	lcdTriggerEN();
	delay_ms(5);

	setData(0x02); // Switch to 4-bit mode
	lcdTriggerEN();
	delay_ms(5);

	lcdWriteCmd(0x28); // 4-bit, 2 line, 5x8
	lcdWriteCmd(0x08); // Instruction Flow
	lcdWriteCmd(0x01); // Clear LCD
	lcdWriteCmd(0x06); // Auto-Increment
	lcdWriteCmd(0x0E); // Display On, Cursor On, No blink
}

void lcdTriggerEN() {
    GPIO_setOutputHighOnPin(LCD_CLK);  // toggle clock (enable) bit
    GPIO_setOutputLowOnPin(LCD_CLK);
}

void lcdWriteData(unsigned char data) {
	GPIO_setOutputHighOnPin(LCD_RS); // Set RS to data
	setData(data >> 4); // Upper nibble
	lcdTriggerEN();
	setData(data); // Lower nibble
	lcdTriggerEN();
	delay_us(50); // Delay > 47 us
}

void lcdWriteCmd(unsigned char cmd) {
    GPIO_setOutputLowOnPin(LCD_RS); // Set RS to command
	setData(cmd >> 4); // Upper nibble
	lcdTriggerEN();
	setData(cmd); // Lower nibble
	lcdTriggerEN();
	delay_ms(5); // Delay > 1.5ms
}

void lcdSetText(char* text, int x, int y) {
	uint8_t i;
	if (x < 16) {
		x |= 0x80; // Set LCD for first line write
		switch (y){
		case 1:
			x |= 0x40; // Set LCD for second line write
			break;
		case 2:
			x |= 0x60; // Set LCD for first line write reverse
			break;
		case 3:
			x |= 0x20; // Set LCD for second line write reverse
			break;
		}
		lcdWriteCmd(x);
	}
	i = 0;

	while (text[i] != '\0') {
		lcdWriteData(text[i]);
		i++;
	}
}

void lcdSetInt(uint32_t val, int x, int y){
	char *result;
	result = number_to_string(val);
	lcdSetText(result, x, y);
}

void lcdClear() {
	lcdWriteCmd(CLEAR);
}

// This function will take a 4 bit data nibble and split it such that the bits
// in position 2,3 are shifted up to positions 5,6 to match the mapping of the GPIO
// to the LCD module.  It will then set the P2OUT ports to the resulting value
void setData(uint8_t data)
{
    (data & 0x01) ? (GPIO_setOutputHighOnPin(LCD_D4)) : (GPIO_setOutputLowOnPin(LCD_D4));
    (data & 0x02) ? (GPIO_setOutputHighOnPin(LCD_D5)) : (GPIO_setOutputLowOnPin(LCD_D5));
    (data & 0x04) ? (GPIO_setOutputHighOnPin(LCD_D6)) : (GPIO_setOutputLowOnPin(LCD_D6));
    (data & 0x08) ? (GPIO_setOutputHighOnPin(LCD_D7)) : (GPIO_setOutputLowOnPin(LCD_D7));
}

char *number_to_string(uint32_t number)
{
    char *p;
    uint32_t digit;

    p = &buffer[BUFFER_SIZE - 1];
    *p-- = '\0';  /* end of string */
    do {
        digit = number % 10;
        *p-- = '0' + digit;
        number /= 10;
    } while (number > 0);
    return p+1;  /* first digit might not be at the start of the buffer */
}

// routine to move cursor for frequency readout; always on line 1
void moveFreqCursor(void)
{
    uint8_t address;
    extern uint32_t freqMultiplier;
    extern uint8_t selectedBand;

    switch (freqMultiplier) {
    case 1 :
        address = 0x08;
        break;
    case 10 :
        address = 0x07;
        break;
    case 100 :
        address = 0x06;
        break;
    case 1000 :
        address = 0x04;
        break;
    case 10000 :
        address = 0x03;
        break;
    }
    if ( selectedBand != BAND_40M )
        address++;

    lcdWriteCmd(MOVE_CURSOR + address);
}

// routine to display frequency
void updateDisplay(void)
{
    char *result;
    extern uint8_t selectedBand;
    extern uint32_t si5351FreqOut;
    extern uint8_t selectedFilter;
    extern uint8_t selectedSideband;
    extern uint16_t batteryVoltage;
    extern uint8_t selectedMenuFunction;
    extern int16_t ritOffset;
    uint8_t i;

    lcdClear();

    // display frequency
    //lcdSetInt(si5351FreqOut, 0, 0);
    result = number_to_string(si5351FreqOut);
    if (selectedBand == BAND_40M)
    {
        freqBuffer[0] = *result++;
        freqBuffer[1] = '.';
        freqBuffer[2] = *result++;
        freqBuffer[3] = *result++;
        freqBuffer[4] = *result++;
        freqBuffer[5] = '.';
        freqBuffer[6] = *result++;
        freqBuffer[7] = *result++;
        freqBuffer[8] = *result++;
        freqBuffer[9] = '\0';
    } else {
        freqBuffer[0] = *result++;
        freqBuffer[1] = *result++;
        freqBuffer[2] = '.';
        freqBuffer[3] = *result++;
        freqBuffer[4] = *result++;
        freqBuffer[5] = *result++;
        freqBuffer[6] = '.';
        freqBuffer[7] = *result++;
        freqBuffer[8] = *result++;
        freqBuffer[9] = *result++;
        freqBuffer[10] = '\0';
    }
    lcdSetText(freqBuffer,0,0);

    // display band
    switch (selectedBand)
    {
    case  BAND_40M :
        lcdSetText("40M",0xD,0);  // put band info at position 13 (0xD) on first row
        break;
    case  BAND_30M :
        lcdSetText("30M",0xD,0);  // put band info at position 13 (0xD) on first row
        break;
    case  BAND_20M :
        lcdSetText("20M",0xD,0);  // put band info at position 13 (0xD) on first row
        break;
    case  BAND_17M :
        lcdSetText("17M",0xD,0);  // put band info at position 13 (0xD) on first row
        break;
    case  BAND_10M :
        lcdSetText("10M",0xD,0);  // put band info at position 13 (0xD) on first row
        break;
    default :
        break;
    }

    // display menu function
    switch (selectedMenuFunction)
    {
    case MENU_FUNCTION_SIDEBAND :
        if (selectedSideband == UPPER_SIDEBAND)
            lcdSetText("USB",0,1);
        else
            lcdSetText("LSB",0,1);
        break;
    case MENU_FUNCTION_BATVOLTAGE :
        getBatteryVoltage();
        setBatVoltText();
        break;
    case MENU_FUNCTION_CWSPEED :
        getCWSpeed();
        setCWSpeedText();
        break;
    case MENU_FUNCTION_RIT :
        lcdSetText("RIT: ",0,1);
        result = number_to_string((uint32_t)(abs(ritOffset)));
        (ritOffset < 0) ? (freqBuffer[0] = '-') : (freqBuffer[0] = '+');
        i = 1;
        while (*result != '\0')
            freqBuffer[i++] = *result++;
        freqBuffer[i] = '\0';
        lcdSetText(freqBuffer,5,1);
        // call function to handle rit
        break;
    case MENU_FUNCTION_MUTE :
        lcdSetText("MUTE",0,1);
        break;
    default :
        break;
    }

    // display filter selection
    if (selectedFilter == SSB_FILTER)
        lcdSetText("SSB",0xD,1);
    else
        lcdSetText("CW",0xE,1);
    moveFreqCursor();
}

/* This routine will take the battery voltage and convert it to text for the LCD */
void setBatVoltText(void)
{
    //uint32_t x;
    //uint32_t upper, lower; /* upper=integer part, lower=fractional part */
    uint32_t result;
    extern uint16_t batteryVoltage;
    char *txt;
    float z;

    // assume external resistor divider is 0.1 and full scale is 1.5V
    // ((batteryVoltage * 1.5/0.1) * 65536)/4096 = batteryVoltage * 240
    // Actual resistor divider is about 0.098, not 0.1, so 245 is used instead of 240

    z = ((float)batteryVoltage/4096.0)*1.5/0.098;
    result = (uint32_t)(z*100.0);
    /*************************
    x = (uint32_t)batteryVoltage * 245;
    upper = (x >> 16);
    lower = (x & 0xffff) >> 12;

    result = upper * 100;
    if (lower & 0b1000)
        result += 50;
    if (lower & 0b0100)
        result += 25;
    if ( lower & 0b0010)
        result += 12;
    if ( lower & 0b0001)
        result += 6;

    // round up
    if ( (result & 0b0111) >= 5 )
        result += 10;
    ******************************/

    /* now convert to string */
    txt = number_to_string(result);

    batVoltBuffer[0] = *txt++;
    if ( result < 1000)
    {
        batVoltBuffer[1] = '.';
        batVoltBuffer[2] = *txt++;
        batVoltBuffer[3] = *txt++;
        batVoltBuffer[4] = ' ';
        batVoltBuffer[5] = 'V';
        batVoltBuffer[6] = '\0';
    }
    else
    {
        batVoltBuffer[1] = *txt++;
        batVoltBuffer[2] = '.';
        batVoltBuffer[3] = *txt++;
        batVoltBuffer[4] = *txt++;
        batVoltBuffer[5] = ' ';
        batVoltBuffer[6] = 'V';
        batVoltBuffer[7] = '\0';
    }
    lcdSetText(batVoltBuffer,0,1);
}

/* This routine will take the battery voltage and convert it to text for the LCD */
void setCWSpeedText(void)
{
    extern uint8_t wpm;
    char *txt;


    lcdSetText("CWSPD: ",0,1);

    /* now convert to string */
    txt = number_to_string((uint32_t)wpm);

    cwSpeedBuffer[0] = *txt++;
    if (wpm > 9)
    {
        cwSpeedBuffer[1] = *txt++;
        cwSpeedBuffer[2] = '\0';
    }  else {
        cwSpeedBuffer[1] = '\0';
    }
    lcdSetText(cwSpeedBuffer,7,1);
}
