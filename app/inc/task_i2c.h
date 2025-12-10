/*
 * task_i2c.h
 *
 *  Created on: Dec 10, 2025
 *      Author: Hikar
 */

#ifndef INC_TASK_I2C_H_
#define INC_TASK_I2C_H_


/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif


/********************** inclusions *******************************************/

/********************** macros ***********************************************/

/********************** typedef **********************************************/

/********************** external data declaration ****************************/
extern uint32_t g_task_i2c_cnt;
extern volatile uint32_t g_task_i2c_tick_cnt;

/********************** external functions declaration ***********************/
extern void task_i2c_init(void *parameters);
extern void task_i2c_update(void *parameters);


/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* INC_TASK_I2C_H_ */
