/*
 * task_display.c
 *
 *  Created on: Mar 10, 2026
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
#include "display.h"
#include "task_display.h"
#include "task_display_attribute.h"


/********************** macros and definitions *******************************/
#define G_TASK_DISPLAY_CNT_INIT			0ul
#define G_TASK_DISPLAY_TICK_CNT_INI		0ul

/* Number of ticks for the display measurement and the starting value */
#define DEL_DISPLAY_TICK_MAX			50ul
#define DEL_DISPLAY_TICK_INIT			0ul


/********************** external data declaration ****************************/
uint32_t g_task_display_cnt;
volatile uint32_t g_task_display_tick_cnt;

/********************** internal data declaration ****************************/
const task_display_cfg_t task_display_cfg_list[] = {
		{ }
};

#define DISPLAY_CFG_QTY	(sizeof(task_display_cfg_list)/sizeof(task_display_cfg_t))

task_display_dta_t task_display_dta_list[] = {
	{ .state = ST_DISPLAY_IDLE, .event = EV_DISPLAY_IDLE, .row = 0, .col = 0 }
};

#define DISPLAY_DTA_QTY	(sizeof(task_display_dta_list)/sizeof(task_display_dta_t))

/********************** internal functions declaration ***********************/
void task_display_statechart(shared_data_type * parameters);

/********************** internal data definition *****************************/
const char *p_task_display 	= "Task Display (Display Statechart)";
const char *p_task_display_ = "Non-Blocking & Update By Time Code";

static uint32_t starting_cycles = 0;	// Ciclo en el que se empezó a escribir el caracter
static char display_buffer[DISPLAY_ROWS][DISPLAY_CHAR_WIDTH + 1];

/********************** external functions definition ************************/
void task_display_init(void *parameters)
{
	uint32_t index;
	task_display_dta_t *p_task_display_dta;
	task_display_st_t state;
	task_display_ev_t event;

	/* Print out: Task Initialized */
	LOGGER_INFO(" ");
	LOGGER_INFO("  %s is running - %s", GET_NAME(task_display_init), p_task_display);
	LOGGER_INFO("  %s is a %s", GET_NAME(task_display), p_task_display_);

	/* Init & Print out: Task execution counter */
	g_task_display_cnt = G_TASK_DISPLAY_CNT_INIT;
	LOGGER_INFO("   %s = %lu", GET_NAME(g_task_display_cnt), g_task_display_cnt);

	for (index = 0; DISPLAY_DTA_QTY > index; index++)
	{
		/* Update Task DISPLAY Data Pointer */
		p_task_display_dta = &task_display_dta_list[index];
		state = p_task_display_dta->state;
		event = p_task_display_dta->event;

		LOGGER_INFO(" ");
		LOGGER_INFO("   %s = %lu   %s = %lu   %s = %lu",
				    GET_NAME(index), index,
					GET_NAME(state), (uint32_t)state,
					GET_NAME(event), (uint32_t)event);
	}
}

void task_display_update(void *parameters) {
	bool b_time_update_required = false;

	/* Protect shared resource */
	__asm("CPSID i");	/* disable interrupts */
    if (g_task_display_tick_cnt > G_TASK_DISPLAY_TICK_CNT_INI) {
		/* Update Tick Counter */
    	g_task_display_tick_cnt--;
    	b_time_update_required = true;
    }
    __asm("CPSIE i");	/* enable interrupts */

    while (b_time_update_required) {
		/* Update Task Counter */
		g_task_display_cnt++;

		/* Run Task DISPLAY Statechart */
		shared_data_type * shared_data = (shared_data_type *) parameters;

    	task_display_statechart(shared_data);

    	/* Protect shared resource */
		__asm("CPSID i");	/* disable interrupts */
		if (g_task_display_tick_cnt > G_TASK_DISPLAY_TICK_CNT_INI) {
			/* Update Tick Counter */
			g_task_display_tick_cnt--;
			b_time_update_required = true;
		} else {
			b_time_update_required = false;
		}
		__asm("CPSIE i");	/* enable interrupts */
    }
}

