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
    uint8_t i = 0;
    timerInit.hours = 0;
    timerInit.minutes = 0;
    timerInit.seconds = 0;
    timerStop.hours = 0;
    timerStop.minutes = 0;
    timerStop.seconds = 0;
    // Initialization dsPIC32EP256MC202
    SYSTEM_Initialize();
    __delay_ms(250);

    // Setup  RF24L01(address and channel y  SIZEDATA in statement.h)
    RF24L01_setup(tx_addr, rx_addr, CHANNEL, SIZEDATA);

    // Set RF24L01 reception mode
    RF24L01_set_mode_RX();

    // Start measuring current
    ADC1_SamplingStart();
    ADC1_SamplingStop();

    // Turn on ADXL355Z
    ADXL355_Write_Byte(POWER_CTL, MEASURING);

    // Signal of Initiation program
    for (i = 0; i < 6; i++) {
        Led_verde_toggle();
        __delay_ms(100);
    }

    // while running program
    while (running) {
        switch (bNrf) {
            case 1:// Data successfully received
                bNrf = 0;
                RF24L01_read_payload(rxRec, SIZEDATA);
                __delay_ms(3); // delay de 3 ms no cambiar
                if (rxRec[0] > 0)
                    task(rxRec[0]); // Task to make 
                break;
            case 2:// Date successfully sent
                bNrf = 0;
                RF24L01_set_mode_RX();
                break;
            case 3:// Maximum number of retransmit (default 2)
                RF24L01_set_mode_RX();
                bNrf = 0;
                break;
            default:
                break;
        } // end switch
    } // End while

    return 0;
}// End main


void task(uint8_t opc) {
    switch (opc) {
        case 1: // Clock synchronization
            syncClock();
            break;
        case 2:// Set Clock
            convertCharToInt(timeMesure);
            setClock(timeMesure);
            txEnv[0] = 1; // request t1
            RF24L01_sendData(txEnv, SIZEDATA);
            //sendTime();
            break;
        case 3: // For sent data ADXL355 from Node to Master
            bPMaster = 1;
            break;
        case 4: // Initiation save data in microSD
            bMesure = 1;
            setTimerMesure();
            txEnv[0] = 6;
            txEnv[1] = idNodo;
            RF24L01_sendData(txEnv, 12);
            break;
        case 5: // Stop sent data ADXL355 from Node to Master
            bPMaster = 0;
            txEnv[0] = 5; // sent notify end measuring
            RF24L01_sendData(txEnv, 12);
            break;
        case 6:
            bMesure = 0;
            running = 0;
        default:
            break;
    } // end switch
} // end task

void syncClock() {
    if (cSync < TIMES) {
        if (rxRec[10] == 1) {
            convertCharToInt(t1);
            ms_diff = differenceMS(t1);
            getTime(t3);
            // request t4
            txEnv[0] = 2;
            // send request
            RF24L01_sendData(txEnv, SIZEDATA);
        } else if (rxRec[10] == 2) {
            // convert t4 received
            convertCharToInt(t1);
            // calculation difference t4 y t3
            sm_diff = differenceSM(t1);
            // calculation offset
            long offset = (ms_diff - sm_diff) / 2;
            // calculation delay
            long delay = (ms_diff + sm_diff) / 2;
            // stored in sum offset and sum delay
            sum_offset += offset;
            sum_delay += delay;
            // request t1
            txEnv[0] = 1;
            // send request
            RF24L01_sendData(txEnv, SIZEDATA);
            // increment
            cSync++;
        }
    } else {
        // execute calculation of average for equal clock and ending synchronization
        long offsetMesure = sum_offset / (TIMES);
        long delayMesure = sum_delay / (TIMES);
        // Set clock
        int32_t in[2] = {0};
        if (offsetMesure < 0)
            offsetMesure = -1 * offsetMesure;
        if (delayMesure < 0)
            delayMesure = -1 * delayMesure;
        getTime(in);
        in[1] = offsetMesure + delayMesure + in[1];
        if (in[1] > 1000000000) {
            in[1] = in[1] - 1000000000;
            in[0]++;
        }
        setClock(in);
        // notification synchronization full
        sendTime();
        // initialization variable
        cSync = 0;
        sum_offset = 0;
        sum_delay = 0;
        offsetMesure = 0;
        delayMesure = 0;
    }
} // en synClock

/**
 * 
 * @param t1 (Master)
 * @return difference between t1(Master) and t2(Slave) 
 */
