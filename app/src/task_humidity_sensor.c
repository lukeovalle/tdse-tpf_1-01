/*
 * task_humidity_sensor.c
 *
 *  Created on: Nov 7, 2025
 *      Author: Hikar
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
#include "task_humidity_sensor_attribute.h"
#include "adc.h"

/********************** macros and definitions *******************************/
#define G_TASK_HUMIDITY_SEN_CNT_INIT           0ul
#define G_TASK_HUMIDITY_SEN_TICK_CNT_INI  		0ul

/* Number of ticks for the sensor measurement and the starting value */
#define DEL_HUMIDITY_SEN_TICK_MAX			50ul
#define DEL_HUMIDITY_SEN_TICK_INIT			0ul

/********************** external data declaration ****************************/
uint32_t g_task_humidity_sensor_cnt;
volatile uint32_t g_task_humidity_sensor_tick_cnt;
extern ADC_HandleTypeDef hadc1;

/********************** internal data declaration ****************************/
const task_humidity_sensor_cfg_t task_humidity_sensor_cfg_list[] = {
	{.hadc = &hadc1, .tick_max = DEL_HUMIDITY_SEN_TICK_MAX }
};

//TODO averiguar como leer canales distintos del ADC

#define HUMIDITY_SENSOR_CFG_QTY	(sizeof(task_humidity_sensor_cfg_list)/sizeof(task_humidity_sensor_cfg_t))

task_humidity_sensor_dta_t task_humidity_sensor_dta_list[] = {
	{.tick_cnt = DEL_HUMIDITY_SEN_TICK_INIT, .state = ST_HUMIDITY_WAITING, .event = EV_HUMIDITY_IDLE,
		.measure = 0.0}
};

#define SENSOR_DTA_QTY	(sizeof(task_humidity_sensor_dta_list)/sizeof(task_humidity_sensor_dta_t))

/********************** internal functions declaration ***********************/
void task_sensor_statechart(shared_data_type * parameters);
float read_humidity_sensor(ADC_HandleTypeDef * hadc);

/********************** internal data definition *****************************/
const char *p_task_humidity_sensor 		= "Task Humidity Sensor (Sensor Statechart)";
const char *p_task_humidity_sensor_ 	= "Non-Blocking & Update By Time Code";



/********************** external functions definition ************************/
void task_humidity_sensor_init(void *parameters)
{
	uint32_t index;
	task_humidity_sensor_dta_t *p_task_sensor_dta;
	task_humidity_sensor_st_t state;
	task_humidity_sensor_ev_t event;

	/* Print out: Task Initialized */
	LOGGER_INFO(" ");
	LOGGER_INFO("  %s is running - %s", GET_NAME(task_humidity_sensor_init), p_task_humidity_sensor);
	LOGGER_INFO("  %s is a %s", GET_NAME(task_humidity_sensor), p_task_humidity_sensor_);

	/* Init & Print out: Task execution counter */
	g_task_humidity_sensor_cnt = G_TASK_HUMIDITY_SEN_CNT_INIT;
	LOGGER_INFO("   %s = %lu", GET_NAME(g_task_humidity_sensor_cnt), g_task_humidity_sensor_cnt);

	for (index = 0; SENSOR_DTA_QTY > index; index++)
	{
		/* Update Task Sensor Data Pointer */
		p_task_sensor_dta = &task_humidity_sensor_dta_list[index];

		LOGGER_INFO(" ");
		LOGGER_INFO("   %s = %lu   %s = %lu   %s = %lu",
				    GET_NAME(index), index,
					GET_NAME(state), (uint32_t)state,
					GET_NAME(event), (uint32_t)event);
	}
}

void task_humidity_sensor_update(void *parameters) {
	bool b_time_update_required = false;

	/* Protect shared resource */
	__asm("CPSID i");	/* disable interrupts */
    if (g_task_humidity_sensor_tick_cnt > G_TASK_HUMIDITY_SEN_TICK_CNT_INI) {
		/* Update Tick Counter */
    	g_task_humidity_sensor_tick_cnt--;
    	b_time_update_required = true;
    }
    __asm("CPSIE i");	/* enable interrupts */

    while (b_time_update_required) {
		/* Update Task Counter */
		g_task_humidity_sensor_cnt++;

		/* Run Task Sensor Statechart */
    	task_sensor_statechart((shared_data_type *) parameters);

    	/* Protect shared resource */
		__asm("CPSID i");	/* disable interrupts */
		if (g_task_humidity_sensor_tick_cnt > G_TASK_HUMIDITY_SEN_TICK_CNT_INI) {
			/* Update Tick Counter */
			g_task_humidity_sensor_tick_cnt--;
			b_time_update_required = true;
		} else {
			b_time_update_required = false;
		}
		__asm("CPSIE i");	/* enable interrupts */
    }
}

void task_hsensor_statechart(shared_data_type * parameters) {
	uint32_t index;
	const task_humidity_sensor_cfg_t * p_task_humidity_sensor_cfg;
	task_humidity_sensor_dta_t * p_task_humidity_sensor_dta;

	for (index = 0; SENSOR_DTA_QTY > index; index++) {
		/* Update Task Sensor Configuration & Data Pointer */
		p_task_humidity_sensor_cfg = &task_humidity_sensor_cfg_list[index];
		p_task_humidity_sensor_dta = &task_humidity_sensor_dta_list[index];

		if (parameters->needs_humidity_measure)
			p_task_humidity_sensor_dta->event = EV_HUMIDITY_REQUEST;
		else
			p_task_humidity_sensor_dta->event = EV_HUMIDITY_IDLE;

		switch (p_task_humidity_sensor_dta->state) {
		case ST_HUMIDITY_WAITING:
			if (p_task_humidity_sensor_dta->event == EV_HUMIDITY_IDLE)
				p_task_humidity_sensor_dta->state = ST_HUMIDITY_WAITING;
			else if (p_task_humidity_sensor_dta->event == EV_HUMIDITY_REQUEST)
				p_task_humidity_sensor_dta->state = ST_HUMIDITY_MEASURING;

			break;

		case ST_HUMIDITY_MEASURING:
			p_task_humidity_sensor_dta->tick_cnt++;

			if(p_task_humidity_sensor_dta->tick_cnt < p_task_humidity_sensor_cfg->tick_max) {
				// TODO leer sensor humedad
				//float val = read_humidity_sensor(p_task_humidity_sensor_cfg->hadc);
				//p_task_humidity_sensor_dta->measure += val;
			} else {
				parameters->humidity_measure = p_task_humidity_sensor_dta->measure / p_task_humidity_sensor_dta->tick_cnt;
				p_task_humidity_sensor_dta->measure = 0.0;
				p_task_humidity_sensor_dta->tick_cnt = DEL_HUMIDITY_SEN_TICK_INIT;
				p_task_humidity_sensor_dta->state = ST_HUMIDITY_WAITING;
			}

			break;

		default:
			p_task_humidity_sensor_dta->tick_cnt = DEL_HUMIDITY_SEN_TICK_INIT;
			p_task_humidity_sensor_dta->event = EV_HUMIDITY_IDLE;
			p_task_humidity_sensor_dta->state = ST_HUMIDITY_WAITING;
			p_task_humidity_sensor_dta->measure = 0.0;
			break;
		}
	}
}


/********************** end of file ******************************************/


