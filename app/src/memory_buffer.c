/*
 * memory_buffer.c
 *
 *  Created on: Jan 26, 2026
 *      Author: luke
 */

#include "memory_buffer.h"
#include <stdlib.h>

#define	MEM_BUFFER_SIZE		32

typedef struct {
	mem_data_t	arr[MEM_BUFFER_SIZE];
	uint16_t	start, end;
	uint16_t	size;
} memory_buffer_t;

static memory_buffer_t buffer = { .start = 0, .end = 0, .size = 0};



mem_buffer_status_t mem_buffer_queue(bool write_mode, uint16_t dir, uint8_t * data, uint16_t size, uint16_t dev_addr, uint16_t mem_addr_size) {
	if (size > MEM_DATA_SLICE_SIZE)
		return ST_MEM_BUF_ERROR_SLICE_SIZE;

	if (buffer.size >= MEM_BUFFER_SIZE)
		return ST_MEM_BUF_ERROR_FULL;

	mem_data_t aux = { .write_mode = write_mode, .dir = dir, .size = size, .dev_addr = dev_addr, .mem_addr_size = mem_addr_size };
	for (size_t i = 0; i < size; i++)
		aux.data[i] = data[i];

	buffer.arr[buffer.end] = aux;
	buffer.end = (buffer.end + 1) % MEM_BUFFER_SIZE;
	buffer.size++;

	return ST_MEM_BUF_OK;
}

mem_data_t mem_buffer_dequeue(void) {
	mem_data_t aux = { .size = 0 };

	if (buffer.size == 0)
		return aux;

	aux = buffer.arr[buffer.start];
	buffer.start = (buffer.start + 1) % MEM_BUFFER_SIZE;
	buffer.size--;

	return aux;
}

uint16_t mem_buffer_size(void) {
	return buffer.size;
}


void mem_buffer_iterate(mem_data_iterator_callback_t callback, void * aux_data) {
	uint16_t it = buffer.start;
	uint16_t cnt = 0;

	while (cnt < buffer.size) {
		callback(&buffer.arr[it], aux_data);

		cnt++;
		it = (it + 1) % MEM_BUFFER_SIZE;
	}
}



