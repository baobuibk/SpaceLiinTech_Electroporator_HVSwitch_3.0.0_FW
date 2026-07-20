/*
 * fsp_frame.c
 *
 *  Created on: May 8, 2026
 *      Author: PV
 */
#include "fsp.h"
#include "fsp_frame.h"
#include "fsp_line_task.h"
#include "cli_command.h"
#include "impedance_task.h"
#include "crc.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "app.h"

static void float_to_string(float num, char *str, uint8_t precision);
static void fsp_print(uint8_t packet_length);

uint8_t FSP_Line_Process(void)
{
    switch (ps_FSP_RX->CMD)
    {
    /*---------- Handle response control CMD--------------*/
    case FSP_CMD_SET_CAP_VOLT_ALL:
        break;

    case FSP_CMD_SET_CAP_VOLT_HV:
        break;

    case FSP_CMD_SET_CAP_VOLT_LV:
        break;

    case FSP_CMD_SET_CAP_CONTROL:
        break;

    case FSP_CMD_SET_CAP_RELEASE:
        break;

    /*---------- Response information user request--------------*/
    case FSP_CMD_GET_CAP_STATE:
    {
        if (ps_FSP_RX->Payload.get_cap_state.hv_state_idle == true)
            UART_Driver_SendString(&XBEE_UART, "HV CAP IS IDLE\n\r> ");

        else if (ps_FSP_RX->Payload.get_cap_state.hv_state_charging == true)
            UART_Driver_SendString(&XBEE_UART, "HV CAP IS CHARGING\n\r> ");

        else if (ps_FSP_RX->Payload.get_cap_state.hv_state_discharging == true)
            UART_Driver_SendString(&XBEE_UART, "HV CAP IS DISCHARGING\n\r> ");

        else if (ps_FSP_RX->Payload.get_cap_state.hv_state_holding_volt == true)
            UART_Driver_SendString(&XBEE_UART, "HV CAP IS HOLDING VOLT\n\r> ");

        if (ps_FSP_RX->Payload.get_cap_state.lv_state_idle == true)
            UART_Driver_SendString(&XBEE_UART, "LV CAP IS IDLE\n\r> ");

        else if (ps_FSP_RX->Payload.get_cap_state.lv_state_charging == true)
            UART_Driver_SendString(&XBEE_UART, "LV CAP IS CHARGING\n\r> ");

        else if (ps_FSP_RX->Payload.get_cap_state.lv_state_discharging == true)
            UART_Driver_SendString(&XBEE_UART, "LV CAP IS DISCHARGING\n\r> ");

        else if (ps_FSP_RX->Payload.get_cap_state.lv_state_holding_volt == true)
            UART_Driver_SendString(&XBEE_UART, "LV CAP IS HOLDING VOLT\n\r> ");

        break;
    }
    case FSP_CMD_GET_CAP_VOLT:
    {
        char msg[64];

        uint16_t hv_set_vol = ((uint16_t)ps_FSP_RX->Payload.get_cap_volt.HV_Volt_High << 8) | ps_FSP_RX->Payload.get_cap_volt.HV_Volt_Low;

        uint16_t lv_set_vol = ((uint16_t)ps_FSP_RX->Payload.get_cap_volt.LV_Volt_High << 8) | ps_FSP_RX->Payload.get_cap_volt.LV_Volt_Low;

        sprintf(msg, "HV CAP IS SET AT: %dV, LV CAP IS SET AT: %dV \n\r> ", hv_set_vol, lv_set_vol);

        UART_Driver_SendString(&XBEE_UART, msg);

        break;
    }
    case FSP_CMD_GET_CAP_CONTROL:
    {
        if (ps_FSP_RX->Payload.get_cap_control.HV_Volt_Charge == true)
            UART_Driver_SendString(&XBEE_UART, "HV CAP IS CHARGING\n\r> ");

        else if (ps_FSP_RX->Payload.get_cap_control.HV_Volt_Charge == false)
            UART_Driver_SendString(&XBEE_UART, "HV CAP IS NOT CHARGING\n\r> ");

        if (ps_FSP_RX->Payload.get_cap_control.LV_Volt_Charge == true)
            UART_Driver_SendString(&XBEE_UART, "LV CAP IS CHARGING\n\r> ");

        else if (ps_FSP_RX->Payload.get_cap_control.LV_Volt_Charge == false)
            UART_Driver_SendString(&XBEE_UART, "LV CAP IS NOT CHARGING\n\r> ");

        break;
    }
    case FSP_CMD_GET_CAP_RELEASE:
    {
        if (ps_FSP_RX->Payload.get_cap_release.HV_Volt_Discharge == true)
            UART_Driver_SendString(&XBEE_UART, "HV CAP IS DISCHARGING\n\r> ");

        else if (ps_FSP_RX->Payload.get_cap_release.HV_Volt_Discharge == false)
            UART_Driver_SendString(&XBEE_UART, "HV CAP IS NOT DISCHARGING\n\r> ");

        if (ps_FSP_RX->Payload.get_cap_release.LV_Volt_Discharge == true)
            UART_Driver_SendString(&XBEE_UART, "LV CAP IS DISCHARGING\n\r> ");

        else if (ps_FSP_RX->Payload.get_cap_release.LV_Volt_Discharge == false)
            UART_Driver_SendString(&XBEE_UART, "LV CAP IS NOT DISCHARGING\n\r> ");
        break;
    }
    case FSP_CMD_GET_CAP_ALL:
    {
        char msg[64];

        uint16_t hv_set_vol = ((uint16_t)ps_FSP_RX->Payload.get_cap_all.HV_Volt_High << 8) | ps_FSP_RX->Payload.get_cap_volt.HV_Volt_Low;

        uint16_t lv_set_vol = ((uint16_t)ps_FSP_RX->Payload.get_cap_all.LV_Volt_High << 8) | ps_FSP_RX->Payload.get_cap_volt.LV_Volt_Low;

        sprintf(msg, "HV CAP IS SET AT: %dV, LV CAP IS SET AT: %dV\n\r> ", hv_set_vol, lv_set_vol);

        UART_Driver_SendString(&XBEE_UART, msg);

        if (ps_FSP_RX->Payload.get_cap_all.HV_Volt_Charge == true)
            UART_Driver_SendString(&XBEE_UART, "HV CAP IS CHARGING\n\r> ");

        else if (ps_FSP_RX->Payload.get_cap_all.HV_Volt_Charge == false)
            UART_Driver_SendString(&XBEE_UART, "HV CAP IS NOT CHARGING\n\r> ");

        if (ps_FSP_RX->Payload.get_cap_all.LV_Volt_Charge == true)
            UART_Driver_SendString(&XBEE_UART, "LV CAP IS CHARGING\n\r> ");

        else if (ps_FSP_RX->Payload.get_cap_all.LV_Volt_Charge == false)
            UART_Driver_SendString(&XBEE_UART, "LV CAP IS NOT CHARGING\n\r> ");

        if (ps_FSP_RX->Payload.get_cap_all.HV_Volt_Discharge == true)
            UART_Driver_SendString(&XBEE_UART, "HV CAP IS DISCHARGING\n\r> ");

        else if (ps_FSP_RX->Payload.get_cap_all.HV_Volt_Discharge == false)
            UART_Driver_SendString(&XBEE_UART, "HV CAP IS NOT DISCHARGING\n\r> ");

        if (ps_FSP_RX->Payload.get_cap_all.LV_Volt_Discharge == true)
            UART_Driver_SendString(&XBEE_UART, "LV CAP IS DISCHARGING\n\r> ");

        else if (ps_FSP_RX->Payload.get_cap_all.LV_Volt_Discharge == false)
            UART_Driver_SendString(&XBEE_UART, "LV CAP IS NOT DISCHARGING\n\r> ");

        break;
    }
    case FSP_CMD_MEASURE_VOLT:
    {
        char msg[64];

        uint32_t hv_raw_mvol = (uint32_t)ps_FSP_RX->Payload.measure_volt.HV_raw_volt[0] |
                               ((uint32_t)ps_FSP_RX->Payload.measure_volt.HV_raw_volt[1] << 8) |
                               ((uint32_t)ps_FSP_RX->Payload.measure_volt.HV_raw_volt[2] << 16) |
                               ((uint32_t)ps_FSP_RX->Payload.measure_volt.HV_raw_volt[3] << 24);
        uint16_t lv_raw_mvol = (uint16_t)ps_FSP_RX->Payload.measure_volt.LV_raw_volt[0] |
                               ((uint16_t)ps_FSP_RX->Payload.measure_volt.LV_raw_volt[1] << 8);

        sprintf(msg, "HV cap: %dmV, LV cap: %dmV\n\r> ", hv_raw_mvol, lv_raw_mvol);
        UART_Driver_SendString(&XBEE_UART, msg);

        break;
    }
    case FSP_CMD_GET_OVV_FLAG:
    {

        bool HV_OVV_Flag = ps_FSP_RX->Payload.get_ovv_flag.HV_OVV_flag;
        bool LV_OVV_Flag = ps_FSP_RX->Payload.get_ovv_flag.LV_OVV_flag;

        if (HV_OVV_Flag == true)
        {
            UART_Driver_SendString(&XBEE_UART, "HV CAP OVER VOLTAGE FLAG STATUS IS TRUE\r\n> ");
        }
        else if (HV_OVV_Flag == false)
        {
            UART_Driver_SendString(&XBEE_UART, "HV CAP OVER VOLTAGE FLAG STATUS IS FALSE\r\n> ");
        }

        if (LV_OVV_Flag == true)
        {
            UART_Driver_SendString(&XBEE_UART, "LV CAP OVER VOLTAGE FLAG STATUS IS TRUE\r\n> ");
        }
        else if (LV_OVV_Flag == false)
        {
            UART_Driver_SendString(&XBEE_UART, "LV CAP OVER VOLTAGE FLAG STATUS IS FALSE\r\n> ");
        }
        break;
    }
    case FSP_CMD_GET_SENSOR_GYRO:
    {

        int32_t read_gyro_x = (int32_t)ps_FSP_RX->Payload.get_sensor_lsm6dsox.Gyro_x[0] |
                              ((int32_t)ps_FSP_RX->Payload.get_sensor_lsm6dsox.Gyro_x[1] << 8) |
                              ((int32_t)ps_FSP_RX->Payload.get_sensor_lsm6dsox.Gyro_x[2] << 16) |
                              ((int32_t)ps_FSP_RX->Payload.get_sensor_lsm6dsox.Gyro_x[3] << 24);

        int32_t read_gyro_y = (int32_t)ps_FSP_RX->Payload.get_sensor_lsm6dsox.Gyro_y[0] |
                              ((int32_t)ps_FSP_RX->Payload.get_sensor_lsm6dsox.Gyro_y[1] << 8) |
                              ((int32_t)ps_FSP_RX->Payload.get_sensor_lsm6dsox.Gyro_y[2] << 16) |
                              ((int32_t)ps_FSP_RX->Payload.get_sensor_lsm6dsox.Gyro_y[3] << 24);

        int32_t read_gyro_z = (int32_t)ps_FSP_RX->Payload.get_sensor_lsm6dsox.Gyro_z[0] |
                              ((int32_t)ps_FSP_RX->Payload.get_sensor_lsm6dsox.Gyro_z[1] << 8) |
                              ((int32_t)ps_FSP_RX->Payload.get_sensor_lsm6dsox.Gyro_z[2] << 16) |
                              ((int32_t)ps_FSP_RX->Payload.get_sensor_lsm6dsox.Gyro_z[3] << 24);

        float gyro_x = (float)read_gyro_x / 1000.0f;
        float gyro_y = (float)read_gyro_y / 1000.0f;
        float gyro_z = (float)read_gyro_z / 1000.0f;

        char msg[128];
        char gyro_x_str[16];
        char gyro_y_str[16];
        char gyro_z_str[16];

        float_to_string(gyro_x, gyro_x_str, 2);
        float_to_string(gyro_y, gyro_y_str, 2);
        float_to_string(gyro_z, gyro_z_str, 2);

        sprintf(msg, "GYRO x: %s dps; GYRO y: %s dps; GYRO z: %s dps\n\r> ", gyro_x_str, gyro_y_str, gyro_z_str);
        UART_Driver_SendString(&XBEE_UART, msg);

        break;
    }
    case FSP_CMD_GET_SENSOR_ACCEL:
    {
        int32_t read_accel_x = (int32_t)ps_FSP_RX->Payload.get_sensor_lsm6dsox.Accel_x[0] |
                               ((int32_t)ps_FSP_RX->Payload.get_sensor_lsm6dsox.Accel_x[1] << 8) |
                               ((int32_t)ps_FSP_RX->Payload.get_sensor_lsm6dsox.Accel_x[2] << 16) |
                               ((int32_t)ps_FSP_RX->Payload.get_sensor_lsm6dsox.Accel_x[3] << 24);

        int32_t read_accel_y = (int32_t)ps_FSP_RX->Payload.get_sensor_lsm6dsox.Accel_y[0] |
                               ((int32_t)ps_FSP_RX->Payload.get_sensor_lsm6dsox.Accel_y[1] << 8) |
                               ((int32_t)ps_FSP_RX->Payload.get_sensor_lsm6dsox.Accel_y[2] << 16) |
                               ((int32_t)ps_FSP_RX->Payload.get_sensor_lsm6dsox.Accel_y[3] << 24);

        int32_t read_accel_z = (int32_t)ps_FSP_RX->Payload.get_sensor_lsm6dsox.Accel_z[0] |
                               ((int32_t)ps_FSP_RX->Payload.get_sensor_lsm6dsox.Accel_z[1] << 8) |
                               ((int32_t)ps_FSP_RX->Payload.get_sensor_lsm6dsox.Accel_z[2] << 16) |
                               ((int32_t)ps_FSP_RX->Payload.get_sensor_lsm6dsox.Accel_z[3] << 24);

        float accel_x = (float)read_accel_x / 1000.0f;
        float accel_y = (float)read_accel_y / 1000.0f;
        float accel_z = (float)read_accel_z / 1000.0f;

        char msg[128];
        char accel_x_str[16];
        char accel_y_str[16];
        char accel_z_str[16];

        float_to_string(accel_x, accel_x_str, 2);
        float_to_string(accel_y, accel_y_str, 2);
        float_to_string(accel_z, accel_z_str, 2);

        sprintf(msg, "ACCEL x: %s mg; ACCEL y: %s mg; ACCEL z: %s mg\n\r> ", accel_x_str, accel_y_str, accel_z_str);
        UART_Driver_SendString(&XBEE_UART, msg);

        break;
    }
    case FSP_CMD_GET_SENSOR_LSM6DSOX:
    {
        int32_t read_gyro_x = (int32_t)ps_FSP_RX->Payload.get_sensor_lsm6dsox.Gyro_x[0] |
                              ((int32_t)ps_FSP_RX->Payload.get_sensor_lsm6dsox.Gyro_x[1] << 8) |
                              ((int32_t)ps_FSP_RX->Payload.get_sensor_lsm6dsox.Gyro_x[2] << 16) |
                              ((int32_t)ps_FSP_RX->Payload.get_sensor_lsm6dsox.Gyro_x[3] << 24);

        int32_t read_gyro_y = (int32_t)ps_FSP_RX->Payload.get_sensor_lsm6dsox.Gyro_y[0] |
                              ((int32_t)ps_FSP_RX->Payload.get_sensor_lsm6dsox.Gyro_y[1] << 8) |
                              ((int32_t)ps_FSP_RX->Payload.get_sensor_lsm6dsox.Gyro_y[2] << 16) |
                              ((int32_t)ps_FSP_RX->Payload.get_sensor_lsm6dsox.Gyro_y[3] << 24);

        int32_t read_gyro_z = (int32_t)ps_FSP_RX->Payload.get_sensor_lsm6dsox.Gyro_z[0] |
                              ((int32_t)ps_FSP_RX->Payload.get_sensor_lsm6dsox.Gyro_z[1] << 8) |
                              ((int32_t)ps_FSP_RX->Payload.get_sensor_lsm6dsox.Gyro_z[2] << 16) |
                              ((int32_t)ps_FSP_RX->Payload.get_sensor_lsm6dsox.Gyro_z[3] << 24);

        int32_t read_accel_x = (int32_t)ps_FSP_RX->Payload.get_sensor_lsm6dsox.Accel_x[0] |
                               ((int32_t)ps_FSP_RX->Payload.get_sensor_lsm6dsox.Accel_x[1] << 8) |
                               ((int32_t)ps_FSP_RX->Payload.get_sensor_lsm6dsox.Accel_x[2] << 16) |
                               ((int32_t)ps_FSP_RX->Payload.get_sensor_lsm6dsox.Accel_x[3] << 24);

        int32_t read_accel_y = (int32_t)ps_FSP_RX->Payload.get_sensor_lsm6dsox.Accel_y[0] |
                               ((int32_t)ps_FSP_RX->Payload.get_sensor_lsm6dsox.Accel_y[1] << 8) |
                               ((int32_t)ps_FSP_RX->Payload.get_sensor_lsm6dsox.Accel_y[2] << 16) |
                               ((int32_t)ps_FSP_RX->Payload.get_sensor_lsm6dsox.Accel_y[3] << 24);

        int32_t read_accel_z = (int32_t)ps_FSP_RX->Payload.get_sensor_lsm6dsox.Accel_z[0] |
                               ((int32_t)ps_FSP_RX->Payload.get_sensor_lsm6dsox.Accel_z[1] << 8) |
                               ((int32_t)ps_FSP_RX->Payload.get_sensor_lsm6dsox.Accel_z[2] << 16) |
                               ((int32_t)ps_FSP_RX->Payload.get_sensor_lsm6dsox.Accel_z[3] << 24);

        float gyro_x = (float)read_gyro_x / 1000.0f;
        float gyro_y = (float)read_gyro_y / 1000.0f;
        float gyro_z = (float)read_gyro_z / 1000.0f;
        float accel_x = (float)read_accel_x / 1000.0f;
        float accel_y = (float)read_accel_y / 1000.0f;
        float accel_z = (float)read_accel_z / 1000.0f;

        char msg[128];
        char gyro_x_str[16];
        char gyro_y_str[16];
        char gyro_z_str[16];

        float_to_string(gyro_x, gyro_x_str, 2);
        float_to_string(gyro_y, gyro_y_str, 2);
        float_to_string(gyro_z, gyro_z_str, 2);

        sprintf(msg, "GYRO x: %s dps; GYRO y: %s dps; GYRO z: %s dps\n\r> ", gyro_x_str, gyro_y_str, gyro_z_str);
        UART_Driver_SendString(&XBEE_UART, msg);

        char accel_x_str[16];
        char accel_y_str[16];
        char accel_z_str[16];

        float_to_string(accel_x, accel_x_str, 2);
        float_to_string(accel_y, accel_y_str, 2);
        float_to_string(accel_z, accel_z_str, 2);

        sprintf(msg, "ACCEL x: %s mg; ACCEL y: %s mg; ACCEL z: %s mg\n\r> ", accel_x_str, accel_y_str, accel_z_str);
        UART_Driver_SendString(&XBEE_UART, msg);

        break;
    }
    case FSP_CMD_GET_SENSOR_TEMP:
    {
        int32_t read_temp = (int32_t)ps_FSP_RX->Payload.get_sensor_bmp390.Temp[0] |
                            ((int32_t)ps_FSP_RX->Payload.get_sensor_bmp390.Temp[1] << 8) |
                            ((int32_t)ps_FSP_RX->Payload.get_sensor_bmp390.Temp[2] << 16) |
                            ((int32_t)ps_FSP_RX->Payload.get_sensor_bmp390.Temp[3] << 24);

        float temp = (float)read_temp / 1000.0f;

        char msg[128];
        char temp_str[16];

        float_to_string(temp, temp_str, 2);

        sprintf(msg, "TEMPERATURE: %s C\n\r> ", temp_str);
        UART_Driver_SendString(&XBEE_UART, msg);

        break;
    }
    case FSP_CMD_GET_SENSOR_PRESSURE:
    {
        int32_t read_press = (int32_t)ps_FSP_RX->Payload.get_sensor_bmp390.Pressure[0] |
                             ((int32_t)ps_FSP_RX->Payload.get_sensor_bmp390.Pressure[1] << 8) |
                             ((int32_t)ps_FSP_RX->Payload.get_sensor_bmp390.Pressure[2] << 16) |
                             ((int32_t)ps_FSP_RX->Payload.get_sensor_bmp390.Pressure[3] << 24);

        float press = (float)read_press / 1000.0f;

        char msg[128];
        char press_str[16];

        float_to_string(press, press_str, 2);

        sprintf(msg, "PRESSURE: %s Pa "
                     ""
                     "\n\r> ",
                press_str);
        UART_Driver_SendString(&XBEE_UART, msg);

        break;
    }
    case FSP_CMD_GET_SENSOR_ALTITUDE:
    {
        int32_t read_altitude = (int32_t)ps_FSP_RX->Payload.get_sensor_bmp390.Altitude[0] |
                                ((int32_t)ps_FSP_RX->Payload.get_sensor_bmp390.Altitude[1] << 8) |
                                ((int32_t)ps_FSP_RX->Payload.get_sensor_bmp390.Altitude[2] << 16) |
                                ((int32_t)ps_FSP_RX->Payload.get_sensor_bmp390.Altitude[3] << 24);

        float altitude = (float)read_altitude / 1000.0f;

        char msg[128];
        char altitude_str[16];
        float_to_string(altitude, altitude_str, 2);

        sprintf(msg, "ALTITUDE: %s m\n\r> ", altitude_str);
        UART_Driver_SendString(&XBEE_UART, msg);

        break;
    }
    case FSP_CMD_GET_SENSOR_BMP390:
    {
        int32_t read_temp = (int32_t)ps_FSP_RX->Payload.get_sensor_bmp390.Temp[0] |
                            ((int32_t)ps_FSP_RX->Payload.get_sensor_bmp390.Temp[1] << 8) |
                            ((int32_t)ps_FSP_RX->Payload.get_sensor_bmp390.Temp[2] << 16) |
                            ((int32_t)ps_FSP_RX->Payload.get_sensor_bmp390.Temp[3] << 24);

        int32_t read_press = (int32_t)ps_FSP_RX->Payload.get_sensor_bmp390.Pressure[0] |
                             ((int32_t)ps_FSP_RX->Payload.get_sensor_bmp390.Pressure[1] << 8) |
                             ((int32_t)ps_FSP_RX->Payload.get_sensor_bmp390.Pressure[2] << 16) |
                             ((int32_t)ps_FSP_RX->Payload.get_sensor_bmp390.Pressure[3] << 24);

        int32_t read_altitude = (int32_t)ps_FSP_RX->Payload.get_sensor_bmp390.Altitude[0] |
                                ((int32_t)ps_FSP_RX->Payload.get_sensor_bmp390.Altitude[1] << 8) |
                                ((int32_t)ps_FSP_RX->Payload.get_sensor_bmp390.Altitude[2] << 16) |
                                ((int32_t)ps_FSP_RX->Payload.get_sensor_bmp390.Altitude[3] << 24);

        float temp = (float)read_temp / 1000.0f;
        float press = (float)read_press / 1000.0f;
        float altitude = (float)read_altitude / 1000.0f;

        char msg[128];
        char temp_str[16];
        char press_str[16];
        char altitude_str[16];

        float_to_string(temp, temp_str, 2);
        float_to_string(press, press_str, 2);
        float_to_string(altitude, altitude_str, 2);

        sprintf(msg, "TEMPERATURE: %s C | PRESSURE: %s Pa | ALTITUDE: %s m\n\r> ", temp_str, press_str, altitude_str);
        UART_Driver_SendString(&XBEE_UART, msg);

        break;
    }
    case FSP_CMD_GET_SENSOR_H3LIS331DL:
    {
        int32_t read_accel_x = (int32_t)ps_FSP_RX->Payload.get_sensor_h3lis331dl.Accel_x[0] |
                               ((int32_t)ps_FSP_RX->Payload.get_sensor_h3lis331dl.Accel_x[1] << 8) |
                               ((int32_t)ps_FSP_RX->Payload.get_sensor_h3lis331dl.Accel_x[2] << 16) |
                               ((int32_t)ps_FSP_RX->Payload.get_sensor_h3lis331dl.Accel_x[3] << 24);

        int32_t read_accel_y = (int32_t)ps_FSP_RX->Payload.get_sensor_h3lis331dl.Accel_y[0] |
                               ((int32_t)ps_FSP_RX->Payload.get_sensor_h3lis331dl.Accel_y[1] << 8) |
                               ((int32_t)ps_FSP_RX->Payload.get_sensor_h3lis331dl.Accel_y[2] << 16) |
                               ((int32_t)ps_FSP_RX->Payload.get_sensor_h3lis331dl.Accel_y[3] << 24);

        int32_t read_accel_z = (int32_t)ps_FSP_RX->Payload.get_sensor_h3lis331dl.Accel_z[0] |
                               ((int32_t)ps_FSP_RX->Payload.get_sensor_h3lis331dl.Accel_z[1] << 8) |
                               ((int32_t)ps_FSP_RX->Payload.get_sensor_h3lis331dl.Accel_z[2] << 16) |
                               ((int32_t)ps_FSP_RX->Payload.get_sensor_h3lis331dl.Accel_z[3] << 24);

        // 2. Khôi phục lại giá trị Float ban đầu (tương ứng lúc bo sơ cấp nhân 1000)
        float accel_x = (float)read_accel_x / 1000000.0f;
        float accel_y = (float)read_accel_y / 1000000.0f;
        float accel_z = (float)read_accel_z / 1000000.0f;

        char msg[140];
        char accel_x_str[16];
        char accel_y_str[16];
        char accel_z_str[16];

        float_to_string(accel_x, accel_x_str, 2);
        float_to_string(accel_y, accel_y_str, 2);
        float_to_string(accel_z, accel_z_str, 2);

        sprintf(msg, "H3LIS331DL x: %s g; y: %s g; z: %s g\n\r> ", accel_x_str, accel_y_str, accel_z_str);
        UART_Driver_SendString(&XBEE_UART, msg);

        break;
    }
    case FSP_CMD_SET_SENSOR_H3LIS331DL_FS:
    {
        uint8_t set_fs_response = ps_FSP_RX->Payload.set_sensor_h3lis331dl_fs.set_h3lis_fs_response;
        if (set_fs_response == 0)
        {
            UART_Driver_SendString(&XBEE_UART, "H3LIS331DL FULL SCALE SET FAIL\r\n> ");
        }
        else if (set_fs_response == 1)
        {
            UART_Driver_SendString(&XBEE_UART, "H3LIS331DL FULL SCALE SET SUCESSFULLY\r\n> ");
        }
        break;
    }
    case FSP_CMD_GET_SENSOR_H3LIS331DL_FS:
    {
        uint8_t get_fs_response = ps_FSP_RX->Payload.get_sensor_h3lis331dl_fs.get_h3lis_fs_response;
        if (get_fs_response == 1)
        {
            uint8_t fs_value = ps_FSP_RX->Payload.get_sensor_h3lis331dl_fs.fs_value;
            if (fs_value == 1)
                UART_Driver_SendString(&XBEE_UART, "H3LIS331DL FULL SCALE: 100G\r\n> ");
            else if (fs_value == 2)
                UART_Driver_SendString(&XBEE_UART, "H3LIS331DL FULL SCALE: 200G\r\n> ");
            else if (fs_value == 4)
                UART_Driver_SendString(&XBEE_UART, "H3LIS331DL FULL SCALE: 400G\r\n> ");
        }
        else if (get_fs_response == 0)
        {
            UART_Driver_SendString(&XBEE_UART, "H3LIS331DL GET FULL SCALE FAIL\r\n> ");
        }

        break;
    }
    case FSP_CMD_GET_SENSOR_HV_TEMP:
    {
        float hv_temp = (uint16_t)(ps_FSP_RX->Payload.get_sensor_hv_temp.HV_temp_high << 8 | ps_FSP_RX->Payload.get_sensor_hv_temp.HV_temp_low) / 100.0f;

        char msg[128];
        char hv_temp_str[16];
        float_to_string(hv_temp, hv_temp_str, 2);

        sprintf(msg, "HV CHANNEL TEMP: %s C\n\r> ", hv_temp_str);
        UART_Driver_SendString(&XBEE_UART, msg);

        break;
    }
    case FSP_CMD_GET_SENSOR_LV_TEMP:
    {
        float lv_temp = (uint16_t)(ps_FSP_RX->Payload.get_sensor_lv_temp.LV_temp_high << 8 | ps_FSP_RX->Payload.get_sensor_lv_temp.LV_temp_low) / 100.0f;

        char msg[128];
        char lv_temp_str[16];
        float_to_string(lv_temp, lv_temp_str, 2);

        sprintf(msg, "LV CHANNEL TEMP: %s C\n\r> ", lv_temp_str);
        UART_Driver_SendString(&XBEE_UART, msg);

        break;
    }
    case FSP_CMD_SET_THRESHOLD_ACCEL:
    {
        uint8_t set_threshold_response = ps_FSP_RX->Payload.set_threshold_accel.set_threshold_response;
        if (set_threshold_response == 0)
        {
            UART_Driver_SendString(&XBEE_UART, "THRESHOLD SET FAIL\r\n> ");
        }
        else if (set_threshold_response == 1)
        {
            UART_Driver_SendString(&XBEE_UART, "THRESHOLD SET SUCESSFULLY\r\n> ");
        }
        break;
    }
    case FSP_CMD_GET_THRESHOLD_ACCEL:
    {
        uint8_t get_threshold_response = ps_FSP_RX->Payload.get_threshold_accel.get_threshold_response;
        if (get_threshold_response == 1)
        {
            int32_t threshold_accel_mg = (int32_t)ps_FSP_RX->Payload.get_threshold_accel.threshold_total_mg[0] |
                                         ((int32_t)ps_FSP_RX->Payload.get_threshold_accel.threshold_total_mg[1] << 8) |
                                         ((int32_t)ps_FSP_RX->Payload.get_threshold_accel.threshold_total_mg[2] << 16) |
                                         ((int32_t)ps_FSP_RX->Payload.get_threshold_accel.threshold_total_mg[3] << 24);

            char msg[128];
            sprintf(msg, "THRESHOLD ACCEL:%d MG\n\r> ", threshold_accel_mg);
            UART_Driver_SendString(&XBEE_UART, msg);
        }
        else{
            UART_Driver_SendString(&XBEE_UART, "THRESHOLD GET FAIL\r\n> ");
        }

        break;
    }
    case FSP_CMD_SET_AUTO_ACCEL:
    {
        uint8_t set_auto_accel = ps_FSP_RX->Payload.set_auto_accel.set_auto_accel_response;
        if (set_auto_accel == 0)
        {
            UART_Driver_SendString(&XBEE_UART, "AUTO PULSE MODE SET FAIL\r\n> ");
        }
        else if (set_auto_accel == 1)
        {
            UART_Driver_SendString(&XBEE_UART, "AUTO PULSE MODE SET SUCESSFULLY\r\n> ");
        }
        break;
    }
    case FSP_CMD_AUTO_ACCEL_TRIGGER:
    {
        H_Bridge_State = HB_TASK_INIT_STATE;
        SchedulerTaskEnable(H_BRIDGE_TASK, 1);

        break;
    }
    case FSP_CMD_GET_SENSOR_HVC_INIT_STATE:
    {
        bool init_state_bmp390 = ps_FSP_RX->Payload.get_sensor_hvc_init_state.init_state_bmp390;
        bool init_state_lsm6d = ps_FSP_RX->Payload.get_sensor_hvc_init_state.init_state_lsm6d;
        bool init_state_h3lis = ps_FSP_RX->Payload.get_sensor_hvc_init_state.init_state_h3lis;
        bool init_state_tc1047_hv = ps_FSP_RX->Payload.get_sensor_hvc_init_state.init_state_tc1047_hv;
        bool init_state_tc1047_lv = ps_FSP_RX->Payload.get_sensor_hvc_init_state.init_state_tc1047_lv;

        if (init_state_bmp390 == true) UART_Driver_SendString(&XBEE_UART, "BMP390 INIT		: OK\r\n> ");
        else UART_Driver_SendString(&XBEE_UART, "BMP390 INIT		: FAIL\r\n> ");

        if (init_state_lsm6d == true) UART_Driver_SendString(&XBEE_UART, "LSM6DSOX INIT		: OK\r\n> ");
        else UART_Driver_SendString(&XBEE_UART, "LSM6DSOX INIT		: FAIL\r\n> ");

        if (init_state_h3lis == true) UART_Driver_SendString(&XBEE_UART, "H3LIS331DL INIT	: OK\r\n> ");
        else UART_Driver_SendString(&XBEE_UART, "H3LIS331DL INIT	: FAIL\r\n> ");

        if (init_state_tc1047_hv == true) UART_Driver_SendString(&XBEE_UART, "TC1047 HV INIT	: OK\r\n> ");
        else UART_Driver_SendString(&XBEE_UART, "TC1047 HV INIT		: FAIL\r\n> ");

        if (init_state_tc1047_lv == true) UART_Driver_SendString(&XBEE_UART, "TC1047 LV INIT	: OK\r\n> ");
        else UART_Driver_SendString(&XBEE_UART, "TC1047 LV INIT		: FAIL\r\n> ");

        break;
    }
    default:
        break;
    }
    return;
}

