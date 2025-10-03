/*
 * task_light_sensor_attribute.h
 *
 *  Created on: Oct 3, 2025
 *      Author: luke
 */

#ifndef INC_TASK_LIGHT_SENSOR_ATTRIBUTE_H_
#define INC_TASK_LIGHT_SENSOR_ATTRIBUTE_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/

/********************** macros ***********************************************/

/********************** typedef **********************************************/

/* Events to excite Task Light Sensor */
typedef enum task_sensor_ev {
	EV_LIGHT_REQUEST
} task_light_sensor_ev_t;

/* States of Task Light Sensor */
typedef enum task_sensor_st {
	ST_LIGHT_MEASURING,
	ST_LIGHT_READ
} task_light_sensor_st_t;


typedef struct {
	ADC_HandleTypeDef *	hadc;
	uint32_t			tick_max;
} task_light_sensor_cfg_t;

typedef struct {
	uint32_t				tick_cnt;
	task_light_sensor_st_t	state;
	task_light_sensor_ev_t	event;
} task_light_sensor_dta_t;

/********************** external data declaration ****************************/
extern task_light_sensor_dta_t task_light_sensor_dta_list[];

/********************** external functions declaration ***********************/



/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* INC_TASK_LIGHT_SENSOR_ATTRIBUTE_H_ */
