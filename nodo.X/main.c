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
    
    for(i = 0; i<5 ;i++){
        Led_verde_toggle();
        __delay_ms(100);
        
    }
    
    Led_verde_setHigh();

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
    DS3234_Time(&rtcTime);

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
    DS3234_Time(&rtcTime);

    // Add data to send
    //Get Data DS3234
    // Get Data ACS722
    /*txEnv[3] = vAdc;
    txEnv[4] = vAdc >> 8;
    //Get Data ADXL355Z
    txEnv[5] = dataAdxl[0];
    txEnv[6] = dataAdxl[1];
    txEnv[7] = dataAdxl[2];
    txEnv[8] = dataAdxl[3];
    txEnv[9] = dataAdxl[4];
    txEnv[10] = dataAdxl[5];
    txEnv[11] = dataAdxl[6];
    txEnv[12] = dataAdxl[7];
    txEnv[13] = dataAdxl[8]; */
    //Sent Data for NRF24L01+
    RF24L01_sendData(txEnv, SIZEDATA);
}

void task(uint8_t opc) {
    switch (opc) {
        case 1: // Clock synchronization
            Led_verde_toggle();
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
            getTime(t3);
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

/**
 * 
 * @param t1 (Master)
 * @return difference between t1(Master) and t2(Slave) 
 */
int32_t differenceMS(int32_t t1[2]) {
    int32_t t2[2] = {0};
    //getTime(t2);
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
