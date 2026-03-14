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
#define CYCLES_PER_US (SystemCoreClock / 1000000)

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
static void display_copy_line(char *dst, const char *src);
static bool display_find_dirty(uint8_t *row, uint8_t *col);

/********************** internal data definition *****************************/
const char *p_task_display 	= "Task Display (Display Statechart)";
const char *p_task_display_ = "Non-Blocking & Update By Time Code";

static uint32_t starting_cycles = 0;	// Ciclo en el que se empezó a escribir el caracter

static char display_target[DISPLAY_ROWS][DISPLAY_CHAR_WIDTH]; // Buffer con el contenido que se quiere mostrar en el display
static char display_shadow[DISPLAY_ROWS][DISPLAY_CHAR_WIDTH]; // Buffer con el contenido que se ha escrito en el display

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

	// Initialize display buffers
	for (uint8_t r = 0; r < DISPLAY_ROWS; r++)
    	for (uint8_t c = 0; c < DISPLAY_CHAR_WIDTH; c++)
    	{
        	display_target[r][c] = ' ';
        	display_shadow[r][c] = ' ';
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

	__asm("CPSID i");

	if (row_1) display_copy_line(display_target[0], row_1);
    if (row_2) display_copy_line(display_target[1], row_2);

	task_display_dta_t * p_task_display_dta = &task_display_dta_list[0];

	p_task_display_dta->event = EV_DISPLAY_WRITE;
	p_task_display_dta->row = 0;
	p_task_display_dta->col = 0;

	__asm("CPSIE i");
}

/********************** internal functions definition ************************/
void task_display_statechart(shared_data_type * parameters)
{
	task_display_dta_t *p_task_display_dta = &task_display_dta_list[0];

	uint32_t curr_cycles;
    uint32_t elapsed_cycles;
    uint32_t elapsed_us;

	switch (p_task_display_dta->state)
{

		case ST_DISPLAY_IDLE:

			if (p_task_display_dta->event == EV_DISPLAY_WRITE)
			{
				p_task_display_dta->event = EV_DISPLAY_IDLE;

				if (display_find_dirty(&p_task_display_dta->row, &p_task_display_dta->col))
				{
					displayCharPositionWrite(p_task_display_dta->col, p_task_display_dta->row);
					p_task_display_dta->state = ST_DISPLAY_WRITE_CHAR;
				}
			}

		break;

		case ST_DISPLAY_WRITE_CHAR:

			displayCharWrite(display_target[p_task_display_dta->row][p_task_display_dta->col]);

			display_shadow[p_task_display_dta->row][p_task_display_dta->col] = display_target[p_task_display_dta->row][p_task_display_dta->col];

			bool continue_line = true;

			p_task_display_dta->col++;

			if (p_task_display_dta->col >= DISPLAY_CHAR_WIDTH || display_target[p_task_display_dta->row][p_task_display_dta->col] != display_shadow[p_task_display_dta->row][p_task_display_dta->col])
				continue_line = false;

			starting_cycles = DWT->CYCCNT;

			if (continue_line)
				p_task_display_dta->state = ST_DISPLAY_WAIT;
			else
				p_task_display_dta->state = ST_DISPLAY_WAIT;

		break;

		case ST_DISPLAY_WAIT:

			curr_cycles = DWT->CYCCNT;
			elapsed_cycles = curr_cycles - starting_cycles;
			elapsed_us = elapsed_cycles / CYCLES_PER_US;

			if (elapsed_us >= DISPLAY_DEL_37US)
			{
				if (p_task_display_dta->col < DISPLAY_CHAR_WIDTH &&
					display_target[p_task_display_dta->row][p_task_display_dta->col] != display_shadow[p_task_display_dta->row][p_task_display_dta->col])
				{
					p_task_display_dta->state = ST_DISPLAY_WRITE_CHAR;
				}
				else
				{
					p_task_display_dta->state = ST_DISPLAY_FIND_NEXT;
				}
			}

		break;

		case ST_DISPLAY_FIND_NEXT:

			if (display_find_dirty(&p_task_display_dta->row, &p_task_display_dta->col))
			{
				displayCharPositionWrite(p_task_display_dta->col, p_task_display_dta->row);
				p_task_display_dta->state = ST_DISPLAY_WRITE_CHAR;
			}
			else
			{
				p_task_display_dta->state = ST_DISPLAY_IDLE;
			}

		break;

		default:
			p_task_display_dta->state = ST_DISPLAY_IDLE;
			p_task_display_dta->event = EV_DISPLAY_IDLE;
		break;
    }
}

static void display_copy_line(char *dst, const char *src)
{
    for (uint8_t i = 0; i < DISPLAY_CHAR_WIDTH; i++)
	{
        if (src && src[i])
            dst[i] = src[i];
        else
            dst[i] = ' ';
    }
}

static bool display_find_dirty(uint8_t *row, uint8_t *col)
{
    for (uint8_t r = 0; r < DISPLAY_ROWS; r++)
    {
        for (uint8_t c = 0; c < DISPLAY_CHAR_WIDTH; c++)
        {
            if (display_target[r][c] != display_shadow[r][c])
            {
                *row = r;
                *col = c;
                return true;
            }
        }
    }

    return false;
}

/********************** end of file ******************************************/
