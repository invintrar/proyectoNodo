#include "system.h"

void SYSTEM_Initialize(void) {
    oscConfig();

    pines_Conf_Init();

    SPI2_Init();

    INTERRUPT_Initialize();

    EXT_INT_Initialize();



    //Initialize module ADC1
    ADC1_Initialize();

    INTERRUPT_GlobalEnable();

    //Check uSD Inserted
    if (SD_Detect() == DETECTED) {
        buSDState = SD_Init();
    } else {
        SPI1_Init(FAST);
    }

    //Initialize Real clock time
    DS3234_Init();

    // Initialize accelerometer
    ADXL355_Init();

    //Initialize module nRF24L01
    RF24L01_Init();

    TMR1_Initialize();

    TMR2_Initialize();

    TMR4_Initialize();

    SYSTEM_CORCONModeOperatingSet(CORCON_MODE_PORVALUES);

}

/**
 End of File
 */