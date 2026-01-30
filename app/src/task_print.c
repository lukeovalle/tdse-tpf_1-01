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

// TEST: leer 64 bytes en bloque de la EEPROM (bloqueante)
void eeprom_dump_blocking(void) {
    uint8_t buf[64];
    uint16_t start = 0x00; // o 0x01 si usás config en 0x01
    uint16_t dev8 = 0xA0;  // vuestra convención 8-bit

    // esperar que la cola se procese (no haya escrituras pendientes)
    while (mem_buffer_size() > 0) {
        task_i2c_update(NULL);
        HAL_Delay(1);
    }

    // ack-polling por si el chip todavía está escribiendo
    while (HAL_I2C_IsDeviceReady(&hi2c1, dev8, 5, 5) != HAL_OK) { HAL_Delay(1); }

    HAL_StatusTypeDef st = HAL_I2C_Mem_Read(&hi2c1, dev8, start, I2C_MEMADD_SIZE_8BIT, buf, sizeof(buf), 1000);
    LOGGER_LOG("EEPROM read status: %d\n", (int)st);
    if (st == HAL_OK) {
        for (int i = 0; i < 64; ++i) {
            LOGGER_LOG("0x%02X: %02X\n", start + i, buf[i]);
        }
    }
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
			LOGGER_LOG("mem_buffer_size = %u\n", mem_buffer_size());
		}

		contador++;

		if (contador >= 5000 && !is_read) {
			LOGGER_LOG("va a leer memoria\n");

			eeprom_dump_blocking();
			is_read = true;
			break;

			// pedir lectura al task i2c (usa la cola y no pelea con task_i2c)
			if (memory_read_config(&read) == ST_MEM_OK) {
			    // esperar no-bloqueante: dejá que el scheduler/ticks corran
			    while (!memory_finished_reading()) {
			        task_i2c_update(NULL); // si necesitás forzar procesado en tests
			        HAL_Delay(1);
			    }
			    // ahora 'read' contiene los floats
			    uint8_t * p = (uint8_t *)&read;
			    for (uint16_t i = 0; i < sizeof(read); ++i) {
			        LOGGER_LOG("mem_cfg_t offset %2u:\t%02x\n", i, p[i]);
			    }
			}

			is_read = true;
			break;

			for (uint16_t i = 0; i < 16; i++) {
				uint8_t byte_leido = 0;
				HAL_I2C_Mem_Read_IT(&hi2c1, 0xA0, i+1, I2C_MEMADD_SIZE_8BIT, &byte_leido, 1);
				HAL_Delay(40);

				LOGGER_LOG("byte %2u:\t%2x\n", i+1, byte_leido);
			}

			for (uint16_t i = 0; i < sizeof(read); i++) {
				uint8_t * inicio = (uint8_t *)&read;

				LOGGER_LOG("mem_cfg_t offset %2u:\t%2x\n", i, *(inicio + i));
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
