/*----------------------------------------------------------------------------
 HEADER FILES
 -----------------------------------------------------------------------------*/
#include "spi2.h"

/*----------------------------------------------------------------------------
 FUNCTION PROTOTYPES
 -----------------------------------------------------------------------------*/

void SPI2_Exchange(uint8_t *pTransmitData, uint8_t *pReceiveData);

/*----------------------------------------------------------------------------
 FUNCTIONS
 -----------------------------------------------------------------------------*/

void SPI2_Init(void) {
    SPI2STATbits.SPIEN = 0;

    /*40MHz /(4*4) = 2.5MHz*/
    SPI2CON1 = 0x232;

    SPI2CON2 = 0x0000;
    
    SPI2STAT = 0x0000;

    SPI2STATbits.SPIEN = 1;
}

void SPI2_Exchange(uint8_t *pTransmitData, uint8_t *pReceiveData) {
    while (SPI2STATbits.SPITBF == true) {
    }

    SPI2BUF = *((uint8_t*) pTransmitData);
    
    while (SPI2STATbits.SPIRBF == false);

    *((uint8_t*) pReceiveData) = SPI2BUF;

}

uint8_t SPI2_Exchange_Byte(uint8_t data) {
    uint8_t receiveData;

    SPI2_Exchange(&data, &receiveData);

    return (receiveData);
}
/*
 * En File
 */