/*
 * DK7IH_si5351.c
 *
 *  Created on: Jan 15, 2022
 *      Author: fishi
 */
/*****************************************************************/
/*             RF generator with Si5153 and ATMega8              */
/*  ************************************************************ */
/*  Mikrocontroller:  ATMEL AVR ATmega8, 8 MHz                   */
/*                                                               */
/*  Compiler:         GCC (GNU AVR C-Compiler)                   */
/*  Author:           Peter Rachow (DK7IH)                       */
/*  Last Change:      2017-FEB-23                                */
/*****************************************************************/
//Important:
//This is an absolute minimum software to generate a 10MHz signal with
//an ATMega8 and the SI5351 chip. Only one CLK0 and CLK1 are used
//to supply rf to RX and TX module seperately.

//I have tested this software with my RIGOL 100Mhz scope. Up to this
//frequency the Si5331 produced output.

//The software is more for educational purposes but can be modfied
//to get more stuff out of the chip.
//
//73 de Peter (DK7IH)

// Modified by David McNeill 16-Jan-2022

#include "main.h"
#include "i2c.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>




////////////////////////////////
//
// Si5351A commands
//
/////////////////////////////

// Set PLLs (VCOs) to internal clock rate of 900 MHz
// Equation fVCO = fXTAL * (a+b/c) (=> AN619 p. 3
void si5351_start(void)
{
  unsigned long a, b, c;
  unsigned long p1, p2, p3;

  // Init clock chip
  i2cSendRegister(XTAL_LOAD_CAP, 0xD2);      // Set crystal load capacitor to 10pF (default),
                                          // for bits 5:0 see also AN619 p. 60
  i2cSendRegister(CLK_ENABLE_CONTROL, 0xFF); // Disable all outputs
  i2cSendRegister(CLK0_CONTROL, 0x0F);       // Set PLLA to CLK0, 8 mA output
  i2cSendRegister(CLK1_CONTROL, 0x2F);       // Set PLLB to CLK1, 8 mA output
  i2cSendRegister(CLK2_CONTROL, 0x2F);       // Set PLLB to CLK2, 8 mA output
  i2cSendRegister(PLL_RESET, 0xA0);          // Reset PLLA and PLLB
  i2cSendRegister(CLK_ENABLE_CONTROL, 0xFE); // Enable only CLK0

  // Set VCOs of PLLA and PLLB to 891 MHz
  a = 33;           // Division factor 900/27 MHz
  b = 0;            // Numerator, sets b/c=0
  c = 1048575;      //Max. resolution, but irrelevant in this case (b=0)

  //Formula for splitting up the numbers to register data, see AN619
  p1 = 128 * a + (unsigned long) (128 * b / c) - 512;
  p2 = 128 * b - c * (unsigned long) (128 * b / c);
  p3  = c;

  //Write data to registers PLLA and PLLB so that both VCOs are set to 891MHz intermal freq
  i2cSendRegister(SYNTH_PLL_A, (p3 & 0xFF00) >> 8);
  i2cSendRegister(SYNTH_PLL_A + 1, (p3 & 0xFF));
  i2cSendRegister(SYNTH_PLL_A + 2, (p1 & 0x00030000) >> 16);
  i2cSendRegister(SYNTH_PLL_A + 3, (p1 & 0x0000FF00) >> 8);
  i2cSendRegister(SYNTH_PLL_A + 4, (p1 & 0x000000FF));
  i2cSendRegister(SYNTH_PLL_A + 5, 0xF0 | ((p2 & 0x000F0000) >> 16));
  i2cSendRegister(SYNTH_PLL_A + 6, (p2 & 0x0000FF00) >> 8);
  i2cSendRegister(SYNTH_PLL_A + 7, (p2 & 0x000000FF));

  i2cSendRegister(SYNTH_PLL_B, (p3 & 0xFF00) >> 8);
  i2cSendRegister(SYNTH_PLL_B + 1, (p3 & 0xFF));
  i2cSendRegister(SYNTH_PLL_B + 2, (p1 & 0x00030000) >> 16);
  i2cSendRegister(SYNTH_PLL_B + 3, (p1 & 0x0000FF00) >> 8);
  i2cSendRegister(SYNTH_PLL_B + 4, (p1 & 0x000000FF));
  i2cSendRegister(SYNTH_PLL_B + 5, 0xF0 | ((p2 & 0x000F0000) >> 16));
  i2cSendRegister(SYNTH_PLL_B + 6, (p2 & 0x0000FF00) >> 8);
  i2cSendRegister(SYNTH_PLL_B + 7, (p2 & 0x000000FF));

}

