/*
 * INA229.h
 *
 * Created on: Jun 25, 2026
 * Author: PV
 */

#ifndef DEVICE_INA229_INA229_H_
#define DEVICE_INA229_INA229_H_

#include "spi.h"
#include "stm32f4xx_ll_gpio.h"
#include <stdint.h>
#include <stdbool.h>

/* INA229 Register Map */
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

/* INA229 Register Data Sizes */
#define INA229_REG_CONFIG_SIZE          2U
#define INA229_REG_ADC_CONFIG_SIZE      2U
#define INA229_REG_SHUNT_CAL_SIZE       2U
#define INA229_REG_SHUNT_TEMPCO_SIZE    2U
#define INA229_REG_VSHUNT_SIZE          3U
#define INA229_REG_VBUS_SIZE            3U
#define INA229_REG_DIETEMP_SIZE         2U
#define INA229_REG_CURRENT_SIZE         3U
#define INA229_REG_POWER_SIZE           3U
#define INA229_REG_ENERGY_SIZE          5U
#define INA229_REG_CHARGE_SIZE          5U
#define INA229_REG_DIAG_ALRT_SIZE       2U
#define INA229_REG_SOVL_SIZE            2U
#define INA229_REG_SUVL_SIZE            2U
#define INA229_REG_BOVL_SIZE            2U
#define INA229_REG_BUVL_SIZE            2U

#define INA229_VBUS_LSB            (195.3125e-6)  /* 195.3125 uV/LSB */
#define INA229_VSHUNT_LSB          (312.5e-9)     /* 312.5 nV/LSB (ADCRANGE=0) */
#define INA229_TEMP_LSB            (7.8125e-3)    /* 7.8125 mC/LSB */

#define INA229_SOVL_LSB_RANGE0   5.0e-6      // ADCRANGE = 0: 5 µV/LSB
#define INA229_SOVL_LSB_RANGE1   1.25e-6     // ADCRANGE = 1: 1.25 µV/LSB

typedef struct {
    spi_driver_t* spi;          
    GPIO_TypeDef* cs_port;      
    uint32_t      cs_pin;
    
    double        current_lsb;  /* Calibrated Current LSB in Amperes */

    uint8_t       dma_tx_buf[8]; 
    uint8_t       dma_rx_buf[8];
} ina229_t;

void INA229_CS_Low(ina229_t* dev);
void INA229_CS_High(ina229_t* dev);

void INA229_Init(ina229_t* dev) ;
void INA229_Calibrate(ina229_t* dev, double r_shunt_ohms, double max_current_amps);

void INA229_WriteReg16(ina229_t* dev, uint8_t reg, uint16_t val);

void INA229_Set_ShuntOverVoltage(ina229_t* dev, double r_shunt_ohms, double current_amps);
void INA229_Set_ShuntUnderVoltage(ina229_t* dev, double r_shunt_ohms, double current_amps);
void INA229_Set_BusOverVoltage(ina229_t* dev, double voltage_raw);
void INA229_Set_BusUnderVoltage(ina229_t* dev, double voltage_raw);
void INA229_Clear_AlertFlags(ina229_t* dev);


double INA229_GetBusVoltage(ina229_t* dev);
double INA229_GetShuntVoltage(ina229_t* dev);
double INA229_GetTemperature(ina229_t* dev);
double INA229_GetCurrent(ina229_t* dev);
double INA229_GetPower(ina229_t* dev);
double INA229_GetEnergy(ina229_t* dev);
double INA229_GetCharge(ina229_t* dev);
bool INA229_Get_OverCurrent_Flag(ina229_t* dev) ;
uint16_t INA229_Get_All_AlertFlags(ina229_t* dev);
double INA229_Get_ShuntOverVoltage(ina229_t* dev);

bool   INA229_Start_ReadReg_DMA(ina229_t* dev, uint8_t reg, uint8_t data_size);

double INA229_Parse_BusVoltage_DMA(ina229_t* dev);
double INA229_Parse_ShuntVoltage_DMA(ina229_t* dev);
double INA229_Parse_Current_DMA(ina229_t* dev);
double INA229_Parse_Power_DMA(ina229_t* dev);

#endif /* DEVICE_INA229_INA229_H_ */
