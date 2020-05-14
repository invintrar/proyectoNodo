/**
    Section: Includes
 */
#include <xc.h>

/**
    void INTERRUPT_Initialize (void)
 */
void INTERRUPT_Initialize(void) {
    //    INT1I: External Interrupt 1
    //    Priority: 3
    IPC5bits.INT1IP = 4;
    //    INT2I: External Interrupt 2
    //    Priority: 3
    IPC7bits.INT2IP = 3;
    //    ADI: ADC1 Convert Done
    //    Priority: 2
    IPC3bits.AD1IP = 2;
    //    TI: Timer 4
    //    Priority: 1
    IPC6bits.T4IP = 1;
    //    TNI: Timer 5
    //    Priority: 1
    IPC7bits.T5IP = 1;
    //    TI: Timer 2
    //    Priority: 1
    IPC1bits.T2IP = 1;
    //    TNI: Timer 3
    //    Priority: 1
    IPC2bits.T3IP = 1;
    //    TI: Timer 1
    //    Priority: 1
    IPC0bits.T1IP = 1;


}
