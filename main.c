#include "driverlib.h"
#include "main.h"
#include "lcdlib.h"


uint8_t encoderCWCount, encoderCCWCount;
uint32_t si5351FreqOut;
uint32_t freqMultiplier;
uint8_t selectedBand;
uint8_t selectedFilter;
uint8_t selectedSideband;
uint16_t batteryVoltage;
uint32_t maxBandFreq;
uint32_t minBandFreq;
uint8_t selectedMenuFunction;
uint8_t txKeyState;
uint8_t ritState;
int16_t ritOffset;


int main(void) {

    uint32_t deltaFreq;
    extern uint8_t volatile buttonPressed;


    WDT_A_hold(WDT_A_BASE);
    initGPIO();
    initClocks();
    lcdInit();
    initADC();
    si5351_start();

    // disable TX and RIT
    txKeyState = TX_KEY_UP; // means not transmitting
    ritState = DISABLED;
    ritOffset = 0;

    // set defaults
    freqMultiplier = 1;
    selectedBand = BAND_40M;
    selectedFilter = SSB_FILTER;
    selectedSideband = LOWER_SIDEBAND;
    selectedMenuFunction = MENU_FUNCTION_BATVOLTAGE;
    selectBand();
    selectFilter();
    selectSideband();
    selectMenuFunction();

    // set initial si5351 clock to 7MHz
    si5351FreqOut = BAND_40M_LOWER;
    si5351_set_RX_freq(si5351FreqOut<<2);
    si5351_set_TX_freq(si5351FreqOut);

    // setup encoder variables
    encoderCWCount = 0;
    encoderCCWCount = 0;

    buttonPressed = BTN_PRESSED_NONE;
    __bis_SR_register(GIE);
    while (1)
    {
        // check encoder
        if ((encoderCWCount != 0) || (encoderCCWCount != 0))  // if true, the encoder knob was turned
        {
            if ( encoderCWCount > encoderCCWCount ) // net count indicates frequency increase
            {
                deltaFreq = encoderCWCount - encoderCCWCount;
                if ( (si5351FreqOut + deltaFreq*freqMultiplier) <= maxBandFreq )
                {
                    (ritState == ENABLED) ? (ritOffset += deltaFreq*freqMultiplier) : (si5351FreqOut += deltaFreq*freqMultiplier);
                    si5351_set_RX_freq((si5351FreqOut+ritOffset)<<2);
                    si5351_set_TX_freq(si5351FreqOut);
                    updateDisplay();
                }
            }
            else if ( encoderCWCount < encoderCCWCount )  // frequency decrease
            {
                deltaFreq = encoderCCWCount - encoderCWCount;
                if ( (si5351FreqOut - deltaFreq*freqMultiplier) >= minBandFreq )
                {
                    (ritState == ENABLED) ? (ritOffset -= deltaFreq*freqMultiplier) : (si5351FreqOut -= deltaFreq*freqMultiplier);
                    si5351_set_RX_freq((si5351FreqOut+ritOffset)<<2);
                    si5351_set_TX_freq(si5351FreqOut);
                    updateDisplay();
                }
            }
        encoderCWCount = encoderCCWCount = 0;
        }

        // check buttons
        switch (buttonPressed)
        {
        case BTN_PRESSED_NONE :
            break;
        case BTN_PRESSED_CWSPEED :
            buttonPressed = BTN_PRESSED_NONE;
            break;
        case BTN_PRESSED_RIT :
            buttonPressed = BTN_PRESSED_NONE;
            break;
            /*
        case BTN_PRESSED_SB_SELECT :
            (selectedSideband == UPPER_SIDEBAND) ? (selectedSideband = LOWER_SIDEBAND):(selectedSideband = UPPER_SIDEBAND);
            selectSideband();
            buttonPressed = BTN_PRESSED_NONE;
            break;
            */
        case BTN_PRESSED_FILTER_SELECT :
            (selectedFilter == CW_FILTER) ? (selectedFilter = SSB_FILTER) : (selectedFilter = CW_FILTER);
            selectFilter();
            buttonPressed = BTN_PRESSED_NONE;
            break;
        case BTN_PRESSED_BAND_SELECT :
            if (selectedBand == BAND_10M)
            {
                si5351_start(); // going from 10M to 40M band, so need to reset pllB to a fixed value
                selectedBand = BAND_40M;
            }
            else
            {
                selectedBand++;
            }
            selectBand();
            buttonPressed = BTN_PRESSED_NONE;
            break;
        case BTN_PRESSED_SPOT :
            buttonPressed = BTN_PRESSED_NONE;
            break;
        case BTN_PRESSED_TUNE :
            buttonPressed = BTN_PRESSED_NONE;
            break;
        case BTN_PRESSED_MENU :
            (selectedMenuFunction == MENU_FUNCTION_MUTE) ? (selectedMenuFunction = MENU_FUNCTION_SIDEBAND) : selectedMenuFunction++;
            selectMenuFunction();
            buttonPressed = BTN_PRESSED_NONE;
            break;
        case BTN_PRESSED_DIGIT_SELECT :
            if (freqMultiplier == 10000)
            {
                freqMultiplier = 1;
            }
            else
            {
                freqMultiplier *= 10;
            }
            moveFreqCursor();
            buttonPressed = BTN_PRESSED_NONE;
            break;
        default :
            break;
        }
    }
}

