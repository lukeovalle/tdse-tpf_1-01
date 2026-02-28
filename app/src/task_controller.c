/*
 * task_controller.c
 *
 *  Created on: Feb 11, 2026
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
#include "ext_memory.h"
#include "task_actuator.h"
#include "task_controller.h"
#include "task_controller_attribute.h"
#include "task_sensor.h"
#include "utils.h"


/********************** macros and definitions *******************************/
#define G_TASK_CONTROLLER_CNT_INIT			0ul
#define G_TASK_CONTROLLER_TICK_CNT_INI		0ul

#define DAY_START_HOUR			8
#define DAY_END_HOUR			20

#define TEMP_THRESHOLD			5
#define HUMIDITY_THRESHOLD		5

#define PUMP_PULSES_MAX			5
// en minutos
#define IRRIGATION_TIME			1
#define IRRIGATION_WAIT_TIME	30

/********************** external data declaration ****************************/
uint32_t g_task_controller_cnt;
volatile uint32_t g_task_controller_tick_cnt;

/********************** internal data declaration ****************************/
const task_controller_cfg_t task_controller_cfg_list[] = {
		{ }
};

#define CONTROLLER_CFG_QTY	(sizeof(task_controller_cfg_list)/sizeof(task_controller_cfg_t))

task_controller_dta_t task_controller_dta_list[] = {
	{ .state = ST_CONTROLLER_IDLE, .event = EV_CONTROLLER_IDLE }
};

#define CONTROLLER_DTA_QTY	(sizeof(task_controller_dta_list)/sizeof(task_controller_dta_t))

static mem_cfg_t config;

/********************** internal functions declaration ***********************/
void task_controller_statechart(shared_data_type * parameters);
void reset_controller_data(task_controller_dta_t * controller_data);
void regulate_temperature(task_controller_dta_t * controller_data);
void regulate_light(task_controller_dta_t * controller_data);
void regulate_humidity(task_controller_dta_t * controller_data);
bool is_day(date_time_t * date);

/********************** internal data definition *****************************/
const char *p_task_controller 	= "Task Controller (Controller Statechart)";
const char *p_task_controller_ 	= "Non-Blocking & Update By Time Code";

static uint8_t last_minute = 255;
/********************** external functions definition ************************/
void task_controller_init(void *parameters)
{
	uint32_t index;
	task_controller_dta_t *p_task_controller_dta;
	task_controller_st_t state;
	task_controller_ev_t event;

	/* Print out: Task Initialized */
	LOGGER_INFO(" ");
	LOGGER_INFO("  %s is running - %s", GET_NAME(task_controller_init), p_task_controller);
	LOGGER_INFO("  %s is a %s", GET_NAME(task_controller), p_task_controller_);

	/* Init & Print out: Task execution counter */
	g_task_controller_cnt = G_TASK_CONTROLLER_CNT_INIT;
	LOGGER_INFO("   %s = %lu", GET_NAME(g_task_controller_cnt), g_task_controller_cnt);

	for (index = 0; CONTROLLER_DTA_QTY > index; index++)
	{
		/* Update Task CONTROLLER Data Pointer */
		p_task_controller_dta = &task_controller_dta_list[index];

		LOGGER_INFO(" ");
		LOGGER_INFO("   %s = %lu   %s = %lu   %s = %lu",
				    GET_NAME(index), index,
					GET_NAME(state), (uint32_t)state,
					GET_NAME(event), (uint32_t)event);
	}

	memory_read_config(&config);
}

