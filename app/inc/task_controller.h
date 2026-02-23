/*
 * task_controller.h
 *
 *  Created on: Feb 11, 2026
 *      Author: luke
 */

#ifndef INC_TASK_CONTROLLER_H_
#define INC_TASK_CONTROLLER_H_


/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif


/********************** inclusions *******************************************/

/********************** macros ***********************************************/

/********************** typedef **********************************************/

/********************** external data declaration ****************************/
extern uint32_t g_task_controller_cnt;
extern volatile uint32_t g_task_controller_tick_cnt;

/********************** external functions declaration ***********************/
extern void task_controller_init(void *parameters);
extern void task_controller_update(void *parameters);

void controller_request_update_config(void);

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif


#endif /* INC_TASK_CONTROLLER_H_ */
