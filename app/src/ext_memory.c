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

/* macros and definitions */
#define MEMORY_CONFIG_ADDR		0x01
#define	MEMORY_CONFIG_SIZE		sizeof(mem_type_cfg_t)
#define	MEMORY_LOG_COUNT_ADDR	(MEMORY_CONFIG_ADDR + MEMORY_CONFIG_SIZE)
#define	MEMORY_LOG_COUNT_SIZE	sizeof(uint16_t)
#define	MEMORY_LOG_DATA_ADDR	(MEMORY_LOG_COUNT_ADDR + MEMORY_LOG_COUNT_SIZE)

#define	DEVICE_ADDRESS_7BIT		0x50 /* AT24C08A 0101 000x primeros 7 bits */

/* External data declaration */
extern shared_i2c_data_t shared_i2c_data;

static uint32_t log_size;

/* Internal function declarations */
mem_status_t memory_get_log_size(uint32_t * size, bool * is_finished);
mem_buffer_status_t append_to_buffer(uint16_t data_size, uint16_t mem_addr, uint8_t * data_ptr);

/* Function definitions */
void ext_memory_init(bool * is_finished) {
	memory_get_log_size(&log_size, is_finished);
}

mem_status_t memory_write_config_field(mem_type_cfg_t type, float * value, bool * is_finished) {
	mem_buffer_status_t error;

	if (!is_finished)
		return ST_MEM_NULL_PTR;

	if (!*is_finished || !shared_i2c_data.is_i2c_finished)
		return ST_MEM_BUSY;

	error = append_to_buffer(sizeof(float), MEMORY_CONFIG_ADDR + sizeof(float) * type, (uint8_t *)value);
	if (error)
		return ST_MEM_FAIL;

	return ST_MEM_OK;
}

mem_status_t memory_read_config(mem_cfg_t * config, bool * is_finished) {
	if(!config || !is_finished)
		return ST_MEM_NULL_PTR;

	if(!*is_finished || !shared_i2c_data.is_i2c_finished)
		return ST_MEM_BUSY;

	shared_i2c_data.request_read = true;
	shared_i2c_data.dev_addr = DEVICE_ADDRESS_7BIT;
	shared_i2c_data.mem_addr = MEMORY_CONFIG_ADDR;
	shared_i2c_data.mem_addr_size = I2C_MEMADD_SIZE_16BIT;
	shared_i2c_data.data = (uint8_t *)config;
	shared_i2c_data.data_size = MEMORY_CONFIG_SIZE;

	return ST_MEM_OK;
}

mem_status_t memory_append_log(mem_type_log_t type, float * value, bool * is_finished) {
	if(!value || !is_finished)
		return ST_MEM_NULL_PTR;

	if (!*is_finished || !shared_i2c_data.is_i2c_finished)
		return ST_MEM_BUSY;

	/* Escribimos datos al final del log */
	mem_log_t log_aux = {.type = type, .value = *value } ;
	log_aux.timestamp = 67;

	uint16_t data_size = sizeof(mem_log_t);

	mem_buffer_status_t error;
	error = append_to_buffer(data_size, MEMORY_LOG_DATA_ADDR + memory_log_size() * data_size,
			(uint8_t *)&log_aux);
	if (error)
		return ST_MEM_FAIL;

	/* Aumentamos el tamaño del log */
	log_size++;

	error = append_to_buffer(sizeof(log_size), MEMORY_LOG_COUNT_ADDR, (uint8_t *)log_size);
	if (error)
		return ST_MEM_FAIL;

	return ST_MEM_OK;
}

mem_status_t memory_read_log_range(uint32_t start, uint32_t size, mem_log_t * data, bool * is_finished) {
	if (!data || !is_finished)
		return ST_MEM_NULL_PTR;

	if (!*is_finished || !shared_i2c_data.is_i2c_finished)
		return ST_MEM_BUSY;

	if (start + size > memory_log_size())
		return ST_MEM_FAIL;

	shared_i2c_data.request_read = true;
	shared_i2c_data.dev_addr = DEVICE_ADDRESS_7BIT;
	shared_i2c_data.mem_addr = MEMORY_LOG_DATA_ADDR + start * sizeof(mem_log_t);
	shared_i2c_data.mem_addr_size = I2C_MEMADD_SIZE_16BIT;
	shared_i2c_data.data = (uint8_t *)data;
	shared_i2c_data.data_size = size * sizeof(mem_log_t);

	return ST_MEM_OK;
}

uint32_t memory_log_size(void) {
	return log_size;
}

/* Funciones internas  */

mem_status_t memory_get_log_size(uint32_t * size, bool * is_finished) {
	if (!size || !is_finished)
		return ST_MEM_NULL_PTR;

	if (!*is_finished || !shared_i2c_data.is_i2c_finished)
		return ST_MEM_BUSY;

	shared_i2c_data.request_read = true;
	shared_i2c_data.dev_addr = DEVICE_ADDRESS_7BIT;
	shared_i2c_data.mem_addr = MEMORY_LOG_COUNT_ADDR;
	shared_i2c_data.mem_addr_size = I2C_MEMADD_SIZE_16BIT;
	shared_i2c_data.data = (uint8_t *)size;
	shared_i2c_data.data_size = MEMORY_LOG_COUNT_SIZE;

	return ST_MEM_OK;
}

mem_buffer_status_t append_to_buffer(uint16_t data_size, uint16_t mem_addr, uint8_t * data_ptr) {
	mem_buffer_status_t error;

	uint16_t remaining_data_size = data_size;
	while (remaining_data_size) {
		uint16_t space_in_page = MEM_PAGE_SIZE_BYTES - (mem_addr % MEM_PAGE_SIZE_BYTES);	// ej. si mem_addr = 17, space_in_page = 15
		data_size = (remaining_data_size < space_in_page) ? remaining_data_size : space_in_page; // menor entre remaining_data_size y space_in_page

		error = mem_buffer_queue(mem_addr, data_ptr, data_size, DEVICE_ADDRESS_7BIT, I2C_MEMADD_SIZE_16BIT);
		if (error)
			return error;

		mem_addr += data_size;
		data_ptr += data_size;
		remaining_data_size =- data_size;
	}

	return ST_MEM_BUF_OK;
}


