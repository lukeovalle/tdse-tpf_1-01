/*
 * task_display.h
 *
 *  Created on: Mar 10, 2026
 *      Author: luke
 */

#ifndef INC_TASK_DISPLAY_H_
#define INC_TASK_DISPLAY_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/

/********************** macros ***********************************************/

/********************** typedef **********************************************/

/********************** external data declaration ****************************/
extern uint32_t g_task_display_cnt;
extern volatile uint32_t g_task_display_tick_cnt;

/********************** external functions declaration ***********************/
extern void task_display_init(void *parameters);
extern void task_display_update(void *parameters);

void task_display_request_write(char * row_1, char * row_2);

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* INC_TASK_DISPLAY_H_ */
