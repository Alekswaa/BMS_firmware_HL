//System libraries
#include <stdbool.h>
#include <stdint.h>

//Custom libraries
#include "function_defs.h"
#include "configurations.h"

// Global parameters
/*******************************************/
//Starting BMS in the INIT state
#ifdef TESTING
    state main_state = INIT_TESTING_STATE;
#else
    state bms_state = INIT;
#endif


void acquisition()
{
    // Should current be measured before ltc initiate?
    // Decrease kf time or decrease system time?
    measure_current();
    //send message to ltc for starting measuring
    //Initiate the SPI peripheral to send the data to memory? DMA?
    initiate_ltc_measuring();
    bms_state = SOC;
}

void check_cell_parameters(){

}

uint16_t* voltage_memory;
void init_firmware(){
    voltage_memory = malloc(sizeof(uint16_t)*NUMBER_OF_MODULES_PER_BMS);
}

void main()
{


    //FSM that runs as long as the BMS runs
    while(true){
        switch(bms_state){
            case INIT:
                init_peripherals();
                init_firmware();
                break;
            case ACQUISITION:
                acquisition();
                break;
            case CELL_SAFETY:
                check_cell_parameters();
                break;
            case COMMUNICATION:
                send_CAN_messages();
                break;
            case SOC:
                kalman_filter();
                break;
            //Emergency and stop does the same stuff (isj)
            case EMERGENCY:
            case STOP:
                break;
        }
    }
    

}
