/*
 * task_i2c_attribute.h
 *
 *  Created on: Dec 10, 2025
 *      Author: Hikar
 */

#ifndef INC_TASK_I2C_ATTRIBUTE_H_
#define INC_TASK_I2C_ATTRIBUTE_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/

/********************** macros ***********************************************/
#define MEM_PAGE_SIZE_BYTES	16u

/********************** typedef **********************************************/

/* Events to excite Task Light Sensor */
typedef enum task_i2c_ev {
	EV_I2C_IDLE = 0,
	EV_I2C_WRITE,
	EV_I2C_READ
} task_i2c_ev_t;

/* States of Task Sensor */
typedef enum task_i2c_st {
	ST_I2C_IDLE = 0,
	ST_I2C_WRITING,
	ST_I2C_READING,
	ST_I2C_WAITING_WRITE,
	ST_I2C_WAITING_READ
} task_i2c_st_t;


typedef struct {
} task_i2c_cfg_t;

typedef struct {
	task_i2c_st_t	state;
	task_i2c_ev_t	event;
} task_i2c_dta_t;


/********************** external data declaration ****************************/

/********************** external functions declaration ***********************/



/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* INC_TASK_I2C_ATTRIBUTE_H_ */
