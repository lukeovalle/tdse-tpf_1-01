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
#include <stdbool.h>

/********************** macros ***********************************************/

/********************** typedef **********************************************/

/********************** external data declaration ****************************/
extern uint32_t g_task_i2c_cnt;
extern volatile uint32_t g_task_i2c_tick_cnt;

/********************** external functions declaration ***********************/
extern void task_i2c_init(void *parameters);
extern void task_i2c_update(void *parameters);

//devuelve true si se puede leer y false si no se puede leer
bool		task_i2c_request_read(uint16_t dev_addr,		// Dirección del dispositivo I2C (7 bits)
								  uint16_t mem_addr_size,	// I2C_MEMADD_SIZE_8BIT o I2C_MEMADD_SIZE_16BIT
								  uint16_t mem_addr,		// Dirección de memoria
								  uint8_t * data,			// Puntero a los datos
								  uint16_t data_size);		// Tamaño de los datos (en bytes)
bool		task_i2c_finished_reading(void);
inline bool	task_i2c_finished_writing(void);
/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* INC_TASK_I2C_H_ */