void task_controller_update(void *parameters) {
	bool b_time_update_required = false;

	/* Protect shared resource */
	__asm("CPSID i");	/* disable interrupts */
    if (g_task_controller_tick_cnt > G_TASK_CONTROLLER_TICK_CNT_INI) {
		/* Update Tick Counter */
    	g_task_controller_tick_cnt--;
    	b_time_update_required = true;
    }
    __asm("CPSIE i");	/* enable interrupts */

    while (b_time_update_required) {
		/* Update Task Counter */
		g_task_controller_cnt++;

		/* Run Task CONTROLLER Statechart */
		shared_data_type * shared_data = (shared_data_type *) parameters;

		task_controller_dta_list[0].curr_time = clock_get_time();

		if (task_controller_dta_list[0].curr_time.seconds == 0 && task_controller_dta_list[0].curr_time.minutes != last_minute) {
			last_minute = task_controller_dta_list[0].curr_time.minutes;
			task_controller_dta_list[0].event = EV_CONTROLLER_MINUTE_ELAPSED;
		}

    	task_controller_statechart(shared_data);

    	/* Protect shared resource */
		__asm("CPSID i");	/* disable interrupts */
		if (g_task_controller_tick_cnt > G_TASK_CONTROLLER_TICK_CNT_INI) {
			/* Update Tick Counter */
			g_task_controller_tick_cnt--;
			b_time_update_required = true;
		} else {
			b_time_update_required = false;
		}
		__asm("CPSIE i");	/* enable interrupts */
    }
}

void controller_request_update_config(void) {
	task_controller_dta_list[0].event = EV_CONTROLLER_UPDATE_CONFIG;
}

/********************** internal functions definition ************************/
void task_controller_statechart(shared_data_type * parameters) {
	uint32_t index;
	const task_controller_cfg_t * p_task_controller_cfg;
	task_controller_dta_t * p_task_controller_dta;

	for (index = 0; CONTROLLER_DTA_QTY > index; index++) {
		/* Update Task CONTROLLER Configuration & Data Pointer */
		p_task_controller_cfg = &task_controller_cfg_list[index];
		p_task_controller_dta = &task_controller_dta_list[index];

		task_controller_st_t state = p_task_controller_dta->state;
		switch (state) {
		case ST_CONTROLLER_IDLE:
			switch (p_task_controller_dta->event) {
			case EV_CONTROLLER_IDLE:
				p_task_controller_dta->state = ST_CONTROLLER_IDLE;
				break;

			case EV_CONTROLLER_MINUTE_ELAPSED:
				sensor_request_measurement(SENSOR_LIGHT, &p_task_controller_dta->light);
				sensor_request_measurement(SENSOR_TEMP, &p_task_controller_dta->temp);
				sensor_request_measurement(SENSOR_HUMIDITY, &p_task_controller_dta->humidity);

				p_task_controller_dta->state = ST_CONTROLLER_WAITING_MEASUREMENT;
				p_task_controller_dta->event = EV_CONTROLLER_IDLE;
				break;

			case EV_CONTROLLER_UPDATE_CONFIG:
				p_task_controller_dta->state = ST_CONTROLLER_UPDATE_CONFIG;
				p_task_controller_dta->event = EV_CONTROLLER_IDLE;
			default:
				break;

			}
			break;

		case ST_CONTROLLER_WAITING_MEASUREMENT:
			if (sensor_measurement_ready(SENSOR_LIGHT)
			&& sensor_measurement_ready(SENSOR_TEMP)
			&& sensor_measurement_ready(SENSOR_HUMIDITY))
				p_task_controller_dta->state = ST_CONTROLLER_SAVE_LOG;

			break;

		case ST_CONTROLLER_SAVE_LOG:
			date_time_t * curr_time = &p_task_controller_dta->curr_time;
			uint8_t save_freq = (uint8_t) config.save_freq;
			if (!save_freq) save_freq = 24; // Si no tiene un valor configurado

			if  (!curr_time) {
				LOGGER_LOG("Puntero a tiempo actual nulo. archivo %s, línea %d\n", __FILE__, __LINE__);
			}

			bool have_to_log = curr_time->hours % save_freq;
			if (curr_time->minutes == 0 && have_to_log)
				memory_append_log(&p_task_controller_dta->humidity,
						&p_task_controller_dta->light,
						&p_task_controller_dta->temp);

			p_task_controller_dta->state = ST_CONTROLLER_REGULATE_ACTUATORS;
			break;

		case ST_CONTROLLER_REGULATE_ACTUATORS:
			reset_controller_data(p_task_controller_dta);
			regulate_temperature(p_task_controller_dta);
			regulate_light(p_task_controller_dta);
			regulate_humidity(p_task_controller_dta);

			p_task_controller_dta->state = ST_CONTROLLER_IDLE;

			break;

		case ST_CONTROLLER_UPDATE_CONFIG:
			memory_read_config(&config);
			p_task_controller_dta->event = EV_CONTROLLER_IDLE;
			p_task_controller_dta->state = ST_CONTROLLER_IDLE;

			break;

		default:
			p_task_controller_dta->event = EV_CONTROLLER_IDLE;
			p_task_controller_dta->state = ST_CONTROLLER_IDLE;
			break;
		}
	}
}

