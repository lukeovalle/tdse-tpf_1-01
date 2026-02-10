/*
 * memory_buffer.h
 *
 *  Created on: Jan 26, 2026
 *      Author: luke
 */

#ifndef INC_MEMORY_BUFFER_H_
#define INC_MEMORY_BUFFER_H_

//#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#define MEM_DATA_SLICE_SIZE 16

typedef enum {
	ST_MEM_BUF_OK = 0,
	ST_MEM_BUF_ERROR_FULL,
	ST_MEM_BUF_ERROR_SLICE_SIZE,
} mem_buffer_status_t;

typedef struct {
	bool write_mode; // True para escribir, false para leer
	uint16_t dev_addr, mem_addr_size;
	uint16_t dir;
	uint8_t	data[MEM_DATA_SLICE_SIZE];
	uint16_t size;
} mem_data_t;

mem_buffer_status_t mem_buffer_queue(bool write_mode, uint16_t dir, uint8_t * data, uint16_t size, uint16_t dev_addr, uint16_t mem_addr_size);
mem_data_t			mem_buffer_dequeue(void);	// Si el data.size es 0, es porque la cola estaba vacía
uint16_t			mem_buffer_size(void);

typedef void (*mem_data_iterator_callback_t)(mem_data_t * data, void * aux_data);
void mem_buffer_iterate(mem_data_iterator_callback_t callback, void * aux_data);

#endif /* INC_MEMORY_BUFFER_H_ */
