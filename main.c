//System libraries
#include <stdbool.h>
#include <stdint.h>
#include <stdbool.h>

//Atmel libraries
#include "driver_init.h"

//Custom libraries
#include "function_defs.h"
#include "configurations.h"

#ifdef TESTING
    enum BMS_STATE g_bms_state = INIT_TESTING_STATE;
#else
    //Starting BMS in the INIT state
    enum BMS_STATE g_bms_state = ACQUIRE;
#endif


/* Save newest received voltage, temperature, current data in these registers */
uint16_t *regVoltA, *regVoltB, *regVoltC, *regVoltD;
uint16_t *regTempA, *regTempB;
uint16_t *regCurrent;

/* Contains pointers to the registers */
uint16_t* rxBufs[6];

/* Main memory for data */
float* voltage_memory, *temperature_memory, *current_memory;

uint16_t read_cmds[] = {RDCVA, RDCVB, RDCVC, RDCVD, RDAUXA, RDAUXB};

//Init LTC6811 measurements bools
bool start_init = true;
extern bool init_meas_done;
extern int nr_regs_read;

void main(){
    // State that gives which value we acquire from the LTC6811
    enum acquisition_states acq_state = INIT_MEAS;
    
    //See FSM diagram in README
    switch(g_bms_state){
        case INIT_BMS:
            /* Initialize all peripherals */
            /* Initialize memory */
            init_memory_buffers();	
            init_peripherals();
            init_ltc6811_conf();
            delay_ms(2); //LTC needs a bit of time to turn on
            g_fsm_state = ACQUIRE;
            break;
        case INIT_LV:
            g_fsm_state = ACQUIRE;
            break;
        case INIT_HV:
            g_fsm_state = ACQUIRE;
            break;
        case ACQUIRE:
				if(acq_state == INIT_MEAS_VOLTAGE)
				{
					if(start_init){
						init_ltc_meas(VOLTAGE);
						start_init = false;
						delay_us(2200); //Potentially start timer
					}
					if(init_meas_done){
						acq_state = INIT_READ_VOLTAGE;
						init_meas_done = false;
						start_init = true;
					}
				}
				else if(acq_state == INIT_READ_VOLTAGE)
				{
					spi_enable_cs();
					spi_read_volt_reg_ltc6811(read_cmds[0], (uint8_t*)rxBufs[0]);
					acq_state = INIT_MEAS_TEMPERATURE;
					state = SAFETY_CHECK;
				}
				else if(acq_state == INIT_MEAS_TEMPERATURE)
				{
					if(start_init){
						init_ltc_meas(TEMPERATURE);
						start_init = false;
						delay_us(2300); //Potentially start timer
					}
					if(init_meas_done){
						init_meas_done = false;
						start_init = true;
						acq_state = INIT_READ_TEMPERATURE;
					}
				}
				else if(acq_state == INIT_READ_TEMPERATURE)
				{
					spi_enable_cs();
					spi_read_volt_reg_ltc6811(read_cmds[4], (uint8_t*)rxBufs[4]);
					acq_state = INIT_MEAS_VOLTAGE;
					state = SAFETY_CHECK;
				}
				break;
			case SAFETY_CHECK:
				if(reg_check_idx < nr_regs_read && reg_check_idx < 6)
				{
					check_values(rxBufs[reg_check_idx], reg_check_idx);
					reg_check_idx++;
				}
				else if(reg_check_idx == 6)
				{
					state = WAIT;
					nr_regs_read = 0;
					reg_check_idx = 0;
					is_next_meas_temperature = false;
					something = false;
				}
				break;
        case SEND_CAN_MESSAGES:
            g_fsm_state = ACQUIRE;
            break;
        case SOC:
                kalman_filter();
                break;
        case EMERGENCY:
        case STOP:
            break; //Literally

    }
}

/**
 * @brief Initate the memory buffers for received voltages and temperatures a
 * and a large buffer for storing a contiguous flow of values as floats. 
 * Contiguous mean
 * 
 */
