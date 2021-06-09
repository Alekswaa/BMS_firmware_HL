#include <stdlib.h>
#include <string.h>
#include "atmel_start.h"
#include "utils.h"
#include "PEC.h"
#include "conf.h"
#include "function_declarations.h"


uint16_t PEC_calculate(uint8_t*, int);

void spi_enable_cs(){
	delay_us(10); // NB! Check if this is needed
	gpio_set_pin_level(NPCS0, false);
}

void spi_disable_cs(){
	gpio_set_pin_level(NPCS0, true);
}

uint8_t* message;
void spi_read_volt_reg_ltc6811(uint16_t command, uint8_t* receive_values)
{
	uint16_t PEC;
	const int num_bytes = 4 + (sizeof(uint16_t)*3 + 2)*NUMBER_OF_SLAVES; // cmd + PEC + (2bytes per voltage * 3voltages per reg + 2bytes for PEC) *NUMBER_OF_SLAVES
    // Enable SPI and register voltage read callback function
    spi_m_async_register_callback(&SPI_0, SPI_M_ASYNC_CB_XFER, (FUNC_PTR)SPI_voltage_cb);
	spi_m_async_enable(&SPI_0);

	

    // Init message transmit buffer
    message = malloc(sizeof(uint8_t) *  num_bytes);
    memset(message, 0xFF, num_bytes); // Check if these should be high or low;
    
    // Set command and PEC in the msg register
    message[0] = command >> 8;
	message[1] = command & 0xFF;
    PEC = PEC_calculate(message, 2);
    message[2] = PEC >> 8;
    message[3] = PEC & 0xFF;
	
    // Transfer data, SPI receive buffer should be in FIFO style
	spi_m_async_transfer(&SPI_0, message, receive_values, num_bytes);
}

void wake_up_ltc(){
	spi_enable_cs();
	spi_disable_cs();
	delay_us(400);
}
uint8_t* init_msg;
void init_ltc6811_conf(){
	uint16_t PEC;
	const int num_bytes = 4 + 6 + 2; // command + PEC + register conf + PEC

	uint8_t reg_cfga[6] = {0};
	reg_cfga[0] = (0xF8 | REFON | ADCOPT_OFF); //Register for enabling the reference and setting fs
    
	// Enable SPI and register voltage read callback function
    spi_m_async_register_callback(&SPI_0, SPI_M_ASYNC_CB_XFER, (FUNC_PTR)SPI_init);
	spi_m_async_enable(&SPI_0);

    // Init message transmit buffer, free in interrupt
    init_msg = malloc(sizeof(uint8_t) * num_bytes);

	//Set command and its PEC
    init_msg[0] = WRCFGA >> 8;
	init_msg[1] = WRCFGA & 0xFF;
    PEC = PEC_calculate(init_msg, 2);
    init_msg[2] = PEC >> 8;
    init_msg[3] = PEC & 0xFF;
	
	// Copy  configuration and its PEC into message
	memcpy(init_msg+4, reg_cfga, 6);
	PEC = PEC_calculate(reg_cfga, 6);
	init_msg[10] = PEC >> 8;
	init_msg[11] = PEC & 0xFF;
	
	
	wake_up_ltc();
    // Transfer data
	spi_enable_cs();
	spi_m_async_transfer(&SPI_0, init_msg, NULL, num_bytes);
	
}
uint8_t* msg;
void init_ltc_meas(init_command_t cmd)
{
	uint16_t PEC;
	uint16_t command;
	spi_m_async_disable(&SPI_0);
	switch(cmd){
		case VOLTAGE:
			command = ADCV | FOURTY_TWO_kHz | DISCHARGE_NOT_PERMITTED | ALL_CELLS;
			break;
		case TEMPERATURE:
			command = ADAX | SEVEN_kHz | ALL_CELLS; //The last value is the voltage reference that should be 3V
			break;
		default:
			printf("ERROR: WRONG INIT COMMAND FOR LTC, assuming voltage command");
			command = ADCV | FOURTY_TWO_kHz | DISCHARGE_NOT_PERMITTED | ALL_CELLS;
			break;
	}
	spi_m_async_register_callback(&SPI_0, SPI_M_ASYNC_CB_XFER, (FUNC_PTR)SPI_init_meas);
   
	spi_m_async_enable(&SPI_0);

    // Init message transmit buffer, free in interrupt callback
	msg = malloc(4);
	
	//Set command and its PEC
    msg[0] = command >> 8;
	msg[1] = command & 0xFF;
    PEC = PEC_calculate((uint8_t*)&command, 2);
    msg[2] = PEC >> 8;
    msg[3] = PEC & 0xFF;
	
	wake_up_ltc();
    // Transfer data
	spi_enable_cs();
	spi_m_async_transfer(&SPI_0, msg, NULL, 4);
	
}

/**
 * \brief Calculate PEC for message
 * \param[in] data Pointer to char array containing PEC
 * \param[in] len  Number of bytes in the PEC
 * @returns the 2 PEC bytes
 */
uint16_t PEC_calculate(uint8_t *data , int len){
	
    unsigned int remainder, address;
    remainder = 16; /* PEC seed */
    int i;
    for (i = 0; i < len; i++){
        address = ((remainder >> 7) ^ data[i]) & 0xff; /*calculate PEC table address*/
        remainder = (remainder << 8 ) ^ crc15Table[address];  //lookup table found in header
    }
    //return remainder * 2;;
    return remainder << 1; /*The CRC15 has a 0 in the LSB so the final value must be multiplied by 2*/
}

int PEC_verify(unsigned char *data, unsigned int n, unsigned int PEC){

	if(PEC_calculate(data, n) == PEC){
		return 0;
	}
	return -1;
}	