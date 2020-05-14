#ifndef SD_H
#define SD_H

#include "main.h"
#include "spi1.h"

/*****************************************************************************/
/* Structures definition*/

/*----------------------------------------------------------------------------
 Variables Extern
 -----------------------------------------------------------------------------*/
extern uint8_t ccs;
extern uint8_t buSDState;

/*---------------------------------------------------------------------------
 SD Card Chip Select I/O definition
 ----------------------------------------------------------------------------*/
#define SD_CS_Lat  LATB
#define SD_CS_Tris TRISB
#define SD_CS_Port PORTB
#define SD_CS_Bit  0
// Indicator led port definitions
#define SD_Led_Tris TRISA
#define SD_Led_Lat  LATA
#define SD_Led_Bit  1

/*---------------------------------------------------------------------------
 SD Card Commands
 ----------------------------------------------------------------------------*/
#define GO_IDLE_STATE       0x00        // CMD0
#define SEND_OP_COND        0x01        // CMD1
#define SEND_IF_COND        0x08        // CMD8
#define SEND_CSD            0x09        // CMD9
#define SEND_CID            0x0A        // CMD10
#define SEND_STATUS         0x0D        // CMD13
#define SET_BLOCKLEN        0x10        // CMD16
#define READ_SINGLE_BLOCK   0x11        // CMD17
#define WRITE_BLOCK         0x18        // CMD24
#define APP_CMD             0x37        // CMD55
#define READ_OCR            0x3A        // CMD58
#define CRC_ON_OFF          0x3B        // CMD59
#define SD_STATUS           0x0D        // ACMD13
#define SD_SEND_OP_COND     0x29        // ACMD41
#define SET_CLR_CARD_DETECT 0x2A        // ACMD42
#define SEND_SCR            0x33        // ACMD51

/*----------------------------------------------------------------------------
 SD Card Command Responses
 -----------------------------------------------------------------------------*/
// R1 response (8bits)
#define IDLE_STATE              0
#define ERASE_RESET             1
#define ILLEGAL_COMMAND         2
#define COM_CRC_ERROR           3
#define ERASE_SEQUENCE_ERROR    4
#define ADDRESS_ERROR           5
#define PARAMETER_ERROR         6
// First 8bits of R2 response 
#define CARD_IS_LOCKED          8
#define WP_ERASE_SKIP           9
#define ERROR                   10
#define CC_ERROR                11
#define CARD_ECC_FAILED         12
#define WP_VIOLATION            13
#define ERASE_PARAM             14
#define OUT_OF_RANGE            15
/* SD Card Initialization status (for this implementation) */
#define CARD_NOT_INSERTED       16
#define SD_NOT_READY            17
#define UNUSABLE_CARD           18
#define ECHO_BACK_ERROR         19
#define INCOMPATIBLE_VOLTAGE    20
#define TOKEN_NOT_RECEIVED      21
#define DATA_ACCEPTED           22
#define DATA_REJECTED_CRC_ERROR 23
#define DATA_REJECTED_WR_ERROR  24
#define SUCCESSFUL_INIT         0xAA

// OCR (first 32bits of R3 response)
#define VOLTAGE_RANGE_MASK      0x00FF8000
#define POWER_UP_STATUS         31
#define CAPACITY_STATUS         30

// First 32bits of R7 response 
#define ECHO_BACK_MASK          0x000000FF
#define VOLTAGE_ACCEPTED_MASK   0x00000F00

#define SD_TIME_OUT             2000

#define DETECTED                0xDE

/*----------------------------------------------------------------------------
    SD Card Function Prototype
 -----------------------------------------------------------------------------*/

unsigned char SD_Init(void);
unsigned char SD_Init_Try(unsigned char);
unsigned char SD_Write_Block(unsigned char*, unsigned long);
unsigned char SD_Read_Block(unsigned char*, unsigned long);
unsigned char SD_Read(unsigned char*, unsigned int);
void SD_Send_Command(unsigned char, unsigned long, unsigned char);
unsigned char R1_Response(void);
unsigned int R2_Response(void);
unsigned long Response_32b(void);
unsigned char SD_Ready(void);
void Select_SD(void);
void Release_SD(void);
unsigned char Detect_SD(void);
void SD_Led_On(void);
void SD_Led_Off(void);
unsigned char SD_Detect(void);
void SD_Check(void);

#endif      // SD_H