void reset_controller_data(task_controller_dta_t * controller_data) {
	if (controller_data->curr_time.hours == DAY_START_HOUR && controller_data->curr_time.minutes == 0) {
		controller_data->pump_pulses = 0;
		controller_data->light_minutes = 0;
	}
}


void regulate_temperature(task_controller_dta_t * controller_data) {
	bool day = is_day(&controller_data->curr_time);
	uint32_t current_minutes = datetime_to_timestamp(&controller_data->curr_time) / 60; // minuto de inicio del riego

	if ((day && controller_data->temp < config.temp_day_min)
	|| (!day && controller_data->temp < config.temp_night_min)) {
		LOGGER_INFO("Tengo frío >w<"); // TODO: Avisar por display
	}

	bool recently_irrigated = false;
	if (current_minutes - controller_data->pump_start < IRRIGATION_WAIT_TIME)
		recently_irrigated = true;

	if (day && controller_data->temp > config.temp_day_max && !recently_irrigated)
		actuator_request_turn_on(ACTUATOR_FAN);
	if (day && controller_data->temp < config.temp_day_max - TEMP_THRESHOLD)
		actuator_request_turn_off(ACTUATOR_FAN);
	if (!day && controller_data->temp < config.temp_night_max - TEMP_THRESHOLD)
		actuator_request_turn_off(ACTUATOR_FAN);
}

void regulate_light(task_controller_dta_t * controller_data) {
	if (controller_data->light > config.light_threshold)
		controller_data->light_minutes++;

	uint8_t remaining_hours = config.light_hours_needed - controller_data->light_minutes/60;
	uint8_t start_hour = MAX(DAY_START_HOUR, (DAY_END_HOUR - remaining_hours));	//  inicia a las 20 - horas_faltantes, o a las 8

	if (controller_data->light < config.light_threshold
	&& controller_data->curr_time.hours >= start_hour && remaining_hours > 0)
		actuator_request_turn_on(ACTUATOR_LIGHT_PANEL);

	if (remaining_hours == 0)
		actuator_request_turn_off(ACTUATOR_LIGHT_PANEL);
}

void regulate_humidity(task_controller_dta_t * controller_data) {
	bool day = is_day(&controller_data->curr_time);
	uint32_t current_minutes = datetime_to_timestamp(&controller_data->curr_time) / 60; // minuto de inicio del riego

	if (day && controller_data->humidity < config.humidity_min
	&& controller_data->pump_pulses < PUMP_PULSES_MAX
	&& current_minutes - controller_data->pump_start >= IRRIGATION_WAIT_TIME ) {
		actuator_request_turn_on(ACTUATOR_PUMP);
		controller_data->pump_pulses++;
		controller_data->pump_start = current_minutes;
	}

	if (current_minutes - controller_data->pump_start >= IRRIGATION_TIME)
		actuator_request_turn_off(ACTUATOR_PUMP);
}

bool is_day(date_time_t * date) {
	if (date->hours >= DAY_START_HOUR && date->hours < DAY_END_HOUR)
		return true;

	return false;
}

/********************** end of file ******************************************/
