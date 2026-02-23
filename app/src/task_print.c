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
/*
		if (!first_run) {
			first_run = true;

			float aux;
			aux = 20;
			memory_write_config_field(MEM_CFG_HUMIDITY_MIN, &aux);
			aux = 80;
			memory_write_config_field(MEM_CFG_HUMIDITY_MAX, &aux);
			aux = 15;
			memory_write_config_field(MEM_CFG_TEMP_DAY_MIN, &aux);
			aux = 40;
			memory_write_config_field(MEM_CFG_TEMP_DAY_MAX, &aux);
			aux = 10;
			memory_write_config_field(MEM_CFG_TEMP_NIGHT_MIN, &aux);
			aux = 30;
			memory_write_config_field(MEM_CFG_TEMP_NIGHT_MAX, &aux);
			aux = 10000;
			memory_write_config_field(MEM_CFG_LIGHT_THRESHOLD, &aux);
			aux = 8;
			memory_write_config_field(MEM_CFG_LIGHT_HOURS_NEEDED, &aux);
			aux = 2;
			memory_write_config_field(MEM_CFG_SAVE_FREQ, &aux);
		}
*/

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
