/*
 * adc.c
 *
 *  Created on: Oct 31, 2025
 *      Author: Hikar 7w7
 */
#include "adc.h"

//	Requests start of conversion, waits until conversion done
HAL_StatusTypeDef ADC_Poll_Read(ADC_HandleTypeDef* hadc, uint16_t *value) {
	HAL_StatusTypeDef res;

	res = HAL_ADC_Start(hadc);

	if (res == HAL_OK) {
		res = HAL_ADC_PollForConversion(hadc, 0);

		if (res == HAL_OK) {
			*value = HAL_ADC_GetValue(hadc);
		}
	}

	return res;
}
