#include "configurations.h"
#include "function_defs.h"
#include "driver_init.h" 
#include <stdint.h>
#include "testing.h"
#include "function_defs.h"

#define ADC_12BIT 4096.0

extern state g_bms_state;
extern uint16_t* regCurrent;

/* Buffer for ADC values */
uint8_t buffer[32];


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
       a heartbeat to be sent */
}


//ISOSPI-------------------------------------------------------------

/* Rx bufs saved in array for easy access */
extern uint16_t* rxBufs[];
extern uint16_t read_cmds[];

/* Number of voltage registers read */
int nr_regs_read = 0;

/* Callback interrupt handler for voltage transfer has completed(rx should also be done at this point)*/
extern uint8_t* message;
extern enum BMS_STATE state;

/**
 * @brief Callback function that is run when the async spi has finished a transfer(when this cb is activated).
 * The first four times is for the voltage registers, the consequent two are for the temperature GPIOs. 
 * Initiates new transfers and keeps count of how many registers have been read. 
 */
static int dec_one = 0;
void SPI_voltage_cb()
{
	free(message);
	spi_disable_cs();

	nr_regs_read++;
	if(nr_regs_read == 4){
		state = ACQUIRE;
		dec_one = 1;
		return;
	}else if(nr_regs_read > 6){
		dec_one = 0;
		return;
	}

	spi_enable_cs();
	spi_read_volt_reg_ltc6811(read_cmds[nr_regs_read - dec_one], (uint8_t*)rxBufs[nr_regs_read - dec_one]);
	return;
}

bool init_done = false;
/**
 * @brief Used for writing the configuration to the LTC6811
 */
extern uint8_t* init_msg;
void SPI_init(){
	spi_disable_cs();
	free(init_msg);
}

bool init_meas_done;
extern uint8_t* msg;
void SPI_init_meas(){
	free(msg);
	init_meas_done = true;
	spi_disable_cs();
}








//-------------------------------------------------------------