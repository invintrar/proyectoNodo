#include "rf24l01.h"

void RF24L01_powerDown(void);

/* Init Pines */
void RF24L01_Init(void) {
    //Chip Select
    RF24L01_CS_SetDigitalOutput();
    RF24L01_CS_SetHigh();

    //CHIP ENABLE PARA TRANSMISION Y RECEPCION
    RF24L01_CE_SetDigitalOutput();
    RF24L01_CE_SetLow();
    __delay_ms(100);
}

/**
 * Function Send Command
 * @param uint8_t command
 */
void RF24L01_send_command(uint8_t command) {
    //Chip selec set Low
    RF24L01_CS_SetLow();
    //Send command
    SPI1_Exchange_Byte(command);
    //Chip select set High
    RF24L01_CS_SetHigh();
}

/**
 * Function Read Register
 * @param uint8_t Register Address
 * @return uint8_t Data of Register
 */
uint8_t RF24L01_read_register(uint8_t register_addr) {
    uint8_t result;
    //Chip select -> Low
    RF24L01_CS_SetLow();

    //Send address and read command
    SPI1_Exchange_Byte(RF24L01_command_R_REGISTER | register_addr);

    //Get data
    result = SPI1_Exchange_Byte(0x00);

    //Chip select -> High
    RF24L01_CS_SetHigh();

    return result;
}

/**
 * Function Write Register
 * @param Register Address
 * @param Data
 * @param Length
 */
void RF24L01_write_register(uint8_t register_addr, uint8_t *value, uint8_t length) {
    uint8_t i;

    //Chip select -> Low
    RF24L01_CS_SetLow();
    //Send address and write command
    SPI1_Exchange_Byte(RF24L01_command_W_REGISTER | register_addr);
    //Send data  
    for (i = 0; i < length; i++) {
        SPI1_Exchange_Byte(value[i]);
    }
    //Chip select -> High
    RF24L01_CS_SetHigh();
    
}

/**
 * Function Setup
 * @param TX Address
 * @param RX Address
 * @param Channel
 */
void RF24L01_setup(uint8_t *tx_addr, uint8_t *rx_addr, uint8_t channel) {
    RF24L01_CE_SetLow();

    //Enable Auto Acknowledgment (0x01)
    RF24L01_reg_EN_AA_content EN_AA;
    *((uint8_t *) & EN_AA) = 0;
    // Enable Auto Ack Pipe0
    EN_AA.ENAA_P0 = 1;
    RF24L01_write_register(RF24L01_reg_EN_AA, ((uint8_t *) & EN_AA), 1);

    // Enable RX Address(0x02) 
    RF24L01_reg_EN_RXADDR_content RX_ADDR;
    *((uint8_t *) & RX_ADDR) = 0;
    //Enable data pipe0
    RX_ADDR.ERX_P0 = 1;
    RF24L01_write_register(RF24L01_reg_EN_RXADDR, ((uint8_t *) & RX_ADDR), 1);

    // Setup of Address Widths(0x03)
    RF24L01_reg_SETUP_AW_content SETUP_AW;
    *((uint8_t *) & SETUP_AW) = 0;
    //RX/TX Address field width ('11' -> 5 bytes)
    SETUP_AW.AW = 0x03;
    RF24L01_write_register(RF24L01_reg_SETUP_AW, ((uint8_t *) & SETUP_AW), 1);

    // Setup of Automatic Retransmission(0x04)
    RF24L01_reg_SETUP_RETR_content SETUP_RETR;
    *((uint8_t *) & SETUP_RETR) = 0;
    //Up to 10 Re-Transmit on fail of AA
    SETUP_RETR.ARC = 0X02;
    //Auto Retransmit Delay(wait 500uS) 
    SETUP_RETR.ARD = 0X01;
    RF24L01_write_register(RF24L01_reg_SETUP_RETR, ((uint8_t *) & SETUP_RETR), 1);

    // RF Channel(0x05)
    RF24L01_reg_RF_CH_content RF_CH;
    *((uint8_t *) & RF_CH) = 0;
    //Sets the frequency channel nRF24L01 operates on
    RF_CH.RF_CH = channel;
    RF24L01_write_register(RF24L01_reg_RF_CH, ((uint8_t *) & RF_CH), 1);

    //RF Setup Register(0x06)
    RF24L01_reg_RF_SETUP_content RF_SETUP;
    *((uint8_t *) & RF_SETUP) = 0;
    //Set RF output power in TX mode(0dBm)
    RF_SETUP.RF_PWR = 0x03;
    //Speed date rate 2Mbps
    RF_SETUP.RF_DR_HIGH = 1;
    RF24L01_write_register(RF24L01_reg_RF_SETUP, ((uint8_t *) & RF_SETUP), 1);

    //Write the direction RX in pipe0(0x0A)
    RF24L01_write_register(RF24L01_reg_RX_ADDR_P0, rx_addr, 5);

    //Write the direction TX(0x10)
    RF24L01_write_register(RF24L01_reg_TX_ADDR, tx_addr, 5);

    //RX payload in data pipe0(0x11)
    RF24L01_reg_RX_PW_P0_content RX_PW_P0;
    *((uint8_t *) & RX_PW_P0) = 0;
    //Number of bytes in RX payload in data Pipe0 (14 bytes)
    RX_PW_P0.RX_PW_P0 = 0x0E;
    RF24L01_write_register(RF24L01_reg_RX_PW_P0, ((uint8_t *) & RX_PW_P0), 1);
    
    RF24L01_reg_STATUS_content status;  
    *((uint8_t *) & status) = 0;
    status.RX_DR = 1;
    status.TX_DS = 1;
    status.MAX_RT = 1;
    RF24L01_write_register(RF24L01_reg_STATUS, (uint8_t*) & status, 1);
    
    RF24L01_send_command(RF24L01_command_FLUSH_RX);
    RF24L01_send_command(RF24L01_command_FLUSH_TX);

    // Configuration Register (0x00)
    RF24L01_reg_CONFIG_content config;
    *((uint8_t *) & config) = 0;
    //Power Up
    config.PWR_UP = 1;
    // RX/TX 1:PRX, 0:PTX
    config.PRIM_RX = 1;
    //CRC(Cyclic Redundancy Check) encoding scheme '0'-1 byte or '1'-2 bytes
    config.CRCO = 1;
    //Enable CRC
    config.EN_CRC = 1;
    RF24L01_write_register(RF24L01_reg_CONFIG, ((uint8_t *) & config), 1);

    __delay_ms(5);
    
    RF24L01_CE_SetHigh();
    

}// End Setup

