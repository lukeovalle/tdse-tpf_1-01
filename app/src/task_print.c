/*
 * task_print.c
 *
 *  Created on: Nov 7, 2025
 *      Author: Hikar unu
 */

/********************** inclusions *******************************************/
/* Project includes */
#include "main.h"

/* Demo includes */
#include "logger.h"
#include "dwt.h"

/* Application & Tasks includes */
#include "board.h"
#include "app.h"
#include "ext_memory.h"
#include "memory_buffer.h"
#include <stdbool.h>

/********************** macros and definitions *******************************/
#define G_TASK_PRINT_CNT_INIT           0ul
#define G_TASK_PRINT_TICK_CNT_INI  		0ul



/********************** external data declaration ****************************/
uint32_t g_task_print_cnt;
volatile uint32_t g_task_print_tick_cnt;
extern I2C_HandleTypeDef hi2c1;

/********************** internal data declaration ****************************/
uint16_t counter = 0;
mem_cfg_t config = { .humidity = 30.0, .temp = 99.0, .light = 69.0, .save_freq = 3.0 };
mem_cfg_t read;

bool saved = false;
bool is_read = false;
uint32_t contador = 0;
uint16_t ultimo_buffer_size = 0;
/********************** internal functions declaration ***********************/


/********************** internal data definition *****************************/
const char *p_task_print 		= "Task Light Sensor (Sensor Statechart)";
const char *p_task_print_ 		= "Non-Blocking & Update By Time Code";



/********************** external functions definition ************************/
extern void task_print_init(void *parameters) {
	return;
}
extern void task_print_update(void *parameters) {
	bool b_time_update_required = false;

	/* Protect shared resource */
	__asm("CPSID i");	/* disable interrupts */
    if (g_task_print_tick_cnt > G_TASK_PRINT_TICK_CNT_INI) {
		/* Update Tick Counter */
    	g_task_print_tick_cnt--;
    	b_time_update_required = true;
    }
    __asm("CPSIE i");	/* enable interrupts */

    while (b_time_update_required) {
		/* Update Task Counter */
		g_task_print_cnt++;

    	/* Protect shared resource */
		__asm("CPSID i");	/* disable interrupts */
		if (g_task_print_tick_cnt > G_TASK_PRINT_TICK_CNT_INI) {
			/* Update Tick Counter */
			g_task_print_tick_cnt--;
			b_time_update_required = true;
		} else {
			b_time_update_required = false;
		}
		__asm("CPSIE i");	/* enable interrupts */

		uint16_t buffer_size = mem_buffer_size();
		if (ultimo_buffer_size != buffer_size) {
			LOGGER_LOG("buffer size: %u\n", buffer_size);
			ultimo_buffer_size = buffer_size;
		}

		if (!saved) {
			memory_write_config_field(MEM_CFG_HUMIDITY, &config.humidity);
			memory_write_config_field(MEM_CFG_LIGHT, &config.light);
			memory_write_config_field(MEM_CFG_TEMP, &config.temp);
			memory_write_config_field(MEM_CFG_SAVE_FREQ, &config.save_freq);

			saved = true;
		}

		contador++;

		if (contador >= 5000 && !is_read) {
			LOGGER_LOG("va a leer memoria\n");

			for (uint16_t i = 0; i < 16; i++) {
				uint8_t byte_leido = 0;
				HAL_I2C_Mem_Read_IT(&hi2c1, 0xA0, i+1, I2C_MEMADD_SIZE_8BIT, &byte_leido, 1);
				HAL_Delay(40);

				LOGGER_LOG("byte %u:\t%x\n", i+1, byte_leido);

			}

			is_read = true;
		}

/*
		if (contador != 7000)
			continue;

		uint16_t mem_addr = 0x67;
		uint16_t dev_addr = 0xA0 | 0x02;
		uint8_t data = 69;

		mem_buffer_queue(mem_addr, &data, 1, dev_addr, I2C_MEMADD_SIZE_8BIT);

		mem_data_t datito = mem_buffer_dequeue();

		HAL_I2C_Mem_Write_IT(&hi2c1, datito.dev_addr, datito.dir, datito.mem_addr_size, datito.data, datito.size);
		HAL_Delay(10);

		uint8_t data_2 = 0;
		HAL_I2C_Mem_Read_IT(&hi2c1, dev_addr, mem_addr, I2C_MEMADD_SIZE_8BIT, &data_2, 1);

		HAL_Delay(20);
		LOGGER_LOG("Valor original: %u\t valor leído: %u\n", data, data_2);
*/

    }
}
