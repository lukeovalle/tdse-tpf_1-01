/*
 * task_sensor_attribute.h
 *
 *  Created on: Oct 3, 2025
 *      Author: luke
 */

#ifndef INC_TASK_SENSOR_ATTRIBUTE_H_
#define INC_TASK_SENSOR_ATTRIBUTE_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/

/********************** macros ***********************************************/
/* TODO definir valores límite de luminiscencia */
#define MIN_LIGHT 1e0  // lux estimado para tomates
#define MAX_LIGHT 30e3

#define MIN_HUMIDITY 0 // el minimo es 0 si no está sumergido en nada
#define MAX_HUMIDITY 125 // debería dar 100% de humedad tomando este valor como máximo

#define MIN_TEMP 15 // valores de datasheet
#define MAX_TEMP 1500
// TODO: revisar valores de temperatura y humedad

/********************** typedef **********************************************/

typedef enum {
	SENSOR_LIGHT = 0,
	SENSOR_TEMP,
	SENSOR_HUMIDITY
} sensor_name_t;

/* Events to excite Task Light Sensor */
typedef enum task_sensor_ev {
	EV_SENSOR_IDLE,
	EV_SENSOR_REQUEST
} task_sensor_ev_t;

/* States of Task Sensor */
typedef enum task_sensor_st {
	ST_SENSOR_IDLE,
	ST_SENSOR_REQUEST,
	ST_SENSOR_WAITING,
	ST_SENSOR_COMPLETED
} task_sensor_st_t;

typedef float (*resistor_conv_fn_t)(float res);

typedef struct {
	sensor_name_t		name;
	uint32_t			tick_max;
	float				r_div; // Resistencia del divisor resistivo del sensor
	resistor_conv_fn_t	resistor_conv_fn; // Función de conversión de resistencia a valores representativos
} task_sensor_cfg_t;

typedef struct {
	uint32_t			tick_cnt;
	task_sensor_st_t	state;
	task_sensor_ev_t	event;
	float *				measure;
} task_sensor_dta_t;


/********************** external data declaration ****************************/
extern task_sensor_dta_t task_sensor_dta_list[];

/********************** external functions declaration ***********************/



/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* INC_TASK_SENSOR_ATTRIBUTE_H_ */
