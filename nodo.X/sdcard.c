#include "sdcard.h"
/*****************************************************************************/
/* SD Card Functions */

/*****************************************************************************/
uint8_t ccs;

unsigned char SD_Init(void) {
    // Local variables required
    unsigned int i;
    unsigned char temp = 0;
    unsigned long temp_long;

    // Set Chip Select Digital Input
    Release_SD();

    // Initialize SPI interface at slow speed
    SPI1_Init(SLOW);

    // Toggle CLK for 80 cycles with SDO high
    for (i = 0; i < 80; i++)
        SPI1_Exchange_Byte(0xFF);

    // Set Chip Select Digital Output and Set Low
    Select_SD();

    for (i = 0; i < SD_TIME_OUT; i++) {
        SD_Send_Command(GO_IDLE_STATE, 0x00000000, 0x4A); // CMD0
        temp = R1_Response();
        if (temp == (1 << IDLE_STATE)) {
            break;
        }
        if (i == (SD_TIME_OUT - 1)) {
            return CARD_NOT_INSERTED;
        }
    }

    // Send CMD8 Command (2.7-3.6V range = 0x01. Check pattern = 0xAA)
    if (SD_Ready() == 0)
        return SD_NOT_READY;
    SD_Send_Command(SEND_IF_COND, 0x000001AA, 0x43); // CMD8
    temp = R1_Response();
    if (temp != (1 << IDLE_STATE)) {
        // Possible old card. Send CMD1
        for (i = 0; i < SD_TIME_OUT; i++) {
            if (SD_Ready() == 0)
                return SD_NOT_READY;
            SD_Send_Command(SEND_OP_COND, 0x00000000, 0x7C); // CMD1
            temp = R1_Response();
            if (temp == 0x00)
                break;
            if (i == (SD_TIME_OUT - 1))
                return UNUSABLE_CARD;
        }
    } else if (temp == (1 << IDLE_STATE)) {
        temp_long = Response_32b();
        temp = (temp_long & ECHO_BACK_MASK);
        if (temp != 0xAA)
            return ECHO_BACK_ERROR;
        temp = ((temp_long & VOLTAGE_ACCEPTED_MASK) >> 8);
        if (temp != 0x01)
            return INCOMPATIBLE_VOLTAGE;

        // Read OCR
        if (SD_Ready() == 0)
            return SD_NOT_READY;
        SD_Send_Command(READ_OCR, 0x00000000, 0x7E); // CMD58
        temp = R1_Response();
        if (temp != (1 << IDLE_STATE))
            return temp;
        temp_long = Response_32b();
        if ((temp_long & VOLTAGE_RANGE_MASK) != VOLTAGE_RANGE_MASK)
            return INCOMPATIBLE_VOLTAGE;

        // Activate CRC before issuing ACMD41
        if (SD_Ready() == 0)
            return SD_NOT_READY;
        SD_Send_Command(CRC_ON_OFF, 0x00000001, 0x41); // CMD59
        temp = R1_Response();
        if (temp != (1 << IDLE_STATE))
            return temp;

        // Check if card initialization is done
        for (i = 0; i < SD_TIME_OUT; i++) {
            if (SD_Ready() == 0)
                return SD_NOT_READY;
            SD_Send_Command(APP_CMD, 0x00000000, 0x32); // CMD55
            temp = R1_Response();
            if (SD_Ready() == 0)
                return SD_NOT_READY;
            SD_Send_Command(SD_SEND_OP_COND, 0x40000000, 0x3B); // ACMD41
            temp = R1_Response();
            if (temp == 0x00)
                break; // Initialization done
            if (i == (SD_TIME_OUT - 1))
                return UNUSABLE_CARD;
        }
    } else
        return temp; // Some error of the R1 response type

    // Disable CRC verification. CRC7 is ignored in subsequent commands.
    if (SD_Ready() == 0)
        return SD_NOT_READY;

    SD_Send_Command(CRC_ON_OFF, 0x00000000, 0x48); // CMD59
    temp = R1_Response();
    if (temp != 0x00)
        return temp;

    // Set Block to 512 bytes for read/write operations
    if (SD_Ready() == 0)
        return SD_NOT_READY;
    SD_Send_Command(SET_BLOCKLEN, 0x00000200, 0x0A); // CMD16
    temp = R1_Response();
    if (temp != 0x00)
        return temp;

    // Read Card Capacity Status (CCS)
    if (SD_Ready() == 0)
        return SD_NOT_READY;
    SD_Send_Command(READ_OCR, 0x00000000, 0x7E); // CMD58
    temp = R1_Response();
    if (temp != 0x00)
        return temp;
    temp_long = Response_32b();
    ccs = (long) (temp_long >> 30);

    // Configure SPI to maximum speed
    Release_SD();

    SPI1_Init(FAST);

    return SUCCESSFUL_INIT;

}

unsigned char SD_Init_Try(unsigned char try_value) {
    unsigned char i, init_status;
    if (try_value == 0) try_value = 1;
    for (i = 0; i < try_value; i++) {
        init_status = SD_Init();
        if (init_status == SUCCESSFUL_INIT) break;
        Release_SD();

        __delay_ms(10);
    }
    return init_status;
}