int32_t differenceMS(int32_t t1[2]) {
    int32_t t2[2] = {0};
    getTime(t2);
    return (TO_NSEC(t2) - TO_NSEC(t1));
} // end dms

/**
 * 
 * @param t4(Master)
 * @return difference between t4(Master) and t3(Slave) 
 */
int32_t differenceSM(int32_t t4[2]) {
    return (TO_NSEC(t4) - TO_NSEC(t3));
} // end dsm

/**
 * Convert data received(uint8_t) to (int32_t) 
 * @param out
 */
void convertCharToInt(int32_t out[2]) {
    out[0] = (int32_t) rxRec[4] << 24 | (int32_t) rxRec[3] << 16 |
            (int32_t) rxRec[2] << 8 | rxRec[1]; // Seconds
    out[1] = (int32_t) rxRec[8] << 24 | (int32_t) rxRec[7] << 16 |
            (int32_t) rxRec[6] << 8 | rxRec[5]; // Nano Seconds

} // end charToInt32

void setClock(int32_t in[2]) {
    time_t timer;
    struct tm *newtime;

    if (in[1] > 1000000000) {
        in[1] = in[1] - 1000000000;
        in[0]++;
    }
    timer = in[0];
    newtime = gmtime(&timer);
    ds3234_date_time timeSet;
    timeSet.seconds = newtime->tm_sec;
    timeSet.minutes = newtime->tm_min;
    timeSet.hours = newtime->tm_hour;
    timeSet.day = (newtime->tm_wday + 1);
    timeSet.date = newtime->tm_mday;
    timeSet.month = newtime->tm_mon + 1;
    timeSet.year = (newtime->tm_year - 100);
    DS3234_setTime(timeSet);
    uint32_t aux1 = in[1];
    uint32_t aux = aux1 / 25;
    TMR2_Counter32BitSet(aux);
}// set clock of raspberry pi

void setTimerMesure() {
    int32_t timeMaster[2];
    convertCharToInt(timeMaster);
    time_t timer;
    struct tm *newtime;
    timer = timeMaster[0];
    newtime = gmtime(&timer);
    // Use for initiation timer after 1 minute
    timerInit.hours = newtime->tm_hour;
    timerInit.minutes = newtime->tm_min + 1;
    timerInit.seconds = newtime->tm_sec;
    if (timerInit.minutes > 59) {
        timerInit.minutes = timerInit.minutes - 59;
        timerInit.hours = timerInit.hours + 1;
    }
    // Use for stop timer 
    timerStop.seconds = newtime->tm_sec + rxRec[9];
    timerStop.minutes = newtime->tm_min + rxRec[10] + 1;
    timerStop.hours = newtime->tm_hour + rxRec[11];
    if (timerStop.seconds > 59) {
        timerStop.minutes = timerStop.minutes + 1;
        timerStop.seconds = timerStop.seconds - 59;
    }
    if (timerStop.minutes > 59) {
        timerStop.hours = timerStop.hours + 1;
        timerStop.minutes = timerStop.minutes - 59;
    }
}

void getTime(int32_t in[2]) {
    ds3234_date_time timeGet;
    // Get time of the DS3234
    DS3234_getTime(&timeGet);
    time_t whattime;
    struct tm nowtime;
    nowtime.tm_sec = timeGet.seconds;
    nowtime.tm_min = timeGet.minutes;
    nowtime.tm_hour = timeGet.hours;
    nowtime.tm_wday = (timeGet.day - 1);
    nowtime.tm_mday = timeGet.date;
    nowtime.tm_mon = (timeGet.month - 1);
    nowtime.tm_year = (timeGet.year + 100);
    // convert to seconds
    whattime = mktime(&nowtime);
    in[0] = (int32_t) whattime; // time in seconds
    // get time of the timer 2
    in[1] = TMR2_Counter32BitGet() * 25; // time in nano seconds
} // end getTime

/**
 * Send data when end synchronization 
 */
void sendTime() {
    // Read RTC time
    ds3234_date_time timeSent;
    DS3234_getTime(&timeSent);
    txEnv[0] = 3; // Option notification for the master end synchronization
    txEnv[1] = timeSent.seconds;
    txEnv[2] = timeSent.minutes;
    txEnv[3] = timeSent.hours;
    txEnv[4] = timeSent.date;
    txEnv[5] = timeSent.month;
    txEnv[6] = timeSent.year;
    txEnv[7] = timeSent.day;
    RF24L01_sendData(txEnv, SIZEDATA);
}