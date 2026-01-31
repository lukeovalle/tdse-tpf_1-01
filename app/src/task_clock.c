/*
 * task_clock.c
 *
 *  Created on: Jan 30, 2026
 *      Author: luke
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
#include "task_clock.h"
#include "task_clock_attribute.h"


/********************** macros and definitions *******************************/
#define G_TASK_CLOCK_CNT_INIT			0ul
#define G_TASK_CLOCK_TICK_CNT_INI		0ul

/* Number of ticks for the clock measurement and the starting value */
#define DEL_CLOCK_TICK_MAX			50ul
#define DEL_CLOCK_TICK_INIT			0ul

/********************** external data declaration ****************************/
uint32_t g_task_clock_cnt;
volatile uint32_t g_task_clock_tick_cnt;

/********************** internal data declaration ****************************/
const task_clock_cfg_t task_clock_cfg_list[] = {
		{ }
};

#define CLOCK_CFG_QTY	(sizeof(task_clock_cfg_list)/sizeof(task_clock_cfg_t))

task_clock_dta_t task_clock_dta_list[] = {
	{ .state = ST_CLOCK_IDLE, .event = EV_CLOCK_IDLE }
};

#define CLOCK_DTA_QTY	(sizeof(task_clock_dta_list)/sizeof(task_clock_dta_t))

static uint32_t clock = 0;		// segundos que pasaron desde 01/01/2000 00:00:00
static uint32_t us_counter = 0;	// contador de microsegundos para ver cuándo pasa un segundo
static uint32_t prev_time = 0;	// última medición del clock del microcontrolador

/********************** internal functions declaration ***********************/
void task_clock_statechart(shared_data_type * parameters);
bool second_elapsed(void);

/********************** internal data definition *****************************/
const char *p_task_clock 	= "Task Clock (Clock Statechart)";
const char *p_task_clock_ 	= "Non-Blocking & Update By Time Code";

/********************** external functions definition ************************/
void task_clock_init(void *parameters)
{
	uint32_t index;
	task_clock_dta_t *p_task_clock_dta;
	task_clock_st_t state;
	task_clock_ev_t event;

	/* Print out: Task Initialized */
	LOGGER_INFO(" ");
	LOGGER_INFO("  %s is running - %s", GET_NAME(task_clock_init), p_task_clock);
	LOGGER_INFO("  %s is a %s", GET_NAME(task_clock), p_task_clock_);

	/* Init & Print out: Task execution counter */
	g_task_clock_cnt = G_TASK_CLOCK_CNT_INIT;
	LOGGER_INFO("   %s = %lu", GET_NAME(g_task_clock_cnt), g_task_clock_cnt);

	for (index = 0; CLOCK_DTA_QTY > index; index++)
	{
		/* Update Task CLOCK Data Pointer */
		p_task_clock_dta = &task_clock_dta_list[index];

		LOGGER_INFO(" ");
		LOGGER_INFO("   %s = %lu   %s = %lu   %s = %lu",
				    GET_NAME(index), index,
					GET_NAME(state), (uint32_t)state,
					GET_NAME(event), (uint32_t)event);
	}
}

void task_clock_update(void *parameters) {
	bool b_time_update_required = false;

	/* Protect shared resource */
	__asm("CPSID i");	/* disable interrupts */
    if (g_task_clock_tick_cnt > G_TASK_CLOCK_TICK_CNT_INI) {
		/* Update Tick Counter */
    	g_task_clock_tick_cnt--;
    	b_time_update_required = true;
    }
    __asm("CPSIE i");	/* enable interrupts */

    while (b_time_update_required) {
		/* Update Task Counter */
		g_task_clock_cnt++;

		/* Run Task CLOCK Statechart */
		shared_data_type * shared_data = (shared_data_type *) parameters;

		if (second_elapsed())
			task_clock_dta_list[0].event = EV_CLOCK_SECOND_ELAPSED;

    	task_clock_statechart(shared_data);

    	/* Protect shared resource */
		__asm("CPSID i");	/* disable interrupts */
		if (g_task_clock_tick_cnt > G_TASK_CLOCK_TICK_CNT_INI) {
			/* Update Tick Counter */
			g_task_clock_tick_cnt--;
			b_time_update_required = true;
		} else {
			b_time_update_required = false;
		}
		__asm("CPSIE i");	/* enable interrupts */
    }
}

void clock_config_set_year(uint16_t year) {
}

void clock_config_set_month(uint8_t month) {
}

void clock_config_set_day(uint8_t day) {
}

void clock_config_set_hour(uint8_t hour) {
}

void clock_config_set_minute(uint8_t minute) {
}

date_time_t clock_get_time(void) {
	date_time_t aux = { .year = 2000, .month = 1, .day = 1, .hour = 0, .minute = 0 };
	return aux;
}

/********************** internal functions definition ************************/
void task_clock_statechart(shared_data_type * parameters) {
	uint32_t index;
	const task_clock_cfg_t * p_task_clock_cfg;
	task_clock_dta_t * p_task_clock_dta;

	for (index = 0; CLOCK_DTA_QTY > index; index++) {
		/* Update Task CLOCK Configuration & Data Pointer */
		p_task_clock_cfg = &task_clock_cfg_list[index];
		p_task_clock_dta = &task_clock_dta_list[index];

		task_clock_st_t state = p_task_clock_dta->state;
		switch (state) {
		case ST_CLOCK_IDLE:
			switch (p_task_clock_dta->event) {
			case EV_CLOCK_IDLE:
				p_task_clock_dta->state = ST_CLOCK_IDLE;
				break;
			case EV_CLOCK_SECOND_ELAPSED:
				p_task_clock_dta->state = ST_CLOCK_INCREASE_SECOND;
				p_task_clock_dta->event = EV_CLOCK_IDLE;
				break;
			default:
				break;
			}
			break;
		case ST_CLOCK_INCREASE_SECOND:
			clock++;

			switch (p_task_clock_dta->event) {
			case EV_CLOCK_IDLE:
				p_task_clock_dta->state = ST_CLOCK_IDLE;
				break;
			case EV_CLOCK_SECOND_ELAPSED:
				p_task_clock_dta->state = ST_CLOCK_INCREASE_SECOND;
				p_task_clock_dta->event = EV_CLOCK_IDLE;
			}
		default:
			p_task_clock_dta->event = EV_CLOCK_IDLE;
			p_task_clock_dta->state = ST_CLOCK_IDLE;
			break;
		}
	}
}

#define MICROSECONDS_IN_1_SECOND	1000000u
bool second_elapsed(void) {
	uint32_t curr_time = cycle_counter_get_time_us();
	uint32_t elapsed;

	if (curr_time < prev_time) { // overflow
		elapsed = UINT32_MAX - prev_time + curr_time + 1;
	} else {
		elapsed = curr_time - prev_time;
	}
	prev_time = curr_time;

	us_counter += elapsed;
	if (us_counter < MICROSECONDS_IN_1_SECOND)
		return false;

	us_counter -= MICROSECONDS_IN_1_SECOND;
	return true;
}

/********************** end of file ******************************************/
