#include "configurations.h"
#include "function_defs.h"
#include "testing.h"
extern state bms_state;

// Timer interrupt
void irq_handler_battery_acquisition(){
    /* Battery acquisition has finished, 
       start checking that values are within 
       acceptable ranges
    */
   /*Save KF current state.
    Potentially wait for the current 
    calculation to finish*/
    bms_state = CELL_SAFETY;
}


void irq_handler_heartbeat()
{
    /* VCU heartbeat.
       Timer that periodically triggers
       a heartbeat to be sent*/

}