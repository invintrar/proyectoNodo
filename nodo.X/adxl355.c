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
    __delay_ms(10);
    
    ADXL355_Write_Byte(POWER_CTL, TEMP_OFF | STANDBY);
    ADXL355_Write_Byte(Range, _2G | INT_ACTIVE_HIGH);

    ADXL355_Write_Byte(Sync, INT_SYNC); //internal clock
    ADXL355_Write_Byte(Filter, _62_5_Hz); //250Hz

    ADXL355_Write_Byte(INT_MAP, FULL_EN1);
    ADXL355_Write_Byte(FIFO_ENTRIES, STORED_FIFO);
    ADXL355_Write_Byte(FIFO_SAMPLES, WATERMARK);

    __delay_ms(100);
}

void ADXL355_Write_Byte(char address, char data) {
    address = (address << 1)& 0xFE;

    ADXL355_CS_SetLow();
    SPI1_Exchange_Byte(address);
    SPI1_Exchange_Byte(data);
    ADXL355_CS_SetHigh();

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

void ADXL355_Read_FIFO() {
    // FIFO stores 3 bytes of data for each axis, the 20 most significant bits
    // are the values in two's complement
    char dataByte [9]; //
    int i, z, w;
    char cbandera, cempty;

    ADXL355_CS_SetLow();
    SPI1_Exchange_Byte((FIFO_DATA << 1) | 1);
    for (i = 0; i < 9; i++) {
        dataByte[i] = SPI1_Exchange_Byte(0x00);
    }
    ADXL355_CS_SetHigh();
    __delay_us(5);

    // The 16 most significant bits are kept in an integer 
    for (z = 0; z < 3; z++) {

        (z == 0) ? (w = 2) : (w += 3);

        cbandera = dataByte[w];
        cempty = cbandera & 0x02;

        if (cempty == 0) {//El buffer está lleno
            dataBuffer[z] = ((dataByte[z * 3] << 8) | (dataByte[z * 3 + 1]));
        }else{//El buffer está vacio
            dataBuffer[z] = 0x00;
        }

    }
}

void ADXL355_Read_FIFO_Full() {
    int i;
    ADXL355_CS_SetLow();
    SPI1_Exchange_Byte((FIFO_DATA << 1) | 1);
    for (i = 0; i < 63; i++) {
        dataAdxl[i] = SPI1_Exchange_Byte(0x00);
    }
    ADXL355_CS_SetHigh();
    __delay_us(5);

}