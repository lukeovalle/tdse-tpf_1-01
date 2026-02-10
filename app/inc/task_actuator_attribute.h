/*
 * task_actuator_attribute.h
 *
 *  Created on: Feb 5, 2026
 *      Author: luke
 */

#ifndef INC_TASK_ACTUATOR_ATTRIBUTE_H_
#define INC_TASK_ACTUATOR_ATTRIBUTE_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/
#include "main.h"
#include "task_actuator.h"

/********************** macros ***********************************************/


/********************** typedef **********************************************/

/* Events to excite Task Actuator */
typedef enum {
	EV_ACTUATOR_IDLE = 0,
	EV_ACTUATOR_TURN_ON,
	EV_ACTUATOR_TURN_OFF
} task_actuator_ev_t;

/* States of Task Actuator */
typedef enum {
	ST_ACTUATOR_ON,
	ST_ACTUATOR_OFF
} task_actuator_st_t;

typedef struct {
	actuator_name_t		name;
	GPIO_TypeDef *		gpio_port;
	uint16_t			gpio_pin;
} task_actuator_cfg_t;

typedef struct {
	task_actuator_st_t	state;
	task_actuator_ev_t	event;
} task_actuator_dta_t;


/********************** external data declaration ****************************/
extern task_actuator_dta_t task_actuator_dta_list[];

/********************** external functions declaration ***********************/



/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* INC_TASK_ACTUATOR_ATTRIBUTE_H_ */
