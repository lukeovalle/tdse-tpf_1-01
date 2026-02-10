/*
 * task_print.h
 *
 *  Created on: Nov 7, 2025
 *      Author: Hikar
 */

#ifndef INC_TASK_PRINT_H_
#define INC_TASK_PRINT_H_

extern volatile uint32_t g_task_print_tick_cnt;

extern void task_print_init(void *parameters);
extern void task_print_update(void *parameters);

#endif /* INC_TASK_PRINT_H_ */
