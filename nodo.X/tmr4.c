
/**
  Section: Included Files
 */

#include <stdio.h>
#include "tmr4.h"

/**
 Section: File specific functions
 */
void (*TMR4_InterruptHandler)(void) = NULL;
void TMR4_CallBack(void);

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

static TMR_OBJ tmr4_obj;

/**
  Section: Driver Interface
 */

void TMR4_Initialize(void) {
    //TMR5 0; 
    TMR5 = 0x00;
    //PR5 36621; 
    PR5 = 0x8F0D;
    //TMR4 0; 
    TMR4 = 0x00;
    //Period = 60 s; Frequency = 40000000 Hz; PR4 6143; 
    PR4 = 0x17FF;
    //TCKPS 1:1; T32 32 Bit; TON enabled; TSIDL disabled; TCS FOSC/2; TGATE disabled; 
    T4CON = 0x8008;

    if (TMR4_InterruptHandler == NULL) {
        TMR4_SetInterruptHandler(&TMR4_CallBack);
    }

    IFS1bits.T5IF = false;
    IEC1bits.T5IE = true;

    tmr4_obj.timerElapsed = false;

}

void __attribute__((interrupt, no_auto_psv)) _T5Interrupt() {
    /* Check if the Timer Interrupt/Status is set */

    //***User Area Begin

    // ticker function call;
    // ticker is 1 -> Callback function gets called everytime this ISR executes
    if (TMR4_InterruptHandler) {
        TMR4_InterruptHandler();
    }

    //***User Area End

    tmr4_obj.count++;
    tmr4_obj.timerElapsed = true;
    IFS1bits.T5IF = false;
}

void TMR4_Period32BitSet(uint32_t value) {
    /* Update the counter values */
    PR4 = (value & 0x0000FFFF);
    PR5 = ((value & 0xFFFF0000) >> 16);
}

uint32_t TMR4_Period32BitGet(void) {
    uint32_t periodVal = 0xFFFFFFFF;

    /* get the timer period value and return it */
    periodVal = (((uint32_t) PR5 << 16) | PR4);

    return ( periodVal);

}

void TMR4_Counter32BitSet(uint32_t value) {
    /* Update the counter values */
    TMR5HLD = ((value & 0xFFFF0000) >> 16);
    TMR4 = (value & 0x0000FFFF);

}

uint32_t TMR4_Counter32BitGet(void) {
    uint32_t countVal = 0xFFFFFFFF;
    uint16_t countValUpper;
    uint16_t countValLower;

    countValLower = TMR4;
    countValUpper = TMR5HLD;

    /* get the current counter value and return it */
    countVal = (((uint32_t) countValUpper << 16) | countValLower);

    return ( countVal);

}

void __attribute__((weak)) TMR4_CallBack(void) {
    // Add your custom callback code here
    //Timer 60Seconds
    timeSec = 0;
    timeMin++;
    if (timeMin == 60) {
        timeHor++;
    }
}

void TMR4_SetInterruptHandler(void (* InterruptHandler)(void)) {
    IEC1bits.T5IE = false;
    TMR4_InterruptHandler = InterruptHandler;
    IEC1bits.T5IE = true;
}

void TMR4_Start(void) {
    /* Reset the status information */
    tmr4_obj.timerElapsed = false;

    /*Enable the interrupt*/
    IEC1bits.T5IE = true;

    /* Start the Timer */
    T4CONbits.TON = 1;
}

void TMR4_Stop(void) {
    /* Stop the Timer */
    T4CONbits.TON = false;

    /*Disable the interrupt*/
    IEC1bits.T5IE = false;
}

bool TMR4_GetElapsedThenClear(void) {
    bool status;

    status = tmr4_obj.timerElapsed;

    if (status == true) {
        tmr4_obj.timerElapsed = false;
    }
    return status;
}

int TMR4_SoftwareCounterGet(void) {
    return tmr4_obj.count;
}

void TMR4_SoftwareCounterClear(void) {
    tmr4_obj.count = 0;
}

/**
 End of File
 */
