
## Peripherals used
* CAN x2 - High and low pri
* SPI for isoSPI
* SPI for ADC for current measurement
* Internal ADC for CCC
* I/Os for RCU


## Finite state machine in main
1. Init
    * Low voltage
    * Send message confirming LW
    * High voltage
    * Measure battery voltages and temperatures for initial check
    * Perform initial SoC
    * Send message confirming parameters and HW
2. Init voltage/temperature measurement
3. Acquire data from registers
4. Data conversion and parameter checking
    * Convert ADC values into floats
    * Place in buffer to create a contiguous flow of cells 1-10, instead of cells 1-2-3, 8-9-10,.... 
4. Communication
    * Send voltages
    * Send temperatures
    * Send current
    * Send state og SoC
5. SoC
    * Perform KF algorithm
6. Emergency/Stop
    * Something wrong -> Shut down
    * Run is done     -> Shut down

### Timer
Most of the time will be spent in the KF algorithm. A timer is used to send an irq periodically to change the state from the KF to acquisition. When the acquisition is in a waiting period, like after the init, the MCU wil go back to the KF. Once the acq waiting period is finished, the KF will finish its current calculation and go back to the acquisition state to perform the acquisition. We are using an asynchronous driver that configures the SPI peripheral and is free to continue whatever until the SPI triggers an interrupt. Once the interrupt triggers the SPI will be configured to acquire the next register of data from the slaves and teh data will be moved to memory where the values of the parameters are compared to a set max and min, if the acquired values are outside of this range, the BMS will move to the EMERGENCY state where the batteries are disconnected and the system is halted.
    

### Interleave acquisition and cell safety
Once the first data register has been read by the SPI peripheral the next will be initiated. While that is working we can spen the time checking the cell parameters for the first register.For a single slave, this will not save an incredible amount of time, but for 9 slaves this will be a better approach. 


## BMS I/Os

| Index | I/O Pin | Name                               |
| ----- | ------- | ---------------------------------- |
| 0     | PC31    | BBDR1                              |
| 1     | PC30    | BSOL1                              |
| 2     | PC29    | BBDR2                              |
| 3     | PC20    | LV+ Enable                         |
| 4     | PC15    | BLSOL1                             |
| 5     | PC13    | TBDR1                              |
| 6     | PC3     | ?                                  |
| 7     | PC2     | ?                                  |
| 8     | PC1     | ?                                  |
| 9     | PC0     | Precharge                          |
| 10    | PA31    | ADC_CONVST (**Conversion start?**) |
| 11    | PA17    | AD6(CH3)                           |
| 12    | PA18    | AD7(CH3)                           |
| 13    | PA10    | BUSCNV TX                          |
| 14    | PA9     | BUSCNV RX                          |
| 15    | PD22    | SPI ADC MISO                       |
| 16    | PD21    | SPI ADC SPCK                       |
| 17    | PE3     | BLSOL2                             |
| 18    | PE2     | DISCHARGE2                         |
| 19    | PE1     | DISCHARGE1                         |
| 20    | PB1     | BSOL2                              |
| 21    | PB0     | TBDR2                              |
| 22    | PD24    | ?                                  |