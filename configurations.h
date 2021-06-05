#ifndef CONFIGURATIONS_H
#define CONFIGURATIONS_H


/*******************Configurables***************************/
#define NUMBER_OF_CELLS_PER_MODULE 10
#define NUMBER_OF_MODULES_PER_BMS   9
#define NTC_PER_MODULE 5


#define MAX_CELL_VOLTAGE 4.2
#define MIN_CELL_VOLTAGE 3.2

#define MAX_CELL_TEMPERATURE 50

#define NUMBER_OF_SLAVES NUMBER_OF_MODULES_PER_BMS




/*******************Defined by configurables****************/
#define NUMBER_OF_NTC_PER_BMS (NUMBER_OF_MODULES_PER_BMS*NTC_PER_MODULE)
#define NUMBER_OF_CELLS_PER_BMS (NUMBER_OF_CELLS_PER_MODULE*NUMBER_OF_MODULES_PER_BMS)


//todo
//Recalculate cell voltage to the corresponding bits as 16bit value
#define MAX_CELL_VOLTAGE_UINT16 (MAX_CELL_VOLTAGE)
#define MIN_CELL_VOLTAGE_UINT16 (MIN_CELL_VOLTAGE)

#define MAX_CELL_TEMPERATURE_UINT16 (MAX_CELL_TEMPERATURE)



#endif