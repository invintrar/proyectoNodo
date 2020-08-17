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
 * Interrupt for DS3234 each second
 */
void __attribute__((weak)) EX_INT1_CallBack(void) {
    //Interrupt DS3234 each second
    TMR1_Counter16BitSet(0);
    TMR2_Counter32BitSet(0);
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
    EX_INT1_NegativeEdgeSet();
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
