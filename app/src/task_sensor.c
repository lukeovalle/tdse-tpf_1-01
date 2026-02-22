/*
 * task_sensor.c
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
#include "task_sensor_attribute.h"
#include "adc.h"

/********************** macros and definitions *******************************/
#define G_TASK_SEN_CNT_INIT           0ul
#define G_TASK_SEN_TICK_CNT_INI  		0ul

/* Number of ticks for the sensor measurement and the starting value */
#define DEL_SEN_TICK_MAX			50ul
#define DEL_SEN_TICK_INIT			0ul

/********************** external data declaration ****************************/
uint32_t g_task_sensor_cnt;
volatile uint32_t g_task_sensor_tick_cnt;
extern ADC_HandleTypeDef hadc1;

/********************** internal data declaration ****************************/
const task_sensor_cfg_t task_sensor_cfg_list[] = {
	{ .name = SENSOR_LIGHT,		.tick_max = DEL_SEN_TICK_MAX, .min_val = MIN_LIGHT,		.max_val = MAX_LIGHT },
	{ .name = SENSOR_TEMP,		.tick_max = DEL_SEN_TICK_MAX, .min_val = MIN_TEMP,		.max_val = MAX_TEMP },
	{ .name = SENSOR_HUMIDITY,	.tick_max = DEL_SEN_TICK_MAX, .min_val = MIN_HUMIDITY,	.max_val = MAX_HUMIDITY }
};

#define SENSOR_CFG_QTY	(sizeof(task_sensor_cfg_list)/sizeof(task_sensor_cfg_t))

task_sensor_dta_t task_sensor_dta_list[] = {
	{.tick_cnt = DEL_SEN_TICK_INIT, .state = ST_SENSOR_IDLE, .event = EV_SENSOR_IDLE,
		.measure = 0.0},
	{.tick_cnt = DEL_SEN_TICK_INIT, .state = ST_SENSOR_IDLE, .event = EV_SENSOR_IDLE,
		.measure = 0.0},
	{.tick_cnt = DEL_SEN_TICK_INIT, .state = ST_SENSOR_IDLE, .event = EV_SENSOR_IDLE,
		.measure = 0.0}
};

#define SENSOR_DTA_QTY	(sizeof(task_sensor_dta_list)/sizeof(task_sensor_dta_t))

static volatile uint16_t ADC_vals[SENSOR_DTA_QTY];
static bool is_ADC_reading = false;
static bool is_ADC_finished = false;
static uint16_t adc_counter;

/********************** internal functions declaration ***********************/
void task_sensor_statechart(shared_data_type * parameters);
HAL_StatusTypeDef read_sensors(void);
float take_sensor_value(const task_sensor_cfg_t * cfg);
void assign_read_value(shared_data_type * parameters, const task_sensor_cfg_t * cfg, task_sensor_dta_t * data);

/********************** internal data definition *****************************/
const char *p_task_sensor 		= "Task Sensor (Sensor Statechart)";
const char *p_task_sensor_ 		= "Non-Blocking & Update By Time Code";



/********************** external functions definition ************************/
void task_sensor_init(void *parameters)
{
	uint32_t index;
	task_sensor_dta_t *p_task_sensor_dta;
	task_sensor_st_t state;
	task_sensor_ev_t event;

	/* Print out: Task Initialized */
	LOGGER_INFO(" ");
	LOGGER_INFO("  %s is running - %s", GET_NAME(task_sensor_init), p_task_sensor);
	LOGGER_INFO("  %s is a %s", GET_NAME(task_sensor), p_task_sensor_);

	/* Init & Print out: Task execution counter */
	g_task_sensor_cnt = G_TASK_SEN_CNT_INIT;
	LOGGER_INFO("   %s = %lu", GET_NAME(g_task_sensor_cnt), g_task_sensor_cnt);

	for (index = 0; SENSOR_DTA_QTY > index; index++)
	{
		/* Update Task Sensor Data Pointer */
		p_task_sensor_dta = &task_sensor_dta_list[index];

		LOGGER_INFO(" ");
		LOGGER_INFO("   %s = %lu   %s = %lu   %s = %lu",
				    GET_NAME(index), index,
					GET_NAME(state), (uint32_t)state,
					GET_NAME(event), (uint32_t)event);
	}
}

