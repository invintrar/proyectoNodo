/**
  Section: Included Files
 */

#include <stdio.h>
#include "tmr1.h"

/**
 Section: File specific functions
 */
void (*TMR1_InterruptHandler)(void) = NULL;
void TMR1_CallBack(void);

/**
  Section: Data Type Definitions
 */

/** TMR Driver Hardware Instance Object

  @Summary
    Defines the object required for the maintenance of the hardware instance.

  @Description
    This defines the object required for the maintenance of the hardware
    instance. This object exists once per hardware instance of the peripheral.

  Remarks:
    None.
 */

typedef struct _TMR_OBJ_STRUCT {
    /* Timer Elapsed */
    volatile bool timerElapsed;
    /*Software Counter value*/
    volatile uint8_t count;

} TMR_OBJ;

static TMR_OBJ tmr1_obj;

/**
  Section: Driver Interface
 */

void TMR1_Initialize(void) {
    //TMR1 0; 
    TMR1 = 0x00;
    //Period = 0.004 s; Frequency = 40000000 Hz; PR1 2499; 
    PR1 = 0x9C3;
    
    //Period = 0.005 s; Frequency = 40000000 Hz; PR1 3124; 
    //PR1 = 0xC34;
    //TCKPS 1:64; TON enabled; TSIDL disabled; TCS FOSC/2; TSYNC disabled; TGATE disabled; 
    T1CON = 0x8020;

    if (TMR1_InterruptHandler == NULL) {
        TMR1_SetInterruptHandler(&TMR1_CallBack);
    }

    IFS0bits.T1IF = false;
    IEC0bits.T1IE = true;

    tmr1_obj.timerElapsed = false;

}

void __attribute__((interrupt, no_auto_psv)) _T1Interrupt() {
    /* Check if the Timer Interrupt/Status is set */

    //***User Area Begin

    // ticker function call;
    // ticker is 1 -> Callback function gets called everytime this ISR executes
    if (TMR1_InterruptHandler) {
        TMR1_InterruptHandler();
    }

    //***User Area End

    tmr1_obj.count++;
    tmr1_obj.timerElapsed = true;
    IFS0bits.T1IF = false;
}

void TMR1_Period16BitSet(uint16_t value) {
    /* Update the counter values */
    PR1 = value;
    /* Reset the status information */
    tmr1_obj.timerElapsed = false;
}

uint16_t TMR1_Period16BitGet(void) {
    return ( PR1);
}

void TMR1_Counter16BitSet(uint16_t value) {
    /* Update the counter values */
    TMR1 = value;
    /* Reset the status information */
    tmr1_obj.timerElapsed = false;
}

uint16_t TMR1_Counter16BitGet(void) {
    return ( TMR1);
}

void __attribute__((weak)) TMR1_CallBack(void) {
    // Add your custom callback code here
    uint8_t data[9] = {0};
    uint8_t st = 0;
    if (bPMaster) {
        st = ADXL355_Status();
        ADXL355_Read_FIFO(data); // read data of ADXL355z and save in dataAdxl
        Led_verde_toggle();
        if (contEnv > 50) {
            ADC1_SamplingStart();
            ADC1_SamplingStop();
            uint8_t env[12] = {0};
            contEnv = 0;
            env[0] = 4;
            env[1] = data[0];
            env[2] = data[1];
            env[3] = data[2];
            env[4] = data[3];
            env[5] = data[4];
            env[6] = data[5];
            env[7] = data[6];
            env[8] = data[7];
            env[9] = data[8];
            env[10] = vAdc;
            env[11] = vAdc >> 8;
            //Sent Data for NRF24L01+
            RF24L01_sendData(env, 12);
        }
        contEnv++;
    } else if (bSaveData) {
        bDataAdxl = 1;
        st = ADXL355_Status();
        ADXL355_Read_FIFO(data); // read data of ADXL355z and save in dataAdxl
        st = ADXL355_Status();
        uint8_t i = 0;
        for (i = 0; i < 9; i++) {
            dataAdxl[i] = data[i];
        }
    }

}

void TMR1_SetInterruptHandler(void (* InterruptHandler)(void)) {
    IEC0bits.T1IE = false;
    TMR1_InterruptHandler = InterruptHandler;
    IEC0bits.T1IE = true;
}

void TMR1_Start(void) {
    /* Reset the status information */
    tmr1_obj.timerElapsed = false;

    /*Enable the interrupt*/
    IEC0bits.T1IE = true;

    /* Start the Timer */
    T1CONbits.TON = 1;
}

void TMR1_Stop(void) {
    /* Stop the Timer */
    T1CONbits.TON = false;

    /*Disable the interrupt*/
    IEC0bits.T1IE = false;
}

bool TMR1_GetElapsedThenClear(void) {
    bool status;

    status = tmr1_obj.timerElapsed;

    if (status == true) {
        tmr1_obj.timerElapsed = false;
    }
    return status;
}

int TMR1_SoftwareCounterGet(void) {
    return tmr1_obj.count;
}

void TMR1_SoftwareCounterClear(void) {
    tmr1_obj.count = 0;
}

/**
 End of File
 */
