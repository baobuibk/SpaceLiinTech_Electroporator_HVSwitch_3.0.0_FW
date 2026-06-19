/*
 * db_pulsing.c
 *
 *  Created on: Jun 17, 2026
 *      Author: PV
 */
#include "db_pulsing.h"
#include <string.h>

static db_pulse_data_t pulse_db;

typedef struct
{
    void* data_ptr;
    size_t data_size;
} db_lookup_t;

static const db_lookup_t db_map[DB_ID_PULSE_MAX] = {

    [DB_ID_PULSE_CMD_START]       = { &pulse_db.cmd_start,       sizeof(pulse_db.cmd_start) },
    [DB_ID_PULSE_CMD_STOP]        = { &pulse_db.cmd_stop,        sizeof(pulse_db.cmd_stop) },
    [DB_ID_PULSE_CMD_MANUAL]      = { &pulse_db.cmd_manual,      sizeof(pulse_db.cmd_manual) },

    [DB_ID_PULSE_IS_RUNNING]      = { &pulse_db.is_running,      sizeof(pulse_db.is_running) },
    [DB_ID_PULSE_CURRENT_SEQ_IDX] = { &pulse_db.current_seq_idx, sizeof(pulse_db.current_seq_idx) },
    [DB_ID_PULSE_CURRENT_STAGE]   = { &pulse_db.current_stage,   sizeof(pulse_db.current_stage) },


    [DB_ID_PULSE_SEQ_00] = { &pulse_db.seq_array[0],  sizeof(Pulse_Sequence_t) },
    [DB_ID_PULSE_SEQ_01] = { &pulse_db.seq_array[1],  sizeof(Pulse_Sequence_t) },
    [DB_ID_PULSE_SEQ_02] = { &pulse_db.seq_array[2],  sizeof(Pulse_Sequence_t) },
    [DB_ID_PULSE_SEQ_03] = { &pulse_db.seq_array[3],  sizeof(Pulse_Sequence_t) },
    [DB_ID_PULSE_SEQ_04] = { &pulse_db.seq_array[4],  sizeof(Pulse_Sequence_t) },
    [DB_ID_PULSE_SEQ_05] = { &pulse_db.seq_array[5],  sizeof(Pulse_Sequence_t) },
    [DB_ID_PULSE_SEQ_06] = { &pulse_db.seq_array[6],  sizeof(Pulse_Sequence_t) },
    [DB_ID_PULSE_SEQ_07] = { &pulse_db.seq_array[7],  sizeof(Pulse_Sequence_t) },
    [DB_ID_PULSE_SEQ_08] = { &pulse_db.seq_array[8],  sizeof(Pulse_Sequence_t) },
    [DB_ID_PULSE_SEQ_09] = { &pulse_db.seq_array[9],  sizeof(Pulse_Sequence_t) },
    [DB_ID_PULSE_SEQ_10] = { &pulse_db.seq_array[10], sizeof(Pulse_Sequence_t) },
    [DB_ID_PULSE_SEQ_11] = { &pulse_db.seq_array[11], sizeof(Pulse_Sequence_t) },
    [DB_ID_PULSE_SEQ_12] = { &pulse_db.seq_array[12], sizeof(Pulse_Sequence_t) },
    [DB_ID_PULSE_SEQ_13] = { &pulse_db.seq_array[13], sizeof(Pulse_Sequence_t) },
    [DB_ID_PULSE_SEQ_14] = { &pulse_db.seq_array[14], sizeof(Pulse_Sequence_t) },
    [DB_ID_PULSE_SEQ_15] = { &pulse_db.seq_array[15], sizeof(Pulse_Sequence_t) },
    [DB_ID_PULSE_SEQ_16] = { &pulse_db.seq_array[16], sizeof(Pulse_Sequence_t) },
    [DB_ID_PULSE_SEQ_17] = { &pulse_db.seq_array[17], sizeof(Pulse_Sequence_t) },
    [DB_ID_PULSE_SEQ_18] = { &pulse_db.seq_array[18], sizeof(Pulse_Sequence_t) },
    [DB_ID_PULSE_SEQ_19] = { &pulse_db.seq_array[19], sizeof(Pulse_Sequence_t) },
};


void db_pulse_init(void)
{
    memset(&pulse_db, 0, sizeof(db_pulse_data_t));
    pulse_db.current_stage = 0; // Idle
}

bool db_pulse_read(db_pulse_id_t id, void* out_data)
{
    if (id >= DB_ID_PULSE_MAX || out_data == NULL) return false;
    memcpy(out_data, db_map[id].data_ptr, db_map[id].data_size);

    return true;
}

bool db_pulse_write(db_pulse_id_t id, const void* in_data)
{
    if (id >= DB_ID_PULSE_MAX || in_data == NULL) return false;
    memcpy(db_map[id].data_ptr, in_data, db_map[id].data_size);

    return true;
}

