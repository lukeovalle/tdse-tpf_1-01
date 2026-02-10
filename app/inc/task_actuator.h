/*
 * task_actuator.h
 *
 *  Created on: Feb 5, 2026
 *      Author: luke
 */

#ifndef INC_TASK_ACTUATOR_H_
#define INC_TASK_ACTUATOR_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif


/********************** inclusions *******************************************/
#include <stdint.h>
/********************** macros ***********************************************/

/********************** typedef **********************************************/
typedef enum {
	ACTUATOR_PUMP = 0,
	ACTUATOR_FAN
} actuator_name_t;

/********************** external data declaration ****************************/
extern uint32_t g_task_actuator_cnt;
extern volatile uint32_t g_task_actuator_tick_cnt;

/********************** external functions declaration ***********************/
extern void task_actuator_init(void *parameters);
extern void task_actuator_update(void *parameters);

void actuator_request_turn_on(actuator_name_t actuator);
void actuator_request_turn_off(actuator_name_t actuator);

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* INC_TASK_ACTUATOR_H_ */
