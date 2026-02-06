/*
 * Copyright (c) 2023 Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * @file   : task_menu_attribute.h
 * @date   : Set 26, 2023
 * @author : Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>
 * @version	v1.0.0
 */

#ifndef TASK_INC_TASK_MENU_ATTRIBUTE_H_
#define TASK_INC_TASK_MENU_ATTRIBUTE_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/
#include "display.h"
/********************** macros ***********************************************/

/********************** typedef **********************************************/
/* Menu Statechart - State Transition Table */
/* 	------------------------+-----------------------+-----------------------+-----------------------+--------------------------------------------------------------------------
 * 	| Current               | Event                 |                       						| Next                  |                       							|
 * 	| State                 | (Parameters)          | [Guard]               						| State                 | Actions              								|
 * 	|=======================+=======================+=======================+=======================+=======================|====================================================
 * 	| ST_INIT				| EV_PRESS_HASH/ASTK													| ST_INIT				| Seleccionar configuración o lectura				|
	| ST_INIT				| EV_PRESS_NEXT			| seleccionado configuracion					| ST_CONFIG				|													|
	| ST_INIT				| EV_PRESS_NEXT			| seleccionado lectura y Fecha valida guardada	| ST_READ				|
	| ST_CONFIG				| EV_PRESS_HASH/ASTK	| 												| ST_CONFIG				| Seleccionar tiempo, temperatura, humedad o luz
	| ST_CONFIG				| EV_PRESS_BACK			| 												| ST_INIT				|
	| ST_CONFIG				| EV_PRESS_NEXT			| seleccionado tiempo							| ST_CONFIG_TIME		|
	| ST_CONFIG				| EV_PRESS_NEXT			| seleccionado temperatura						| ST_CONFIG_TEMP		|
	| ST_CONFIG				| EV_PRESS_NEXT			| seleccionado humedad							| ST_CONFIG_HUM			|
	| ST_CONFIG				| EV_PRESS_NEXT			| seleccionado luz								| ST_CONFIG_LIG			|
	| ST_CONFIG_TIME		| EV_PRESS_HASH/ASTK	| 												| ST_CONFIG_TIME		| Cambiar entre dia/fecha/año y hora/minuto
	| ST_CONFIG_TIME		| EV_PRESS_ENTER		| Fecha/Horario introducido valido				| ST_CONFIG_TIME		| Guarda valores ingresados
	| ST_CONFIG_TIME		| EV_PRESS_BACK			| 												| ST_CONFIG				|
	| ST_CONFIG_TEMP		| EV_PRESS_HASH/ASTK	| 												| ST_CONFIG_TEMP		| Cambiar entre maxima y minima
	| ST_CONFIG_TEMP		| EV_PRESS_ENTER		| Temperatura valida							| ST_CONFIG_TEMP		| Guarda valores ingresados
	| ST_CONFIG_TEMP		| EV_PRESS_BACK			|												| ST_CONFIG				|
	| ST_CONFIG_HUM			| EV_PRESS_HASH/ASTK	|												| ST_CONFIG_HUM			| Cambiar entre maxima y minima
	| ST_CONFIG_HUM			| EV_PRESS_ENTER		| Humedad valida								| ST_CONFIG_HUM			| Guarda valores ingresados
	| ST_CONFIG_HUM			| EV_PRESS_BACK			|												| ST_CONFIG				|
	| ST_CONFIG_LIG			| EV_PRESS_HASH/ASTK	|												| ST_CONFIG_LIG			| Cambiar entre maxima y minima
	| ST_CONFIG_LIG			| EV_PRESS_ENTER		| Luminocidad valida							| ST_CONFIG_LIG			| Guarda valores ingresados
	| ST_CONFIG_LIG			| EV_PRESS_BACK			|												| ST_CONFIG				|
	| ST_READ				| EV_PRESS_HASH/ASTK	|												| ST_READ				| Seleccionar tiempo, temperatura, humedad o luz
	| ST_READ				| EV_PRESS_BACK			| 												| ST_INIT				|
	| ST_READ				| EV_PRESS_NEXT			| seleccionado tiempo							| ST_READ_TIME			| Muestra fecha y hora actuales
	| ST_READ				| EV_PRESS_NEXT	 		| seleccionado temperatura						| ST_READ_TEMP			|
	| ST_READ				| EV_PRESS_NEXT			| seleccionado humedad							| ST_READ_HUM			|
	| ST_READ				| EV_PRESS_NEXT			| seleccionado luz								| ST_READ_LIG			|
	| ST_READ_TIME			| EV_PRESS_BACK			|												| ST_READ				|
	| ST_READ_TEMP			| EV_PRESS_HASH/ASTK	|												| ST_READ_TEMP			| Cambiar entre configurada e historicas
	| ST_READ_TEMP			| EV_PRESS_NEXT			| Seleccionada configurada						| ST_READ_TEMP_CON		| Muestra temperaturas maxima y minima
	| ST_READ_TEMP			| EV_PRESS_BACK			|												| ST_READ				|
	| ST_READ_TEMP_CON		| EV_PRESS_BACK			| 												| ST_READ_TEMP			|
	| ST_READ_TEMP			| EV_PRESS_NEXT			| Seleccionada historicas						| ST_READ_TEMP_HIS		| Muestra medición más reciente
	| ST_READ_TEMP_HIS		| EV_PRESS_HASH/ASTK	|												| ST_READ_TEMP_HIS		| Va cambiando las mediciones mostradas
	| ST_READ_TEMP_HIS		| EV_PRESS_BACK			|												| ST_READ_TEMP			|
	| ST_READ_HUM			| EV_PRESS_HASH/ASTK	|												| ST_READ_HUM			| Cambiar entre configurada e historicas
	| ST_READ_HUM			| EV_PRESS_NEXT			| Seleccionada configurada						| ST_READ_HUM_CON		| Muestra humedades maxima y minima
	| ST_READ_HUM_CON		| EV_PRESS_BACK			| 												| ST_READ_HUM			|
	| ST_READ_HUM			| EV_PRESS_NEXT			| Seleccionada historicas						| ST_READ_HUM_HIS		| Muestra medición más reciente
	| ST_READ_HUM_HIS		| EV_PRESS_HASH/ASTK	|												| ST_READ_HUM_HIS		| Va cambiando las mediciones mostradas
	| ST_READ_HUM_HIS		| EV_PRESS_BACK			|												| ST_READ_HUM			|
	| ST_READ_LIG			| EV_PRESS_HASH/ASTK	|												| ST_READ_LIG			| Cambiar entre configurada e historicas
	| ST_READ_LIG			| EV_PRESS_NEXT			| Seleccionada configurada						| ST_READ_LIG_CON		| Muestra luminocidades maxima y minima
	| ST_READ_LIG_CON		| EV_PRESS_BACK			| 												| ST_READ_LIG			|
	| ST_READ_LIG			| EV_PRESS_NEXT			| Seleccionada historicas						| ST_READ_LIG_HIS 		| Muestra medición más reciente
	| ST_READ_LIG_HIS		| EV_PRESS_HASH/ASTK	|												| ST_READ_LIG_HIS		| Va cambiando las mediciones mostradas
	| ST_READ_LIG_HIS		| EV_PRESS_BACK			| 												| ST_READ_LIG			|
 * 	------------------------+-----------------------+-----------------------+-----------------------+------------------------
 */

