/*----------------------------------------------------------------------------
 HEADER FILES
 -----------------------------------------------------------------------------*/
#include "adxl355.h"

/*----------------------------------------------------------------------------
 VARIABLE DECLARATIONS
 -----------------------------------------------------------------------------*/
int dataBuffer[];

/*----------------------------------------------------------------------------
 FUNCTION PROTOTYPES
 -----------------------------------------------------------------------------*/

void ADXL355_Init() {
    //Reset the device
    ADXL355_Write_Byte(Reset, 0x52);
    __delay_ms(25);

    ADXL355_Write_Byte(POWER_CTL, TEMP_OFF | STANDBY);
    ADXL355_Write_Byte(Range, _2G | INT_ACTIVE_HIGH);

    ADXL355_Write_Byte(Sync, INT_SYNC); //internal clock
    ADXL355_Write_Byte(Filter, _62_5_Hz); //250Hz and filter Hight

    ADXL355_Write_Byte(INT_MAP, RDY_EN1);
    //ADXL355_Write_Byte(FIFO_ENTRIES, STORED_FIFO);
    //ADXL355_Write_Byte(FIFO_SAMPLES, WATERMARK);
    __delay_ms(25);
}

void ADXL355_Write_Byte(char address, char data) {
    address = (address << 1)& 0xFE;

    ADXL355_CS_SetLow();
    SPI1_Exchange_Byte(address);
    SPI1_Exchange_Byte(data);
    ADXL355_CS_SetHigh();

}

void ADXL355_Power_On() {
    ADXL355_Write_Byte(POWER_CTL, MEASURING);
}

void ADXL355_Power_Off() {
    ADXL355_Write_Byte(POWER_CTL, STANDBY);
}

unsigned char ADXL355_Read_Byte(unsigned char address) {
    unsigned char temp = 0x00;

    address = (address << 1) | 0x01;

    ADXL355_CS_SetLow();
    temp = SPI1_Exchange_Byte(address);
    temp = SPI1_Exchange_Byte(0x00);
    ADXL355_CS_SetHigh();

    return (temp);
}

void ADXL355_Read_FIFO(uint8_t data[]) {
    // FIFO stores 3 bytes of data for each axis, the 20 most significant bits
    // are the values in two's complement
    char dataByte [9] = {0}; //
    int i;

    ADXL355_CS_SetLow();
    SPI1_Exchange_Byte((FIFO_DATA << 1) | 1);
    for (i = 0; i < 9; i++) {
        dataByte[i] = SPI1_Exchange_Byte(0x00);
    }
    ADXL355_CS_SetHigh();
    for (i = 0; i < 9; i++) {
        data[i] = dataByte[i];
    }
    __delay_us(5);
}

void ADXL355_Read_FIFO_Full(uint8_t getData[]) {
    uint8_t i;
    ADXL355_CS_SetLow();
    SPI1_Exchange_Byte((FIFO_DATA << 1) | 1);
    for (i = 0; i < 63; i++) {
        getData[i] = SPI1_Exchange_Byte(0x00);
    }
    ADXL355_CS_SetHigh();
    __delay_us(5);

}

uint8_t ADXL355_Status() {
    uint8_t value = 0;
    ADXL355_CS_SetLow();
    SPI1_Exchange_Byte((Status << 1) | 1);
    value = SPI1_Exchange_Byte(0x00);
    ADXL355_CS_SetHigh();


    return value;
}