/*----------------------------------------------------------------------------
 CONFIGURATION BITS
 -----------------------------------------------------------------------------*/
// FICD
/* ICD Communication Channel Select bits->Communicate on PGEC1 and PGED1*/
#pragma config ICS = PGD1
/* JTAG Enable bit->JTAG is disabled*/
#pragma config JTAGEN = OFF    //

// FPOR
/* Alternate I2C1 pins (I2C1 mapped to SDA1/SCL1 pins)*/
#pragma config ALTI2C1 = OFF
/* Alternate I2C2 pins (I2C2 mapped to SDA2/SCL2 pins) */
#pragma config ALTI2C2 = OFF
/* Watchdog Window Select bits->WDT Window is 25% of WDT period*/
#pragma config WDTWIN = WIN25 

// FWDT
/* Watchdog Timer Postscaler bits->1:32768 */
#pragma config WDTPOST = PS32768
/* Watchdog Timer Prescaler bit->1:128 */
#pragma config WDTPRE = PR128
/* PLL Lock Enable bit->Clock switch to PLL source will wait until 
 * the PLL lock signal is valid.*/
#pragma config PLLKEN = ON
/* Watchdog Timer Window Enable bit->Watchdog Timer in Non-Window mode */
#pragma config WINDIS = OFF
/* Watchdog Timer Enable bit->Watchdog timer enabled/disabled 
 * by user software */
#pragma config FWDTEN = OFF    

// FOSC
/* Primary Oscillator Mode Select bits->HS Crystal Oscillator Mode */
#pragma config POSCMD = HS 
/* OSC2 Pin Function bit->OSC2 is general purpose digital I/O pin */
#pragma config OSCIOFNC = OFF
/* Peripheral pin select configuration (Allow multiple reconfigurations)*/
#pragma config IOL1WAY = OFF
/* Clock Switching Mode bits->Clock switching is enabled,Fail-safe 
 * Clock Monitor is disabled */
#pragma config FCKSM = CSECMD 

// FOSCSEL
/* Oscillator Source Selection->Internal Fast RC (FRC) */
#pragma config FNOSC = FRC
/* PWM Lock Enable bit (PWM registers may be written without key sequence) */
#pragma config PWMLOCK = OFF 
/* //Two-speed Oscillator Start-up Enable bit->Start 
 * Start up with user-selected oscillator source */
#pragma config IESO = OFF    

// FGS
/* General Segment Write-Protect bit->General Segment may be written */
#pragma config GWRP = OFF
/* General Segment Code-Protect bit->General Segment Code protect is Disabled */
#pragma config GCP = OFF

/*----------------------------------------------------------------------------
 HEADER FILES
 -----------------------------------------------------------------------------*/
#include "osc.h"

/*----------------------------------------------------------------------------
 FUNCTIONS
 -----------------------------------------------------------------------------*/
void oscConfig(void) {
    // Configure Oscillator to operate the device at 60Mhz
    // Fosc= Fin*M/(N1*N2), Fcy=Fosc/2 = 40Mhz
    // Fosc= 20M*16/(2*2) = 80Mhz for 4M input clock
    // Configure PLL prescaler, PLL postscaler, PLL divisor
    PLLFBD = 14; // M = 16
    CLKDIVbits.PLLPOST = 0; // N2 = 2
    CLKDIVbits.PLLPRE = 0; // N1 = 2

    // Initiate Clock Switch to Primary Oscillator with PLL (NOSC = 0b011)
    __builtin_write_OSCCONH(0x03);

    __builtin_write_OSCCONL(OSCCON || 0x01);

    // Wait for Clock switch to occur
    while (OSCCONbits.COSC != 0b011);

    // Wait for PLL to lock
    while (OSCCONbits.LOCK != 1);

}
