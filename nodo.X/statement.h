/**
 * @author DzhL
 */
#ifndef STATEMENT
#define	STATEMENT

#define TIMES                   500
#define TO_NSEC(t)          (((long)t[0] * 1000000000L) + t[1])
/*----------------------------------------------------------------------------
 VARIABLE GLOBALES
 -----------------------------------------------------------------------------*/
//Variable that use for save in uSD
uint8_t dataSentuSD[512];
//Variable for get data from the ADXL355Z
uint8_t dataAdxl[63];
//Address for Nrf RX or TX
uint8_t rx_addr[5], tx_addr[5];
//Data receive or transmit 
uint8_t nrfDataRx[14], nrfDataTx[14];
uint8_t rxRec[12] ;
uint8_t txEnv[12] ; 
//Flag to use in the software
uint8_t buSDState, j, idNodo;
uint8_t bNrf, bMrx, bInt1, bInituSD, bNrfsync; //bNrfsync variable de sincronizacion
uint16_t i;
uint16_t timeMcs;
uint16_t per;
uint16_t timeMls;
uint8_t timeSec;
uint8_t timeMin;
uint8_t timeHor;
 
unsigned char wuSD;
//Variable use in DS3234
ds3234_data_time rtc;
ds3234_time rtcTime;
ds3234_time_sync time1Sync, time2Sync, time3Sync, time4Sync; // variables para sincronizacion
uint32_t sector;
//Variable use in ACS722 sensor current
uint16_t vAdc;

bool running;

long sum_delay;
long sum_offset;
long sm_diff;
long ms_diff;
int t1[2];
int t3[2];

int cSync = 0;


/*----------------------------------------------------------------------------
 FUNCTION PROTOTYPES
 -----------------------------------------------------------------------------*/
void task(uint8_t );
void syncClock(void);
long delayDifSM(int t4[2]);
long differenceMS(int t1[2]);
long differenceSM(int t4[2]);
void convertCharToInt(int out[2]);
void sentDataFull();
void initVariables(void);
void getTime1(int in[2]);


#endif
/*
 * End file
 */

