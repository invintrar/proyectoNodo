#ifndef _INTERRUPT_MANAGER_H
#define _INTERRUPT_MANAGER_H

void INTERRUPT_Initialize(void);

inline static void INTERRUPT_GlobalEnable(void)
{
    INTCON2bits.GIE = 1;
}

inline static void INTERRUPT_GlobalDisable(void)
{
    INTCON2bits.GIE = 0;
}


#endif