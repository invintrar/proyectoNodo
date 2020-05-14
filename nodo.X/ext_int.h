#ifndef _EXT_INT_H
#define _EXT_INT_H

/**
    Section: Includes
*/
#include "adxl355.h"
#include <p33EP256MC202.h>


// Provide C++ Compatibility
#ifdef __cplusplus  

extern "C" {

#endif

/**
    Section: Macros
*/

#define EX_INT0_InterruptFlagClear()       (IFS0bits.INT0IF = 0)

#define EX_INT0_InterruptDisable()     (IEC0bits.INT0IE = 0)

#define EX_INT0_InterruptEnable()       (IEC0bits.INT0IE = 1)

#define EX_INT0_NegativeEdgeSet()          (INTCON2bits.INT0EP = 1)

#define EX_INT0_PositiveEdgeSet()          (INTCON2bits.INT0EP = 0)


void EX_INT0_CallBack(void);

#define EX_INT1_InterruptFlagClear()       (IFS1bits.INT1IF = 0)

#define EX_INT1_InterruptDisable()     (IEC1bits.INT1IE = 0)

#define EX_INT1_InterruptEnable()       (IEC1bits.INT1IE = 1)

#define EX_INT1_NegativeEdgeSet()          (INTCON2bits.INT1EP = 1)

#define EX_INT1_PositiveEdgeSet()          (INTCON2bits.INT1EP = 0)

void EX_INT1_CallBack(void);

#define EX_INT2_InterruptFlagClear()       (IFS1bits.INT2IF = 0)

#define EX_INT2_InterruptDisable()     (IEC1bits.INT2IE = 0)

#define EX_INT2_InterruptEnable()       (IEC1bits.INT2IE = 1)

#define EX_INT2_NegativeEdgeSet()          (INTCON2bits.INT2EP = 1)

#define EX_INT2_PositiveEdgeSet()          (INTCON2bits.INT2EP = 0)



void EX_INT2_CallBack(void);

/**
  @Summary
    Initializes the external interrupt pins

  @Description
    This routine initializes the EXT_INT driver to detect the configured edge, clear
    the interrupt flag and enable the interrupt.

    The following external interrupts will be initialized:
    INT0 - EX_INT0
    INT1 - EX_INT1
    INT2 - EX_INT2

*/

extern uint8_t bNrf;
extern uint8_t bInt1;
extern uint8_t nrfDataRx[14];

void EXT_INT_Initialize(void);
// Provide C++ Compatibility
#ifdef __cplusplus  

}

#endif
#endif
