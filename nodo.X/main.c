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
    // Put all values of the time to cero 
    (*(uint8_t *) & rtc) = 0x00;

    SYSTEM_Initialize();
    __delay_ms(250);

    //Setup  RF24L01
    RF24L01_setup(tx_addr, rx_addr, CHANNEL, SIZEDATA);

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


    RF24L01_set_mode_RX();

    for (i = 0; i < 5; i++) {
        Led_verde_toggle();
        __delay_ms(100);

    }

    Led_verde_setHigh();
    //ADXL355_Write_Byte(POWER_CTL, MEASURING);

    //convertCharToInt(timeMesure);
    //DS3234_getTime(&timeSent);

    //microSec = TMR2_Counter32BitGet();
    //TMR2_Counter32BitSet(0);
    //microSec = TMR2_Counter32BitGet() * 25;// get time in ns



    // while
    while (running) {
        switch (bNrf) {
            case 1://Data received
                bNrf = 0;
                RF24L01_read_payload(rxRec, SIZEDATA);
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

    return 0;
}// End main

/**
 * Use for sent data for NRF24L1+
 */
void sendDataFull() {
    // Start measuring
    ADC1_SamplingStart();
    ADC1_SamplingStop();

    // Read RTC time
    DS3234_time(&rtcTime);

    // Add data to send
    //Get Data DS3234
    txEnv[0] = 4;
    // put data ACS722
    txEnv[1] = vAdc;
    txEnv[2] = vAdc >> 8;
    // put data ADXL355Z
    txEnv[3] = dataAdxl[0];
    txEnv[4] = dataAdxl[1];
    txEnv[5] = dataAdxl[2];
    txEnv[6] = dataAdxl[3];
    txEnv[7] = dataAdxl[4];
    txEnv[8] = dataAdxl[5];
    txEnv[9] = dataAdxl[6];
    txEnv[10] = dataAdxl[7];
    txEnv[11] = dataAdxl[8];
    //Sent Data for NRF24L01+
    RF24L01_sendData(txEnv, SIZEDATA);
} // end sentDataFull 

/**
 * Send data for synchronization 
 */
void sendTime() {
    // Read RTC time
    ds3234_date_time timeSent;
    DS3234_getTime(&timeSent);
    txEnv[0] = 3;
    txEnv[1] = timeSent.seconds;
    txEnv[2] = timeSent.minutes;
    txEnv[3] = timeSent.hours;
    txEnv[4] = timeSent.date;
    txEnv[5] = timeSent.month;
    txEnv[6] = timeSent.year;
    txEnv[7] = timeSent.day;
    RF24L01_sendData(txEnv, SIZEDATA);
}

void task(uint8_t opc) {
    switch (opc) {
        case 1: // Clock synchronization
            syncClock();
            break;
        case 2:// Set Clock
            convertCharToInt(timeMesure);
            setClock(timeMesure);
            //txEnv[0] = 1;// request t1
            //RF24L01_sendData(txEnv, SIZEDATA);
            sendTime();
            break;
        case 3: // Turn off application
            //RF24L01_powerDown();
            running = !running;
            break;
        case 4: // test ADXL355Z
            
            break;
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
        TMR2_Counter32BitSet(in[1]);
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
    int32_t aux = 0;
    int32_t aux1 = 0;
    int32_t aux2 = 0;
    int32_t aux3 = 0;

    aux3 = rxRec[4];
    aux3 = aux3 << 24;
    aux2 = rxRec[3];
    aux2 = aux2 << 16;
    aux1 = rxRec[2];
    aux1 = aux1 << 8;
    aux = rxRec[1];
    aux = aux3 | aux2 | aux1 | aux;
    out[0] = aux; // seconds

    aux3 = rxRec[8];
    aux3 = aux3 << 24;
    aux2 = rxRec[7];
    aux2 = aux2 << 16;
    aux1 = rxRec[6];
    aux1 = aux1 << 8;
    aux = rxRec[5];
    aux = aux3 | aux2 | aux1 | aux;
    out[1] = aux; //nseconds
} // end charToInt32

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

void setClock(int32_t in[2]) {
    time_t timer;
    struct tm *newtime;
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

}// set clock of raspberry pi
