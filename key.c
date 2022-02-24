/*
 * key.c
 *
 *  Created on: Feb 23, 2022
 *      Author: fishi
 */

#include "driverlib.h"
#include "main.h"
#include "lcdlib.h"


// This routine is to handle a dit key press
void dit(void)
{
    uint8_t done;
    do {

        // dit key pressed; set output high for one unit
        Timer_A_startCounter(TIMER_A0_BASE,TIMER_A_UP_MODE);  // start side tone
        Timer_A_clearCaptureCompareInterrupt(TIMER_A2_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
        Timer_A_clear(TIMER_A2_BASE);  // clear key timer
        do {
            done = Timer_A_getCaptureCompareInterruptStatus(TIMER_A2_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_0,TIMER_A_CAPTURECOMPARE_INTERRUPT_FLAG);
        } while (done != TIMER_A_CAPTURECOMPARE_INTERRUPT_FLAG);
        //lastKey = DIT;

        //  wait one unit
        Timer_A_stop(TIMER_A0_BASE);  // stop side tone
        Timer_A_clearCaptureCompareInterrupt(TIMER_A2_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
        do {
            done = Timer_A_getCaptureCompareInterruptStatus(TIMER_A2_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_0,TIMER_A_CAPTURECOMPARE_INTERRUPT_FLAG);
        } while (done != TIMER_A_CAPTURECOMPARE_INTERRUPT_FLAG);
        Timer_A_clearCaptureCompareInterrupt(TIMER_A2_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
    } while (GPIO_getInputPinValue(DIT_KEY) == GPIO_INPUT_PIN_LOW);
}

// This routine is to handle the dah key
void dah(void)
{
    uint8_t done;
    uint8_t dahCount;

    do {
        // dah key pressed
        Timer_A_startCounter(TIMER_A0_BASE,TIMER_A_UP_MODE);  // start side tone
        Timer_A_clearCaptureCompareInterrupt(TIMER_A2_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
        Timer_A_clear(TIMER_A2_BASE);  // clear timer
        dahCount = 0;
        while (dahCount < 3)
        {
            done = Timer_A_getCaptureCompareInterruptStatus(TIMER_A2_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_0,TIMER_A_CAPTURECOMPARE_INTERRUPT_FLAG);
            if (done == TIMER_A_CAPTURECOMPARE_INTERRUPT_FLAG)
            {
                dahCount++;
                Timer_A_clearCaptureCompareInterrupt(TIMER_A2_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
            }
        }
        // lastKey = DAH;
        //  wait one unit
        Timer_A_stop(TIMER_A0_BASE);  // stop side tone
        Timer_A_clearCaptureCompareInterrupt(TIMER_A2_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
        do {
            done = Timer_A_getCaptureCompareInterruptStatus(TIMER_A2_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_0,TIMER_A_CAPTURECOMPARE_INTERRUPT_FLAG);
        } while (done != TIMER_A_CAPTURECOMPARE_INTERRUPT_FLAG);
        Timer_A_clearCaptureCompareInterrupt(TIMER_A2_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
    } while (GPIO_getInputPinValue(DAH_KEY) == GPIO_INPUT_PIN_LOW);

}

// This routine is to handle the dit and dah key
// 'key' is either DIT or DAH (i.e. 1 or 3)
void ditdah(uint8_t key)
{
    uint8_t done;
    uint8_t count;

    do {

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
        /*************
        if ((GPIO_getInputPinValue(DAH_KEY) == GPIO_INPUT_PIN_LOW) && (GPIO_getInputPinValue(DIT_KEY) == GPIO_INPUT_PIN_LOW) )  // iambic mode, so alternate dit/dah
        {
            (key == DIT) ? (key = DAH) : (key = DIT);
        }
        ***************/
        if ((key == DIT) && (GPIO_getInputPinValue(DIT_KEY) == GPIO_INPUT_PIN_HIGH))
            break;
        if ((key == DAH) && (GPIO_getInputPinValue(DAH_KEY) == GPIO_INPUT_PIN_HIGH))
            break;
    } while ((GPIO_getInputPinValue(DAH_KEY) == GPIO_INPUT_PIN_LOW) || (GPIO_getInputPinValue(DIT_KEY) == GPIO_INPUT_PIN_LOW));

}
