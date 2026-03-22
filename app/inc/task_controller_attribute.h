/*
 * task_controller_attribute.h
 *
 *  Created on: Feb 11, 2026
 *      Author: luke
 */

#ifndef INC_TASK_CONTROLLER_ATTRIBUTE_H_
#define INC_TASK_CONTROLLER_ATTRIBUTE_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/
#include "task_clock.h"

/********************** macros ***********************************************/

/********************** typedef **********************************************/

/* Events to excite Task Light Sensor */
typedef enum task_i2c_ev {
	EV_CONTROLLER_IDLE,
	EV_CONTROLLER_MINUTE_ELAPSED,
	EV_CONTROLLER_UPDATE_CONFIG
} task_controller_ev_t;

/* States of Task Sensor */
typedef enum task_i2c_st {
	ST_CONTROLLER_IDLE,
	ST_CONTROLLER_WAITING_MEASUREMENT,
	ST_CONTROLLER_SAVE_LOG,
	ST_CONTROLLER_REGULATE_ACTUATORS,
	ST_CONTROLLER_UPDATE_CONFIG
} task_controller_st_t;


typedef struct {
} task_controller_cfg_t;

typedef struct {
	task_controller_st_t	state;
	task_controller_ev_t	event;
	date_time_t				curr_time;
	float					light, temp, humidity;
	uint8_t					pump_pulses;
	uint32_t				pump_start;
	uint16_t				light_minutes;
} task_controller_dta_t;


/********************** external data declaration ****************************/

/********************** external functions declaration ***********************/



/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* INC_TASK_CONTROLLER_ATTRIBUTE_H_ */
