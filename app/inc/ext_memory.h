/*
 * memory.h
 *
 *  Created on: Jan 22, 2026
 *      Author: luke
 */

#ifndef INC_EXT_MEMORY_H_
#define INC_EXT_MEMORY_H_

#define	DEVICE_ADDRESS_8BIT		0xA0 /* AT24C08A 1010 0xxx */

typedef enum mem_status {
	ST_MEM_OK,
	ST_MEM_FAIL,
	ST_MEM_NULL_PTR,
	ST_MEM_BUSY,
	ST_MEM_FULL
} mem_status_t;

typedef enum mem_type_cfg {
	MEM_CFG_TEMP_DAY_MIN = 0,
	MEM_CFG_TEMP_DAY_MAX,
	MEM_CFG_TEMP_NIGHT_MIN,
	MEM_CFG_TEMP_NIGHT_MAX,
	MEM_CFG_HUMIDITY_MIN ,
	MEM_CFG_HUMIDITY_MAX,
	MEM_CFG_LIGHT_THRESHOLD,
	MEM_CFG_LIGHT_HOURS_NEEDED,
	MEM_CFG_SAVE_FREQ
} mem_type_cfg_t;

typedef struct mem_cfg {
	float temp_day_min, temp_day_max;
	float temp_night_min, temp_night_max;

	float humidity_min, humidity_max;

	float light_threshold;
	float light_hours_needed;

	float save_freq;
} mem_cfg_t;

typedef struct mem_log {
	float humidity, light, temperature;
	uint32_t timestamp;	// Segundos desde 01/01/2000
} mem_log_t;

void ext_memory_init(void);

mem_status_t memory_write_config_field(mem_type_cfg_t type, float * value);
mem_status_t memory_read_config(mem_cfg_t * config);

mem_status_t memory_append_log(float * humidity, float * light, float * temperature);
mem_status_t memory_read_log_range(uint32_t start, uint32_t size, mem_log_t * data);
mem_status_t memory_clear_log(void);
uint8_t memory_log_size(void);

bool memory_finished_reading(void);
bool memory_finished_writing(void);

#endif /* INC_EXT_MEMORY_H_ */
