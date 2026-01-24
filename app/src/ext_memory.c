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
#include "task_i2c.h"

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
static mem_log_t log_aux;

/* Internal function declarations */
mem_status_t memory_get_log_size(uint32_t * size, bool * is_finished);

/* Function definitions */
void ext_memory_init(bool * is_finished) {
	memory_get_log_size(&log_size, is_finished);
}

mem_status_t memory_write_config_field(mem_type_cfg_t type, float * valor, bool * is_finished) {
	if (!is_finished)
		return ST_MEM_NULL_PTR;

	if (!*is_finished || !shared_i2c_data.is_i2c_finished)
		return ST_MEM_BUSY;

	shared_i2c_data.request_write = true;
	shared_i2c_data.dev_addr = DEVICE_ADDRESS_7BIT;
	shared_i2c_data.mem_addr = MEMORY_CONFIG_ADDR + sizeof(float) * type; // Dirección + offset del campo a guardar
	shared_i2c_data.mem_addr_size = I2C_MEMADD_SIZE_16BIT;
	shared_i2c_data.data = (uint8_t *)valor;
	shared_i2c_data.data_size = sizeof(float);

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

	log_aux.type = type;
	log_aux.value =  *value;
	log_aux.timestamp = 67;

	shared_i2c_data.request_write = true;
	shared_i2c_data.dev_addr = DEVICE_ADDRESS_7BIT;
	shared_i2c_data.mem_addr = MEMORY_LOG_DATA_ADDR + memory_log_size() * sizeof(mem_log_t);
	shared_i2c_data.mem_addr_size = I2C_MEMADD_SIZE_16BIT;
	shared_i2c_data.data = (uint8_t *)&log_aux;
	shared_i2c_data.data_size = sizeof(mem_log_t);

	log_size++;

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
