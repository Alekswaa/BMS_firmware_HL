#include <stdint.h>

#include "function_defs.h"

extern state bms_state;
extern uint16_t current_memory;
void start_acq()
{
    // Should current be measured before ltc initiate?
    // Decrease kf time or decrease system time?
    initiate_ADC_meas();
    //send message to ltc for starting measuring
    //Initiate the SPI peripheral to send the data to memory? DMA?
    initiate_ltc_measuring();
    bms_state = SOC;
}

void initiate_ltc_measuring(){
    wake_up_ltc();
    bms_init_ltc6811();
    bms_start_meas();
}

void initiate_ADC_meas(){
    //ADC current measurement driver
    //ADC can just continously gather new measurements
    //and send to memory location (DMA)/ interrupt transfer
    adc_async_start_conversion(&ADC_0);
}

