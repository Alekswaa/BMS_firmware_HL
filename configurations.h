#ifndef CONFIGURATIONS_H
#define CONFIGURATIONS_H


/*******************Configurables***************************/

#define NUMBER_OF_SLAVES 1
#define VOLTAGE_MEM_SIZE sizeof(float) * 3 * 4 * NUMBER_OF_SLAVES //one float pr voltage * nr of voltage pr register * nr of registers * number of slaves
#define TEMPERATURE_MEM_SIZE sizeof(float) * 5 * NUMBER_OF_SLAVES //one float pr temperature * nr of temperatures pr register * nr of registers * number of slaves
#define VOLT_BUFFER_SIZE (2 * 3 + 2) * NUMBER_OF_SLAVES
#define TEMP_BUFFER_SIZE (2 * 3 + 2) * NUMBER_OF_SLAVES
#define MAX_VOLTAGE	4.2 //Voltage
#define MIN_VOLTAGE 3.2 //Voltage
#define MAX_TEMPERATURE 50 //degrees celsius
#define MIN_TEMPERATURE 10 //degrees celsius






#endif