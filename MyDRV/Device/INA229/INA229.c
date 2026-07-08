/**
 * @file ina229.c
 * @brief INA229 Application Layer Driver Implementation (Polling & DMA)
 */

#include "INA229.h"
#include <string.h>


// --- Helper Functions điều khiển chân CS ---
void INA229_CS_Low(ina229_t* dev) {
    LL_GPIO_ResetOutputPin(dev->cs_port, dev->cs_pin);
}

void INA229_CS_High(ina229_t* dev) {
    LL_GPIO_SetOutputPin(dev->cs_port, dev->cs_pin);
}


void INA229_WriteReg16(ina229_t* dev, uint8_t reg, uint16_t val) {
    uint8_t tx[3];
    tx[0] = (reg << 2) & ~0x01;  
    tx[1] = (uint8_t)(val >> 8);
    tx[2] = (uint8_t)(val & 0xFF);
    
    INA229_CS_Low(dev);
    SPI_Transmit_Polling(dev->spi, tx, 3);
    INA229_CS_High(dev);
}

static uint64_t INA229_ReadReg_Polling(ina229_t* dev, uint8_t reg, uint8_t data_size) {
    uint8_t tx[8] = {0};
    uint8_t rx[8] = {0};
    
    // Khởi tạo tx bằng 0xFF để tạo Dummy Clock cho việc nhận
    memset(tx, 0xFF, data_size + 1); 
    tx[0] = (reg << 2) | 0x01; // Opcode đọc
    
    INA229_CS_Low(dev);
    SPI_TransmitReceive_Polling(dev->spi, tx, rx, data_size + 1);
    INA229_CS_High(dev);
    
    uint64_t val = 0;
    for (int i = 1; i <= data_size; i++) {
        val = (val << 8) | rx[i];
    }
    return val;
}

/* ====================================================================
 * INITIALIZATION 
 * ==================================================================== */

void INA229_Init(ina229_t* dev) {
    SPI_Init(dev->spi);
    INA229_CS_High(dev); 
}

void INA229_Calibrate(ina229_t* dev, double r_shunt_ohms, double max_current_amps) {
    dev->current_lsb = max_current_amps / 524288.0;
    double shunt_cal_val = 13107.2e6 * dev->current_lsb * r_shunt_ohms;
    INA229_WriteReg16(dev, INA229_REG_SHUNT_CAL, (uint16_t)shunt_cal_val);
}

//void INA229_Set_ShuntOverVoltage(ina229_t* dev, double r_shunt_ohms, double current_amps) {
//    double voltage_v = current_amps * r_shunt_ohms;
//    int16_t limit_val = (int16_t)(voltage_v / INA229_VSHUNT_LSB);
//    INA229_WriteReg16(dev, INA229_REG_SOVL, (uint16_t)limit_val);
//}
//
//void INA229_Set_ShuntUnderVoltage(ina229_t* dev, double r_shunt_ohms, double current_amps) {
//    double voltage_v = current_amps * r_shunt_ohms;
//    int16_t limit_val = (int16_t)(voltage_v / INA229_VSHUNT_LSB);
//    INA229_WriteReg16(dev, INA229_REG_SUVL, (uint16_t)limit_val);
//}

void INA229_Set_ShuntOverVoltage(ina229_t* dev, double r_shunt_ohms,
                                  double current_amps)
{
    double voltage_v = current_amps * r_shunt_ohms;
    double lsb = INA229_SOVL_LSB_RANGE0;

    double raw = voltage_v / lsb;
    // clamp để tránh tràn int16_t
    if (raw > 32767.0)  raw = 32767.0;
    if (raw < -32768.0) raw = -32768.0;

    int16_t limit_val = (int16_t)raw;   // round, không truncate
    INA229_WriteReg16(dev, INA229_REG_SOVL, (uint16_t)limit_val);
}

void INA229_Set_ShuntUnderVoltage(ina229_t* dev, double r_shunt_ohms,
                                   double current_amps)
{
    double voltage_v = current_amps * r_shunt_ohms;
    double lsb = INA229_SOVL_LSB_RANGE0;

    double raw = voltage_v / lsb;
    if (raw > 32767.0)  raw = 32767.0;
    if (raw < -32768.0) raw = -32768.0;

    int16_t limit_val = (int16_t)raw;
    INA229_WriteReg16(dev, INA229_REG_SUVL, (uint16_t)limit_val);
}

void INA229_Set_BusOverVoltage(ina229_t* dev, double voltage_raw) {
    double voltage_v = voltage_raw * 0.03;
    int16_t limit_val = (int16_t)(voltage_v / INA229_VBUS_LSB);
    INA229_WriteReg16(dev, INA229_REG_BOVL, (uint16_t)limit_val);
}

void INA229_Set_BusUnderVoltage(ina229_t* dev, double voltage_raw) {
    double voltage_v = voltage_raw * 0.03;
    int16_t limit_val = (int16_t)(voltage_v / INA229_VBUS_LSB);
    INA229_WriteReg16(dev, INA229_REG_BUVL, (uint16_t)limit_val);
}

/* ====================================================================
 *  POLLING
 * ==================================================================== */

double INA229_Get_ShuntOverVoltage(ina229_t* dev) {
    // Đọc 2 byte từ thanh ghi SOVL
    int16_t val16 = (int16_t)INA229_ReadReg_Polling(dev, INA229_REG_SOVL, 2);

    // Tính toán lại giá trị điện áp (Volt) từ giá trị raw
    return (double)val16 * INA229_SOVL_LSB_RANGE0;
}

