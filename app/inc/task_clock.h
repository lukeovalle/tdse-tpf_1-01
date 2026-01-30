/*
 * task_clock.h
 *
 *  Created on: Jan 30, 2026
 *      Author: luke
 */

#ifndef INC_TASK_CLOCK_H_
#define INC_TASK_CLOCK_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif


/********************** inclusions *******************************************/

/********************** macros ***********************************************/

/********************** typedef **********************************************/
typedef struct {
	uint16_t	year;
	uint8_t		month, day;
	uint8_t		hour, minute;
} date_time_t;

/********************** external data declaration ****************************/
extern uint32_t g_task_clock_cnt;
extern volatile uint32_t g_task_clock_tick_cnt;

/********************** external functions declaration ***********************/
extern void task_clock_init(void *parameters);
extern void task_clock_update(void *parameters);



/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* INC_TASK_CLOCK_H_ */
