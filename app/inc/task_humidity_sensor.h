/*
 * task_humidity_sensor.h
 *
 *  Created on: Nov 7, 2025
 *      Author: Hikar
 */

#ifndef INC_TASK_HUMIDITY_SENSOR_H_
#define INC_TASK_HUMIDITY_SENSOR_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif


/********************** inclusions *******************************************/

/********************** macros ***********************************************/

/********************** typedef **********************************************/

/********************** external data declaration ****************************/
extern uint32_t g_task_humidity_sensor_cnt;
extern volatile uint32_t g_task_humidity_sensor_tick_cnt;

/********************** external functions declaration ***********************/
extern void task_humidity_sensor_init(void *parameters);
extern void task_humidity_sensor_update(void *parameters);


/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* INC_TASK_HUMIDITY_SENSOR_H_ */
