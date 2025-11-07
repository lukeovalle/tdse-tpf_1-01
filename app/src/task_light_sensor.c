/*
 * task_light_sensor.c
 *
 *  Created on: Oct 3, 2025
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
#include "task_light_sensor_attribute.h"
#include "adc.h"

/********************** macros and definitions *******************************/
#define G_TASK_LIGHT_SEN_CNT_INIT           0ul
#define G_TASK_LIGHT_SEN_TICK_CNT_INI  		0ul

/* Number of ticks for the sensor measurement and the starting value */
#define DEL_LIGHT_SEN_TICK_MAX			50ul
#define DEL_LIGHT_SEN_TICK_INIT			0ul

/********************** external data declaration ****************************/
uint32_t g_task_light_sensor_cnt;
volatile uint32_t g_task_light_sensor_tick_cnt;
extern ADC_HandleTypeDef hadc1;

/********************** internal data declaration ****************************/
const task_light_sensor_cfg_t task_light_sensor_cfg_list[] = {
	{.hadc = &hadc1, .tick_max = DEL_LIGHT_SEN_TICK_MAX }
};

#define LIGHT_SENSOR_CFG_QTY	(sizeof(task_light_sensor_cfg_list)/sizeof(task_light_sensor_cfg_t))

task_light_sensor_dta_t task_light_sensor_dta_list[] = {
	{.tick_cnt = DEL_LIGHT_SEN_TICK_INIT, .state = ST_LIGHT_WAITING, .event = EV_LIGHT_IDLE,
		.measure = 0.0}
};

#define SENSOR_DTA_QTY	(sizeof(task_light_sensor_dta_list)/sizeof(task_light_sensor_dta_t))

/********************** internal functions declaration ***********************/
void task_sensor_statechart(shared_data_type * parameters);
float read_light_sensor(ADC_HandleTypeDef * hadc);

/********************** internal data definition *****************************/
const char *p_task_light_sensor 		= "Task Light Sensor (Sensor Statechart)";
const char *p_task_light_sensor_ 		= "Non-Blocking & Update By Time Code";



/********************** external functions definition ************************/
void task_light_sensor_init(void *parameters)
{
	uint32_t index;
	task_light_sensor_dta_t *p_task_sensor_dta;
	task_light_sensor_st_t state;
	task_light_sensor_ev_t event;

	/* Print out: Task Initialized */
	LOGGER_INFO(" ");
	LOGGER_INFO("  %s is running - %s", GET_NAME(task_light_sensor_init), p_task_light_sensor);
	LOGGER_INFO("  %s is a %s", GET_NAME(task_light_sensor), p_task_light_sensor_);

	/* Init & Print out: Task execution counter */
	g_task_light_sensor_cnt = G_TASK_LIGHT_SEN_CNT_INIT;
	LOGGER_INFO("   %s = %lu", GET_NAME(g_task_light_sensor_cnt), g_task_light_sensor_cnt);

	for (index = 0; SENSOR_DTA_QTY > index; index++)
	{
		/* Update Task Sensor Data Pointer */
		p_task_sensor_dta = &task_light_sensor_dta_list[index];

		LOGGER_INFO(" ");
		LOGGER_INFO("   %s = %lu   %s = %lu   %s = %lu",
				    GET_NAME(index), index,
					GET_NAME(state), (uint32_t)state,
					GET_NAME(event), (uint32_t)event);
	}
}

void task_light_sensor_update(void *parameters) {
	bool b_time_update_required = false;

	/* Protect shared resource */
	__asm("CPSID i");	/* disable interrupts */
    if (g_task_light_sensor_tick_cnt > G_TASK_LIGHT_SEN_TICK_CNT_INI) {
		/* Update Tick Counter */
    	g_task_light_sensor_tick_cnt--;
    	b_time_update_required = true;
    }
    __asm("CPSIE i");	/* enable interrupts */

    while (b_time_update_required) {
		/* Update Task Counter */
		g_task_light_sensor_cnt++;

		/* Run Task Sensor Statechart */
    	task_sensor_statechart((shared_data_type *) parameters);

    	/* Protect shared resource */
		__asm("CPSID i");	/* disable interrupts */
		if (g_task_light_sensor_tick_cnt > G_TASK_LIGHT_SEN_TICK_CNT_INI) {
			/* Update Tick Counter */
			g_task_light_sensor_tick_cnt--;
			b_time_update_required = true;
		} else {
			b_time_update_required = false;
		}
		__asm("CPSIE i");	/* enable interrupts */
    }
}

void task_sensor_statechart(shared_data_type * parameters) {
	uint32_t index;
	const task_light_sensor_cfg_t * p_task_light_sensor_cfg;
	task_light_sensor_dta_t * p_task_light_sensor_dta;

	for (index = 0; SENSOR_DTA_QTY > index; index++) {
		/* Update Task Sensor Configuration & Data Pointer */
		p_task_light_sensor_cfg = &task_light_sensor_cfg_list[index];
		p_task_light_sensor_dta = &task_light_sensor_dta_list[index];

		if (parameters->needs_light_measure)
			p_task_light_sensor_dta->event = EV_LIGHT_REQUEST;
		else
			p_task_light_sensor_dta->event = EV_LIGHT_IDLE;

		switch (p_task_light_sensor_dta->state) {
		case ST_LIGHT_WAITING:
			if (p_task_light_sensor_dta->event == EV_LIGHT_IDLE)
				p_task_light_sensor_dta->state = ST_LIGHT_WAITING;
			else if (p_task_light_sensor_dta->event == EV_LIGHT_REQUEST)
				p_task_light_sensor_dta->state = ST_LIGHT_MEASURING;

			break;

		case ST_LIGHT_MEASURING:
			p_task_light_sensor_dta->tick_cnt++;

			if(p_task_light_sensor_dta->tick_cnt < p_task_light_sensor_cfg->tick_max) {
				float val = read_light_sensor(p_task_light_sensor_cfg->hadc);
				p_task_light_sensor_dta->measure += val;
			} else {
				parameters->light_measure = p_task_light_sensor_dta->measure / p_task_light_sensor_dta->tick_cnt;
				p_task_light_sensor_dta->measure = 0.0;
				p_task_light_sensor_dta->tick_cnt = DEL_LIGHT_SEN_TICK_INIT;
				p_task_light_sensor_dta->state = ST_LIGHT_WAITING;
			}

			break;

		default:
			p_task_light_sensor_dta->tick_cnt = DEL_LIGHT_SEN_TICK_INIT;
			p_task_light_sensor_dta->event = EV_LIGHT_IDLE;
			p_task_light_sensor_dta->state = ST_LIGHT_WAITING;
			p_task_light_sensor_dta->measure = 0.0;
			break;
		}
	}
}

float read_light_sensor(ADC_HandleTypeDef * hadc) {
	uint16_t measure;
	float value;

	if (ADC_Poll_Read(hadc, &measure) != HAL_OK) {
		LOGGER_LOG("Error measuring light sensor\tF%s L%D\n", __FILE__, __LINE__);
		return 0.0;
	}

	float aux = (float) measure / (float) UINT16_MAX;
	value = MIN_LIGHT * (1.0 - aux) + MAX_LIGHT * aux;

	return value;
}

/********************** end of file ******************************************/