/* Events to excite Task Menu */
typedef enum task_menu_ev {EV_PRESS_BACK,
						   EV_PRESS_ENTER,
						   EV_PRESS_NEXT,
						   EV_PRESS_SCROLL,
						   EV_PRESS_NUM} task_menu_ev_t;

/* State of Task Menu */
typedef enum task_menu_st {ST_MENU_INIT,
						   ST_MENU_CONFIG,
						   ST_MENU_CONFIG_TIME,
						   ST_MENU_CONFIG_TEMP,
						   ST_MENU_CONFIG_HUM,
						   ST_MENU_CONFIG_LIG,
						   ST_MENU_READ,
						   ST_MENU_READ_TIME,
						   ST_MENU_READ_TEMP,
						   ST_MENU_READ_TEMP_CON,
						   ST_MENU_READ_TEMP_HIS,
						   ST_MENU_READ_HUM,
						   ST_MENU_READ_HUM_CON,
						   ST_MENU_READ_HUM_HIS,
						   ST_MENU_READ_LIG,
						   ST_MENU_READ_LIG_CON,
						   ST_MENU_READ_LIG_HIS} task_menu_st_t;

typedef struct
{
	uint32_t		tick;
	task_menu_st_t	state;
	tas_menu_st_t	prev_state;
	task_menu_ev_t	event;
	uint32_t		ev_value;
	bool			flag;
} task_menu_dta_t;

/********************** external data declaration ****************************/
extern task_menu_dta_t task_menu_dta;

/********************** external functions declaration ***********************/
void task_menu_push_event(bool btn_pressed, keypad_key_t key);
uint32_t key_to_event(keypad_key_t key);
/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* TASK_INC_TASK_MENU_ATTRIBUTE_H_ */

/********************** end of file ******************************************/
