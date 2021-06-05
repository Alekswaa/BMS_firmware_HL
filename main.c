//System libraries
#include <stdbool.h>
#include <stdint.h>
#include <stdbool.h>

//Atmel libraries
#include "driver_init.h"

//Custom libraries
#include "function_defs.h"
#include "configurations.h"

// Global parameters
/*******************************************/

#ifdef TESTING
    state main_state = INIT_TESTING_STATE;
#else
    //Starting BMS in the INIT state
    state g_bms_state = INIT;
#endif

/* Function declarations */
int check_cell_parameters();
void init_firmware();

float* voltage_memory, *temperature_memory, *current_memory;

void main()
{
    //FSM that runs as long as the BMS runs
    while(true){
        switch(g_bms_state){
            case INIT:
                init_peripherals();
                init_firmware();
                
                /* Initalize the LTC6811 slaves */
                bms_init_ltc6811();

                //TODO #2
                init_low_voltage();
                init_high_voltage();
                break;

            case ACQUISITION:
                start_acq();
                g_bms_state = SOC;
                break;

            case CELL_SAFETY:
                if (check_cell_parameters() == NO_ERROR) {
                    g_bms_state = SEND_DATA;
                } else {
                    g_bms_state = EMERGENCY;
                }
                break;

            case SEND_DATA:
                send_CAN_messages();
                //Interrupt can modify state, we shouldn't modify it after that
                if(g_bms_state == SEND_DATA)
                    g_bms_state = SOC;
                break;

            case SOC:
                kalman_filter();
                break;

            //Emergency and stop does the same stuff (isj)
            case EMERGENCY:
            case STOP:
                break; //Literally
        }
    }
}


void init_firmware(){
    //LTC memory for voltage and temperature
    voltage_memory = malloc(sizeof(uint16_t) * NUMBER_OF_CELLS_PER_BMS);
    temperature_memory = malloc(sizeof(uint16_t) * NUMBER_OF_NTC_PER_BMS);

    //ADC memory for current
    current_memory = malloc(sizeof(uint16_t));

    /* Init CAN message memory */
    init_CAN_msg();

}

void init_peripherals(){

    /* ADC for CCC*/ //TODO #7
    adc_async_register_callback(&ADC_0, CONF_ADC_0_CHANNEL_4, ADC_ASYNC_CONVERT_CB, convert_cb_ADC_0_CHANNEL_4);
	adc_async_register_callback(&ADC_0, CONF_ADC_0_CHANNEL_5, ADC_ASYNC_CONVERT_CB, convert_cb_ADC_0_CHANNEL_5);
    adc_async_enable_channel(&ADC_0, CONF_ADC_0_CHANNEL_4);
	adc_async_enable_channel(&ADC_0, CONF_ADC_0_CHANNEL_5);

    /* SPI0 for  isoSPI*/


    /* SPI1 for current ADC */


    /* CAN0 for high Pri msgs */


    /* CAN1 for high Pri msgs */


}


