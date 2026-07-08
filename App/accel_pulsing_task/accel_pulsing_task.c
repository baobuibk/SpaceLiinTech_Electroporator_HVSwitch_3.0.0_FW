/*
 * accel_pulsing_task.c
 *
 *  Created on: Jul 8, 2026
 *      Author: PV
 */

#include "accel_pulsing_task.h"


Accel_Pulsing_Task_State accel_pulsing_task_state = ACCEL_PULSING_STATE_IDLE;


void Accel_Pulsing_Task_Init(void){

}


void Accel_Pulsing_Task(void*){
    switch(accel_pulsing_task_state){
        case ACCEL_PULSING_STATE_IDLE:
        {
            break;
        }
        case ACCEL_PULSING_STATE_INITIALIZE:
        {
            break;
        }
        case ACCEL_PULSING_STATE_WAITING_TRIGGER:
        {
            break;
        }
        case ACCEL_PULSING_STATE_PULSING:
        {
            break;
        }
        case ACCEL_PULSING_STATE_ERROR:
        {
            break;
        }
    }
}
