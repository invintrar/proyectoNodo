#include "ds3234.h"
/*****************************************************************************/
/* DS3234 Functions */
/*****************************************************************************/

/* Configure the DS3234 */
void DS3234_Init(void) {
    ds3234_control_register config;
    *((uint8_t *) & config) = 0;
    config.INTCN = 0;
    config.BBSQW = 1;
    // Square wave output frequency= 1Hz
    config.RS1 = 0;
    config.RS1 = 0;

    DS3234_ChepSelect = 0; // Enable SPI2 
    SPI2_Exchange_Byte(WRITE_CONTROL_REG);
    SPI2_Exchange_Byte(*(uint8_t *) & config);
    DS3234_ChepSelect = 1; // Un enable SPI2

}

void DS3234_readTime(ds3234_data_time *p) {
    DS3234_ChepSelect = 0;
    //Pointer at the content of a structure of two form:
    //p->seconds o (*p).seconds
    SPI2_Exchange_Byte(READ_TIME_REG);
    p->seconds = SPI2_Exchange_Byte(0x00);
    p->minutes = SPI2_Exchange_Byte(0x00);
    p->hours = SPI2_Exchange_Byte(0x00);
    p->day = SPI2_Exchange_Byte(0x00);
    p->date = SPI2_Exchange_Byte(0x00);
    p->month = SPI2_Exchange_Byte(0x00);
    p->year = SPI2_Exchange_Byte(0x00);
    DS3234_ChepSelect = 1;
    //Time of read is 256uS with 2.5Mhz spi2

    //Conversion from bcd to int
    (*p).seconds = bcd_to_int(p->seconds);//25ns 
    (*p).minutes = bcd_to_int(p->minutes);
    (*p).hours = bcd_to_int(p->hours);
    (*p).day = bcd_to_int(p->day);
    (*p).date = bcd_to_int(p->date);
    (*p).month = bcd_to_int(p->month);
    (*p).year = bcd_to_int(p->year);

}//End readTime ds3234

void DS3234_Time(ds3234_time *pt) {
    DS3234_ChepSelect = 0;
    //Pointer at the content of a structure of two form:
    //p->seconds o (*p).seconds
    SPI2_Exchange_Byte(READ_TIME_REG);
    pt->seconds = SPI2_Exchange_Byte(0x00);
    pt->minutes = SPI2_Exchange_Byte(0x00);
    pt->hours = SPI2_Exchange_Byte(0x00);
    DS3234_ChepSelect = 1;
    //Time of read is 64uS with 2.5Mhz spi2
    //Conversion from bcd to int
    (*pt).seconds = bcd_to_int(pt->seconds);
    (*pt).minutes = bcd_to_int(pt->minutes);
    (*pt).hours = bcd_to_int(pt->hours);

}// En DS3234_Time

/**
 * Function set of time 
 * @param data_time is a variable structure with seconds(0-59), minutes(0-59), 
 * hours(0-59), day(1:Sunday,2:Monday,...-7),date(0-31), month(1-12),year(0-99) 
 */
void DS3234_setTime(ds3234_data_time data_time) {
    //Conversion from int to bcd
    data_time.seconds = int_to_bcd(data_time.seconds);
    data_time.minutes = int_to_bcd(data_time.minutes);
    data_time.hours = int_to_bcd(data_time.hours);
    data_time.day = int_to_bcd(data_time.day);
    data_time.date = int_to_bcd(data_time.date);
    data_time.month = int_to_bcd(data_time.month);
    data_time.year = int_to_bcd(data_time.year);

    DS3234_ChepSelect = 0;

    SPI2_Exchange_Byte(WRITE_TIME_REG);
    SPI2_Exchange_Byte(data_time.seconds);
    SPI2_Exchange_Byte(data_time.minutes);
    SPI2_Exchange_Byte(data_time.hours);
    SPI2_Exchange_Byte(data_time.day);
    SPI2_Exchange_Byte(data_time.date);
    SPI2_Exchange_Byte(data_time.month);
    SPI2_Exchange_Byte(data_time.year);

    DS3234_ChepSelect = 1;
}// End Write