void task_sensor_update(void *parameters) {
	bool b_time_update_required = false;

	/* Protect shared resource */
	__asm("CPSID i");	/* disable interrupts */
    if (g_task_sensor_tick_cnt > G_TASK_SEN_TICK_CNT_INI) {
		/* Update Tick Counter */
    	g_task_sensor_tick_cnt--;
    	b_time_update_required = true;
    }
    __asm("CPSIE i");	/* enable interrupts */

    while (b_time_update_required) {
		/* Update Task Counter */
		g_task_sensor_cnt++;

		/* Run Task Sensor Statechart */
		shared_data_type * shared_data = (shared_data_type *) parameters;

		if (shared_data->needs_light_measure)
			task_sensor_dta_list[SENSOR_LIGHT].event = EV_SENSOR_REQUEST;
		if (shared_data->needs_temp_measure)
			task_sensor_dta_list[SENSOR_TEMP].event = EV_SENSOR_REQUEST;
		if (shared_data->needs_humidity_measure)
			task_sensor_dta_list[SENSOR_HUMIDITY].event = EV_SENSOR_REQUEST;

		shared_data->needs_light_measure = false;
		shared_data->needs_temp_measure = false;
		shared_data->needs_humidity_measure = false;

    	task_sensor_statechart(shared_data);

    	/* Protect shared resource */
		__asm("CPSID i");	/* disable interrupts */
		if (g_task_sensor_tick_cnt > G_TASK_SEN_TICK_CNT_INI) {
			/* Update Tick Counter */
			g_task_sensor_tick_cnt--;
			b_time_update_required = true;
		} else {
			b_time_update_required = false;
		}
		__asm("CPSIE i");	/* enable interrupts */
    }
}

void task_sensor_statechart(shared_data_type * parameters) {
	uint32_t index;
	const task_sensor_cfg_t * p_task_sensor_cfg;
	task_sensor_dta_t * p_task_sensor_dta;

	for (index = 0; SENSOR_DTA_QTY > index; index++) {
		/* Update Task Sensor Configuration & Data Pointer */
		p_task_sensor_cfg = &task_sensor_cfg_list[index];
		p_task_sensor_dta = &task_sensor_dta_list[index];

		task_sensor_st_t state = p_task_sensor_dta->state;
		switch (state) {
		case ST_SENSOR_IDLE:
			if (p_task_sensor_dta->event == EV_SENSOR_IDLE)
				p_task_sensor_dta->state = ST_SENSOR_IDLE;
			else if (p_task_sensor_dta->event == EV_SENSOR_REQUEST)
				p_task_sensor_dta->state = ST_SENSOR_REQUEST;

			break;

		case ST_SENSOR_REQUEST:
			if (!is_ADC_reading) {
				if (read_sensors() != HAL_OK) {
					LOGGER_LOG("Error measuring sensors\n"); // TODO: preguntar como manejar este error
				} else
					is_ADC_reading = true;
			}

			p_task_sensor_dta->state = ST_SENSOR_WAITING;
			break;

		case ST_SENSOR_WAITING:
			if (is_ADC_finished)
				p_task_sensor_dta->state = ST_SENSOR_COMPLETED;

			break;

		case ST_SENSOR_COMPLETED:
			is_ADC_finished = false;
			is_ADC_reading = false;
			p_task_sensor_dta->tick_cnt++;

			if (p_task_sensor_dta->tick_cnt <= p_task_sensor_cfg->tick_max) {
				float val = take_sensor_value(p_task_sensor_cfg);
				p_task_sensor_dta->measure += val;
				p_task_sensor_dta->state = ST_SENSOR_REQUEST;
			} else {
				assign_read_value(parameters, p_task_sensor_cfg, p_task_sensor_dta);
				p_task_sensor_dta->measure = 0.0;
				p_task_sensor_dta->tick_cnt = DEL_SEN_TICK_INIT;
				p_task_sensor_dta->event = EV_SENSOR_IDLE;
				p_task_sensor_dta->state = ST_SENSOR_IDLE;
			}

			break;

		default:
			p_task_sensor_dta->tick_cnt = DEL_SEN_TICK_INIT;
			p_task_sensor_dta->event = EV_SENSOR_IDLE;
			p_task_sensor_dta->state = ST_SENSOR_IDLE;
			p_task_sensor_dta->measure = 0.0;
			break;
		}
	}
}

HAL_StatusTypeDef read_sensors(void) {
	return HAL_ADC_Start_DMA(&hadc1, (uint32_t*)ADC_vals, SENSOR_DTA_QTY);
}

float take_sensor_value(const task_sensor_cfg_t * cfg) {
	uint16_t measure = ADC_vals[cfg->name];
	float value;

	float aux = (float) measure / (float) 0x0FFF; // 12 bits como máximo
	value = cfg->min_val * (1.0 - aux) + cfg->max_val * aux;

	return value;
}

void assign_read_value(shared_data_type * parameters, const task_sensor_cfg_t * cfg, task_sensor_dta_t * data) {
	float val = data->measure / (float) data->tick_cnt;
	switch (cfg->name) {
	case SENSOR_LIGHT:
		parameters->light_measure = val;
		break;
	case SENSOR_TEMP:
		parameters->temp_measure = val;
		break;
	case SENSOR_HUMIDITY:
		parameters->humidity_measure = val;
		break;
	default:
		break;
	}
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	is_ADC_finished = true;
}

/********************** end of file ******************************************/