void si5351_set_RX_freq(unsigned long freq)
{

  unsigned long  a, b, c;
  unsigned long f_xtal = 27000000;
  double fdiv;  //division factor fvco/freq (will be integer part of a+b/c)
  double rm; //remainder
  unsigned long p1, p2, p3;
  extern uint8_t selectedSideband;

  if ( selectedSideband != BAND_10M )
  {
      // set frequency for SYNTH_MS_0 - CLK0 for QSD input
      a = b = c = 1048575;
      fdiv = (double) (f_xtal * 33) / freq;
      a = (unsigned long) fdiv;
      rm = fdiv - a;  //(equiv. b/c)
      b = rm * c;
      p1  = 128 * a + (unsigned long) (128 * b / c) - 512;
      p2 = 128 * b - c * (unsigned long) (128 * b / c);
      p3 = c;

      //Write data to multisynth registers of synth n
      i2cSendRegister(SYNTH_MS_0, (p3 & 0xFF00) >> 8);      //1048757 MSB
      i2cSendRegister(SYNTH_MS_0 + 1, (p3 & 0xFF));  //1048757 LSB
      i2cSendRegister(SYNTH_MS_0 + 2, (p1 & 0x00030000) >> 16);
      i2cSendRegister(SYNTH_MS_0 + 3, (p1 & 0x0000FF00) >> 8);
      i2cSendRegister(SYNTH_MS_0 + 4, (p1 & 0x000000FF));
      i2cSendRegister(SYNTH_MS_0 + 5, 0xF0 | ((p2 & 0x000F0000) >> 16));
      i2cSendRegister(SYNTH_MS_0 + 6, (p2 & 0x0000FF00) >> 8);
      i2cSendRegister(SYNTH_MS_0 + 7, (p2 & 0x000000FF));
  }
  else
  {
      // 10M band selected; set PLLB based on freq; set output multisynth to divide by 4
      a = b = c = 1048575;
      fdiv = (double) (freq * 4)/f_xtal;
      a = (unsigned long) fdiv;
      rm = fdiv - a;  //(equiv. b/c)
      b = rm * c;
      p1  = 128 * a + (unsigned long) (128 * b / c) - 512;
      p2 = 128 * b - c * (unsigned long) (128 * b / c);
      p3 = c;

      //Write data to registers PLLB so that both VCOs are set to 891MHz intermal freq
      i2cSendRegister(SYNTH_PLL_B, (p3 & 0xFF00) >> 8);
      i2cSendRegister(SYNTH_PLL_B + 1, (p3 & 0xFF));
      i2cSendRegister(SYNTH_PLL_B + 2, (p1 & 0x00030000) >> 16);
      i2cSendRegister(SYNTH_PLL_B + 3, (p1 & 0x0000FF00) >> 8);
      i2cSendRegister(SYNTH_PLL_B + 4, (p1 & 0x000000FF));
      i2cSendRegister(SYNTH_PLL_B + 5, 0xF0 | ((p2 & 0x000F0000) >> 16));
      i2cSendRegister(SYNTH_PLL_B + 6, (p2 & 0x0000FF00) >> 8);
      i2cSendRegister(SYNTH_PLL_B + 7, (p2 & 0x000000FF));

      // set frequency for SYNTH_MS_0 - CLK0 for QSD input
      p1 = 0;
      p2 = 0;
      p3 = 1;

      //Write data to multisynth registers of synth n
      i2cSendRegister(SYNTH_MS_0, (p3 & 0xFF00) >> 8);      //1048757 MSB
      i2cSendRegister(SYNTH_MS_0 + 1, (p3 & 0xFF));  //1048757 LSB
      i2cSendRegister(SYNTH_MS_0 + 2, ((p1 & 0x00030000) >> 16) | 0b00100000);  // set rdiv to 4
      i2cSendRegister(SYNTH_MS_0 + 3, (p1 & 0x0000FF00) >> 8);
      i2cSendRegister(SYNTH_MS_0 + 4, (p1 & 0x000000FF));
      i2cSendRegister(SYNTH_MS_0 + 5, 0xF0 | ((p2 & 0x000F0000) >> 16));
      i2cSendRegister(SYNTH_MS_0 + 6, (p2 & 0x0000FF00) >> 8);
      i2cSendRegister(SYNTH_MS_0 + 7, (p2 & 0x000000FF));

  }
}

