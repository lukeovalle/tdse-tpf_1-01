/*
 * task_actuator.c
 *
 *  Created on: Feb 5, 2026
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
#include "task_actuator_attribute.h"
#include "adc.h"

/********************** macros and definitions *******************************/
#define G_TASK_ACT_CNT_INIT		0ul
#define G_TASK_ACT_TICK_CNT_INI	0ul

/********************** external data declaration ****************************/
uint32_t g_task_actuator_cnt;
volatile uint32_t g_task_actuator_tick_cnt;

/********************** internal data declaration ****************************/
const task_actuator_cfg_t task_actuator_cfg_list[] = {
	{ .name = ACTUATOR_PUMP, 	.gpio_port = GPIOC,	.gpio_pin = GPIO_PIN_8 },
	{ .name = ACTUATOR_FAN,		.gpio_port = GPIOC,	.gpio_pin = GPIO_PIN_9 }
};

#define ACTUATOR_CFG_QTY	(sizeof(task_actuator_cfg_list)/sizeof(task_actuator_cfg_t))

task_actuator_dta_t task_actuator_dta_list[] = {
	{ .state = ST_ACTUATOR_OFF, .event = EV_ACTUATOR_IDLE },
	{ .state = ST_ACTUATOR_OFF, .event = EV_ACTUATOR_IDLE }
};

#define ACTUATOR_DTA_QTY	(sizeof(task_actuator_dta_list)/sizeof(task_actuator_dta_t))

/********************** internal functions declaration ***********************/
void task_actuator_statechart(shared_data_type * parameters);
void actuator_on(task_actuator_cfg_t * cfg);
void actuator_off(task_actuator_cfg_t * cfg);

/********************** internal data definition *****************************/
const char *p_task_actuator 		= "Task Actuator (Actuator Statechart)";
const char *p_task_actuator_ 		= "Non-Blocking & Update By Time Code";

/********************** external functions definition ************************/
void task_actuator_init(void *parameters) {
	uint32_t index;
	task_actuator_dta_t *p_task_actuator_dta;
	task_actuator_st_t state;
	task_actuator_ev_t event;

	/* Print out: Task Initialized */
	LOGGER_INFO(" ");
	LOGGER_INFO("  %s is running - %s", GET_NAME(task_actuator_init), p_task_actuator);
	LOGGER_INFO("  %s is a %s", GET_NAME(task_actuator), p_task_actuator_);

	/* Init & Print out: Task execution counter */
	g_task_actuator_cnt = G_TASK_ACT_CNT_INIT;
	LOGGER_INFO("   %s = %lu", GET_NAME(g_task_actuator_cnt), g_task_actuator_cnt);

	for (index = 0; ACTUATOR_DTA_QTY > index; index++) {
		/* Update Task Actuator Data Pointer */
		p_task_actuator_dta = &task_actuator_dta_list[index];

		LOGGER_INFO(" ");
		LOGGER_INFO("   %s = %lu   %s = %lu   %s = %lu",
				    GET_NAME(index), index,
					GET_NAME(state), (uint32_t)state,
					GET_NAME(event), (uint32_t)event);
	}
}

void task_actuator_update(void *parameters) {
	bool b_time_update_required = false;

	/* Protect shared resource */
	__asm("CPSID i");	/* disable interrupts */
    if (g_task_actuator_tick_cnt > G_TASK_ACT_TICK_CNT_INI) {
		/* Update Tick Counter */
    	g_task_actuator_tick_cnt--;
    	b_time_update_required = true;
    }
    __asm("CPSIE i");	/* enable interrupts */

    while (b_time_update_required) {
		/* Update Task Counter */
		g_task_actuator_cnt++;

		/* Run Task Actuator Statechart */
		shared_data_type * shared_data = (shared_data_type *) parameters;

    	task_actuator_statechart(shared_data);

    	/* Protect shared resource */
		__asm("CPSID i");	/* disable interrupts */
		if (g_task_actuator_tick_cnt > G_TASK_ACT_TICK_CNT_INI) {
			/* Update Tick Counter */
			g_task_actuator_tick_cnt--;
			b_time_update_required = true;
		} else {
			b_time_update_required = false;
		}
		__asm("CPSIE i");	/* enable interrupts */
    }
}

void actuator_request_turn_on(actuator_name_t actuator) {
	if (actuator > ACTUATOR_DTA_QTY)
		return;

	task_actuator_dta_list[actuator].event = EV_ACTUATOR_TURN_ON;
}

void actuator_request_turn_off(actuator_name_t actuator) {
	if (actuator > ACTUATOR_DTA_QTY)
		return;

	task_actuator_dta_list[actuator].event = EV_ACTUATOR_TURN_OFF;
}

/********************** internal functions definition ************************/
void task_actuator_statechart(shared_data_type * parameters) {
	uint32_t index;
	const task_actuator_cfg_t * p_task_actuator_cfg;
	task_actuator_dta_t * p_task_actuator_dta;

	for (index = 0; ACTUATOR_DTA_QTY > index; index++) {
		/* Update Task Actuator Configuration & Data Pointer */
		p_task_actuator_cfg = &task_actuator_cfg_list[index];
		p_task_actuator_dta = &task_actuator_dta_list[index];

		task_actuator_st_t state = p_task_actuator_dta->state;
		task_actuator_ev_t event = p_task_actuator_dta->event;
		switch (state) {
		case ST_ACTUATOR_OFF:
			switch (event) {
			case EV_ACTUATOR_IDLE:
				break;

			case EV_ACTUATOR_TURN_OFF:
				p_task_actuator_dta->event = EV_ACTUATOR_IDLE;
				break;

			case EV_ACTUATOR_TURN_ON:
				actuator_on(p_task_actuator_cfg);
				p_task_actuator_dta->state = ST_ACTUATOR_ON;
				p_task_actuator_dta->event = EV_ACTUATOR_IDLE;
				break;

			default:
				break;
			}

			break;

		case ST_ACTUATOR_ON:
			switch (event) {
			case EV_ACTUATOR_IDLE:
				break;

			case EV_ACTUATOR_TURN_OFF:
				actuator_off(p_task_actuator_cfg);
				p_task_actuator_dta->state = ST_ACTUATOR_OFF;
				p_task_actuator_dta->event = EV_ACTUATOR_IDLE;
				break;

			case EV_ACTUATOR_TURN_ON:
				p_task_actuator_dta->event = EV_ACTUATOR_IDLE;
				break;

			default:
				break;
			}

			break;

		default:
			p_task_actuator_dta->event = EV_ACTUATOR_IDLE;
			p_task_actuator_dta->state = ST_ACTUATOR_OFF;
			break;
		}
	}
}

void actuator_on(task_actuator_cfg_t * cfg) {
	HAL_GPIO_WritePin(cfg->gpio_port, cfg->gpio_pin, GPIO_PIN_SET);
}

void actuator_off(task_actuator_cfg_t * cfg) {
	HAL_GPIO_WritePin(cfg->gpio_port, cfg->gpio_pin, GPIO_PIN_RESET);
}

/********************** end of file ******************************************/
