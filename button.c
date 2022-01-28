/*
 * button.c
 *
 *  Created on: Aug 8, 2019
 *      Author: fishi
 */

#include "main.h"
uint8_t volatile buttonPressed;

// Port 4 interrupt service routine
#pragma vector=PORT4_VECTOR
__interrupt void Port_4(void)
{
    extern uint8_t encoderCWCount, encoderCCWCount;
    switch(__even_in_range(P4IV,16))
    {
      case  0: break;                         // Vector  0:  No interrupt
      case  2:
          // P4.0 = Menu button
          buttonPressed = BTN_PRESSED_MENU;
          GPIO_clearInterrupt(BTN_MENU);

          break;                         // Vector  2:  Port 4 Bit 0
      case  4: break;                         // Vector  4:  Port 4 Bit 1
      case  6:
          break;                         // Vector  6:  Port 4 Bit 2
      case  8: break;                         // Vector  8:  Port 4 Bit 3
      case  10: break;                         // Vector  10:  Port 4 Bit 4
      case  12:
          // P4.5 = Encoder A
          // now get encoder state
          if (GPIO_getInputPinValue(ENCODER_A) != GPIO_getInputPinValue(ENCODER_B))
          {
              encoderCWCount++;
          } else {
              encoderCCWCount++;
          }
          GPIO_clearInterrupt(ENCODER_A);
          // now toggle interrupt edge
          P4IES ^= BIT5;
          break;                         // Vector  12:  Port 4 Bit 5
      case  14:
          // P4.6 = Encoder button
          buttonPressed = BTN_PRESSED_ENCODER;
          GPIO_clearInterrupt(ENCODER_SWITCH);
          break;                         // Vector  14:  Port 4 Bit 6
      case  16: break;                         // Vector  16:  Port 4 Bit 7
      default: break;
    }
}

// Port 5 interrupt service routine
#pragma vector=PORT5_VECTOR
__interrupt void Port_5(void)
{
    switch(__even_in_range(P5IV,16))
    {
      case  0: break;                         // Vector  0:  No interrupt
      case  2: break;                         // Vector  2:  Port 5 Bit 0
      case  4: break;                         // Vector  4:  Port 5 Bit 1
      case  6: break;                         // Vector  6:  Port 5 Bit 2
      case  8: break;                         // Vector  8:  Port 5 Bit 3
      case  10: break;                         // Vector  10:  Port 5 Bit 4
      case  12: break;                         // Vector  12:  Port 5 Bit 5
      case  14: break;                         // Vector  14:  Port 5 Bit 6
      case  16:
          // P5.7 = CW Speed button
          buttonPressed = BTN_PRESSED_CWSPEED;
          GPIO_clearInterrupt(BTN_CWSPEED);
          break;                         // Vector  16:  Port 5 Bit 7
      default: break;
    }
}

// Port 3 interrupt service routine
#pragma vector=PORT3_VECTOR
__interrupt void Port_3(void)
{
    switch(__even_in_range(P3IV,16))
    {
      case  0: break;                         // Vector  0:  No interrupt
      case  2: break;                         // Vector  2:  Port 3 Bit 0
      case  4:
          // P3.1 = Band Select button
          buttonPressed = BTN_PRESSED_BAND_SELECT;
          GPIO_clearInterrupt(BTN_BAND_SELECT);
          break;                         // Vector  4:  Port 3 Bit 1
      case  6: break;                         // Vector  6:  Port 3 Bit 2
      case  8:
          // P3.3 = RIT button
          buttonPressed = BTN_PRESSED_RIT;
          GPIO_clearInterrupt(BTN_RIT);
          break;                         // Vector  8:  Port 3 Bit 3
      case  10:
          // P3.4 = Filter Select button
          buttonPressed = BTN_PRESSED_FILTER_SELECT;
          GPIO_clearInterrupt(BTN_FILTER_SELECT);
          break;                         // Vector  10:  Port 3 Bit 4
      case  12: break;                         // Vector  12:  Port 3 Bit 5
      case  14: break;                         // Vector  14:  Port 3 Bit 6
      case  16:
          // P3.7 = Tune button
          buttonPressed = BTN_PRESSED_TUNE;
          GPIO_clearInterrupt(BTN_TUNE);
          break;                         // Vector  16:  Port 3 Bit 7
      default: break;
    }
}


// Port 2 interrupt service routine
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
    extern uint8_t txKeyState;
    extern uint8_t keyStateChanged;
    switch(__even_in_range(P2IV,16))
    {
      case  0: break;                         // Vector  0:  No interrupt
      case  2: break;                         // Vector  2:  Port 2 Bit 0
      case  4: break;                         // Vector  4:  Port 2 Bit 1
      case  6: break;                         // Vector  6:  Port 2 Bit 2
      case  8:
          // P2.3 = Sideband select button
          buttonPressed = BTN_PRESSED_DIGIT_SELECT;
          GPIO_clearInterrupt(BTN_DIGIT_SELECT);
          break;                         // Vector  8:  Port 2 Bit 3
      case  10:
          // P2.4 = Spot button
          buttonPressed = BTN_PRESSED_SPOT;
          GPIO_clearInterrupt(BTN_SPOT);
          break;                         // Vector  10:  Port 2 Bit 4
      case  12: break;                         // Vector  12:  Port 2 Bit 5
      case  14: break;                         // Vector  14:  Port 2 Bit 6
      case  16:
          // P2.7 = straight key input - i.e. cw keyer input
          keyStateChanged = 1;
          if (txKeyState == TX_KEY_UP)
          {
              // key must have been pressed
              txKeyState = TX_KEY_DOWN;  // to indicate key down
              GPIO_clearInterrupt(STRAIGHT_KEY);
              P2IES ^= BIT7;  // change interrupt edge to detect when key has been released
              // now enable transmit
              //selectAudioState(MUTE);
              GPIO_setOutputHighOnPin(CW_OUT);
          }
          else
          {
              // key was released
              txKeyState = TX_KEY_UP;  // to indicate key up
              GPIO_clearInterrupt(STRAIGHT_KEY);
              P2IES ^= BIT7;  // change interrupt edge to detect when key has been pressed
              // now enable receive
              //selectAudioState(UNMUTE);
              GPIO_setOutputLowOnPin(CW_OUT);
          }

          break;                         // Vector  16:  Port 2 Bit 7
      default: break;
    }
}


