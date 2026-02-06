/*
 * memory.c
 *
 *  Created on: Jan 22, 2026
 *      Author: luke
 */

/* Includes */
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "main.h"
#include "ext_memory.h"
#include "memory_buffer.h"
#include "task_i2c.h"
#include "task_i2c_attribute.h"
#include "task_clock.h"

/* macros and definitions */
typedef struct {
	uint8_t start;
	uint8_t end;
	uint8_t size;
} log_ring_limits_t;

#define MEMORY_CONFIG_ADDR		0x00
#define	MEMORY_CONFIG_SIZE		sizeof(mem_cfg_t)
#define	MEMORY_LOG_LIMITS_ADDR	(MEMORY_CONFIG_ADDR + MEMORY_CONFIG_SIZE)
#define	MEMORY_LOG_LIMITS_SIZE	sizeof(log_ring_limits_t)
#define	MEMORY_LOG_DATA_ADDR	(MEMORY_LOG_LIMITS_ADDR + MEMORY_LOG_LIMITS_SIZE)
#define MEMORY_LOG_DATA_SIZE	sizeof(mem_log_t)
#define MEMORY_MAX_BYTES		1024
#define MEMORY_MAX_LOGS			((MEMORY_MAX_BYTES - MEMORY_CONFIG_SIZE - MEMORY_LOG_LIMITS_SIZE) / MEMORY_LOG_DATA_SIZE)

/* External data declaration */
static log_ring_limits_t limits;
/* Internal function declarations */
mem_status_t memory_get_log_size(log_ring_limits_t * lim);
mem_buffer_status_t append_to_buffer(uint16_t data_size, uint16_t mem_addr, uint8_t * data_ptr);

/* Function definitions */
void ext_memory_init() {
	memory_get_log_size(&limits);
}

mem_status_t memory_write_config_field(mem_type_cfg_t type, float * value) {
	mem_buffer_status_t error;

	error = append_to_buffer(sizeof(float), MEMORY_CONFIG_ADDR + sizeof(float) * type, (uint8_t *)value);
	if (error)
		return ST_MEM_FAIL;

	return ST_MEM_OK;
}

mem_status_t memory_read_config(mem_cfg_t * config) {
	if(!config)
		return ST_MEM_NULL_PTR;

	bool can_read = task_i2c_request_read(DEVICE_ADDRESS_8BIT, I2C_MEMADD_SIZE_8BIT, MEMORY_CONFIG_ADDR,
			(uint8_t *)config, MEMORY_CONFIG_SIZE);

	if (can_read)
		return ST_MEM_OK;

	return ST_MEM_BUSY;
}

mem_status_t memory_append_log(float * humidity, float * light, float * temperature) {
	if(!humidity || !light || !temperature)
		return ST_MEM_NULL_PTR;

	/* Escribimos datos al final del log */
	mem_log_t log_aux = { .humidity = *humidity, .light = *light, .temperature = *temperature } ;

	date_time_t now = clock_get_time();
	log_aux.timestamp = datetime_to_timestamp(&now);

	uint16_t data_size = sizeof(mem_log_t);

	mem_buffer_status_t error;
	error = append_to_buffer(data_size, MEMORY_LOG_DATA_ADDR + limits.end * data_size,
			(uint8_t *)&log_aux);
	if (error)
		return ST_MEM_FAIL;

	/* Aumentamos el tamaño del log */
	limits.end = (limits.end + 1) % MEMORY_MAX_LOGS;
	if (limits.size == MEMORY_MAX_LOGS) {
		limits.start = (limits.start + 1) % MEMORY_MAX_LOGS;
	} else {
		limits.size++;
	}

	error = append_to_buffer(MEMORY_LOG_LIMITS_SIZE, MEMORY_LOG_LIMITS_ADDR, (uint8_t *)&limits);
	if (error)
		return ST_MEM_FAIL;

	return ST_MEM_OK;
}

mem_status_t memory_read_log_range(uint32_t start, uint32_t size, mem_log_t * data) {
	if (!data)
		return ST_MEM_NULL_PTR;

	if (start + size > memory_log_size())
		return ST_MEM_FAIL;

	uint16_t real_start = (limits.start + start) % MEMORY_MAX_LOGS;

	uint16_t mem_addr = MEMORY_LOG_DATA_ADDR + real_start * sizeof(mem_log_t);
	uint8_t mem_addr_high_bits = (mem_addr >> 8) & 0x03;
	uint8_t dev_addr = DEVICE_ADDRESS_8BIT | (mem_addr_high_bits << 1);

	/* TODO: ver que pasa si queremos leer un bloque no continuo de memoria */
	bool can_read = task_i2c_request_read(dev_addr, I2C_MEMADD_SIZE_8BIT, mem_addr & 0xFF,
			(uint8_t *)data, size * sizeof(mem_log_t));

	if (can_read)
		return ST_MEM_OK;

	return ST_MEM_BUSY;
}

mem_status_t memory_clear_log(void) {
	limits.start = 0;
	limits.end = 0;
	limits.size = 0;

	if (append_to_buffer(MEMORY_LOG_LIMITS_SIZE, MEMORY_LOG_LIMITS_ADDR, (uint8_t *)&limits))
		return ST_MEM_FAIL;

	return ST_MEM_OK;
}

uint8_t memory_log_size(void) {
	return limits.size;
}

bool memory_finished_reading(void) {
	return task_i2c_finished_reading();
}

bool memory_finished_writing(void) {
	return task_i2c_finished_writing();
}


/* Funciones internas  */

mem_status_t memory_get_log_size(log_ring_limits_t * lim) {
	if (!lim)
		return ST_MEM_NULL_PTR;

	bool can_read = task_i2c_request_read(DEVICE_ADDRESS_8BIT, I2C_MEMADD_SIZE_8BIT, MEMORY_LOG_LIMITS_ADDR,
			(uint8_t *)lim, MEMORY_LOG_LIMITS_SIZE);

	if (can_read)
		return ST_MEM_OK;

	return ST_MEM_BUSY;
}

mem_buffer_status_t append_to_buffer(uint16_t data_size, uint16_t mem_addr, uint8_t * data_ptr) {
	mem_buffer_status_t error;

	uint16_t remaining_data_size = data_size;
	while (remaining_data_size) {
		uint16_t space_in_page = MEM_PAGE_SIZE_BYTES - (mem_addr % MEM_PAGE_SIZE_BYTES);	// ej. si mem_addr = 17, space_in_page = 15
		data_size = (remaining_data_size < space_in_page) ? remaining_data_size : space_in_page; // menor entre remaining_data_size y space_in_page

		uint8_t mem_addr_high_bits = (mem_addr >> 8) & 0x03;
		uint8_t dev_addr = DEVICE_ADDRESS_8BIT | (mem_addr_high_bits << 1);
		error = mem_buffer_queue(mem_addr & 0xFF, data_ptr, data_size, dev_addr, I2C_MEMADD_SIZE_8BIT);
		if (error)
			return error;

		mem_addr += data_size;
		data_ptr += data_size;
		remaining_data_size -= data_size;
	}

	return ST_MEM_BUF_OK;
}


