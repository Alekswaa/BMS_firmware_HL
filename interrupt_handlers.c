#include "configurations.h"
#include "function_defs.h"
#include "driver_init.h" 
#include <stdint.h>
#include "testing.h"


#define ADC_12BIT 4096.0

extern state g_bms_state;
extern uint16_t* current_memory;

/* Buffer for ADC values */
uint8_t buffer[32];

// Timer interrupt
void irq_handler_battery_acquisition(){
    /* Battery acquisition has finished, 
       start checking that values are within 
       acceptable ranges
    */
   /*Save KF current state.
    Potentially wait for the current 
    calculation to finish*/
    g_bms_state = CELL_SAFETY;
}

/* ADC interrupt when measurement is finished */
/* This measurement should be for the CCC*/
static void ADC_CCC_voltage_check(const struct adc_async_descriptor *const descr, const uint8_t channel)
{
	
	adc_async_read_channel(&ADC_0, CONF_ADC_0_CHANNEL_4, buffer, 4);
    uint16_t value = (*(uint16_t*)buffer);
    double voltage = 3.3/ADC_12BIT * value;
    // printf("Voltage4 - %f\n", voltage);
		
}

/* Change to ACQUISITION state on an interval */
static void TIMER_0_change_state_cb(const struct timer_task *const timer_task)
{
    g_bms_state = ACQUISITION;
}

static void TIMER_0_heartbeat_cb(const struct timer_task *const timer_task)
{
     /* VCU heartbeat.
       Timer that periodically triggers
       a heartbeat to be sent*/
}