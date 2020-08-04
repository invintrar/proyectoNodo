/**
 * @author DzhL
 */
#ifndef STATEMENT
#define	STATEMENT

#include "ds3234.h"


#define TIMES           10
#define TO_NSEC(t)      ((t[0] * 1000000000L) + t[1])
//#define CLOCKS_PER_SEC  1
#define SECS_IN_DAY     (24 * 60 * 60)
// Define Channel 
#define CHANNEL         22
// Define size data NRF24L01+ (1...32Bytes)
#define SIZEDATA        12
/*----------------------------------------------------------------------------
 VARIABLE GLOBALES
 -----------------------------------------------------------------------------*/
// Variable that use for save in uSD
uint8_t dataSentuSD[512] = {0};
// Variable for get data from the ADXL355Z
uint8_t dataAdxl[9] = {0};
// Identifier Node use for notify master
uint8_t idNodo = 2;
// Address for NRF24L01+ RX and TX change for different node
uint8_t rx_addr[5] = {0xA2, 0xA2, 0xA2, 0xA2, 0xA2}; // Change address for different node1:0xA1...0xA9
uint8_t tx_addr[5] = {0xA2, 0xA2, 0xA2, 0xA2, 0xA2}; // Node 10 address: 0x79
//Data receive or transmit 
uint8_t rxRec[SIZEDATA] = {0} ;
uint8_t txEnv[SIZEDATA] = {0}; 
// Flag to use for monitor initiation uSD
uint8_t buSDState = 0; 
// flags use for NRF24L01+
uint8_t bNrf = 0;
// flag use for write first block in uSD
uint8_t bInituSD = 1; 
// flag use for check data ADXL355z
uint8_t bDataAdxl = 0;
// flag use for turn on ADXL255z
uint8_t bTurnOnAdxl = 0;
// flag use initiation save data in microSD
uint8_t bSaveData = 0;
uint8_t bMesure = 0;
// Use for send 512 bytes to uSD
uint16_t countUsd = 0;
// Use for library micro SD check that is SSC
uint8_t ccs = 0; 
// Use for start and stop timer
ds3234_time timerStop;
ds3234_time timerInit;
// Start address sector of uSD
uint32_t sector = 42048;
//Variable use in ACS722 sensor current for get value current
uint16_t vAdc = 0;
// flag use for initiation or stop send data station base
uint8_t bPMaster = 0;
// use for after 15 send data Adxl355z
uint8_t contEnv = 0;
// use for running program
bool running = true;
// Variable for use synchronization
int32_t sum_delay = 0;
int32_t sum_offset = 0;
int32_t sm_diff = 0;
int32_t ms_diff = 0;
int32_t timeMesure[2] = {0};
int32_t t1[2] = {0};
int32_t t3[2] = {0};
uint8_t timeInitMesure = 0 ;
// use for counter times for synchronization
int cSync = 0;
/*----------------------------------------------------------------------------
 FUNCTION PROTOTYPES
 -----------------------------------------------------------------------------*/
void task(uint8_t );
void syncClock(void);
int32_t differenceMS(int32_t t1[2]);
int32_t differenceSM(int32_t t4[2]);
void convertCharToInt(int32_t out[2]);
void initVariables(void);
void getTime(int32_t in[2]);
void setClock(int32_t in[2]);
void saveDataMsd();
void setTimerMesure();
void sendTime();
void saveMicroSd();// save test delete after
#endif
/*
 * End file
 */