void init_memory_buffers()
{
	/* Initialize voltage register memories */
	regVoltA = (uint16_t*)malloc(VOLT_BUFFER_SIZE); // 3 volt values of 16bit + 2PEC
	regVoltB = (uint16_t*)malloc(VOLT_BUFFER_SIZE);
	regVoltC = (uint16_t*)malloc(VOLT_BUFFER_SIZE);
	regVoltD = (uint16_t*)malloc(VOLT_BUFFER_SIZE);
	rxBufs[0] = regVoltA;
	rxBufs[1] = regVoltB;
	rxBufs[2] = regVoltC;
	rxBufs[3] = regVoltD;

	/* Initialize temperature register memories */
	regTempA = (uint16_t*)malloc(TEMP_BUFFER_SIZE); // 3 volt values of 16bit + 2PEC
	regTempB = (uint16_t*)malloc(TEMP_BUFFER_SIZE);
	rxBufs[4] = regTempA;
	rxBufs[5] = regTempB;

	regCurrent = (uint16_t*)malloc(sizeof(uint16_t));

	/* Initialize main memory registers*/
	voltage_memory =     (float*)malloc(VOLTAGE_MEM_SIZE);
	temperature_memory = (float*)malloc(TEMPERATURE_MEM_SIZE); //
	current_memory =     (float*)malloc(sizeof(float));

}

void check_values(uint16_t* buffer, int reg_idx)
{
	
	if(reg_idx < 4) //First 4 registers are for voltages. The consequent two are for temperatures
	{
		int shift = 0;
		for(int i = 0; i < VOLT_BUFFER_SIZE/2; i++){
			if(i % 4 == 3){
				shift++;
				continue;
			}

			float voltage = buffer[i+2] * 100e-6;
			voltage_memory[i + shift*8	 + reg_idx*3] = voltage; //Placing voltage to create a contiguous memory flow
			if( voltage < MAX_VOLTAGE || voltage < MIN_VOLTAGE){
				//printf("ERROR IN CELL!!\n");
			}	
		}
	}
	else 
	{
		int shift = 0;
		float value;
		for(int i = 0; i < TEMP_BUFFER_SIZE/2; i++)
		{
			if(i % 4 == 3){ //PEC
				shift++;
				continue;
			}
			if(i % 4 == 2 && reg_idx == 5){ //Reference value
				value = buffer[i] * 100e-6; // Reference Voltage
			}else{
				float Vtemp = buffer[i] * 100e-6;
				float Rtherm = Vtemp*10000.0/(3.0 - Vtemp);
				value = 3435.0/(logf(Rtherm/(10000.0*exp(-3435.0/298.15)))) - 273.15;
				if( value < MAX_TEMPERATURE || value < MIN_TEMPERATURE){
					//printf("ERROR IN CELL!!\n");
				}
			}
			temperature_memory[i + shift*3 - shift + (reg_idx-4)*3] = value; //Placing voltage to create a contiguous memory flow	
		}
	}
}


void init_peripherals(){

    /* ADC for CCC*/ //TODO #7
    adc_async_register_callback(&ADC_0, CONF_ADC_0_CHANNEL_4, ADC_ASYNC_CONVERT_CB, convert_cb_ADC_0_CHANNEL_4);
	adc_async_register_callback(&ADC_0, CONF_ADC_0_CHANNEL_5, ADC_ASYNC_CONVERT_CB, convert_cb_ADC_0_CHANNEL_5);
    adc_async_enable_channel(&ADC_0, CONF_ADC_0_CHANNEL_4);
	adc_async_enable_channel(&ADC_0, CONF_ADC_0_CHANNEL_5);

    /* SPI0 for  isoSPI*/
    spi_m_async_set_data_order(&SPI_0, SPI_DATA_ORDER_MSB_1ST);
	spi_m_async_set_char_size( &SPI_0, SPI_CHAR_SIZE_8); 
	spi_m_async_set_baudrate(  &SPI_0, 600000); //Min freq is 150MHz/255 = 588kHz

	spi_m_async_register_callback(&SPI_0, SPI_M_ASYNC_CB_XFER, (FUNC_PTR)SPI_voltage_cb);
	spi_m_async_enable(&SPI_0);

    /* SPI1 for current ADC */


    /* CAN0 for high Pri msgs */


    /* CAN1 for high Pri msgs */


}