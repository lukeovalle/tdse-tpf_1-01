/*
 * task_light_sensor.h
 *
 *  Created on: Oct 3, 2025
 *      Author: luke
 */

#ifndef INC_TASK_LIGHT_SENSOR_H_
#define INC_TASK_LIGHT_SENSOR_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif


/********************** inclusions *******************************************/

/********************** macros ***********************************************/

/********************** typedef **********************************************/

/********************** external data declaration ****************************/
extern uint32_t g_task_light_sensor_cnt;
extern volatile uint32_t g_task_light_sensor_tick_cnt;

/********************** external functions declaration ***********************/
extern void task_light_sensor_init(void *parameters);
extern void task_light_sensor_update(void *parameters);


/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* INC_TASK_LIGHT_SENSOR_H_ */
