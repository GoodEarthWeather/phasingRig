/*
 * init.c
 *
 *  Created on: June 26, 2019
 *      Author: dmcneill
 *//////
#include "main.h"

//This file contains the routines to initialize everything
// Phasing rig control board

// initialize the clock system
void initClocks(void)
{

    // Configure one FRAM waitstate as required by the device datasheet for MCLK
    // operation beyond 8MHz _before_ configuring the clock system.
    FRAMCtl_configureWaitStateControl(FRAMCTL_ACCESS_TIME_CYCLES_1);

    //Initialize external 32.768kHz clock
    CS_setExternalClockSource(32768);
    CS_turnOnXT1LF(CS_XT1_DRIVE_3);

    //Set DCO frequency to 8MHz
    CS_initClockSignal(CS_FLLREF,CS_XT1CLK_SELECT,CS_CLOCK_DIVIDER_1);
    CS_initFLLSettle(8000,244);  // 244*32.768 is approximately 8000kHz = 8MHz
    //Set ACLK = External 32.768kHz clock with frequency divider of 1
    CS_initClockSignal(CS_ACLK,CS_XT1CLK_SELECT,CS_CLOCK_DIVIDER_1);
    //Set SMCLK = DCO with frequency divider of 1
    CS_initClockSignal(CS_SMCLK,CS_DCOCLKDIV_SELECT,CS_CLOCK_DIVIDER_1);
    //Set MCLK = DCO with frequency divider of 1
    CS_initClockSignal(CS_MCLK,CS_DCOCLKDIV_SELECT,CS_CLOCK_DIVIDER_1);

    //Clear all OSC fault flag
    CS_clearAllOscFlagsWithTimeout(1000);
}

// initialize GPIO
void initGPIO(void)
{
    // Configure Pins for I2C
    //Set P1.3 and P1.2 as Primary Module Function Input.
    //Select Port 1
    //Set Pin 2, 3 to input Primary Module Function, (UCB0SIMO/UCB0SDA, UCB0SOMI/UCB0SCL).
   GPIO_setAsPeripheralModuleFunctionInputPin(
       GPIO_PORT_P1,
       GPIO_PIN2 + GPIO_PIN3,
       GPIO_PRIMARY_MODULE_FUNCTION
       );

   /*
   * Set Pin 2.0, 2.1 to input Primary Module Function, LFXT.
   * This is for configuration of the external 32.768kHz crystal
   */
   GPIO_setAsPeripheralModuleFunctionInputPin(
       GPIO_PORT_P2,
       GPIO_PIN0 + GPIO_PIN1,
       GPIO_PRIMARY_MODULE_FUNCTION
   );


   // Configure P1.1 (A1) as analog ADC input
   GPIO_setAsPeripheralModuleFunctionInputPin(
           GPIO_PORT_P1,
           GPIO_PIN1,
           GPIO_TERNARY_MODULE_FUNCTION
   );


   //Initialize rotary encoder inputs and rotary encoder switch
   // Use P2.2 and P4.5 as encoder inputs; P4.6 as encoder switch
   GPIO_setAsInputPin(
       GPIO_PORT_P4,
       GPIO_PIN5 + GPIO_PIN6
       );
   GPIO_setAsInputPin(
       GPIO_PORT_P2,
       GPIO_PIN2
       );
   GPIO_selectInterruptEdge(GPIO_PORT_P4, GPIO_PIN5, GPIO_HIGH_TO_LOW_TRANSITION);  // interrupt on falling edge of P4.5
   GPIO_selectInterruptEdge(GPIO_PORT_P4, GPIO_PIN6, GPIO_HIGH_TO_LOW_TRANSITION);  // interrupt on falling edge of encoder switch
   // Configure interrupts for encoder
   GPIO_enableInterrupt(ENCODER_A);
   GPIO_clearInterrupt(ENCODER_A);
   GPIO_enableInterrupt(ENCODER_SWITCH);
   GPIO_clearInterrupt(ENCODER_SWITCH);

   // configure GPIO for LCD
   // P1.7, P4.3, P4.4, P5.3 = data bits 4,5,6,7 respectively
   // P1.5 = RS bit
   // P1.6 = EN bit = clock
   GPIO_setAsOutputPin(
       GPIO_PORT_P1,
       GPIO_PIN5 + GPIO_PIN6 + GPIO_PIN7
       );
   GPIO_setAsOutputPin(
       GPIO_PORT_P4,
       GPIO_PIN3 + GPIO_PIN4
       );
   GPIO_setAsOutputPin(
       GPIO_PORT_P5,
       GPIO_PIN3
       );
   // set all LCD bits low
   GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN5 + GPIO_PIN6 + GPIO_PIN7);
   GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN3 + GPIO_PIN4);
   GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN3);


   // Configure all button inputs
   // Menu button
   GPIO_setAsInputPin(
       GPIO_PORT_P4,
       GPIO_PIN0
       );

   // CW speed button
   GPIO_setAsInputPin(
       GPIO_PORT_P5,
       GPIO_PIN7
       );

   // Band select, Mute, filter select, tune buttons
   GPIO_setAsInputPin(
       GPIO_PORT_P3,
       GPIO_PIN1 + GPIO_PIN3 + GPIO_PIN4 + GPIO_PIN7
       );

   // digit select, spot buttons, straight key input
   GPIO_setAsInputPin(
       GPIO_PORT_P2,
       GPIO_PIN3 + GPIO_PIN4 + GPIO_PIN7
       );

   // select interrupt edges
   GPIO_selectInterruptEdge(GPIO_PORT_P4, GPIO_PIN0, GPIO_HIGH_TO_LOW_TRANSITION);
   GPIO_selectInterruptEdge(GPIO_PORT_P5, GPIO_PIN7, GPIO_HIGH_TO_LOW_TRANSITION);
   GPIO_selectInterruptEdge(GPIO_PORT_P3, GPIO_PIN1 + GPIO_PIN3 + GPIO_PIN4 + GPIO_PIN7, GPIO_HIGH_TO_LOW_TRANSITION);
   GPIO_selectInterruptEdge(GPIO_PORT_P2, GPIO_PIN3 + GPIO_PIN4 + GPIO_PIN7, GPIO_HIGH_TO_LOW_TRANSITION);


   // configure transceiver outputs - default all to low output except for mute
   GPIO_setAsOutputPin(TR_SWITCH);
   GPIO_setAsOutputPin(TR_MUTE);
   GPIO_setAsOutputPin(CW_OUT);
   GPIO_setOutputLowOnPin(TR_SWITCH);
   GPIO_setOutputHighOnPin(TR_MUTE);
   GPIO_setOutputLowOnPin(CW_OUT);

   // configure band selection and default to 40M
   GPIO_setAsOutputPin(BAND_40M_SELECT);
   GPIO_setAsOutputPin(BAND_30M_SELECT);
   GPIO_setAsOutputPin(BAND_20M_SELECT);
   GPIO_setAsOutputPin(BAND_17M_SELECT);
   GPIO_setAsOutputPin(BAND_10M_SELECT);
   GPIO_setOutputHighOnPin(BAND_40M_SELECT);  // default to 40M
   GPIO_setOutputLowOnPin(BAND_30M_SELECT);
   GPIO_setOutputLowOnPin(BAND_20M_SELECT);
   GPIO_setOutputLowOnPin(BAND_17M_SELECT);
   GPIO_setOutputLowOnPin(BAND_10M_SELECT);

   GPIO_setAsOutputPin(FILTER_SELECT);
   GPIO_setAsOutputPin(SIDEBAND_SELECT);


   // enable and clear interrupts
   GPIO_enableInterrupt(BTN_CWSPEED);
   GPIO_enableInterrupt(BTN_MUTE);
   GPIO_enableInterrupt(BTN_DIGIT_SELECT);
   GPIO_enableInterrupt(BTN_FILTER_SELECT);
   GPIO_enableInterrupt(BTN_BAND_SELECT);
   GPIO_enableInterrupt(BTN_SPOT);
   GPIO_enableInterrupt(BTN_TUNE);
   GPIO_enableInterrupt(BTN_MENU);
   GPIO_enableInterrupt(STRAIGHT_KEY);

   GPIO_clearInterrupt(BTN_CWSPEED);
   GPIO_clearInterrupt(BTN_MUTE);
   GPIO_clearInterrupt(BTN_DIGIT_SELECT);
   GPIO_clearInterrupt(BTN_FILTER_SELECT);
   GPIO_clearInterrupt(BTN_BAND_SELECT);
   GPIO_clearInterrupt(BTN_SPOT);
   GPIO_clearInterrupt(BTN_TUNE);
   GPIO_clearInterrupt(BTN_MENU);
   GPIO_clearInterrupt(STRAIGHT_KEY);



   /*
    * Disable the GPIO power-on default high-impedance mode to activate
    * previously configured port settings
    */
   PMM_unlockLPM5();
}

