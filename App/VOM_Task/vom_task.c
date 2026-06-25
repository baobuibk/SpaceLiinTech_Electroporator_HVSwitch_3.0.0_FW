/*
 * vom_task.c
 *
 *  Created on: Jun 25, 2026
 *      Author: PV
 */

#include "vom_task.h"
#include "spi.h"
#include "ina229.h"
#include "board.h"

#include <stdint.h>
#include <stdbool.h>



uint8_t vom_spi_tx_buffer[SPI_BUFFER_SIZE];
uint8_t vom_spi_rx_buffer[SPI_BUFFER_SIZE];

spi_driver_t    vom_spi;
ina229_t        vom_ina229_dev;
vom_data_t      vom_data;



void VOM_Task_Init(void){

    SPI_Init(   &vom_spi, VOM_SPI_HANDLE, VOM_SPI_IRQ, 
                vom_spi_tx_buffer, SPI_BUFFER_SIZE, 
                vom_spi_rx_buffer, SPI_BUFFER_SIZE,
                VOM_SPI_CS_PORT, VOM_SPI_CS_PIN);

    vom_data.bus_voltage = 0.0;
    vom_data.shunt_voltage = 0.0;
    vom_data.temperature = 0.0;
    vom_data.current = 0.0;
    vom_data.power = 0.0;
    vom_data.energy = 0.0;
    vom_data.charge = 0.0;
                
    INA229_Init(&vom_ina229_dev, &vom_spi);
    
    INA229_WriteReg16(&vom_ina229_dev, INA229_REG_CONFIG, 0x8000);
  
  
    INA229_Calibrate(&vom_ina229_dev, SHUNT_RESISTOR_OHMS, MAX_EXPECTED_AMPS);

}


void VOM_Task (void*){
    vom_data.bus_voltage = INA229_GetBusVoltage(&vom_ina229_dev);
    vom_data.shunt_voltage = INA229_GetShuntVoltage(&vom_ina229_dev);
    vom_data.temperature = INA229_GetTemperature(&vom_ina229_dev);
    vom_data.current = INA229_GetCurrent(&vom_ina229_dev);
    vom_data.power = INA229_GetPower(&vom_ina229_dev);
    vom_data.energy = INA229_GetEnergy(&vom_ina229_dev);
    vom_data.charge = INA229_GetCharge(&vom_ina229_dev);
}




