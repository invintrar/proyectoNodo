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
    //Period = 0.000001 s; Frequency = 40000000 Hz; PR1 39; 
    PR1 = 0x27;
    //TCKPS 1:1; TON enabled; TSIDL disabled; TCS FOSC/2; TSYNC disabled; TGATE disabled; 
    T1CON = 0x8000;

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
    //Espera 1uS
    timeMcs++;
    if (timeMcs == 1000) {
        timeMls++;
        timeMcs = 0;
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