// routine to select band
void selectBand(void)
{
    selectAudioState(MUTE);
    switch (selectedBand)
    {
    case BAND_40M :
        GPIO_setOutputHighOnPin(BAND_40M_SELECT);
        GPIO_setOutputLowOnPin(BAND_30M_SELECT);
        GPIO_setOutputLowOnPin(BAND_20M_SELECT);
        GPIO_setOutputLowOnPin(BAND_17M_SELECT);
        GPIO_setOutputLowOnPin(BAND_10M_SELECT);
        si5351FreqOut = BAND_40M_LOWER;
        si5351_set_RX_freq(si5351FreqOut<<2);
        si5351_set_TX_freq(si5351FreqOut);
        selectedSideband = LOWER_SIDEBAND;
        selectSideband();
        maxBandFreq = BAND_40M_UPPER;
        minBandFreq = BAND_40M_LOWER;
        break;
    case BAND_30M :
        GPIO_setOutputLowOnPin(BAND_40M_SELECT);
        GPIO_setOutputHighOnPin(BAND_30M_SELECT);
        GPIO_setOutputLowOnPin(BAND_20M_SELECT);
        GPIO_setOutputLowOnPin(BAND_17M_SELECT);
        GPIO_setOutputLowOnPin(BAND_10M_SELECT);
        si5351FreqOut = BAND_30M_LOWER;
        si5351_set_RX_freq(si5351FreqOut<<2);
        si5351_set_TX_freq(si5351FreqOut);
        selectedSideband = UPPER_SIDEBAND;
        selectSideband();
        maxBandFreq = BAND_30M_UPPER;
        minBandFreq = BAND_30M_LOWER;
        break;
    case BAND_20M :
        GPIO_setOutputLowOnPin(BAND_40M_SELECT);
        GPIO_setOutputLowOnPin(BAND_30M_SELECT);
        GPIO_setOutputHighOnPin(BAND_20M_SELECT);
        GPIO_setOutputLowOnPin(BAND_17M_SELECT);
        GPIO_setOutputLowOnPin(BAND_10M_SELECT);
        si5351FreqOut = BAND_20M_LOWER;
        si5351_set_RX_freq(si5351FreqOut<<2);
        si5351_set_TX_freq(si5351FreqOut);
        selectedSideband = UPPER_SIDEBAND;
        selectSideband();
        maxBandFreq = BAND_20M_UPPER;
        minBandFreq = BAND_20M_LOWER;
        break;
    case BAND_17M :
        GPIO_setOutputLowOnPin(BAND_40M_SELECT);
        GPIO_setOutputLowOnPin(BAND_30M_SELECT);
        GPIO_setOutputLowOnPin(BAND_20M_SELECT);
        GPIO_setOutputHighOnPin(BAND_17M_SELECT);
        GPIO_setOutputLowOnPin(BAND_10M_SELECT);
        si5351FreqOut = BAND_17M_LOWER;
        si5351_set_RX_freq(si5351FreqOut<<2);
        si5351_set_TX_freq(si5351FreqOut);
        selectedSideband = UPPER_SIDEBAND;
        selectSideband();
        maxBandFreq = BAND_17M_UPPER;
        minBandFreq = BAND_17M_LOWER;
        break;
    case BAND_10M :
        GPIO_setOutputLowOnPin(BAND_40M_SELECT);
        GPIO_setOutputLowOnPin(BAND_30M_SELECT);
        GPIO_setOutputLowOnPin(BAND_20M_SELECT);
        GPIO_setOutputLowOnPin(BAND_17M_SELECT);
        GPIO_setOutputHighOnPin(BAND_10M_SELECT);
        si5351FreqOut = BAND_10M_LOWER;
        si5351_set_RX_freq(si5351FreqOut<<2);
        si5351_set_TX_freq(si5351FreqOut);
        selectedSideband = UPPER_SIDEBAND;
        selectSideband();
        maxBandFreq = BAND_10M_UPPER;
        minBandFreq = BAND_10M_LOWER;
        break;
    default :
        break;
    }
    // reset menu function
    ritState = DISABLED;
    ritOffset = 0;
    selectedMenuFunction = MENU_FUNCTION_BATVOLTAGE;
    selectAudioState(UNMUTE);
    updateDisplay();
}

