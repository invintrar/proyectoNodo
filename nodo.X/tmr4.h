#ifndef _TMR4_H
#define _TMR4_H

/**
  Section: Included Files
 */

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

#include "tmr2.h"

#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif

#define TMR4_INTERRUPT_TICKER_FACTOR    1
    /**
     * Extern Variable
     */
    extern uint8_t timeMin;
    extern uint8_t timeSec;
    extern uint8_t timeHor;
    /**
      Section: Interface Routines
     */

    /**
      @Summary
        Initializes hardware and data for the given instance of the TMR module

      @Description
        This routine initializes hardware for the instance of the TMR module,
        using the hardware initialization given data.  It also initializes all
        necessary internal data.

      @Param
        None.

      @Returns
        None
 
      @Example 
        <code>
        bool statusTimer1;
        uint16_t period;
        uint16_t value;

        period = 0x20;

        TMR4_Initialize();

        TMR4_Period16BitSet(period);

        if((value = TMR4_Period16BitGet())== period)
        {
            TMR4_Start();
        }

        while(1)
        {
            TMR4_Tasks();
            if( (statusTimer1 = TMR4_GetElapsedThenClear()) == true)
            {
                TMR4_Stop();
            }
        }
        </code>
     */
    void TMR4_Initialize(void);

    /**
      @Summary
        Updates 32-bit timer value

      @Description
        This routine updates 32-bit timer value

      @Param
        value       - 32-bit period value

      @Returns
        None

      @Example 
        <code>
        bool statusTimer1;
        uint32_t period;
        uint32_t value;

        period = 0x20202020;

        TMR4_Initialize();

        TMR4_Period32BitSet(period);

        if((value = TMR4_Period32BitGet())== period)
        {
            TMR4_Start();
        }

        while(1)
        {
            TMR4_Tasks();
            if( (statusTimer1 = TMR4_IsElapsed()) == true)
            {
                TMR4_Stop();
            }
        }
        </code>
     */

    void TMR4_Period32BitSet(uint32_t value);

    /**
      @Summary
        Provides the timer 32-bit period value

      @Description
        This routine provides the timer 32-bit period value

      @Param
        None

      @Returns
        Timer 32-bit period value
 
      @Example 
        Refer to the example of TMR4_Period32BitSet();
     */

    uint32_t TMR4_Period32BitGet(void);

    /**
      @Summary
        Updates the timer's 32-bit value

      @Description
        This routine updates the timer's 32-bit value

      @Param
        value       - 32-bit Counter value

      @Returns
        None
	
      @Example 
        <code>
        uint32_t value=0xF0F0F0;

        TMR4_Counter32BitSet(value));

        while(1)
        {
            TMR4_Tasks();
            if( (value == TMR4_Counter32BitGet()))
            {
                TMR4_Stop();
            }
        }
        </code>
     */

    void TMR4_Counter32BitSet(uint32_t value);

    /**
      @Summary
        Provides 32-bit  current counter value

      @Description
        This routine provides 32-bit current counter value

      @Param
        None

      @Returns
        32-bit current counter value
 
      @Example 
        Refer to the example of TMR4_Counter32BitSet();
     */

    uint32_t TMR4_Counter32BitGet(void);


    /**
      @Summary
        Assigns a function pointer with a callback address.

      @Description
        This routine assigns a function pointer with a callback address.

      @Param
        Address of the callback routine.

      @Returns
        None
 
      @Example 
        <code>
            TMR4_SetInterruptHandler(&TMR4_CallBack);
        </code>
     */

    void TMR4_SetInterruptHandler(void (* InterruptHandler)(void));

    /**
      @Summary
        Starts the TMR

      @Description
        This routine starts the TMR

      @Param
        None.

      @Returns
        None
 
      @Example 
        Refer to the example of TMR4_Initialize();
     */

    void TMR4_Start(void);

    /**
      @Summary
        Stops the TMR

      @Description
        This routine stops the TMR

      @Param
        None.

      @Returns
        None
 
      @Example 
        Refer to the example of TMR4_Initialize();
     */

    void TMR4_Stop(void);

    /**
      @Summary
        Returns the elapsed status of the timer and clears if flag is set.

      @Description
        This routine returns the elapsed status of the timer and clears 
        flag if its set.

      @Param
        None.

      @Returns
        True - Timer has elapsed.
        False - Timer has not elapsed.
 
      @Example 
        Refer to the example of TMR4_Initialize();
     */

    bool TMR4_GetElapsedThenClear(void);

    /**
      @Summary
        Returns the software counter value.

      @Description
        This routine returns the software counter value.

      @Param
        None.

      @Returns
        Software counter value.
 
      @Example 
        Refer to the example of TMR4_Initialize();
     */

    int TMR4_SoftwareCounterGet(void);

    /**
      @Summary
        Clears the software counter value.

      @Description
        This routine clears the software counter value.

      @Param
        None.

      @Returns
        None
 
      @Example 
        Refer to the example of TMR4_Initialize();
     */

    void TMR4_SoftwareCounterClear(void);

#ifdef __cplusplus  // Provide C++ Compatibility

}

#endif

#endif //_TMR4_H

/**
 End of File
 */
