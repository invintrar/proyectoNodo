/**
   Section: Includes
 */
#include "ext_int.h"

/**
   Section: External Interrupt Handlers
 */

void __attribute__((weak)) EX_INT0_CallBack(void) {
    // Add your custom callback code here
}

/**
  Interrupt Handler for EX_INT0 - INT0
 */
void __attribute__((interrupt, no_auto_psv)) _INT0Interrupt(void) {
    //***User Area Begin->code: External Interrupt 0***
    EX_INT0_CallBack();

    //***User Area End->code: External Interrupt 0***
    EX_INT0_InterruptFlagClear();
}


/**
 * Interrupt for ADXL355Z
 */
void __attribute__((weak)) EX_INT1_CallBack(void) {
    bDataAdxl = 1; // Use for check that data exist 
    ADXL355_Read_FIFO_Full(); // read data of ADXL355z and save in dataAdxl
    if (bPMaster) {
        Led_verde_toggle();
        if (contEnv > 15) {
            ADC1_SamplingStart();
            ADC1_SamplingStop();
            uint8_t env[12] = {0};
            contEnv = 0;
            env[0] = 4;
            env[1] = dataAdxl[0];
            env[2] = dataAdxl[1];
            env[3] = dataAdxl[2];
            env[4] = dataAdxl[3];
            env[5] = dataAdxl[4];
            env[6] = dataAdxl[5];
            env[7] = dataAdxl[6];
            env[8] = dataAdxl[7];
            env[9] = dataAdxl[8];
            env[10] = vAdc;
            env[11] = vAdc >> 8;
            //Sent Data for NRF24L01+
            RF24L01_sendData(env, 12);
        }
        contEnv++;
    }
}

/**
  Interrupt Handler for EX_INT1 - INT1
 */
void __attribute__((interrupt, no_auto_psv)) _INT1Interrupt(void) {
    //***User Area Begin->code: External Interrupt 1***

    EX_INT1_CallBack();

    //***User Area End->code: External Interrupt 1***
    EX_INT1_InterruptFlagClear();
}

/**
 * Interrupt for NRF24L01+
 */
void __attribute__((weak)) EX_INT2_CallBack(void) {
    //Return 1:Data Sent, 2:RX_DR, 3:MAX_RT
    bNrf = RF24L01_status();
    RF24L01_clear_interrupts();
}

/**
  Interrupt Handler for EX_INT2 - INT2
 */
void __attribute__((interrupt, no_auto_psv)) _INT2Interrupt(void) {
    //***User Area Begin->code: External Interrupt 2***

    EX_INT2_CallBack();

    //***User Area End->code: External Interrupt 2***
    EX_INT2_InterruptFlagClear();
}
/**
    Section: External Interrupt Initializers
 */

/**
    void EXT_INT_Initialize(void)

    Initializer for the following external interrupts
    INT0
    INT1
    INT2
 */
void EXT_INT_Initialize(void) {
    /*******
     * INT0
     * Clear the interrupt flag
     * Set the external interrupt edge detect
     * Enable the interrupt, if enabled in the UI. 
     ********/
    EX_INT0_InterruptFlagClear();
    EX_INT0_PositiveEdgeSet();
    EX_INT0_InterruptDisable();
    /*******
     * INT1
     * Clear the interrupt flag
     * Set the external interrupt edge detect
     * Enable the interrupt, if enabled in the UI. 
     ********/
    EX_INT1_InterruptFlagClear();
    EX_INT1_PositiveEdgeSet();
    EX_INT1_InterruptEnable();
    /*******
     * INT2
     * Clear the interrupt flag
     * Set the external interrupt edge detect
     * Enable the interrupt, if enabled in the UI. 
     ********/
    EX_INT2_InterruptFlagClear();
    EX_INT2_NegativeEdgeSet();
    EX_INT2_InterruptEnable();
}
