/**
 * @author DzhL
 * @file main.c
 * @date 6/5/2020
 */

/*----------------------------------------------------------------------------
 HEADER FILES
 -----------------------------------------------------------------------------*/
#include "main.h"
#include "statement.h"

/*============================================================================
 FUNCTION MAIN
 =============================================================================*/
int main(void) {
    // Set Variables
    initVariables();

    SYSTEM_Initialize();
    __delay_ms(250);

    //Setup  RF24L01
    // RF24L01_setup(tx_addr, rx_addr, 22);

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

    // while
    while (running) {
        switch (bNrf) {
            case 1://Data received
                bNrf = 0;
                RF24L01_read_payload(rxRec, sizeof (rxRec));
                __delay_ms(4);
                if (rxRec[0] > 0)
                    task(rxRec[0]);
                break;
            case 2://Date Sent
                bNrf = 0;
                RF24L01_set_mode_RX();
                break;
            case 3://MAX_RT
                RF24L01_set_mode_RX();
                bNrf = 0;
                break;
            default:
                break;
        } // end switch
    } // End while



    // Loop Infinite
    while (1) {
        //Led_verde_toggle();
        //__delay_ms(250);
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
}// End main

// Function for initialize variables

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

    // Set address NRF24L01+ RX and TX
    for (i = 0; i < 5; i++) {
        //RX Address 
        rx_addr[i] = 0xA1;
        //TX Address
        tx_addr[i] = 0x78;
    }

    //Initialization the variable of ADXL355Z
    for (i = 0; i < 63; i++) {
        dataAdxl[i] = 0x00;
    }

    //Initialization the variable of Data NRF24L01+
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
 * Use for sent data for NRF24L1+
 */
void sentDataFull() {
    // Start measuring
    ADC1_SamplingStart();
    ADC1_SamplingStop();

    // Read RTC time
    DS3234_Time(&rtcTime);

    // Add data to send
    //Get Data DS3234
    nrfDataTx[0] = 4;
    // put data ACS722
    nrfDataTx[1] = vAdc;
    nrfDataTx[2] = vAdc >> 8;
    // put data ADXL355Z
    nrfDataTx[3] = dataAdxl[0];
    nrfDataTx[4] = dataAdxl[1];
    nrfDataTx[5] = dataAdxl[2];
    nrfDataTx[6] = dataAdxl[3];
    nrfDataTx[7] = dataAdxl[4];
    nrfDataTx[8] = dataAdxl[5];
    nrfDataTx[9] = dataAdxl[6];
    nrfDataTx[10] = dataAdxl[7];
    nrfDataTx[11] = dataAdxl[8];
    //Sent Data for NRF24L01+
    RF24L01_sendData(nrfDataTx, sizeof (nrfDataTx));
} // end send data 

/**
 * Send data for synchronization 
 */
void sentData_time() {
    // Read RTC time
    DS3234_Time(&rtcTime);

    // Add data to send
    //Get Data DS3234
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
    //Sent Data for NRF24L01+
    RF24L01_sendData(nrfDataTx, sizeof (nrfDataTx));
}

void task(uint8_t opc) {
    switch (opc) {
        case 1: // Clock synchronization
            //syncClock();
            break;
        case 2:// Initiation Measure
            break;
        case 3: // Turn off application
            //RF24L01_powerDown();
            running = !running;
            break;
        default:
            break;
    } // end switch
} // end task

void syncClock(void) {
    if (cSync < TIMES) {
        if (rxRec[10] == 1) {
            convertCharToInt(t1);
            ms_diff = differenceMS(t1);
            // get time
            getTime1(t3);
            txEnv[0] = 2;
            //sendData(txEnv);
        } else if (rxRec[10] == 2) {
            convertCharToInt(t1);
            sm_diff = differenceSM(t1);
            long offset = (ms_diff - sm_diff) / 2;
            long delay = (ms_diff + sm_diff) / 2;
            // add offset and delay
            sum_offset += offset;
            sum_delay += delay;
            txEnv[0] = 1;
            //sendData(txEnv);
            cSync++;
        }
    } else {
        // Show results
        long offsetMesure = sum_offset / (TIMES);
        long delayMesure = sum_delay / (TIMES);
        txEnv[0] = 3;
        //sendData(txEnv);
        // clean variable
        cSync = 0;
        sum_offset = 0;
        sum_delay = 0;
        offsetMesure = 0;
        delayMesure = 0;
    }
} // en synClock

long differenceMS(int t1[2]) {
    int t2[2] = {0};
    //getTime(t2);
    return (TO_NSEC(t2) - TO_NSEC(t1));
} // end dms

long differenceSM(int t4[2]) {
    return (TO_NSEC(t4) - TO_NSEC(t3));
} // end dsm

void convertCharToInt(int out[2]) {
    int aux = 0;
    aux = (rxRec[4] << 24) | (rxRec[3] << 16) | (rxRec[2] << 8) | rxRec[1];
    out[0] = aux; //seconds
    aux = (rxRec[8] << 24) | (rxRec[7] << 16) | (rxRec[6] << 8) | rxRec[5];
    out[1] = aux; //nseconds
} // end cchartoint

void getTime1(int in[2]) {
    if (in != NULL) {
        // Get Seconds and convert to uint8_t
        txEnv[0] = 0;
        txEnv[1] = in[0];
        txEnv[2] = (in[0] >> 8);
        txEnv[3] = (in[0] >> 16);
        txEnv[4] = (in[0] >> 24);
        // Get nanoSeconds and convert to uint8_t
        txEnv[5] = in[1];
        txEnv[6] = (in[1] >> 8);
        txEnv[7] = (in[1] >> 16);
        txEnv[8] = (in[1] >> 24);
        txEnv[9] = 0;
        txEnv[10] = 0;
        txEnv[11] = 0;
        txEnv[12] = 0;
    }
} // end getTime
