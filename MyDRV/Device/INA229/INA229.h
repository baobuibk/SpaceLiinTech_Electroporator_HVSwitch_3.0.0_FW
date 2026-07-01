/*
 * INA229.h
 *
 *  Created on: Jun 25, 2026
 *      Author: PV
 */

#ifndef DEVICE_INA229_INA229_H_
#define DEVICE_INA229_INA229_H_

#include "spi.h"
#include <stdint.h>

#include "spi.h"
#include <stdint.h>

/* INA229 Register Map [cite: 2606] */
#define INA229_REG_CONFIG          0x00
#define INA229_REG_ADC_CONFIG      0x01
#define INA229_REG_SHUNT_CAL       0x02
#define INA229_REG_SHUNT_TEMPCO    0x03
#define INA229_REG_VSHUNT          0x04
#define INA229_REG_VBUS            0x05
#define INA229_REG_DIETEMP         0x06
#define INA229_REG_CURRENT         0x07
#define INA229_REG_POWER           0x08
#define INA229_REG_ENERGY          0x09
#define INA229_REG_CHARGE          0x0A
#define INA229_REG_DIAG_ALRT       0x0B
#define INA229_REG_SOVL            0x0C
#define INA229_REG_SUVL            0x0D
#define INA229_REG_BOVL            0x0E
#define INA229_REG_BUVL            0x0F
#define INA229_REG_TEMP_LIMIT      0x10
#define INA229_REG_PWR_LIMIT       0x11
#define INA229_REG_MANUFACTURER_ID 0x3E
#define INA229_REG_DEVICE_ID       0x3F



#define INA229_VBUS_LSB            (195.3125e-6)  /* 195.3125 uV/LSB [cite: 2797] */
#define INA229_VSHUNT_LSB          (312.5e-9)     /* 312.5 nV/LSB (assuming ADCRANGE=0) [cite: 2797] */
#define INA229_TEMP_LSB            (7.8125e-3)    /* 7.8125 mC/LSB [cite: 2797] */



typedef struct {
    spi_driver_t* spi;          /* Pointer to the SPI HAL driver */
    double        current_lsb;  /* Calibrated Current LSB in Amperes [cite: 2809] */
} ina229_t;


void INA229_Init(ina229_t* dev, spi_driver_t* spi_handle);
void INA229_Calibrate(ina229_t* dev, double r_shunt_ohms, double max_current_amps);


void     INA229_WriteReg16(ina229_t* dev, uint8_t reg, uint16_t val);
uint16_t INA229_ReadReg16(ina229_t* dev, uint8_t reg);
uint32_t INA229_ReadReg24(ina229_t* dev, uint8_t reg);
uint64_t INA229_ReadReg40(ina229_t* dev, uint8_t reg);

void INA229_Set_ShuntOverVoltage(ina229_t* dev, double r_shunt_ohms, double current_amps);
void INA229_Set_ShuntUnderVoltage(ina229_t* dev, double r_shunt_ohms, double current_amps);
void INA229_Set_BusOverVoltage(ina229_t* dev, double voltage_raw);
void INA229_Set_BusUnderVoltage(ina229_t* dev, double voltage_raw);


double INA229_GetBusVoltage(ina229_t* dev);
double INA229_GetShuntVoltage(ina229_t* dev);
double INA229_GetTemperature(ina229_t* dev);
double INA229_GetCurrent(ina229_t* dev);
double INA229_GetPower(ina229_t* dev);
double INA229_GetEnergy(ina229_t* dev);
double INA229_GetCharge(ina229_t* dev);




#endif /* DEVICE_INA229_INA229_H_ */
