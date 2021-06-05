#include <stdint.h>

/*Type definitions for CAN messages*/

typedef struct can_voltage{
    uint16_t voltage[30];

} CAN_voltage_msg_t;

typedef struct can_current{
    uint16_t current;

} CAN_current_msg_t;

typedef struct can_temperature{
    uint16_t temperature[23];

} CAN_temperature_msg_t;