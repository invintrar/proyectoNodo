#include "system.h"

void SYSTEM_Initialize(void) {
    oscConfig();

    pines_Conf_Init();

    INTERRUPT_Initialize();

    EXT_INT_Initialize();

    //Initialize module ADC1
    //ADC1_Initialize();

    INTERRUPT_GlobalEnable();
    // Initialization microSD and SPI1(5MHz)
    //SD_Init();
    
    SPI1_Init(FAST);
    
    // Initialization SPI2 (2.5MHz)
    SPI2_Init();

    //Initialize Real clock time
    DS3234_init();

    // Initialize accelerometer
    //ADXL355_Init();

    // Initialize module nRF24L01
    RF24L01_Init();
    
    // Initialize Timer2 with period 1s
    TMR2_Initialize();

    SYSTEM_CORCONModeOperatingSet(CORCON_MODE_PORVALUES);
}

/**
 End of File
 */