unsigned char SD_Read(unsigned char *Buffer, unsigned int nbytes) {
    unsigned int i;
    unsigned char temp;
    for (i = 0; i < SD_TIME_OUT; i++) {
        temp = SPI1_Exchange_Byte(0xFF);
        if (temp == 0xFE)
            break;
        if (i == SD_TIME_OUT - 1)
            return TOKEN_NOT_RECEIVED;
    }
    for (i = 0; i < nbytes; i++) {
        Buffer[i] = SPI1_Exchange_Byte(0xFF);
    }
    temp = SPI1_Exchange_Byte(0xFF); // Read 16bits of CRC
    temp = SPI1_Exchange_Byte(0xFF); //
    return 0x00; // Successful read
}

unsigned char SD_Read_Block(unsigned char *Buffer, unsigned long Address) {
    unsigned char temp;

    Select_SD();

    if (ccs == 0x02) Address <<= 9; // Address * 512 for SDSC cards
    if (SD_Ready() == 0) return SD_NOT_READY;
    SD_Send_Command(READ_SINGLE_BLOCK, Address, 0xFF);
    temp = R1_Response();
    if (temp != 0x00) return temp;
    temp = SD_Read(Buffer, 512);

    Release_SD();

    return temp;
}

unsigned char SD_Write_Block(unsigned char *Buffer, unsigned long Address) {
    unsigned char temp;
    unsigned int i;

    Select_SD();

    if (ccs == 0x02)
        Address <<= 9; // Address * 512 for SDSC cards
    if (SD_Ready() == 0)
        return SD_NOT_READY;
    SD_Send_Command(WRITE_BLOCK, Address, 0xFF);
    temp = R1_Response();
    if (temp != 0x00)
        return temp;
    temp = SPI1_Exchange_Byte(0xFE); // Send Start Block Token;
    for (i = 0; i < 512; i++) {
        temp = SPI1_Exchange_Byte(Buffer[i]);
    }
    temp = SPI1_Exchange_Byte(0xFF); // Send dummy 16bits CRC
    temp = SPI1_Exchange_Byte(0xFF);
    temp = SPI1_Exchange_Byte(0xFF); // Read Response token (xxx0:status(3b):1)
    temp = (temp & 0x0E) >> 1;
    if (SD_Ready() == 0)
        return SD_NOT_READY;

    Release_SD();

    if (temp == 0x02)
        return DATA_ACCEPTED;
    else if (temp == 0x05)
        return DATA_REJECTED_CRC_ERROR;
    else if (temp == 0x06)
        return DATA_REJECTED_WR_ERROR;
    else
        return ERROR;
}

unsigned char R1_Response(void) {
    unsigned char temp;
    temp = SPI1_Exchange_Byte(0xFF);
    temp = SPI1_Exchange_Byte(0xFF);
    return temp;
}

unsigned int R2_Response(void) {
    unsigned char temp;
    unsigned int response;
    temp = SPI1_Exchange_Byte(0xFF);
    response = SPI1_Exchange_Byte(0xFF);
    temp = SPI1_Exchange_Byte(0xFF);
    response = (response << 8) | temp;
    return response;
}

unsigned long Response_32b(void) {
    unsigned char temp;
    unsigned long response;
    response = SPI1_Exchange_Byte(0xFF);
    temp = SPI1_Exchange_Byte(0xFF);
    response = (response << 8) | temp;
    temp = SPI1_Exchange_Byte(0xFF);
    response = (response << 8) | temp;
    temp = SPI1_Exchange_Byte(0xFF);
    response = (response << 8) | temp;
    return response;
}

void SD_Send_Command(unsigned char command, unsigned long argument, unsigned char crc) {
    SPI1_Exchange_Byte(command |= 0x40);
    SPI1_Exchange_Byte((unsigned char) (argument >> 24));
    SPI1_Exchange_Byte((unsigned char) (argument >> 16));
    SPI1_Exchange_Byte((unsigned char) (argument >> 8));
    SPI1_Exchange_Byte((unsigned char) (argument));
    SPI1_Exchange_Byte((crc << 1) | 0x01);
}

unsigned char SD_Ready(void) {
    unsigned int i;
    unsigned char temp;
    for (i = 0; i < SD_TIME_OUT; i++) {
        temp = SPI1_Exchange_Byte(0xFF);
        if (temp == 0xFF)
            break;
        if (i == (SD_TIME_OUT - 1))
            return 0x00;
    }
    return temp;
}

unsigned char SD_Detect(void) {
    Select_SD(); // CS pin output low
    __delay_us(100); // time to discharge parasitic capacitance;
    Release_SD(); // CS pin in high-impedance
    __delay_us(100); // time to charge parasitic capacitance through 50k SD res

    if (SD_CS_Port & (1 << SD_CS_Bit))
        return DETECTED;
    else
        return 0;
}

void Select_SD(void) {
    SD_CS_Tris &= ~(1 << SD_CS_Bit);
    SD_CS_Lat &= ~(1 << SD_CS_Bit); // CS = 0
    __builtin_nop();
}

void Release_SD(void) {
    // Configure Chip Select pin as input    // CS = 1 with SD pull-up
    SD_CS_Tris |= (1 << SD_CS_Bit);
    __builtin_nop();
}

void SD_Led_On(void) {
    SD_Led_Lat |= (1 << SD_Led_Bit);
}

void SD_Led_Off(void) {
    SD_Led_Lat &= ~(1 << SD_Led_Bit);
}
/*
 * End File
 */