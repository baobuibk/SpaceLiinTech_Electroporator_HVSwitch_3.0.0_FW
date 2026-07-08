/*
 * vom_task.h
 *
 *  Created on: Jun 25, 2026
 *      Author: PV
 */

#ifndef VOM_TASK_VOM_TASK_H_
#define VOM_TASK_VOM_TASK_H_


#include "spi.h"
#include "INA229.h"


#define SPI_BUFFER_SIZE 10

#define SHUNT_RESISTOR_OHMS 0.014f
#define MAX_EXPECTED_AMPS   10.0f

typedef struct {

    double bus_voltage;
    double shunt_voltage;
    double temperature;
    double current;
    double power;
    double energy;
    double charge;

    float current_limit; // Current limit in Amperes
    bool  over_voltage_flag; // Over-voltage flag
    bool  over_current_flag; // Over-current flag
   
} vom_data_t;

typedef enum {
    
    VOM_SAMPLING_VOLTAGE = 0,
    VOM_SAMPLING_CURRENT,

} vom_sampling_state_t;


extern spi_driver_t    vom_spi;
extern ina229_t        vom_ina229_dev;
extern vom_data_t      vom_data;

void VOM_Init(void);

void VOM_Set_Current_Limit(float current_limit_amps);
float VOM_Get_Current_Limit(void);
bool VOM_Get_OVC_Flag(void);
void VOM_Reset_OVC_Flag(void);

void VOM_Timbase_Sampling_Init(void);
void VOM_Sampling_Start(void);
void VOM_Sampling_Stop(void);
void VOM_Sampling_SetFrequency(uint32_t sampling_freq_hz);

void VOM_Task (void*);

void VOM_DMA_Receive_Complete_ISR(void);
void VOM_TIM_Sampling_Trigger_ISR(void);
void VOM_OVC_Handle_ISR(void);

#endif /* VOM_TASK_VOM_TASK_H_ */