void si5351_set_TX_freq(unsigned long freq)
{

  unsigned long  a, b, c;
  unsigned long f_xtal = 27000000;
  double fdiv;  //division factor fvco/freq (will be integer part of a+b/c)
  double rm; //remainder
  unsigned long p1, p2, p3;

  // set frequency for SYNTH_MS_1 - CLK1 for transmitter
  a = b = c = 1048575;
  fdiv = (double) (f_xtal * 33) / freq;
  a = (unsigned long) fdiv;
  rm = fdiv - a;  //(equiv. b/c)
  b = rm * c;
  p1  = 128 * a + (unsigned long) (128 * b / c) - 512;
  p2 = 128 * b - c * (unsigned long) (128 * b / c);
  p3 = c;

  //Write data to multisynth registers of synth n
  i2cSendRegister(SYNTH_MS_1, (p3 & 0xFF00) >> 8);      //1048757 MSB
  i2cSendRegister(SYNTH_MS_1 + 1, (p3 & 0xFF));  //1048757 LSB
  i2cSendRegister(SYNTH_MS_1 + 2, (p1 & 0x00030000) >> 16);
  i2cSendRegister(SYNTH_MS_1 + 3, (p1 & 0x0000FF00) >> 8);
  i2cSendRegister(SYNTH_MS_1 + 4, (p1 & 0x000000FF));
  i2cSendRegister(SYNTH_MS_1 + 5, 0xF0 | ((p2 & 0x000F0000) >> 16));
  i2cSendRegister(SYNTH_MS_1 + 6, (p2 & 0x0000FF00) >> 8);
  i2cSendRegister(SYNTH_MS_1 + 7, (p2 & 0x000000FF));

}

// This routine will enable/disable the RX and TX clocks
void si5351_RXTX_enable(void)
{
    extern uint8_t txKeyState;

    if (txKeyState == TX_KEY_DOWN)
        i2cSendRegister(CLK_ENABLE_CONTROL, 0xFD);  // tx clock enabled, rx clock disabled
    else
        i2cSendRegister(CLK_ENABLE_CONTROL, 0xFE);  // rx clock enabled, tx clock disabled
}
/*******************************
int main(void)
{
    unsigned long t1, freq = 10000000;
    PORTC = 0x30;//I²C-Bus lines: PC4=SDA, PC5=SCL

    twi_init();
    wait_ms(100);
    si5351_start();
    wait_ms(100);

    si5351_set_freq(SYNTH_MS_0, freq);
    wait_ms(5000);

    for(;;)
    {
        //Increase frequency in steps of 1kHz
        for(t1 = 10000000; t1 < 10090000; t1+=1000)
        {
            si5351_set_freq(SYNTH_MS_0, t1);
            wait_ms(1000);
        }
    }
    return 0;
}
********************************/



