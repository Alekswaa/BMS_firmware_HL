#ifndef FUNCTION_DEFS_H
#define FUNCTION_DEFS_H

#define NO_ERROR 0 
#define ERROR 1

//BMS states
enum bms_fsm_states{
    INIT_BMS, //Initial state when turn on/on restart
    INIT_LV,  
    INIT_HV,
    ACQUIRE,
    SAFETY_CHECK,
    SEND_CAN_MESSAGES,
    EMERGENCY,
    STOP
};

enum acquisition_states{
	INIT_MEAS_VOLTAGE,
	INIT_READ_VOLTAGE,
	INIT_MEAS_TEMPERATURE,
	INIT_READ_TEMPERATURE
};

typedef enum init_command {
    VOLTAGE,
    TEMPERATURE
}init_command_t;




void init_SPI();
void start_acq();

int check_cell_parameters();
void init_firmware();
void init_peripherals();

void print_voltage_reg(uint16_t*);
void spi_disable_cs();
void spi_enable_cs();
void spi_read_volt_reg_ltc6811(uint16_t, uint8_t*);

void SPI_voltage_cb();
void SPI_init();
void init_spi();
void SPI_init_meas();
void init_ltc_meas(init_command_t);

void check_values(uint16_t*, int);
void init_ltc6811_conf();

void init_memory_buffers();


void SPI_CS_disable_free();



#endif
