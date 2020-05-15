
/*
 * 
 * File:   main.c
 * Author: Darwin & Jose
 *
 * Created on June 26, 2019, 12:15 AM
 */

/*----------------------------------------------------------------------------
 HEADER FILES
 -----------------------------------------------------------------------------*/
#include "main.h"


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
//Flag to use in the software
uint8_t buSDState, j, idNodo;
uint8_t bNrf, bMrx, bInt1, bInituSD,bNrfsync; //bNrfsync variable de sincronizacion
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
ds3234_time_u time1, time2, time3, time4; // variables para sincronizacion
uint32_t sector;
//Variable use in ACS722 sensor current
uint16_t vAdc;
/*----------------------------------------------------------------------------
 FUNCTION PROTOTYPES
 -----------------------------------------------------------------------------*/
void initVariables(void);
void sentData();
void sentData_time(); // Envio de tiempo para sincronizacion

/*============================================================================
 FUNCTION MAIN
 =============================================================================*/
int main(void) {
    // Set Variables
    initVariables();

    SYSTEM_Initialize();
    __delay_ms(250);

    //Setup  RF24L01
    RF24L01_setup(tx_addr, rx_addr, 22);

    //Set Time of RTC(DS3234)
    //DS3234_setTime(rtc);


    //Turn on ADXL255
    //ADXL355_Write_Byte(POWER_CTL, MEASURING);

    // Read RTC time
    //DS3234_Time(&rtcTime1);
    //__delay_ms(250);


    // Start measuring
    //ADC1_SamplingStart();
    //ADC1_SamplingStop();

    
    // Loop Infinite para sincronizacion
    while (bNrfsync < 5) {

            if (bNrf == 1 && bNrfsync == 0){ //Data Ready // igualo el Ds3234
                //Move from Nrf data receive to variable RTC for sent to RTC
                rtc.seconds = nrfDataRx[0];
                rtc.minutes = nrfDataRx[1];
                rtc.hours = nrfDataRx[2];
                rtc.day = nrfDataRx[3];
                rtc.month = nrfDataRx[4];
                rtc.date = nrfDataRx[5];
                rtc.year = nrfDataRx[6];
                //Equals counter timer
                timeSec = rtc.seconds;
                timeMin = rtc.minutes;
                timeHor = rtc.hours;
                TMR1_Start();
                //TMR2_Start();
                //TMR4_Start();
                // Set Time of the RTC
                DS3234_setTime(rtc);
                bNrfsync = 1; // Asignacion de tiempo en DS3234
                bNrf = 4;
            }
            else if(bNrf == 1 && bNrfsync == 2){ //Data Ready // igualo el Ds3234 // Se obtiene t1
                //Move from Nrf data receive to variable RTC for sent to RTC
                time1.seconds = nrfDataRx[0];
                time1.minutes = nrfDataRx[1];
                time1.hours = nrfDataRx[2];
                time1.microseconds = nrfDataRx[3]; // Almacena el tiempo 1 // EDITAR PARA RECIBIR DE LA EB
                time2.seconds = rtcTime.seconds;
                time2.minutes = rtcTime.minutes;
                time2.hours = rtcTime.hours;
                time2.microseconds = timeMcs; // Almacena el tiempo 2
                //rtc.day = nrfDataRx[3];
                //rtc.month = nrfDataRx[4];
                //rtc.date = nrfDataRx[5];
                //rtc.year = nrfDataRx[6];
                //Equals counter timer
                //timeSec = rtc.seconds;
                //timeMin = rtc.minutes;
                //timeHor = rtc.hours;
                
                //TMR1_Start();
                //TMR2_Start();
                //TMR4_Start();
                // Set Time of the RTC
                //DS3234_setTime(rtc);
                bNrfsync = 3; // Asignacion de tiempo en DS3234
                bNrf = 2; // va a transmitir el tiempo a la EB
            }
            else if (bNrf == 1 && bNrfsync == 4){ //Data Ready // igualo el Ds3234 // Se obtiene t4
                //Move from Nrf data receive to variable RTC for sent to RTC
                time4.seconds = nrfDataRx[0];
                time4.minutes = nrfDataRx[1];
                time4.hours = nrfDataRx[2];
                time4.microseconds = nrfDataRx[3]; // Almacena el tiempo 1 // EDITAR PARA RECIBIR DE LA EB 
                //rtc.day = nrfDataRx[3];
                //rtc.month = nrfDataRx[4];
                //rtc.date = nrfDataRx[5];
                //rtc.year = nrfDataRx[6];
                //Equals counter timer
                //timeSec = rtc.seconds;
                //timeMin = rtc.minutes;
                //timeHor = rtc.hours;
                
                //TMR1_Start();
                //TMR2_Start();
                //TMR4_Start();
                // Set Time of the RTC
                //DS3234_setTime(rtc);
                bNrfsync = 5; // Asignacion de tiempo en DS3234
                //bNrf = 2; // va a transmitir el tiempo a la EB
            }
           
            else if (bNrf == 2 && bNrfsync == 3){ //Data sent TX
                if (timeMcs == 0) {
                    sentData_time(); // envia cuando pasa de segundo
                    bNrfsync = 4; // se prepara para recibir el ultimo tiempo
                    bNrf = 4;
                }
            }
            else if (bNrf == 3 && bNrfsync == 3){ //MAX_RT TX
                if (timeMcs == 0) {
                    sentData_time(); // envia cuando pasa de segundo
                    bNrfsync = 4; // se prepara para recibir el ultimo tiempo
                    bNrf = 4;
                }
            }
            else if (bNrf == 4){ //Mode RX
                RF24L01_set_mode_RX();
                bNrf = 0;
            }
            else {
            }
    }
    
    
    
     
    // Loop Infinite
    while (1) 
    {
        Led_verde_toggle();
        __delay_ms(250);
        /*
        switch (bNrf) {
            case 1://Data Ready
                //Move from Nrf data receive to variable RTC for sent to RTC
                rtc.seconds = nrfDataRx[0];
                rtc.minutes = nrfDataRx[1];
                rtc.hours = nrfDataRx[2];
                rtc.day = nrfDataRx[3];
                rtc.month = nrfDataRx[4];
                rtc.date = nrfDataRx[5];
                rtc.year = nrfDataRx[6];
                //Equals counter timer
                timeSec = rtc.seconds;
                timeMin = rtc.minutes;
                timeHor = rtc.hours;
                TMR1_Start();
                TMR2_Start();
                TMR4_Start();
                // Set Time of the RTC
                DS3234_setTime(rtc);
                bNrf = 2;
                break;
            case 2://Data sent TX
                sentData();
                break;
            case 3://MAX_RT TX
                sentData();
                break;
            case 4://Set Mode RX
                RF24L01_set_mode_RX();
                bNrf = 0;
                break;
            default:
                break;
        }

        //Check uSD 
        if (SD_Detect() == DETECTED) {
            if (bInituSD == 1 && buSDState == SUCCESSFUL_INIT) {
                // Read RTC time
                DS3234_readTime(&rtc);
                dataSentuSD[0] = idNodo;
                dataSentuSD[1] = rtc.seconds;
                dataSentuSD[2] = rtc.minutes;
                dataSentuSD[3] = rtc.hours;
                dataSentuSD[4] = rtc.day;
                dataSentuSD[5] = rtc.date;
                dataSentuSD[6] = rtc.month;
                dataSentuSD[7] = rtc.year;
                wuSD = SD_Write_Block(dataSentuSD, sector);
                if (wuSD == DATA_ACCEPTED) {
                    sector++;
                    bInituSD = 0;
                }

            } else if (bInt1 == 1 && buSDState == SUCCESSFUL_INIT && bInituSD == 0) {
                for (j = 0; j < 63; j++) {
                    dataSentuSD[i] = dataAdxl[j];
                    if (i < 512) {
                        i++;
                    } else {
                        wuSD = SD_Write_Block(dataSentuSD, sector);
                        if (wuSD == DATA_ACCEPTED) {
                            i = 0;
                            sector++;
                        }
                    }
                }
                bInt1 = 0;
            }

        } else {
            Led_verde_setLow();
            SD_Init();
        } */
        
        
    }// end loop 
        
    return 0;
}

