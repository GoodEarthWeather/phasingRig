/*
 * key.c
 *
 *  Created on: Feb 23, 2022
 *      Author: fishi
 */

#include "driverlib.h"
#include "main.h"
#include "lcdlib.h"

uint8_t iambicMode;

// This routine is to handle the dit and dah key
// 'key' is either DIT or DAH (i.e. 1 or 3)
void ditdah(uint8_t key)
{
    uint8_t done;
    uint8_t count;
    uint8_t ditKeyState;
    uint8_t dahKeyState;

    iambicMode = 0;

    do {
        ditKeyState = GPIO_getInputPinValue(DIT_KEY);
        dahKeyState = GPIO_getInputPinValue(DAH_KEY);
        // iambic test
        if ( (dahKeyState == GPIO_INPUT_PIN_LOW) && (ditKeyState == GPIO_INPUT_PIN_LOW) )  // iambic mode, so alternate dit/dah
        {
            (key == DIT) ? (key = DAH) : (key = DIT);
            iambicMode = 1;
        } else {
            iambicMode = 0;
            // not in iambic mode, so set key to whatever key remains pressed
            (dahKeyState == GPIO_INPUT_PIN_LOW) ? (key = DAH) : (key = DIT);
            // end of iambic test
        }

        if ((ditKeyState == GPIO_INPUT_PIN_HIGH) && (dahKeyState == GPIO_INPUT_PIN_HIGH))
            break;

        Timer_A_startCounter(TIMER_A0_BASE,TIMER_A_UP_MODE);  // start side tone
        Timer_A_clearCaptureCompareInterrupt(TIMER_A2_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
        Timer_A_clear(TIMER_A2_BASE);  // clear timer
        count = 0;
        while (count < key)
        {
            done = Timer_A_getCaptureCompareInterruptStatus(TIMER_A2_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_0,TIMER_A_CAPTURECOMPARE_INTERRUPT_FLAG);
            if (done == TIMER_A_CAPTURECOMPARE_INTERRUPT_FLAG)
            {
                count++;
                Timer_A_clearCaptureCompareInterrupt(TIMER_A2_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
            }
        }
        //  wait one unit
        Timer_A_stop(TIMER_A0_BASE);  // stop side tone
        Timer_A_clearCaptureCompareInterrupt(TIMER_A2_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
        do {
            done = Timer_A_getCaptureCompareInterruptStatus(TIMER_A2_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_0,TIMER_A_CAPTURECOMPARE_INTERRUPT_FLAG);
        } while (done != TIMER_A_CAPTURECOMPARE_INTERRUPT_FLAG);
        Timer_A_clearCaptureCompareInterrupt(TIMER_A2_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);


        ditKeyState = GPIO_getInputPinValue(DIT_KEY);
        dahKeyState = GPIO_getInputPinValue(DAH_KEY);

        if ((key == DIT) && (ditKeyState == GPIO_INPUT_PIN_HIGH))
            break;
        if ((key == DAH) && (dahKeyState == GPIO_INPUT_PIN_HIGH))
            break;

    } while (((dahKeyState) == GPIO_INPUT_PIN_LOW) || (ditKeyState == GPIO_INPUT_PIN_LOW));

}
