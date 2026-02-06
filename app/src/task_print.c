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

bool read = false;

/********************** external functions definition ************************/
extern void task_print_init(void *parameters) {

	clock_config_set_year(2067);
	clock_config_set_month(AUGUST);
	clock_config_set_day(80);
	clock_config_set_hour(16);
	clock_config_set_minute(20);

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

		if (!read) {
			date_time_t reloj = clock_get_time();
			LOGGER_LOG("%4u/%02u/%02u %02u:%02u:%02u\n", reloj.year, reloj.month+1, reloj.day, reloj.hours, reloj.minutes, reloj.seconds);
			read = true;

			reloj.year = 2024;
			reloj.month = FEBRUARY;
			reloj.day = 29;
			reloj.hours = 18;
			reloj.minutes = 57;
			reloj.seconds = 45;

			LOGGER_LOG("%4u/%02u/%02u %02u:%02u:%02u\n", reloj.year, reloj.month+1, reloj.day, reloj.hours, reloj.minutes, reloj.seconds);
			uint32_t timestamp = datetime_to_timestamp(&reloj);
			LOGGER_LOG("timestamp: %lu\n", timestamp);
			reloj = timestamp_to_datetime(timestamp);
			LOGGER_LOG("%4u/%02u/%02u %02u:%02u:%02u", reloj.year, reloj.month+1, reloj.day, reloj.hours, reloj.minutes, reloj.seconds);
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
