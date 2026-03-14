/*
 * task_display_attribute.h
 *
 *  Created on: Mar 10, 2026
 *      Author: luke
 */

#ifndef INC_TASK_DISPLAY_ATTRIBUTE_H_
#define INC_TASK_DISPLAY_ATTRIBUTE_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/
#include <stdbool.h>
/********************** macros ***********************************************/

/********************** typedef **********************************************/

/* Events to excite Task Display */
typedef enum task_display_ev {
	EV_DISPLAY_IDLE,
	EV_DISPLAY_WRITE
} task_display_ev_t;

/* States of Task Display */
typedef enum task_display_st {
	ST_DISPLAY_IDLE,
	ST_DISPLAY_WRITE_CHAR,
	ST_DISPLAY_WAIT,
	ST_DISPLAY_FIND_NEXT
} task_display_st_t;


typedef struct {
} task_display_cfg_t;

typedef struct {
	task_display_st_t	state;
	task_display_ev_t	event;
	uint8_t row;
	uint8_t col;
	bool write_row_1;
	bool write_row_2;
} task_display_dta_t;


/********************** external data declaration ****************************/

/********************** external functions declaration ***********************/



/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* INC_TASK_DISPLAY_ATTRIBUTE_H_ */
