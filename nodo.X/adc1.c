/**
  Section: Included Files
 */

#include "adc1.h"

/**
  Section: Data Type Definitions
 */

/* ADC Driver Hardware Instance Object

  @Summary
    Defines the object required for the maintenance of the hardware instance.

  @Description
    This defines the object required for the maintenance of the hardware
    instance. This object exists once per hardware instance of the peripheral.

 */


typedef struct {
    uint8_t intSample;
}

ADC_OBJECT;

static ADC_OBJECT adc1_obj;

/**
  Section: Driver Interface
 */


void ADC1_Initialize(void) {
    // ASAM disabled; ADDMABM disabled; ADSIDL disabled; DONE disabled; SIMSAM Sequential; FORM Absolute decimal result, unsigned, right-justified; SAMP disabled; SSRC Clearing sample bit ends sampling and starts conversion; AD12B 12-bit; ADON enabled; SSRCG disabled; 

    AD1CON1 = 0x8400;

    // CSCNA disabled; VCFG0 AVDD; VCFG1 AVSS; ALTS disabled; BUFM disabled; SMPI Generates interrupt after completion of every sample/conversion operation; CHPS 1 Channel; 

    AD1CON2 = 0x00;

    // SAMC 0; ADRC FOSC/2; ADCS 4; 

    AD1CON3 = 0x04;

    // CH0SA AN0; CH0SB AN0; CH0NB AVSS; CH0NA AVSS; 

    AD1CHS0 = 0x00;

    // CSS25 disabled; CSS24 disabled; CSS31 disabled; CSS30 disabled; 

    AD1CSSH = 0x00;

    // CSS2 disabled; CSS1 disabled; CSS0 disabled; CSS5 disabled; CSS4 disabled; CSS3 disabled; 

    AD1CSSL = 0x00;

    // DMABL Allocates 1 word of buffer to each analog input; ADDMAEN disabled; 

    AD1CON4 = 0x00;

    // CH123SA disabled; CH123SB CH1=OA2/AN0,CH2=AN1,CH3=AN2; CH123NA disabled; CH123NB CH1=VREF-,CH2=VREF-,CH3=VREF-; 

    AD1CHS123 = 0x00;


    adc1_obj.intSample = AD1CON2bits.SMPI;

    // Enabling ADC1 interrupt.
    IEC0bits.AD1IE = 1;

}

void __attribute__((weak)) ADC1_CallBack(void) {
    vAdc = ADC1BUF0;
}

void __attribute__((__interrupt__, auto_psv)) _AD1Interrupt(void) {
    // ADC1 callback function 
    ADC1_CallBack();

    // clear the ADC interrupt flag
    IFS0bits.AD1IF = false;
}


/**
  End of File
 */
