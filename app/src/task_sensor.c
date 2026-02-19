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
#include <math.h>
#include "board.h"
#include "app.h"
#include "task_sensor.h"
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
float light_conversion(float res);
float temp_conversion(float res);
float humidity_conversion(float res);

const task_sensor_cfg_t task_sensor_cfg_list[] = {
	{ .name = SENSOR_LIGHT,		.tick_max = DEL_SEN_TICK_MAX, .r_div = 1.2e3, .resistor_conv_fn = light_conversion },
	{ .name = SENSOR_TEMP,		.tick_max = DEL_SEN_TICK_MAX, .r_div = 10e3, .resistor_conv_fn = temp_conversion },
	{ .name = SENSOR_HUMIDITY,	.tick_max = DEL_SEN_TICK_MAX, .r_div = 27e3, .resistor_conv_fn = humidity_conversion }
};

#define SENSOR_CFG_QTY	(sizeof(task_sensor_cfg_list)/sizeof(task_sensor_cfg_t))

task_sensor_dta_t task_sensor_dta_list[] = {
	{.tick_cnt = DEL_SEN_TICK_INIT, .state = ST_SENSOR_IDLE, .event = EV_SENSOR_IDLE,
		.measure = NULL },
	{.tick_cnt = DEL_SEN_TICK_INIT, .state = ST_SENSOR_IDLE, .event = EV_SENSOR_IDLE,
		.measure = NULL },
	{.tick_cnt = DEL_SEN_TICK_INIT, .state = ST_SENSOR_IDLE, .event = EV_SENSOR_IDLE,
		.measure = NULL }
};

#define SENSOR_DTA_QTY	(sizeof(task_sensor_dta_list)/sizeof(task_sensor_dta_t))

static volatile uint16_t ADC_vals[SENSOR_DTA_QTY];
static bool is_ADC_reading = false;
static bool is_ADC_finished = false;

/********************** internal functions declaration ***********************/
void task_sensor_statechart(shared_data_type * parameters);
HAL_StatusTypeDef read_sensors(void);
float take_sensor_value(const task_sensor_cfg_t * cfg);

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

void sensor_request_measurement(sensor_name_t name, float * data_ptr) {
	task_sensor_dta_list[name].event = EV_SENSOR_REQUEST;
	task_sensor_dta_list[name].measure = data_ptr;

}

bool sensor_measurement_ready(sensor_name_t name) {
	return task_sensor_dta_list[name].state == ST_SENSOR_IDLE ? true : false;
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
			else if (p_task_sensor_dta->event == EV_SENSOR_REQUEST) {
				p_task_sensor_dta->state = ST_SENSOR_REQUEST;
				p_task_sensor_dta->event = EV_SENSOR_IDLE;
			}

			break;

		case ST_SENSOR_REQUEST:
			if (is_ADC_reading) {
				p_task_sensor_dta->state = ST_SENSOR_WAITING;
			} else {
				is_ADC_finished = false;
				is_ADC_reading = true;
				if (read_sensors() == HAL_OK) {
					p_task_sensor_dta->state = ST_SENSOR_WAITING;
				} else {
					is_ADC_reading = false;
					LOGGER_LOG("Error measuring sensors, retrying...\n");
				}
			}

			break;

		case ST_SENSOR_WAITING:
			if (is_ADC_finished)
				p_task_sensor_dta->state = ST_SENSOR_COMPLETED;

			break;

		case ST_SENSOR_COMPLETED:
			p_task_sensor_dta->tick_cnt++;
			float * measure = p_task_sensor_dta->measure;

			if (p_task_sensor_dta->tick_cnt <= p_task_sensor_cfg->tick_max) {
				float val = take_sensor_value(p_task_sensor_cfg);
				*measure += val;
				p_task_sensor_dta->state = ST_SENSOR_REQUEST;
			} else {
				*measure /= (float) p_task_sensor_dta->tick_cnt;
				*measure = p_task_sensor_cfg->resistor_conv_fn(*measure);
				p_task_sensor_dta->tick_cnt = DEL_SEN_TICK_INIT;
				p_task_sensor_dta->state = ST_SENSOR_IDLE;
			}

			break;

		default:
			p_task_sensor_dta->tick_cnt = DEL_SEN_TICK_INIT;
			p_task_sensor_dta->event = EV_SENSOR_IDLE;
			p_task_sensor_dta->state = ST_SENSOR_IDLE;
			*p_task_sensor_dta->measure = 0.0;
			break;
		}
	}
}

HAL_StatusTypeDef read_sensors(void) {
	return HAL_ADC_Start_DMA(&hadc1, (uint32_t *)ADC_vals, SENSOR_DTA_QTY);
}

float take_sensor_value(const task_sensor_cfg_t * cfg) {
	uint16_t measure = (uint16_t) ADC_vals[cfg->name];

	return measure != 0 ? cfg->r_div * ((float)0xFFF / (float)measure - 1.0) : 0; // Valor de resistencia medido
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
	is_ADC_finished = true;
	is_ADC_reading = false;
}

float light_conversion(float res) {
	return res;
}

#define CELSIUS_IN_KELVIN 273.15

float temp_conversion(float res) {
	float c1 = 0.001129148, c2 = 0.000234125, c3 = 0.0000000876741; // valores sacados de https://www.alldatasheet.com/datasheet-pdf/view/2045015/AGELECTRONICA/KY-013.html

	float log_r = log(res);

	float aux = c1 + c2 * log_r + c3 * pow(log_r, 3);
	aux = 1/aux;

	return aux - CELSIUS_IN_KELVIN;
}

float humidity_conversion(float res) {
	return res;
}


/********************** end of file ******************************************/