double INA229_GetBusVoltage(ina229_t* dev) {
    uint32_t val24 = (uint32_t)INA229_ReadReg_Polling(dev, INA229_REG_VBUS, 3);
    uint32_t val20 = val24 >> 4; 
    return (double)val20 * INA229_VBUS_LSB; 
}

double INA229_GetShuntVoltage(ina229_t* dev) {
    uint32_t val24 = (uint32_t)INA229_ReadReg_Polling(dev, INA229_REG_VSHUNT, 3);
    int32_t val_signed = ((int32_t)(val24 << 8)) >> 8; 
    int32_t val20 = val_signed >> 4;
    return (double)val20 * INA229_VSHUNT_LSB; 
}

double INA229_GetTemperature(ina229_t* dev) {
    int16_t val16 = (int16_t)INA229_ReadReg_Polling(dev, INA229_REG_DIETEMP, 2);
    return (double)val16 * INA229_TEMP_LSB;
}

double INA229_GetCurrent(ina229_t* dev) {
    uint32_t val24 = (uint32_t)INA229_ReadReg_Polling(dev, INA229_REG_CURRENT, 3);
    int32_t val_signed = ((int32_t)(val24 << 8)) >> 8;
    int32_t val20 = val_signed >> 4;
    return (double)val20 * dev->current_lsb; 
}

double INA229_GetPower(ina229_t* dev) {
    uint32_t val24 = (uint32_t)INA229_ReadReg_Polling(dev, INA229_REG_POWER, 3) & 0xFFFFFF; 
    return (double)val24 * dev->current_lsb * 3.2; 
}

double INA229_GetEnergy(ina229_t* dev) {
    uint64_t val40 = INA229_ReadReg_Polling(dev, INA229_REG_ENERGY, 5) & 0xFFFFFFFFFFull;
    return (double)val40 * dev->current_lsb * 3.2 * 16.0;
}

double INA229_GetCharge(ina229_t* dev) {
    uint64_t val40 = INA229_ReadReg_Polling(dev, INA229_REG_CHARGE, 5);
    int64_t val_signed = ((int64_t)(val40 << 24)) >> 24; 
    return (double)val_signed * dev->current_lsb; 
}

bool INA229_Get_OverCurrent_Flag(ina229_t* dev) {
    uint16_t diag_alrt = (uint16_t)INA229_ReadReg_Polling(dev, INA229_REG_DIAG_ALRT, 2);
    if (diag_alrt & 0x8000) {
        return true;
    }
    return false;
}


uint16_t INA229_Get_All_AlertFlags(ina229_t* dev) {
    return (uint16_t)INA229_ReadReg_Polling(dev, INA229_REG_DIAG_ALRT, 2);
}

void INA229_Clear_AlertFlags(ina229_t* dev) {
    INA229_ReadReg_Polling(dev, INA229_REG_DIAG_ALRT, 2);
}
/* ====================================================================
                                    DMA 
 * ==================================================================== */


bool INA229_Start_ReadReg_DMA(ina229_t* dev, uint8_t reg, uint8_t data_size) {
    if (data_size > 7 || dev->spi->dma_handle == NULL) return false;
    
    dev->dma_tx_buf[0] = (reg << 2) | 0x01;
    
    for (int i = 1; i <= data_size; i++) {
        dev->dma_tx_buf[i] = 0xFF;
    }
    
    INA229_CS_Low(dev);

    return SPI_TransmitReceive_DMA(dev->spi, dev->dma_tx_buf, dev->dma_rx_buf, data_size + 1);
}


double INA229_Parse_BusVoltage_DMA(ina229_t* dev) {
    uint32_t val24 = ((uint32_t)dev->dma_rx_buf[1] << 16) | 
                     ((uint32_t)dev->dma_rx_buf[2] << 8) | 
                      dev->dma_rx_buf[3];
    uint32_t val20 = val24 >> 4; 
    return (double)val20 * INA229_VBUS_LSB; 
}

double INA229_Parse_ShuntVoltage_DMA(ina229_t* dev) {
    uint32_t val24 = ((uint32_t)dev->dma_rx_buf[1] << 16) | 
                     ((uint32_t)dev->dma_rx_buf[2] << 8) | 
                      dev->dma_rx_buf[3];
    int32_t val_signed = ((int32_t)(val24 << 8)) >> 8; 
    int32_t val20 = val_signed >> 4;
    return (double)val20 * INA229_VSHUNT_LSB; 
}

double INA229_Parse_Current_DMA(ina229_t* dev) {
    uint32_t val24 = ((uint32_t)dev->dma_rx_buf[1] << 16) | 
                     ((uint32_t)dev->dma_rx_buf[2] << 8) | 
                      dev->dma_rx_buf[3];
    int32_t val_signed = ((int32_t)(val24 << 8)) >> 8;
    int32_t val20 = val_signed >> 4;
    return (double)val20 * dev->current_lsb; 
}

double INA229_Parse_Power_DMA(ina229_t* dev) {
    uint32_t val24 = ((uint32_t)dev->dma_rx_buf[1] << 16) | 
                     ((uint32_t)dev->dma_rx_buf[2] << 8) | 
                      dev->dma_rx_buf[3];
    return (double)(val24 & 0xFFFFFF) * dev->current_lsb * 3.2; 
}



