/*
 * db_pulsing.h
 *
 *  Created on: Jun 17, 2026
 *      Author: PV
 */

#ifndef DB_PULSING_H_
#define DB_PULSING_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define MAX_PULSE_SEQUENCES 20

typedef struct
{
	uint8_t		pos_count;
	uint8_t		neg_count;

	uint16_t	delay_ms;

	uint16_t	pos_on_ms;
	uint16_t	pos_off_ms;
	uint16_t	neg_on_ms;
	uint16_t	neg_off_ms;

} Pulse_Stage_t;

typedef struct
{
	bool		is_setted;
	uint16_t	sequence_delay_ms;

	uint8_t		pos_pole_index;
	uint8_t		neg_pole_index;

	uint16_t	pulse_delay_ms;

	Pulse_Stage_t 	HV_stage;
	Pulse_Stage_t	LV_stage;

} Pulse_Sequence_t;

typedef enum {
    // --- Các biến điều khiển toàn cục ---
    DB_ID_PULSE_CMD_START = 0,      // Lệnh chạy chu trình
    DB_ID_PULSE_CMD_STOP,           // Lệnh dừng khẩn cấp (Kill)
    DB_ID_PULSE_CMD_MANUAL,         // Lệnh chạy Manual

    // --- Các biến trạng thái báo cáo ---
    DB_ID_PULSE_IS_RUNNING,         // Hệ thống có đang băm xung không?
    DB_ID_PULSE_CURRENT_SEQ_IDX,    // Đang chạy ở sequence số mấy (0-19)
    DB_ID_PULSE_CURRENT_STAGE,      // Đang chạy chặng nào (HV+, HV-, LV+, LV-)

    // --- Cấu hình 20 Sequence ---
    DB_ID_PULSE_SEQ_00,
    DB_ID_PULSE_SEQ_01,
    DB_ID_PULSE_SEQ_02,
    DB_ID_PULSE_SEQ_03,
    DB_ID_PULSE_SEQ_04,
    DB_ID_PULSE_SEQ_05,
    DB_ID_PULSE_SEQ_06,
    DB_ID_PULSE_SEQ_07,
    DB_ID_PULSE_SEQ_08,
    DB_ID_PULSE_SEQ_09,
    DB_ID_PULSE_SEQ_10,
    DB_ID_PULSE_SEQ_11,
    DB_ID_PULSE_SEQ_12,
    DB_ID_PULSE_SEQ_13,
    DB_ID_PULSE_SEQ_14,
    DB_ID_PULSE_SEQ_15,
    DB_ID_PULSE_SEQ_16,
    DB_ID_PULSE_SEQ_17,
    DB_ID_PULSE_SEQ_18,
    DB_ID_PULSE_SEQ_19,

    DB_ID_PULSE_MAX
} db_pulse_id_t;


typedef struct {
    bool    pulse_cmd_start;
    bool    pulse_cmd_stop;
    bool    pulse_cmd_manual;

    bool    is_pulsing_running;

    uint8_t current_seq_idx;
    uint8_t current_stage;

    Pulse_Sequence_t seq_array[MAX_PULSE_SEQUENCES];
} db_pulse_data_t;

void db_pulse_init(void);
bool db_pulse_read(db_pulse_id_t id, void* out_data);
bool db_pulse_write(db_pulse_id_t id, const void* in_data);


#endif /* DB_PULSING_H_ */