void initVariables() {
    bInt1 = 0;
    wuSD = 0;
    bInituSD = 0;
    bNrf = 4;
    bNrfsync = 0; //variable de sincronizacion
    bMrx = 1;
    idNodo = 1;
    timeMcs = 0;
    timeMls = 0;
    timeSec = 0;
    timeMin = 0;
    timeHor = 0;
    per = 0;
    j = 0;
    //Sector write uSD
    sector = 35000;

    // Put all values of the time to cero 
    (*(uint8_t *) & rtc) = 0x00;

    // Address NrF24L01 RX and TX
    for (i = 0; i < 5; i++) {
        //RX Address 
        rx_addr[i] = 0xA1;
        //TX Address
        tx_addr[i] = 0x78;
    }
    //Initialization the variable of ADXL355z
    for (i = 0; i < 63; i++) {
        dataAdxl[i] = 0x00;
    }
    //Initialization the variable of Data Nrf24L01 +
    for (i = 0; i < 14; i++) {
        nrfDataRx[i] = 0x00;
        nrfDataTx[i] = 0x00;
    }

    //Initialization the variable of uSD
    for (i = 0; i < 512; i++) {
        dataSentuSD[i] = 0x5A;
    }
    i = 0;
}

/**
 * Use for sent data for Nrf24L1+
 */
