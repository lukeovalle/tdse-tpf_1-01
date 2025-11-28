/*
 * adc.h
 *
 *  Created on: Oct 31, 2025
 *      Author: Hikar uwu
 */

#ifndef INC_ADC_H_
#define INC_ADC_H_

#include <stdint.h>
#include "main.h"

//	Requests start of conversion, waits until conversion done
HAL_StatusTypeDef ADC_Poll_Read(ADC_HandleTypeDef* hadc, uint16_t *value);

#endif /* INC_ADC_H_ */
