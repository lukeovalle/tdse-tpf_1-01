/*
 * task_memory.h
 *
 *  Created on: Dec 10, 2025
 *      Author: Hikar
 */

#ifndef INC_TASK_MEMORY_H_
#define INC_TASK_MEMORY_H_


/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif


/********************** inclusions *******************************************/

/********************** macros ***********************************************/

/********************** typedef **********************************************/

/********************** external data declaration ****************************/
extern uint32_t g_task_memory_cnt;
extern volatile uint32_t g_task_memory_tick_cnt;

/********************** external functions declaration ***********************/
extern void task_memory_init(void *parameters);
extern void task_memory_update(void *parameters);


/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* INC_TASK_MEMORY_H_ */