void sentData() {
    // Start measuring
    ADC1_SamplingStart();
    ADC1_SamplingStop();

    // Read RTC time
    DS3234_Time(&rtcTime);

    // Add data to send
    //Get Data DS3234
    nrfDataTx[0] = rtcTime.seconds;
    nrfDataTx[1] = rtcTime.minutes;
    nrfDataTx[2] = rtcTime.hours;
    // Get Data ACS722
    nrfDataTx[3] = vAdc;
    nrfDataTx[4] = vAdc >> 8;
    //Get Data ADXL355Z
    nrfDataTx[5] = dataAdxl[0];
    nrfDataTx[6] = dataAdxl[1];
    nrfDataTx[7] = dataAdxl[2];
    nrfDataTx[8] = dataAdxl[3];
    nrfDataTx[9] = dataAdxl[4];
    nrfDataTx[10] = dataAdxl[5];
    nrfDataTx[11] = dataAdxl[6];
    nrfDataTx[12] = dataAdxl[7];
    nrfDataTx[13] = dataAdxl[8];
    //Sent Data for Nrf24L01+
    RF24L01_sendData(nrfDataTx, sizeof (nrfDataTx));
}



void sentData_time() {
    // Start measuring
    //ADC1_SamplingStart();
    //ADC1_SamplingStop();

    // Read RTC time
    DS3234_Time(&rtcTime);

    // Add data to send
    //Get Data DS3234
    time3.seconds = rtcTime.seconds;
    time3.minutes = rtcTime.minutes;
    time3.hours = rtcTime.hours;
    time3.microseconds = timeMcs; // Almacena el tiempo 3
    nrfDataTx[0] = time3.seconds;
    nrfDataTx[1] = time3.minutes;
    nrfDataTx[2] = time3.hours;
    nrfDataTx[3] = time3.microseconds;
    // Get Data ACS722
    /*nrfDataTx[3] = vAdc;
    nrfDataTx[4] = vAdc >> 8;
    //Get Data ADXL355Z
    nrfDataTx[5] = dataAdxl[0];
    nrfDataTx[6] = dataAdxl[1];
    nrfDataTx[7] = dataAdxl[2];
    nrfDataTx[8] = dataAdxl[3];
    nrfDataTx[9] = dataAdxl[4];
    nrfDataTx[10] = dataAdxl[5];
    nrfDataTx[11] = dataAdxl[6];
    nrfDataTx[12] = dataAdxl[7];
    nrfDataTx[13] = dataAdxl[8]; */
    //Sent Data for Nrf24L01+
    RF24L01_sendData(nrfDataTx, sizeof (nrfDataTx));
}
