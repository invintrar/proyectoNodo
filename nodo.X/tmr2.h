#ifndef _TMR2_H
#define _TMR2_H

/**
  Section: Included Files
 */

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

#include "tmr1.h"

#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif

#define TMR2_INTERRUPT_TICKER_FACTOR    1
    /**
     * Extern Variable
     */
    extern uint16_t timeMcs;
    extern uint16_t timeMls;
    extern uint8_t timeSec;
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

        TMR2_Initialize();

        TMR2_Period16BitSet(period);

        if((value = TMR2_Period16BitGet())== period)
        {
            TMR2_Start();
        }

        while(1)
        {
            TMR2_Tasks();
            if( (statusTimer1 = TMR2_GetElapsedThenClear()) == true)
            {
                TMR2_Stop();
            }
        }
        </code>
     */
    void TMR2_Initialize(void);

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

        TMR2_Initialize();

        TMR2_Period32BitSet(period);

        if((value = TMR2_Period32BitGet())== period)
        {
            TMR2_Start();
        }

        while(1)
        {
            TMR2_Tasks();
            if( (statusTimer1 = TMR2_IsElapsed()) == true)
            {
                TMR2_Stop();
            }
        }
        </code>
     */

    void TMR2_Period32BitSet(uint32_t value);

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
        Refer to the example of TMR2_Period32BitSet();
     */

    uint32_t TMR2_Period32BitGet(void);

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

        TMR2_Counter32BitSet(value));

        while(1)
        {
            TMR2_Tasks();
            if( (value == TMR2_Counter32BitGet()))
            {
                TMR2_Stop();
            }
        }
        </code>
     */

    void TMR2_Counter32BitSet(uint32_t value);

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
        Refer to the example of TMR2_Counter32BitSet();
     */

    uint32_t TMR2_Counter32BitGet(void);


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
            TMR2_SetInterruptHandler(&TMR2_CallBack);
        </code>
     */

    void TMR2_SetInterruptHandler(void (* InterruptHandler)(void));

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
        Refer to the example of TMR2_Initialize();
     */

    void TMR2_Start(void);

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
        Refer to the example of TMR2_Initialize();
     */

    void TMR2_Stop(void);

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
        Refer to the example of TMR2_Initialize();
     */

    bool TMR2_GetElapsedThenClear(void);

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
        Refer to the example of TMR2_Initialize();
     */

    int TMR2_SoftwareCounterGet(void);

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
        Refer to the example of TMR2_Initialize();
     */

    void TMR2_SoftwareCounterClear(void);

#ifdef __cplusplus  // Provide C++ Compatibility

}

#endif

#endif //_TMR2_H

/**
 End of File
 */