/**
 * Function Send Data
 */
void RF24L01_sendData(uint8_t *data, uint8_t size) {
    uint8_t i;
    //CE -> Low
    RF24L01_CE_SetLow();
  
    //Chip select -> Low 
    RF24L01_CS_SetLow();
    //Send address and command
    SPI1_Exchange_Byte(RF24L01_command_W_TX_PAYLOAD);
    //Send data
    for (i = 0; i < size; i++) {
        SPI1_Exchange_Byte(data[i]);
    }
    //Chip select -> High
    RF24L01_CS_SetHigh();

    //Active mode TX
    RF24L01_reg_CONFIG_content config;
    *((uint8_t *) & config) = 0;
    // RX/TX Control (1:PRX, 0:PTX)
    config.PRIM_RX = 0;
    //Power Up
    config.PWR_UP = 1;
    //CRC Encoding 2 byte
    config.CRCO = 1;
    //Enable CRC
    config.EN_CRC = 1;
    RF24L01_write_register(RF24L01_reg_CONFIG, ((uint8_t *) & config), 1);

    //Generates an impulsion for CE to send the data
    RF24L01_CE_SetHigh(); //CE -> High
    //Delay 10uS
    __delay_us(12);
    RF24L01_CE_SetLow(); //CE -> Low

}

/**
 * Function Set Mode RX
 */
void RF24L01_set_mode_RX(void) {
    RF24L01_CE_SetLow();
    
    RF24L01_reg_CONFIG_content config;
    *((uint8_t *) & config) = 0;
    // RX/TX Control (1:PRX, 0:PTX)
    config.PRIM_RX = 1;
    // 1:POWER UP, 0:POWER DOWN
    config.PWR_UP = 1;
    // Encoding (0:1byte, 1:2bytes)
    config.CRCO = 1;
    // Enable CRC
    config.EN_CRC = 1;
    RF24L01_write_register(RF24L01_reg_CONFIG, ((uint8_t *) & config), 1);

    //CE -> High
    RF24L01_CE_SetHigh();
    
    __delay_us(130);
    
}

/**
 * Function Get Status
 * @return  Structure Status
 */
RF24L01_reg_STATUS_content RF24L01_get_status(void) {
    uint8_t status;
        
    status = RF24L01_read_register(RF24L01_reg_STATUS);

    return *((RF24L01_reg_STATUS_content *) & status);
}


/**
 * Function Read Payload
 * @param Data
 * @param Length
 */
void RF24L01_read_payload(uint8_t *data, uint8_t size) {
    uint8_t i;

    //Chip select -> Low
    RF24L01_CS_SetLow();
    //Send address
    SPI1_Exchange_Byte(RF24L01_command_R_RX_PAYLOAD);
    //Get data
    for (i = 0; i < size; i++) {
        *(data++) = SPI1_Exchange_Byte(0x00);
    }
    //Chip select -> High
    RF24L01_CS_SetHigh();

    RF24L01_send_command(RF24L01_command_FLUSH_RX);

}


/**
 * Function STATUS
 * @return 1:DataSent, 2:RX_DR, 3:Max_RT 
 */
uint8_t RF24L01_status(void) {
    uint8_t res = 0;
    RF24L01_reg_STATUS_content status;
    
    status = RF24L01_get_status();

    if (status.RX_DR) {
        res = 1;
    } else if (status.TX_DS) {
        res = 2;
    } else if (status.MAX_RT) {
        res = 3;
    }

    return res;
}

/**
 * Function Clear Interrupts
 */
void RF24L01_clear_interrupts(void) {
    RF24L01_reg_STATUS_content status;  
    
    status = RF24L01_get_status();
    
    if(status.TX_DS){
        RF24L01_send_command(RF24L01_command_FLUSH_TX);
    }else if(status.MAX_RT){
        RF24L01_send_command(RF24L01_command_FLUSH_TX);
    }
    
    *((uint8_t *) & status) = 0;
    status.RX_DR = 1;
    status.TX_DS = 1;
    status.MAX_RT = 1;
    RF24L01_write_register(RF24L01_reg_STATUS, (uint8_t*) & status, 1);
}

void RF24L01_powerDown(void){
    RF24L01_CE_SetLow();
    __delay_us(200);
    
    // Configuration Register (0x00)
    RF24L01_reg_CONFIG_content config;
    *((uint8_t *) & config) = 0;
    //1: PowerUp, 0:PowerDown
    config.PWR_UP = 0;
    //CRC(Cyclic Redundancy Check) encoding scheme '0'-1 byte or '1'-2 bytes
    config.CRCO = 1;
    //Enable CRC
    config.EN_CRC = 1;
    RF24L01_write_register(RF24L01_reg_CONFIG, ((uint8_t *) & config), 1);
    
    __delay_us(200);
}