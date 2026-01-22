/*
 * memory.h
 *
 *  Created on: Jan 22, 2026
 *      Author: luke
 */

#ifndef INC_MEMORY_H_
#define INC_MEMORY_H_

typedef enum {
	ST_MEM_OK,
	ST_MEM_FAIL,
	STM_MEM_NULL_PTR
} mem_status_t;

typedef enum {
	MEM_CFG_HUMIDITY,
	MEM_CFG_LIGHT,
	MEM_CFG_TEMP,
	MEM_CFG_SAVE_FREQ
} mem_type_cfg_t;

typedef enum {
	MEM_LOG_HUMIDITY,
	MEM_LOG_LIGHT,
	MEM_LOG_TEMP
} mem_type_log_t;

typedef struct {
	float humidity;
	float light;
	float temp;
	float save_freq;
} mem_cfg_t;

typedef struct {
	mem_type_log_t type;
	float value;
	uint32_t timestamp;	// Segundos desde 01/01/2000
} mem_log_t;

mem_status_t memory_write_config(mem_type_cfg_t type, float valor, bool * is_finished);
mem_status_t memory_read_config(mem_type_cfg_t type, float * valor, bool * is_finished);
mem_status_t memory_append_log(mem_type_log_t type, float valor, bool * is_finished);
mem_status_t memory_read_log(size_t idx, mem_log_t * data, *is_finished);
uint32_t memory_log_size();

#endif /* INC_MEMORY_H_ */