void task_display_request_write(char * row_1, char * row_2) {
	if (!row_1 && !row_2)
		return;

	task_display_dta_t * p_task_display_dta = &task_display_dta_list[0];
	if (row_1) {
		snprintf(display_buffer[0], DISPLAY_CHAR_WIDTH + 1, "%-16s", row_1);
		p_task_display_dta->write_row_1 = true;
	} else
		p_task_display_dta->write_row_1 = false;

	if (row_2) {
		snprintf(display_buffer[1], DISPLAY_CHAR_WIDTH + 1, "%-16s", row_2);
		p_task_display_dta->write_row_2 = true;
	} else
		p_task_display_dta->write_row_2 = false;

	p_task_display_dta->event = EV_DISPLAY_WRITE;
	p_task_display_dta->row = 0;
	p_task_display_dta->col = 0;
}

/********************** internal functions definition ************************/
void task_display_statechart(shared_data_type * parameters) {
	uint32_t index;
	//const task_display_cfg_t * p_task_display_cfg;
	task_display_dta_t * p_task_display_dta;

	for (index = 0; DISPLAY_DTA_QTY > index; index++) {
		/* Update Task DISPLAY Configuration & Data Pointer */
		//p_task_display_cfg = &task_display_cfg_list[index];
		p_task_display_dta = &task_display_dta_list[index];

		task_display_st_t state = p_task_display_dta->state;
		switch (state) {
		case ST_DISPLAY_IDLE:
			switch (p_task_display_dta->event) {
			case EV_DISPLAY_IDLE:
				p_task_display_dta->state = ST_DISPLAY_IDLE;
				break;
			case EV_DISPLAY_WRITE:
				p_task_display_dta->state = ST_DISPLAY_WRITE_CHAR;
				p_task_display_dta->event = EV_DISPLAY_IDLE;
				break;
			default:
				break;
			}
			break;

		case ST_DISPLAY_WRITE_CHAR:
			bool have_to_write = true;
			uint8_t * p_row = &p_task_display_dta->row;
			uint8_t * p_col = &p_task_display_dta->col;

			// Si no tiene que escribir la primer fila
			if (*p_row == 0 && !p_task_display_dta->write_row_1)
				(*p_row)++;

			// Si no tiene que escribir la segunda fila
			if (*p_row == 1 && !p_task_display_dta->write_row_2) {
				p_task_display_dta->state = ST_DISPLAY_IDLE;
				have_to_write = false;
			}

			if (p_row == DISPLAY_ROWS - 1 && *p_col == DISPLAY_CHAR_WIDTH) {
				p_task_display_dta->state = ST_DISPLAY_IDLE;
				have_to_write = false;
			}

			if (have_to_write) {
				displayCharWrite(display_buffer[*p_row][(*p_col)++]);

				if (*p_col == DISPLAY_CHAR_WIDTH) {
					*p_col = 0;
					(*p_row)++;
				}

				p_task_display_dta->state = ST_DISPLAY_WAIT;
				starting_cycles = DWT->CYCCNT;
			}

			break;

		case ST_DISPLAY_WAIT:
			uint32_t curr_cycles = DWT->CYCCNT;
			uint32_t elapsed_cycles = curr_cycles - starting_cycles;

			uint32_t elapsed_us = elapsed_cycles / ( SystemCoreClock / 1000000 );

			if (elapsed_us >= DISPLAY_DEL_37US)
				p_task_display_dta->state = ST_DISPLAY_WRITE_CHAR;

			break;

		default:
			p_task_display_dta->event = EV_DISPLAY_IDLE;
			p_task_display_dta->state = ST_DISPLAY_IDLE;
			break;
		}
	}
}

/********************** end of file ******************************************/
