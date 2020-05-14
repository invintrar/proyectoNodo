
/**
  Section: Included Files
 */

#include <stdio.h>
#include "tmr2.h"

/**
 Section: File specific functions
 */
void (*TMR2_InterruptHandler)(void) = NULL;
void TMR2_CallBack(void);

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

static TMR_OBJ tmr2_obj;

/**
  Section: Driver Interface
 */

void TMR2_Initialize(void) {
    //TMR3 0; 
    TMR3 = 0x00;
    //PR3 610; 
    PR3 = 0x262;
    //TMR2 0; 
    TMR2 = 0x00;
    //Period = 1 s; Frequency = 40000000 Hz; PR2 23039; 
    PR2 = 0x59FF;
    //TCKPS 1:1; T32 32 Bit; TON enabled; TSIDL disabled; TCS FOSC/2; TGATE disabled; 
    T2CON = 0x8008;

    if (TMR2_InterruptHandler == NULL) {
        TMR2_SetInterruptHandler(&TMR2_CallBack);
    }

    IFS0bits.T3IF = false;
    IEC0bits.T3IE = true;

    tmr2_obj.timerElapsed = false;

}

void __attribute__((interrupt, no_auto_psv)) _T3Interrupt() {
    /* Check if the Timer Interrupt/Status is set */

    //***User Area Begin

    // ticker function call;
    // ticker is 1 -> Callback function gets called everytime this ISR executes
    if (TMR2_InterruptHandler) {
        TMR2_InterruptHandler();
    }

    //***User Area End

    tmr2_obj.count++;
    tmr2_obj.timerElapsed = true;
    IFS0bits.T3IF = false;
}

void TMR2_Period32BitSet(uint32_t value) {
    /* Update the counter values */
    PR2 = (value & 0x0000FFFF);
    PR3 = ((value & 0xFFFF0000) >> 16);
}

uint32_t TMR2_Period32BitGet(void) {
    uint32_t periodVal = 0xFFFFFFFF;

    /* get the timer period value and return it */
    periodVal = (((uint32_t) PR3 << 16) | PR2);

    return ( periodVal);

}

void TMR2_Counter32BitSet(uint32_t value) {
    /* Update the counter values */
    TMR3HLD = ((value & 0xFFFF0000) >> 16);
    TMR2 = (value & 0x0000FFFF);

}

uint32_t TMR2_Counter32BitGet(void) {
    uint32_t countVal = 0xFFFFFFFF;
    uint16_t countValUpper;
    uint16_t countValLower;

    countValLower = TMR2;
    countValUpper = TMR3HLD;

    /* get the current counter value and return it */
    countVal = (((uint32_t) countValUpper << 16) | countValLower);

    return ( countVal);

}

void __attribute__((weak)) TMR2_CallBack(void) {
    // Add your custom callback code here
    //Timer 1s
    timeSec++;
    timeMcs = 0;
    timeMls = 0;
    LATAbits.LATA1^=1;
}

void TMR2_SetInterruptHandler(void (* InterruptHandler)(void)) {
    IEC0bits.T3IE = false;
    TMR2_InterruptHandler = InterruptHandler;
    IEC0bits.T3IE = true;
}

void TMR2_Start(void) {
    /* Reset the status information */
    tmr2_obj.timerElapsed = false;

    /*Enable the interrupt*/
    IEC0bits.T3IE = true;

    /* Start the Timer */
    T2CONbits.TON = 1;
}

void TMR2_Stop(void) {
    /* Stop the Timer */
    T2CONbits.TON = false;

    /*Disable the interrupt*/
    IEC0bits.T3IE = false;
}

bool TMR2_GetElapsedThenClear(void) {
    bool status;

    status = tmr2_obj.timerElapsed;

    if (status == true) {
        tmr2_obj.timerElapsed = false;
    }
    return status;
}

int TMR2_SoftwareCounterGet(void) {
    return tmr2_obj.count;
}

void TMR2_SoftwareCounterClear(void) {
    tmr2_obj.count = 0;
}

/**
 End of File
 */