/* -------------------------- STATIC FUNCTION ---------------------------------------- */
static void float_to_string(float num, char *str, uint8_t precision)
{

    if (num < 0)
    {
        *str++ = '-';
        num = -num;
    }

    num += 0.5f / powf(10.0f, (float)precision);

    uint32_t int_part = (uint32_t)num;

    float frac_part = num - (float)int_part;

    char temp_str[16];
    int i = 0;

    if (int_part == 0)
    {
        temp_str[i++] = '0';
    }
    else
    {
        while (int_part > 0)
        {
            temp_str[i++] = (int_part % 10) + '0';
            int_part /= 10;
        }
    }

    for (int j = i - 1; j >= 0; j--)
    {
        *str++ = temp_str[j];
    }

    if (precision > 0)
    {
        *str++ = '.'; // Thêm dấu chấm thập phân

        while (precision > 0)
        {
            frac_part *= 10.0f;
            uint32_t digit = (uint32_t)frac_part;
            *str++ = digit + '0';
            frac_part -= (float)digit;
            precision--;
        }
    }
    *str = '\0';
}

static void fsp_print(uint8_t packet_length)
{
    s_FSP_TX_Packet.sod = FSP_PKT_SOD;
    s_FSP_TX_Packet.src_adr = fsp_my_adr;
    s_FSP_TX_Packet.dst_adr = FSP_ADR_GPP;
    s_FSP_TX_Packet.length = packet_length;
    s_FSP_TX_Packet.type = FSP_PKT_TYPE_CMD_W_DATA;
    s_FSP_TX_Packet.eof = FSP_PKT_EOF;
    s_FSP_TX_Packet.crc16 = crc16_CCITT(FSP_CRC16_INITIAL_VALUE, &s_FSP_TX_Packet.src_adr, s_FSP_TX_Packet.length + 4);

    uint8_t encoded_frame[25] = {0};
    uint8_t frame_len;
    fsp_encode(&s_FSP_TX_Packet, encoded_frame, &frame_len);

    UART_Driver_SendFSP(&GPC_UART, (char *)encoded_frame, frame_len);
}
