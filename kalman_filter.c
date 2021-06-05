#include "function_defs.h"
#include "configurations.h"

/* Global BMS state */
extern state g_bms_state;

void kalman_filter(){
    static int index = 0;
    while(g_bms_state == SOC){
        /* Do kalman calculations */

        
        if(index >= NUMBER_OF_CELLS_PER_BMS){
            index = 0;
            //TODO #1
        }else{
            index += 1;
        }
    }

}