// Initialize ADC for measuring battery voltage or CW speed pot
void initADC(uint8_t measureType)
{
    ADC_init(ADC_BASE,
        ADC_SAMPLEHOLDSOURCE_SC,
        ADC_CLOCKSOURCE_ADCOSC,
        ADC_CLOCKDIVIDER_1);

    ADC_enable(ADC_BASE);

    ADC_setupSamplingTimer(ADC_BASE,
        ADC_CYCLEHOLD_16_CYCLES,
        ADC_MULTIPLESAMPLESDISABLE);

    ADC_setDataReadBackFormat(ADC_BASE,ADC_UNSIGNED_BINARY);

    //Configure Memory Buffer
    /*
     * Base Address for the ADC Module
     * Use input A1 for VBAT sense input
     * Use positive reference of Internally generated Vref
     * Use negative reference of AVss
     */
    if ( measureType == BATTERY_MEASUREMENT)
    {
        ADC_configureMemory(ADC_BASE,
        ADC_INPUT_A1,
        ADC_VREFPOS_INT,
        ADC_VREFNEG_AVSS);
        ADC_setResolution (ADC_BASE, ADC_RESOLUTION_12BIT);
    } else {  // CW speed measurement
        ADC_configureMemory(ADC_BASE,
        ADC_INPUT_A4,
        ADC_VREFPOS_INT,
        ADC_VREFNEG_AVSS);
        ADC_setResolution (ADC_BASE, ADC_RESOLUTION_10BIT);
    }

    //Internal Reference ON
    PMM_enableInternalReference();

    // Select 1.5V reference voltage
    PMM_selectVoltageReference (PMM_REFVSEL_1_5V);

    //Configure internal reference
    //If ref voltage no ready, WAIT
    while (PMM_REFGEN_NOTREADY ==
            PMM_getVariableReferenceVoltageStatus()) ;

    ADC_disable(ADC_BASE);
}


