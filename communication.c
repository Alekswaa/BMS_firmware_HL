#include "function_defs.h"
#include "configurations.h"
#include "CAN_msg.h"

#include <stdint.h>

//Memory locations for newest measurements  
float* voltage_memory, *temperature_memory, *current_memory;

CAN_voltage_msg_t* voltage_msg1;
CAN_voltage_msg_t* voltage_msg2;
CAN_voltage_msg_t* voltage_msg3;

CAN_temperature_msg_t* temperature_msg1;
CAN_temperature_msg_t* temperature_msg2;
CAN_temperature_msg_t* temperature_msg3;

CAN_current_msg_t* current_msg;

void init_CAN_msg(){

    voltage_msg1 = malloc(sizeof(float)*30);
    voltage_msg2 = malloc(sizeof(float)*30);
    voltage_msg3 = malloc(sizeof(float)*30);

    temperature_msg1 = malloc(sizeof(float)*23);
    temperature_msg2 = malloc(sizeof(float)*23);

    current_msg = malloc(sizeof(float));

}

void send_CAN_messages(){
    
    /*Move voltage data to struct msg*/
    memcpy(voltage_msg1->voltage, voltage_memory   , sizeof(uint16_t)*30);
    memcpy(voltage_msg2->voltage, voltage_memory+30, sizeof(uint16_t)*30);
    memcpy(voltage_msg3->voltage, voltage_memory+60, sizeof(uint16_t)*30);

    //send_can_message(); 

    /*Move temperature data to struct msg*/
    memcpy(temperature_msg1->temperature, temperature_memory   , sizeof(uint16_t)*23);
    memcpy(temperature_msg2->temperature, temperature_memory+23, sizeof(uint16_t)*23);

    //send_can_message();

    /*Move current to msg struct */
    memcpy(current_msg->current, current_memory, sizeof(uint16_t));

    //send_can_message();

}