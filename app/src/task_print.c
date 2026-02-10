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
#include "task_clock.h"
#include <stdbool.h>

/********************** macros and definitions *******************************/
#define G_TASK_PRINT_CNT_INIT           0ul
#define G_TASK_PRINT_TICK_CNT_INI  		0ul



/********************** external data declaration ****************************/
uint32_t g_task_print_cnt;
volatile uint32_t g_task_print_tick_cnt;

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/


/********************** internal data definition *****************************/
const char *p_task_print 		= "Task Light Sensor (Sensor Statechart)";
const char *p_task_print_ 		= "Non-Blocking & Update By Time Code";

bool first_run = false, read = false;
uint16_t counter_task = 0;

mem_cfg_t config_read;
mem_log_t log_leidos[30];

/********************** external functions definition ************************/
extern void task_print_init(void *parameters) {
	clock_config_set_year(2026);
	clock_config_set_month(FEBRUARY);
	clock_config_set_day(10);
	clock_config_set_hour(11);
	clock_config_set_minute(30);

	date_time_t reloj = clock_get_time();
	LOGGER_LOG("%4u/%02u/%02u %02u:%02u:%02u\n", reloj.year, reloj.month+1, reloj.day, reloj.hours, reloj.minutes, reloj.seconds);


	return;
}


extern void task_print_update(void *parameters) {
	bool b_time_update_required = false;

	mem_status_t error;

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

		if (counter_task < 16)
			counter_task++;

		if (!first_run && counter_task > 3) {
			first_run = true;

			float val = 5.0;
			error = memory_write_config_field(MEM_CFG_LIGHT_MIN, &val);
			if (error) {
				LOGGER_LOG("Error %d en la línea %d y archivo %s\n", error, __LINE__, __FILE__);
			}

			val = 1000.0;
			error = memory_write_config_field(MEM_CFG_LIGHT_MAX, &val);
			if (error) {
				LOGGER_LOG("Error %d en la línea %d y archivo %s\n", error, __LINE__, __FILE__);
			}

			float h = 69, l = 67, t = 420;
			error = memory_append_log(&h, &l, &t);
			if (error) {
				LOGGER_LOG("Error %d en la línea %d y archivo %s\n", error, __LINE__, __FILE__);
			}

			LOGGER_LOG("Terminó de escribir? %d\n",	memory_finished_writing());
		}



		if (first_run && !read && counter_task > 10) {
			read = true;

			error = memory_read_config(&config_read);
			if (error) {
				LOGGER_LOG("Error %d en la línea %d y archivo %s\n", error, __LINE__, __FILE__);
			}
			if (!error) {
				LOGGER_LOG("Leyó config bien\n");
			}

			uint16_t log_size = memory_log_size();
			LOGGER_LOG("log size: %u\n", log_size);

			error = memory_read_log_range(0, log_size, log_leidos);
			if (error) {
				LOGGER_LOG("Error %d en la línea %d y archivo %s\n", error, __LINE__, __FILE__);
			}
		}

		if (memory_finished_reading() && counter_task < 15) {
			mem_log_t log = log_leidos[0];
			LOGGER_LOG("primer log: temp: %0lx\thumedad: %0lx\tluz: %0lx\ttiempo: %lu\n", log.temperature, log.humidity, log.light, log.timestamp);
		}

		if (counter_task == 15)  {
			//memory_clear_log();
			uint16_t log_size = memory_log_size();
			LOGGER_LOG("log size: %u\n", log_size);

			LOGGER_LOG("Terminó de escribir? %d\n",	memory_finished_writing());
			LOGGER_LOG("Terminó de leer? %d\n",	memory_finished_reading());
		}




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


    }
}
