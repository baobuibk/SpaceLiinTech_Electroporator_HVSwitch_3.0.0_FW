/**
 * @file ina229.c
 * @brief INA229 Application Layer Driver Implementation
 */

#include "INA229.h"


static uint8_t INA229_SPI_ReadByteBlocking(spi_driver_t* p_spi) {
    while (SPI_is_buffer_empty(&p_spi->RX_read_index, &p_spi->RX_write_index)) {

    }
    uint8_t data = p_spi->p_RX_buffer[p_spi->RX_read_index];
    SPI_advance_buffer_index(&p_spi->RX_read_index, p_spi->RX_size);

    return data;
}


static void INA229_SyncTransfer(ina229_t* dev, uint8_t* tx_data, uint8_t* rx_data, uint16_t size) {

    dev->spi->RX_read_index = dev->spi->RX_write_index;

    SPI_Transmit_IT(dev->spi, tx_data, size);

    for (uint16_t i = 0; i < size; i++) {
        rx_data[i] = INA229_SPI_ReadByteBlocking(dev->spi);
    }
}


void INA229_WriteReg16(ina229_t* dev, uint8_t reg, uint16_t val) {

    uint8_t tx[3];
    tx[0] = (reg << 2) & ~0x01;  
    tx[1] = (uint8_t)(val >> 8);
    tx[2] = (uint8_t)(val & 0xFF);
    
    uint8_t rx[3] = {0};

    INA229_SyncTransfer(dev, tx, rx, 3);
}

uint16_t INA229_ReadReg16(ina229_t* dev, uint8_t reg) {

    uint8_t tx[3] = { (reg << 2) | 0x01, 0xFF, 0xFF }; 
    uint8_t rx[3] = {0};
    
    INA229_SyncTransfer(dev, tx, rx, 3);
    
    return (uint16_t)((rx[1] << 8) | rx[2]);
}

uint32_t INA229_ReadReg24(ina229_t* dev, uint8_t reg) {

    uint8_t tx[4] = { (reg << 2) | 0x01, 0xFF, 0xFF, 0xFF };
    uint8_t rx[4] = {0};
    
    INA229_SyncTransfer(dev, tx, rx, 4);
    
    return ((uint32_t)rx[1] << 16) | ((uint32_t)rx[2] << 8) | rx[3];
}

uint64_t INA229_ReadReg40(ina229_t* dev, uint8_t reg) {

    uint8_t tx[6] = { (reg << 2) | 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    uint8_t rx[6] = {0};
    
    INA229_SyncTransfer(dev, tx, rx, 6);
    
    uint64_t val = 0;
    for (int i = 1; i <= 5; i++) {
        val = (val << 8) | rx[i];
    }
    return val;
}


/* ====================================================================
 * INITIALIZATION & CALIBRATION
 * ==================================================================== */

void INA229_Init(ina229_t* dev, spi_driver_t* spi_handle) {
    dev->spi = spi_handle;
    dev->current_lsb = 0.0;
}

void INA229_Calibrate(ina229_t* dev, double r_shunt_ohms, double max_current_amps) {

    dev->current_lsb = max_current_amps / 524288.0;

    double shunt_cal_val = 13107.2e6 * dev->current_lsb * r_shunt_ohms;
    
    INA229_WriteReg16(dev, INA229_REG_SHUNT_CAL, (uint16_t)shunt_cal_val);
}


/* ====================================================================
 * MEASUREMENT GETTERS
 * ==================================================================== */

double INA229_GetBusVoltage(ina229_t* dev) {
    uint32_t val24 = INA229_ReadReg24(dev, INA229_REG_VBUS);
    
    uint32_t val20 = val24 >> 4; 
    
    return (double)val20 * INA229_VBUS_LSB; 
}

double INA229_GetShuntVoltage(ina229_t* dev) {
    uint32_t val24 = INA229_ReadReg24(dev, INA229_REG_VSHUNT);
    
    int32_t val_signed = ((int32_t)(val24 << 8)) >> 8; 
    int32_t val20 = val_signed >> 4;
    
    return (double)val20 * INA229_VSHUNT_LSB; 
}

double INA229_GetTemperature(ina229_t* dev) {

    int16_t val16 = (int16_t)INA229_ReadReg16(dev, INA229_REG_DIETEMP);
    
    return (double)val16 * INA229_TEMP_LSB;
}

double INA229_GetCurrent(ina229_t* dev) {
    uint32_t val24 = INA229_ReadReg24(dev, INA229_REG_CURRENT);
    
    int32_t val_signed = ((int32_t)(val24 << 8)) >> 8;
    int32_t val20 = val_signed >> 4;
    
    return (double)val20 * dev->current_lsb; 
}

double INA229_GetPower(ina229_t* dev) {
    // 24-bit Unsigned value, no bit shift needed [cite: 2690]
    uint32_t val24 = INA229_ReadReg24(dev, INA229_REG_POWER) & 0xFFFFFF; 
    
    // Power (W) = 3.2 * CURRENT_LSB * POWER [cite: 2838]
    return (double)val24 * dev->current_lsb * 3.2; 
}

double INA229_GetEnergy(ina229_t* dev) {
    // 40-bit Unsigned value [cite: 2695]
    uint64_t val40 = INA229_ReadReg40(dev, INA229_REG_ENERGY) & 0xFFFFFFFFFFull;
    
    // Energy (J) = 16 * 3.2 * CURRENT_LSB * ENERGY [cite: 2845]
    return (double)val40 * dev->current_lsb * 3.2 * 16.0;
}

double INA229_GetCharge(ina229_t* dev) {
    uint64_t val40 = INA229_ReadReg40(dev, INA229_REG_CHARGE);
    
    int64_t val_signed = ((int64_t)(val40 << 24)) >> 24; 
    
    return (double)val_signed * dev->current_lsb; 
}