// routine to select filter
void selectFilter(void)
{
    selectAudioState(MUTE);
    if (selectedFilter == CW_FILTER)
        GPIO_setOutputLowOnPin(FILTER_SELECT);  // set low for CW filter
    else
        GPIO_setOutputHighOnPin(FILTER_SELECT); // set high for SSB filter
    updateDisplay();
    selectAudioState(UNMUTE);
}

// routine to select filter
void selectSideband(void)
{
    if (selectedSideband == UPPER_SIDEBAND)
        GPIO_setOutputHighOnPin(SIDEBAND_SELECT);  // need to check
    else
        GPIO_setOutputLowOnPin(SIDEBAND_SELECT); // need to check
    updateDisplay();
}

// routine to set audio state - mute or unmute
void selectAudioState(uint8_t state)
{
    if ( state == MUTE )
        GPIO_setOutputHighOnPin(TR_MUTE); // set high for mute pin
    else if (state == UNMUTE)
        GPIO_setOutputLowOnPin(TR_MUTE); // set low for unmute pin
}

// routine to select desired menu functions
void selectMenuFunction(void)
{
    switch ( selectedMenuFunction )
    {
    case MENU_FUNCTION_SIDEBAND :
        selectAudioState(UNMUTE);
        // this function will just display the current sideband; no changing of sideband
        break;
    case MENU_FUNCTION_BATVOLTAGE :
        break;
    case MENU_FUNCTION_CWSPEED :
        break;
    case MENU_FUNCTION_RIT :
        ritState = ENABLED;
        ritOffset = 0;
        break;
    case MENU_FUNCTION_MUTE :
        ritState = DISABLED;
        ritOffset = 0;
        selectAudioState(MUTE);
        break;
    default :
        break;
    }
    updateDisplay();
}

// routine to measure battery voltage
void getBatteryVoltage(void)
{

    // start ADC conversion
    ADC_startConversion(ADC_BASE,ADC_SINGLECHANNEL);
    while (ADC_isBusy(ADC_BASE) == ADC_BUSY) {;}

    // get results
    batteryVoltage = (uint16_t)ADC_getResults(ADC_BASE);

}

