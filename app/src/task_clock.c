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
#include "utils.h"
#include "task_clock.h"
#include "task_clock_attribute.h"


/********************** macros and definitions *******************************/
#define G_TASK_CLOCK_CNT_INIT			0ul
#define G_TASK_CLOCK_TICK_CNT_INI		0ul

/* Number of ticks for the clock measurement and the starting value */
#define DEL_CLOCK_TICK_MAX			50ul
#define DEL_CLOCK_TICK_INIT			0ul

#define MICROSECONDS_IN_1_SECOND	1000000u

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


static uint8_t days_in_month[] = {
		31, // jan
		28,	// feb
		31,	// mar
		30,	// apr
		31,	// may
		30,	// jun
		31,	// jul
		31,	// aug
		30,	// sep
		31,	// oct
		30,	// nov
		31	// dec
};

static date_time_t clock = {
		.year		= 2026,
		.month		= JANUARY,
		.day		= 1,
		.hours		= 0,
		.minutes	= 0,
		.seconds	= 0
};

static uint32_t us_counter = 0;	// contador de microsegundos para ver cuándo pasa un segundo
static uint32_t prev_time = 0;	// última medición del clock del microcontrolador

/********************** internal functions declaration ***********************/
void task_clock_statechart(shared_data_type * parameters);
bool second_elapsed(void);
uint8_t last_day(uint16_t year, month_t month);
bool is_leap_year(uint16_t year);
void increase_second(void);

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
	clock.year = year;
}

void clock_config_set_month(month_t month) {
	clock.month = CLAMP(month, 0, 11);
}

void clock_config_set_day(uint8_t day) {
	uint8_t max_day = last_day(clock.year, clock.month);
	clock.day = CLAMP(day, 1, max_day);
}

void clock_config_set_hour(uint8_t hour) {
	clock.hours = CLAMP(hour, 0, 23);
}

void clock_config_set_minute(uint8_t minute) {
	clock.minutes = CLAMP(minute, 0, 59);
}

date_time_t clock_get_time(void) {
	return clock;
}

// días acumulados en cada mes
static uint16_t cumulative_days[] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };

date_time_t timestamp_to_datetime(uint32_t timestamp) {
	date_time_t date;

	date.seconds = timestamp % 60;

	uint32_t minutes = timestamp / 60;
	date.minutes = minutes % 60;

	uint32_t hours = minutes / 60;
	date.hours = hours % 60;

	uint32_t days = hours / 24;

	uint16_t year = 2000;
	uint32_t days_in_year = 365 + (is_leap_year(year) ? 1 : 0);
	while (days > days_in_year) {
		year++;
		days -= days_in_year;
		days_in_year = 365 + (is_leap_year(year) ? 1 : 0);
	}
	date.year = year;

	bool leap_year = is_leap_year(year);
	for (uint16_t month = 0; month < DECEMBER; month++) {
		uint16_t leap_day = (leap_year && month >= FEBRUARY) ? 1 : 0;
		if (days <= cumulative_days[month] + leap_day) {
			date.month = month;
			days -= cumulative_days[month];
			days -= (leap_year && month > FEBRUARY) ? 1 : 0;
			break;
		}
	}
	date.day = days + 1; // Días transcurridos más el del día actual

	return date;
}

uint32_t datetime_to_timestamp(date_time_t * date) {
	if (!date)
		return 0;

	uint16_t leap_day = (is_leap_year(date->year) && date->month > FEBRUARY) ? 1 : 0;

	uint16_t cumulative_leaps = 0;
	for (uint16_t year = 2000; year < date->year; year++)
		cumulative_leaps += is_leap_year(year) ? 1 : 0;

	uint16_t days = (date->day - 1);	// días pasados en el mes
	days += cumulative_days[date->month] + leap_day;	// días acumulados en los meses pasados y el bisiesto
	days += (date->year - 2000) * 365 + cumulative_leaps;	// días acumulados en los años pasados y bisiestos

	uint32_t seconds = date->seconds;
	seconds += date->minutes * 60;
	seconds += date->hours * 3600;	// 60 * 60
	seconds += days * 86400;		// 24 * 60 * 60

	return seconds;
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
			increase_second();

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

uint8_t last_day(uint16_t year, month_t month) {
	return days_in_month[month] + (month == FEBRUARY && is_leap_year(year) ? 1 : 0);
}


bool is_leap_year(uint16_t year) {
	if (year % 4 != 0)
		return false;

	if (year % 100 == 0 && year % 400 != 0)
		return false;

	return true;
}

void increase_second(void) {
	clock.seconds++;
	if (clock.seconds < 60)
		return;

	clock.seconds = 0;
	clock.minutes++;
	if (clock.minutes < 60)
		return;

	clock.minutes = 0;
	clock.hours++;
	if (clock.hours < 24)
		return;

	clock.hours = 0;
	clock.day++;

	if (clock.day <= last_day(clock.year, clock.month))
		return;

	clock.day = 1;
	clock.month++;

	if (clock.month <= DECEMBER)
		return;

	clock.month = JANUARY;
	clock.year++;

	return;
}

/********************** end of file ******************************************/
