/*
 * task_sensor.h
 *
 *  Created on: Oct 3, 2025
 *      Author: luke
 */

#ifndef INC_TASK_SENSOR_H_
#define INC_TASK_SENSOR_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif


/********************** inclusions *******************************************/
#include <stdbool.h>
#include <stdint.h>
#include "task_sensor_attribute.h"

/********************** macros ***********************************************/

/********************** typedef **********************************************/

/********************** external data declaration ****************************/
extern uint32_t g_task_sensor_cnt;
extern volatile uint32_t g_task_sensor_tick_cnt;

/********************** external functions declaration ***********************/
extern void task_sensor_init(void *parameters);
extern void task_sensor_update(void *parameters);

void sensor_request_measurement(sensor_name_t name, float * data_ptr);
bool sensor_measurement_ready(sensor_name_t name);


/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* INC_TASK_SENSOR_H_ */
