#ifndef FUNCTION_DEFS_H
#define FUNCTION_DEFS_H

#define NO_ERROR 0 
#define ERROR 1

//BMS states
typedef enum state{
    INIT,
    ACQUISITION,
    CELL_SAFETY,
    SEND_DATA,
    SOC,
    EMERGENCY,
    STOP
} state;




void init_SPI();
void start_acq();



#endif
