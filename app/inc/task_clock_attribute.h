/*
 * task_clock_attribute.h
 *
 *  Created on: Jan 30, 2026
 *      Author: luke
 */

#ifndef INC_TASK_CLOCK_ATTRIBUTE_H_
#define INC_TASK_CLOCK_ATTRIBUTE_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/

/********************** macros ***********************************************/

/********************** typedef **********************************************/

/* Events to excite Task Light Sensor */
typedef enum task_i2c_ev {
	EV_CLOCK_IDLE,
	EV_CLOCK_SECOND_ELAPSED
} task_clock_ev_t;

/* States of Task Sensor */
typedef enum task_i2c_st {
	ST_CLOCK_IDLE,
	ST_CLOCK_INCREASE_SECOND
} task_clock_st_t;


typedef struct {
} task_clock_cfg_t;

typedef struct {
	task_clock_st_t	state;
	task_clock_ev_t	event;
} task_clock_dta_t;


/********************** external data declaration ****************************/

/********************** external functions declaration ***********************/



/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* INC_TASK_CLOCK_ATTRIBUTE_H_ */
