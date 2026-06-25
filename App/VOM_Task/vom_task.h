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
   
} vom_data_t;


extern spi_driver_t    vom_spi;
extern ina229_t        vom_ina229_dev;
extern vom_data_t      vom_data;

void VOM_Task_Init(void);

void VOM_Task (void*);

#endif /* VOM_TASK_VOM_TASK_H_ */
