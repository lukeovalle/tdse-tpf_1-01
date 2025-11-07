/*
 * task_humidity_sensor_attribute.h
 *
 *  Created on: Nov 7, 2025
 *      Author: Hikar
 */

#ifndef INC_TASK_HUMIDITY_SENSOR_ATTRIBUTE_H_
#define INC_TASK_HUMIDITY_SENSOR_ATTRIBUTE_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/

/********************** macros ***********************************************/
/* TODO definir valores límite de luminiscencia */
#define MIN_HUMIDITY 1e0
#define MAX_HUMIDITY 30e3

/********************** typedef **********************************************/

/* Events to excite Task humidity Sensor */
typedef enum task_sensor_ev {
	EV_HUMIDITY_IDLE,
	EV_HUMIDITY_REQUEST
} task_humidity_sensor_ev_t;

/* States of Task Humidity Sensor */
typedef enum task_sensor_st {
	ST_HUMIDITY_MEASURING,
	ST_HUMIDITY_WAITING
} task_humidity_sensor_st_t;


typedef struct {
	ADC_HandleTypeDef *	hadc;
	uint32_t			tick_max;
} task_humidity_sensor_cfg_t;

typedef struct {
	uint32_t				tick_cnt;
	task_humidity_sensor_st_t	state;
	task_humidity_sensor_ev_t	event;
	float					measure;
} task_humidity_sensor_dta_t;

/********************** external data declaration ****************************/
extern task_humidity_sensor_dta_t task_humidity_sensor_dta_list[];

/********************** external functions declaration ***********************/



/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* INC_TASK_HUMIDITY_SENSOR_ATTRIBUTE_H_ */
