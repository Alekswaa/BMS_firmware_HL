#ifndef LTC6811_H
#define LTC6811_H
//-------------------------------------------------------------------
#include "configurations.h"


#define VOLTAGE_MEM_SIZE sizeof(float) * 3 * 4 * NUMBER_OF_SLAVES //one float per voltage * Number of voltager per register * nr of registers * number of slaves
#define TEMPERATURE_MEM_SIZE sizeof(float) * 5 * NUMBER_OF_SLAVES //one float per temperature * Number of temperatures per register * nr of registers * number of slaves
#define VOLT_BUFFER_SIZE (2 * 3 + 2) * NUMBER_OF_SLAVES
#define TEMP_BUFFER_SIZE (2 * 3 + 2) * NUMBER_OF_SLAVES



/* LTC6811 Commands. See page 59 of datasheet */
#define WRCFGA  0x0001
#define RDCFGA  0x0002 //bit at index 1 is constantly 2

#define RDCVA   0x0004
#define RDCVB   0x0006
#define RDCVC   0x0008
#define RDCVD   0x000A
#define PLADC	0x0714

#define RDAUXA  0x000C
#define RDAUXB  0x000E
#define RDSTATA 0x0010
#define RDSTATB 0x0012
#define ADAX    0x0460

#define REFON 1<<2
#define ADCOPT_ON  1
#define ADCOPT_OFF 0
#define ADCV 0x0260
#define SEVEN_kHz 2<<7
#define FOURTY_TWO_kHz 1<<7
#define DISCHARGE_NOT_PERMITTED 0<<4
#define ALL_CELLS 0



/**/
typedef enum init_command {
    VOLTAGE,
    TEMPERATURE
}init_command_t;

//-------------------------------------------------------------------
#endif