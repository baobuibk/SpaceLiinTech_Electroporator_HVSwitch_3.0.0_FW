#ifndef H_TASK_H_
#define H_TASK_H_

#include <stdint.h>
#include <stdbool.h>

#define MAX_SEQUENCES 10

typedef enum {
	HB_PULSE_IDLE = 0,

	HB_HV_POS_PULSE,
	HB_HV_NEG_PULSE,
	HB_HV_DELAY,

	HB_LV_POS_PULSE,
	HB_LV_NEG_PULSE,
	HB_LV_DELAY,

	HB_DELAY,

	HB_SEQUENCE_DELAY,

} H_Bridge_Process_State;

typedef enum{

	HB_TASK_IDLE= 0,
	HB_TASK_INIT_STATE,
	HB_TASK_PULSING,
    HB_TASK_ERROR_STATE,

}H_Bridge_Task_State;

typedef struct{
	bool		is_edit_enable;
	bool 		is_confirm;

    uint16_t    sequence_delay_ms;

    uint8_t     pos_pole_index;
    uint8_t     neg_pole_index;

    uint16_t    pulse_delay_ms;

    uint8_t     hv_pos_count;
    uint8_t     hv_neg_count;

    uint16_t    hv_delay_ms;

    uint16_t    hv_pos_on_ms;
    uint16_t    hv_pos_off_ms;
    uint16_t    hv_neg_on_ms;
    uint16_t    hv_neg_off_ms;

    uint8_t     lv_pos_count;
    uint8_t     lv_neg_count;

    uint16_t    lv_delay_ms;

    uint16_t    lv_pos_on_ms;
    uint16_t    lv_pos_off_ms;
    uint16_t    lv_neg_on_ms;
    uint16_t    lv_neg_off_ms;

}H_Bridge_Sequence_t;



extern H_Bridge_Sequence_t Sequence_List[MAX_SEQUENCES];
extern uint8_t 			   total_active_sequences;
extern H_Bridge_Task_State H_Bridge_State;



void H_Bridge_Task_Init(void);
void H_Bridge_Task_Set_State(H_Bridge_Task_State state);
H_Bridge_Task_State H_Bridge_Task_Get_State(void);

void H_Bridge_Task(void*);



#endif /* H_TASK_H_ */
