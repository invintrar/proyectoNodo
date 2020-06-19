/**
 * @author DzhL
 */
#ifndef STATEMENT
#define	STATEMENT

#define TIMES           100
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
uint8_t dataAdxl[63] = {0};
// Address for NRF24L01+ RX and TX
uint8_t rx_addr[5] = {0xA1, 0xA1, 0xA1, 0xA1, 0xA1}; 
uint8_t tx_addr[5] = {0xA1, 0xA1, 0xA1, 0xA1, 0xA1}; 
//Data receive or transmit 
uint8_t rxRec[SIZEDATA] = {0} ;
uint8_t txEnv[SIZEDATA] = {0}; 
// Flag to use in the software
uint8_t buSDState = 0; 
uint8_t idNodo = 0;
uint8_t bNrf = 0;
uint8_t bMrx = 1;
uint8_t bInt1 = 0;
uint8_t bInituSD = 0; 
uint8_t bNrfsync = 0;
uint8_t bInitExt = 0;
uint8_t bDataAdxl = 0;
uint16_t i = 0;
uint16_t per = 0;
// Handling time
uint8_t timeSec = 0;
uint8_t timeMin = 0;
uint8_t timeHor = 0;
uint8_t bMesure = 0;
 
unsigned char wuSD = 0;
// Variable use in DS3234
ds3234_date_time rtc;
ds3234_time rtcTime;
ds3234_time timerMesure;
uint8_t timerInitMesure = 0;
// Start address sector of uSD
uint32_t sector = 35000;
//Variable use in ACS722 sensor current
uint16_t vAdc = 0;
uint8_t bPMaster = 0;
uint8_t contEnv = 0;
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
uint8_t bSaveData = 0;// Initiation data in microSD
int cSync = 0;
int32_t microSec = 0;


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


#endif
/*
 * End file
 */

