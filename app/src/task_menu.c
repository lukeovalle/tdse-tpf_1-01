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
 * @file   : task_menu.c
 * @date   : Set 26, 2023
 * @author : Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>
 * @version	v1.0.0
 */

/********************** inclusions *******************************************/
/* Project includes. */
#include "main.h"

/* Demo includes. */
#include "logger.h"
#include "dwt.h"

/* Application & Tasks includes. */
#include "board.h"
#include "app.h"
#include "task_menu_attribute.h"
#include "task_menu_interface.h"
#include "task_keypad.h"
#include "display.h"
#include "memory_buffer.h"
#include "task_clock.h"
#include "ext_memory.h"
#include "num_buffer.h"

/********************** macros and definitions *******************************/
#define G_TASK_MEN_CNT_INI			0ul
#define G_TASK_MEN_TICK_CNT_INI		0ul

#define DEL_MEN_XX_MIN				0ul
#define DEL_MEN_XX_MED				50ul
#define DEL_MEN_XX_MAX				500ul

#define EVENT_UNDEFINED	(255)
#define MAX_EVENTS		(16)
#define KEY_VALUE_INVALID 10
#define SCROLL_UP   0
#define SCROLL_DOWN 1
#define SCROLL_VOID 0
#define NUM_BUFFER_SIZE 4

/********************** internal data declaration ****************************/
task_menu_dta_t task_menu_dta =
	{DEL_MEN_XX_MIN, ST_MENU_INIT, ST_MENU_INIT, EV_PRESS_BACK, KEY_VALUE_INVALID, SCROLL_VOID, SCROLL_VOID, false};

#define MENU_DTA_QTY	(sizeof(task_menu_dta)/sizeof(task_menu_dta_t))

/********************** internal functions declaration ***********************/
void task_menu_statechart(void);
void scrolling(task_menu_dta_t *s_task_menu_dta, uint32_t value);
void scroll_reset(task_menu_dta_t *s_task_menu_dta, uint32_t max);
void display_init(uint32_t idx);
void display_config(uint32_t idx);
void display_cfg_time(uint32_t idx);
void display_cfg_temp(uint32_t idx);
void display_cfg_hum(uint32_t idx);
void display_cfg_lig(uint32_t idx);
void display_read(uint32_t idx);

/********************** internal data definition *****************************/
const char *p_task_menu 		= "Task Menu (Interactive Menu)";
const char *p_task_menu_ 		= "Non-Blocking & Update By Time Code";
const char header_text[]        = "Enter/Next/Back";
const char empty_line[]			= "                ";

/********************** external data declaration ****************************/
uint32_t g_task_menu_cnt;
volatile uint32_t g_task_menu_tick_cnt;

extern num_buffer_t num_buff;

/********************** external functions definition ************************/
void task_menu_init(void *parameters)
{
	char menu_str[DISPLAY_CHAR_WIDTH + 1];
	task_menu_dta_t *p_task_menu_dta;
	num_buffer_t *p_num_buf;
	task_menu_st_t	state;
	task_menu_ev_t	event;
	bool b_event;

	/* Print out: Task Initialized */
	LOGGER_INFO(" ");
	LOGGER_INFO("  %s is running - %s", GET_NAME(task_menu_init), p_task_menu);
	LOGGER_INFO("  %s is a %s", GET_NAME(task_menu), p_task_menu_);

	/* Init & Print out: Task execution counter */
	g_task_menu_cnt = G_TASK_MEN_CNT_INI;
	LOGGER_LOG("   %s = %lu\r\n", GET_NAME(g_task_menu_cnt), g_task_menu_cnt);

	init_queue_event_task_menu();

	/* Update Task Actuator Configuration & Data Pointer */
	p_task_menu_dta = &task_menu_dta;
	p_num_buf =	&num_buff;

	/* Init & Print out: Task execution FSM */
	state = ST_MENU_INIT;
	p_task_menu_dta->state = state;

	event = EV_PRESS_BACK;
	p_task_menu_dta->event = event;

	b_event = false;
	p_task_menu_dta->flag = b_event;

	LOGGER_INFO(" ");
	LOGGER_INFO("   %s = %lu   %s = %lu   %s = %s",
				 GET_NAME(state), (uint32_t)state,
				 GET_NAME(event), (uint32_t)event,
				 GET_NAME(b_event), (b_event ? "true" : "false"));

	cycle_counter_init();
	cycle_counter_reset();

	/* Init & Print out: LCD Display */
	displayInit( DISPLAY_CONNECTION_GPIO_4BITS );

    displayCharPositionWrite(0, 0);
	displayStringWrite("TdSE Bienvenidos");

	displayCharPositionWrite(0, 1);
	displayStringWrite("TPF integrador");

	HAL_Delay(2000);

	scroll_reset(p_task_menu_dta, 2);
	display_init(p_task_menu_dta->scroll_idx);

	g_task_menu_tick_cnt = G_TASK_MEN_TICK_CNT_INI;
}

void task_menu_update(void *parameters)
{
	task_menu_dta_t *p_task_menu_dta;
	num_buffer_t *p_num_buf;
	bool b_time_update_required = false;
	char menu_str[DISPLAY_CHAR_WIDTH + 1];

	/* Update Task Menu Counter */
	g_task_menu_cnt++;

	/* Protect shared resource (g_task_menu_tick) */
	__asm("CPSID i");	/* disable interrupts*/
    if (G_TASK_MEN_TICK_CNT_INI < g_task_menu_tick_cnt)
    {
    	g_task_menu_tick_cnt--;
    	b_time_update_required = true;
    }
    __asm("CPSIE i");	/* enable interrupts*/

    while (b_time_update_required)
    {
		/* Protect shared resource (g_task_menu_tick) */
		__asm("CPSID i");	/* disable interrupts*/
		if (G_TASK_MEN_TICK_CNT_INI < g_task_menu_tick_cnt)
		{
			g_task_menu_tick_cnt--;
			b_time_update_required = true;
		}
		else
		{
			b_time_update_required = false;
		}
		__asm("CPSIE i");	/* enable interrupts*/

    	/* Update Task Menu Data Pointer */
		p_task_menu_dta = &task_menu_dta;
		p_num_buf =	&num_buff;

    	if (DEL_MEN_XX_MIN < p_task_menu_dta->tick) {
			p_task_menu_dta->tick--;
            continue;
		}

        p_task_menu_dta->tick = DEL_MEN_XX_MAX;

        /* Run Task Menu Statechart */
        task_menu_statechart();
	}
}


/********************** internal functions definition ***********************/


//Función de scrolleo para cambiar configuraciones/lecturas en un mismo estado
void scrolling(task_menu_dta_t *s_task_menu_dta, uint32_t value) {
	if (s_task_menu_dta->scroll_max == 0) return;

    if (value == SCROLL_UP) {
        if (s_task_menu_dta->scroll_idx == 0)
        	s_task_menu_dta->scroll_idx = s_task_menu_dta->scroll_max - 1;
        else s_task_menu_dta->scroll_idx--;
    }
    else if (value == SCROLL_DOWN) {
    	s_task_menu_dta->scroll_idx++;
        if (s_task_menu_dta->scroll_idx >= s_task_menu_dta->scroll_max)
        	s_task_menu_dta->scroll_idx = 0;
    }
}

void scroll_reset(task_menu_dta_t *s_task_menu_dta, uint32_t max) {
	s_task_menu_dta->scroll_idx = 0;
	s_task_menu_dta->scroll_max = max;
}


//Display del estado init modificable por scroll
void display_init(uint32_t idx) {
	displayClearScreen();

	if (idx == 1) {
		displayCharPositionWrite(0, 0);
		displayStringWrite("Lectura de");
		displayCharPositionWrite(0, 1);
		displayStringWrite("datos");
	}
	else  {
		displayCharPositionWrite(0, 0);
		displayStringWrite("Configurar");
		displayCharPositionWrite(0, 1);
		displayStringWrite("parametros");
	}
}

//Display del estado config modificable por scroll
void display_config(uint32_t idx) {
	displayClearScreen();

	displayCharPositionWrite(0, 0);
	displayStringWrite("Configuracion");
	displayCharPositionWrite(0, 1);
	switch (idx) {
		case 0:
			displayStringWrite("temporal");
			break;
		case 1:
			displayStringWrite("temperatura");
			break;
		case 2:
			displayStringWrite("humedad");
			break;
		case 3:
			displayStringWrite("luminocidad");
			break;
	}
}

//Display del estado config_time modificable por scroll
void display_cfg_time(uint32_t idx) {
	displayClearScreen();

	switch (idx) {
		case 0:
			displayCharPositionWrite(0, 0);
			displayStringWrite("Fijar día");
			break;
		case 1:
			displayCharPositionWrite(0, 0);
			displayStringWrite("Fijar mes");
			break;
		case 2:
			displayCharPositionWrite(0, 0);
			displayStringWrite("Fijar año");
			break;
		case 3:
			displayCharPositionWrite(0, 0);
			displayStringWrite("Fijar hora");
			break;
		case 4:
			displayCharPositionWrite(0, 0);
			displayStringWrite("Fijar minuto");
			break;
		case 5:
			displayCharPositionWrite(0, 0);
			displayStringWrite("Freq muestreo");
			break;
	}
}

//Display del estado config_temp modificable por scroll
void display_cfg_temp(uint32_t idx) {
	displayClearScreen();

	displayCharPositionWrite(0, 0);
	switch (idx) {
		case 0:
			displayStringWrite("Diurna minima");
			break;
		case 1:
			displayStringWrite("Diurna maxima");
			break;
		case 2:
			displayStringWrite("Nocturna minima");
			break;
		case 3:
			displayStringWrite("Nocturna maxima");
			break;
	}
}

//Display del estado config_hum modificable por scroll
void display_cfg_hum(uint32_t idx) {
	displayClearScreen();

	displayCharPositionWrite(0, 0);
	switch (idx) {
		case 0:
			displayStringWrite("Humedad minima");
			break;
		case 1:
			displayStringWrite("Humedad maxima");
			break;
	}
}

//Display del estado config_lig modificable por scroll
void display_cfg_lig(uint32_t idx) {
	displayClearScreen();

	displayCharPositionWrite(0, 0);
	switch (idx) {
		case 0:
			displayStringWrite("Luminocidad minima");
			break;
		case 1:
			displayStringWrite("Horas de luz");
			break;
	}
}

//Display del estado read modificable por scroll
void display_read(uint32_t idx) {
	displayClearScreen();

	displayCharPositionWrite(0, 0);
	displayStringWrite("Lectura de datos");
	displayCharPositionWrite(0, 1);
	switch (idx) {
		case 0:
			displayStringWrite("temporales");
			break;
		case 1:
			displayStringWrite("temperatura");
			break;
		case 2:
			displayStringWrite("humedad");
			break;
		case 3:
			displayStringWrite("luminocidad");
			break;
	}
}

//Falta implementar conversión de uint y enum meses a str
void display_read_time() {
	displayClearScreen();

    date_time_t clk = clock_get_time();
    char line1[17];
    char line2[17];

    snprintf(line1, sizeof(line1), "%02u/%02u/%04u",
             clk.day, clk.month, clk.year);
    snprintf(line2, sizeof(line2), "%02u:%02u",
             clk.hours, clk.minutes);

    displayCharPositionWrite(0, 0);
    displayStringWrite(line1);
    displayCharPositionWrite(0, 1);
    displayStringWrite(line2);
}


/*Falta crear funciones de acciones, guardas y display (condicional cambiar los elif por switch anidados con if para guardas) */
//Maquina de estados
void task_menu_statechart(void)
{
	task_menu_dta_t *p_task_menu_dta;
	num_buffer_t *p_num_buf;

    /* Update Task Menu Data Pointer */
	p_task_menu_dta = &task_menu_dta;
	p_num_buf =	&num_buff;


	/*Use for numerical values and scroll identification */
	uint32_t value = KEY_VALUE_INVALID;

	//Desencola evento y recupera boton presionado
	if (any_event_task_menu()) {
		p_task_menu_dta->flag = true;
		p_task_menu_dta->event = get_event_task_menu(&value);
	}
	else return;

	//Guardado de estado previo y verificacion de cambio de estado (solo en back y enter)
	bool change_state = p_task_menu_dta->state != p_task_menu_dta->prev_state;
	p_task_menu_dta->prev_state = p_task_menu_dta->state;


	/*El flag se vuelve redundante para la maquina de estados*/
	p_task_menu_dta->flag = false;

	//Elección de estado
	switch (p_task_menu_dta->state) {
		case ST_MENU_INIT:
			if (change_state) {
				//Inicializa scroll y display
				scroll_reset(p_task_menu_dta, 2);
				display_init(p_task_menu_dta->scroll_idx);
			}
	    	if (p_task_menu_dta->event == EV_PRESS_SCROLL) {
	    		/* Selección y display*/
	    		scrolling(p_task_menu_dta, value);
	    		display_init(p_task_menu_dta->scroll_idx);
	        }
	        else if (p_task_menu_dta->event == EV_PRESS_NEXT) {
	        	if (p_task_menu_dta->scroll_idx == 0)
	        		p_task_menu_dta->state = ST_MENU_CONFIG;
	            else p_task_menu_dta->state = ST_MENU_READ;
	        }
	    	break;

		case ST_MENU_CONFIG:
			if (change_state) {
				//Inicializa scroll y display
				scroll_reset(p_task_menu_dta, 4);
	    		display_config(p_task_menu_dta->scroll_idx);
			}
	    	if (p_task_menu_dta->event == EV_PRESS_SCROLL) {
	    		/* Selección*/
	    		scrolling(p_task_menu_dta, value);
	    		display_config(p_task_menu_dta->scroll_idx);
	        }
	        else if (p_task_menu_dta->event == EV_PRESS_BACK) {
	                p_task_menu_dta->state = ST_MENU_INIT;
	        }
	        else if (p_task_menu_dta->event == EV_PRESS_NEXT) {
	        	if (p_task_menu_dta->scroll_idx == 0)      p_task_menu_dta->state = ST_MENU_CONFIG_TIME;
	        	else if (p_task_menu_dta->scroll_idx == 1) p_task_menu_dta->state = ST_MENU_CONFIG_TEMP;
	        	else if (p_task_menu_dta->scroll_idx == 2)  p_task_menu_dta->state = ST_MENU_CONFIG_HUM;
	            else if (p_task_menu_dta->scroll_idx == 3)  p_task_menu_dta->state = ST_MENU_CONFIG_LIG;
	        }
	        break;

		case ST_MENU_CONFIG_TIME:
			if (change_state) {
				/*Inicializa scroll y display*/
				scroll_reset(p_task_menu_dta, 6);
				/*dia/mes/año/hora/minuto/frecuencia de muestreo*/
	    		display_cfg_time(p_task_menu_dta->scroll_idx);
			}
	    	if (p_task_menu_dta->event == EV_PRESS_SCROLL) {
	    		/* Selección*/
	    		scrolling(p_task_menu_dta, value);
	    		display_cfg_time(p_task_menu_dta->scroll_idx);
	        }// Guarda cada dato por separado
	        else if (p_task_menu_dta->event == EV_PRESS_ENTER) {
				switch (p_task_menu_dta->scroll_idx){
					case 0:
						clock_config_set_day(num_buffer_to_int(p_num_buf));
				        displayCharPositionWrite(0, 1);
				        displayStringWrite("OK");
						break;

					case 1:
						clock_config_set_month(num_buffer_to_int(p_num_buf));
				        displayCharPositionWrite(0, 1);
				        displayStringWrite("OK");
						break;

					case 2:
						clock_config_set_year(num_buffer_to_int(p_num_buf));
				        displayCharPositionWrite(0, 1);
				        displayStringWrite("OK");
						break;

					case 3:
						clock_config_set_hour(num_buffer_to_int(p_num_buf));
				        displayCharPositionWrite(0, 1);
				        displayStringWrite("OK");
						break;

					case 4:
						clock_config_set_minute(num_buffer_to_int(p_num_buf));
				        displayCharPositionWrite(0, 1);
				        displayStringWrite("OK");
						break;

					case 5:
						mem_status_t status;
						uint16_t num = num_buffer_to_int(p_num_buf);
						status = memory_write_config_field(MEM_CFG_SAVE_FREQ, &num);
						if (status == ST_MEM_OK) {
				        	displayCharPositionWrite(0, 1);
				        	displayStringWrite("OK");
						}
						break;
				}
				num_buffer_reset(p_num_buf);
	        }
	        else if (p_task_menu_dta->event == EV_PRESS_BACK) {
	            p_task_menu_dta->state = ST_MENU_CONFIG;
	        	num_buffer_reset(p_num_buf);
	        }//Guardar cada valor numerico
	        else if (p_task_menu_dta->event == EV_PRESS_NUM) {
	        	num_buffer_push(p_num_buf, value);
	        	char pushed_num[NUM_BUFFER_SIZE + 1];
	        	displayCharPositionWrite(0, 1);
	        	displayStringWrite(num_buffer_to_str(p_num_buf, pushed_num));
	        }
	        break;

		case ST_MENU_CONFIG_TEMP:
			if (change_state) {
				/*Inicializa scroll y display*/
				scroll_reset(p_task_menu_dta, 4);
				/*El scroll 0 es configurar minima diurna, 1 maxima diurna, 2 minima nocturna y 3 maxima nocturna*/
				display_cfg_temp(p_task_menu_dta->scroll_idx);
			}
	    	if (p_task_menu_dta->event == EV_PRESS_SCROLL) {
	    		/* Selección*/
	    		scrolling(p_task_menu_dta, value);
	    		display_cfg_temp(p_task_menu_dta->scroll_idx);
	        }// Guarda cada dato por separado
			else if (p_task_menu_dta->event == EV_PRESS_ENTER) {
				mem_status_t status;
				uint16_t num = num_buffer_to_int(p_num_buf);
				switch (p_task_menu_dta->scroll_idx){
					case 0:
						status = memory_write_config_field(MEM_CFG_TEMP_DAY_MIN, &num);
						if (status == ST_MEM_OK) {
				        	displayCharPositionWrite(0, 1);
				        	displayStringWrite("OK");
						}
						break;

					case 1:
						status = memory_write_config_field(MEM_CFG_TEMP_DAY_MAX, &num);
						if (status == ST_MEM_OK) {
				        	displayCharPositionWrite(0, 1);
				        	displayStringWrite("OK");
						}
						break;

					case 2:
						status = memory_write_config_field(MEM_CFG_TEMP_NIGHT_MIN, &num);
						if (status == ST_MEM_OK) {
				        	displayCharPositionWrite(0, 1);
				        	displayStringWrite("OK");
						}
						break;

					case 3:
						status = memory_write_config_field(MEM_CFG_TEMP_NIGHT_MAX, &num);
						if (status == ST_MEM_OK) {
				        	displayCharPositionWrite(0, 1);
				        	displayStringWrite("OK");
						}
						break;
				}
				num_buffer_reset(p_num_buf);
	        }
			else if (p_task_menu_dta->event == EV_PRESS_BACK) {
	        	p_task_menu_dta->state = ST_MENU_CONFIG;
	        	num_buffer_reset(p_num_buf);
	        }//Guardar cada valor numerico
	        else if (p_task_menu_dta->event == EV_PRESS_NUM) {
	        	num_buffer_push(p_num_buf, value);
	        	char pushed_num[NUM_BUFFER_SIZE + 1];
	        	displayCharPositionWrite(0, 1);
	        	displayStringWrite(num_buffer_to_str(p_num_buf, pushed_num));
	        }
			break;

		case ST_MENU_CONFIG_HUM:
			if (change_state) {
				/*Inicializa scroll y display*/
				scroll_reset(p_task_menu_dta, 2);
				/*El scroll 0 es configurar humedad minima y el 1 configurar humedad maxima*/
				display_cfg_hum(p_task_menu_dta->scroll_idx);
			}
	    	if (p_task_menu_dta->event == EV_PRESS_SCROLL) {
	    		/* Selección*/
	    		scrolling(p_task_menu_dta, value);
				display_cfg_hum(p_task_menu_dta->scroll_idx);
	        }// Guarda cada dato por separado
			else if (p_task_menu_dta->event == EV_PRESS_ENTER) {
				mem_status_t status;
				uint16_t num = num_buffer_to_int(p_num_buf);
				switch (p_task_menu_dta->scroll_idx){
					case 0:
						status = memory_write_config_field(MEM_CFG_HUMIDITY_MIN, &num);
						if (status == ST_MEM_OK) {
				        	displayCharPositionWrite(0, 1);
				        	displayStringWrite("OK");
						}
						break;

					case 1:
						status = memory_write_config_field(MEM_CFG_HUMIDITY_MAX, &num);
						if (status == ST_MEM_OK) {
				        	displayCharPositionWrite(0, 1);
				        	displayStringWrite("OK");
						}
						break;
				}
				num_buffer_reset(p_num_buf);
	        }
			else if (p_task_menu_dta->event == EV_PRESS_BACK) {
				p_task_menu_dta->state = ST_MENU_CONFIG;
	        	num_buffer_reset(p_num_buf);
	        }//Guardar cada valor numerico
	        else if (p_task_menu_dta->event == EV_PRESS_NUM) {
	        	num_buffer_push(p_num_buf, value);
	        	char pushed_num[NUM_BUFFER_SIZE + 1];
	        	displayCharPositionWrite(0, 1);
	        	displayStringWrite(num_buffer_to_str(p_num_buf, pushed_num));
	        }
			break;

		case ST_MENU_CONFIG_LIG:
			if (change_state) {
				/*Inicializa scroll y display*/
				scroll_reset(p_task_menu_dta, 2);
				/*El scroll 0 es configurar luminosiad minima y el 1 configurar horas de luz*/
				display_cfg_lig(p_task_menu_dta->scroll_idx);
			}
	    	if (p_task_menu_dta->event == EV_PRESS_SCROLL) {
	    		/* Selección*/
	    		scrolling(p_task_menu_dta, value);
				display_cfg_lig(p_task_menu_dta->scroll_idx);
	        }// Guarda cada dato por separado
			else if (p_task_menu_dta->event == EV_PRESS_ENTER) {
				mem_status_t status;
				uint16_t num = num_buffer_to_int(p_num_buf);
				switch (p_task_menu_dta->scroll_idx){
					case 0:
						status = memory_write_config_field(MEM_CFG_LIGHT_THRESHOLD, &num);
						if (status == ST_MEM_OK) {
				        	displayCharPositionWrite(0, 1);
				        	displayStringWrite("OK");
						}
						break;

					case 1:
						status = memory_write_config_field(MEM_CFG_LIGHT_HOURS_NEEDED, &num);
						if (status == ST_MEM_OK) {
				        	displayCharPositionWrite(0, 1);
				        	displayStringWrite("OK");
						}
						break;
				}
				num_buffer_reset(p_num_buf);
	        }
			else if (p_task_menu_dta->event == EV_PRESS_BACK) {
				p_task_menu_dta->state = ST_MENU_CONFIG;
	        	num_buffer_reset(p_num_buf);
	        }//Guardar cada valor numerico
	        else if (p_task_menu_dta->event == EV_PRESS_NUM) {
	        	num_buffer_push(p_num_buf, value);
	        	char pushed_num[NUM_BUFFER_SIZE + 1];
	        	displayCharPositionWrite(0, 1);
	        	displayStringWrite(num_buffer_to_str(p_num_buf, pushed_num));
	        }
			break;

		case ST_MENU_READ:
			if (change_state) {
				/*Inicializa scroll y display*/
				scroll_reset(p_task_menu_dta, 4);
				display_read(p_task_menu_dta->scroll_idx);
			}
	    	if (p_task_menu_dta->event == EV_PRESS_SCROLL) {
	    		/* Selección*/
	    		scrolling(p_task_menu_dta, value);
				display_read(p_task_menu_dta->scroll_idx);
	        }
			else if (p_task_menu_dta->event == EV_PRESS_BACK){
	            p_task_menu_dta->state = ST_MENU_INIT;
	        }
			else if (p_task_menu_dta->event == EV_PRESS_NEXT) {
				if (p_task_menu_dta->scroll_idx == 0) p_task_menu_dta->state = ST_MENU_READ_TIME;
				/*else if (p_task_menu_dta->scroll_idx == 1) p_task_menu_dta->state = ST_MENU_READ_TEMP;
				else if (p_task_menu_dta->scroll_idx == 2)  p_task_menu_dta->state = ST_MENU_READ_HUM;
				else if (p_task_menu_dta->scroll_idx == 3)  p_task_menu_dta->state = ST_MENU_READ_LIG;*/
	        }
			break;
		//Hora actual
		case ST_MENU_READ_TIME:
			display_read_time();
			if (p_task_menu_dta->event == EV_PRESS_BACK) {
				p_task_menu_dta->state = ST_MENU_READ;
			}
			break;
		//Pre estado de lecturas de temperatura
/*		case ST_MENU_READ_TEMP:
			if (change_state) {
				//Inicializa scroll y display
				scroll_reset(p_task_menu_dta, 2);
				// 0 para lecturas de configuración y 1 para muestras
				display_read_temp(p_task_menu_dta->scroll_idx);
			}
	    	if (p_task_menu_dta->event == EV_PRESS_SCROLL) {
	    		// Selección
	    		scrolling(p_task_menu_dta, value);
				display_read_temp(p_task_menu_dta->scroll_idx);
	        }
			else if (p_task_menu_dta->event == EV_PRESS_NEXT) {
				if (p_task_menu_dta->scroll_idx == 0) p_task_menu_dta->state = ST_MENU_READ_TEMP_CON;
	            else p_task_menu_dta->state = ST_MENU_READ_TEMP_HIS;
	        }
			else if (p_task_menu_dta->event == EV_PRESS_BACK) {
				p_task_menu_dta->state = ST_MENU_READ;
			}
			break;
		//Pre estado de lecturas de humedad
		case ST_MENU_READ_HUM:
			if (change_state) {
				//Inicializa scroll y display
				scroll_reset(p_task_menu_dta, 2);
				// 0 para lecturas de configuración y 1 para muestras
				display_read_hum(p_task_menu_dta->scroll_idx);
			}
	    	if (p_task_menu_dta->event == EV_PRESS_SCROLL) {
	    		// Selección
	    		scrolling(p_task_menu_dta, value);
				display_read_hum(p_task_menu_dta->scroll_idx);
	        }
			else if (p_task_menu_dta->event == EV_PRESS_NEXT) {
				if (p_task_menu_dta->scroll_idx == 0) p_task_menu_dta->state = ST_MENU_READ_HUM_CON;
	            else p_task_menu_dta->state = ST_MENU_READ_HUM_HIS;
	        }
			else if (p_task_menu_dta->event == EV_PRESS_BACK) {
				p_task_menu_dta->state = ST_MENU_READ;
			}
			break;
		//Pre estado de lecturas de luz
		case ST_MENU_READ_LIG:
			if (change_state) {
				//Inicializa scroll y display
				scroll_reset(p_task_menu_dta, 2);
				// 0 para lecturas de configuración y 1 para muestras
				display_read_lig(p_task_menu_dta->scroll_idx);
			}
	    	if (p_task_menu_dta->event == EV_PRESS_SCROLL) {
	    		//Selección
	    		scrolling(p_task_menu_dta, value);
				display_read_lig(p_task_menu_dta->scroll_idx);
	        }
			else if (p_task_menu_dta->event == EV_PRESS_NEXT) {
				if (p_task_menu_dta->scroll_idx == 0) p_task_menu_dta->state = ST_MENU_READ_LIG_CON;
	            else p_task_menu_dta->state = ST_MENU_READ_LIG_HIS;
	        }
			else if (p_task_menu_dta->event == EV_PRESS_BACK) {
				p_task_menu_dta->state = ST_MENU_READ;
			}
			break;
		//Lectura de configuraciones de temperatura
		case ST_MENU_READ_TEMP_CON:
			if (change_state) {
				//Inicializa scroll y display
				scroll_reset(p_task_menu_dta, 4);
				// 0 para minima diurna, 1 para maxima diurna, 2 para minima nocturna y 3 para maxima nocturna
				display_read_temp_con(p_task_menu_dta->scroll_idx);
			}
	    	if (p_task_menu_dta->event == EV_PRESS_SCROLL) {
	    		// Selección
	    		scrolling(p_task_menu_dta, value);
	    		display_read_temp_con(p_task_menu_dta->scroll_idx);
	        }
			if (p_task_menu_dta->event == EV_PRESS_BACK) {
				p_task_menu_dta->state = ST_MENU_READ_TEMP;
			}
			break;
		//Lectura de muestras de temperatura
		case ST_MENU_READ_TEMP_HIS:
			if (change_state) {
				scroll_reset(p_task_menu_dta, data_count()); //Función que devuelve cantidad total de muestras
	    		display_read_temp_samples(p_task_menu_dta->scroll_idx);
			}
	    	if (p_task_menu_dta->event == EV_PRESS_SCROLL) {
	    		// Selección
	    		scrolling(p_task_menu_dta, value);
	    		display_read_temp_samples(p_task_menu_dta->scroll_idx);
	        }
			if (p_task_menu_dta->event == EV_PRESS_BACK) {
				p_task_menu_dta->state = ST_MENU_READ_TEMP;
			}
			break;
		case ST_MENU_READ_HUM_CON:
		//Lectura de configuraciones de humedad
			if (change_state) {
				//Inicializa scroll y display
				scroll_reset(p_task_menu_dta, 2);
				// 0 para minima 1 para maxima
				display_read_hum_con(p_task_menu_dta->scroll_idx);
			}
	    	if (p_task_menu_dta->event == EV_PRESS_SCROLL) {
	    		// Selección
	    		scrolling(p_task_menu_dta, value);
	    		display_read_hum_con(p_task_menu_dta->scroll_idx);
	        }
			if (p_task_menu_dta->event == EV_PRESS_BACK) {
				p_task_menu_dta->state = ST_MENU_READ_HUM;
			}
			break;
		//Lectura de muestras de humedad
		case ST_MENU_READ_HUM_HIS:
			if (change_state) {
				p_task_menu_dta->scroll_idx = 0;
				p_task_menu_dta->scroll_max = data_count(); //Función que devuelve cantidad total de muestras
				display_read_hum_samples(p_task_menu_dta->scroll_idx);
			}
	    	if (p_task_menu_dta->event == EV_PRESS_SCROLL) {
	    		//Selección
	    		scrolling(p_task_menu_dta, value);
				display_read_hum_samples(p_task_menu_dta->scroll_idx);
	        }
			if (p_task_menu_dta->event == EV_PRESS_BACK) {
				p_task_menu_dta->state = ST_MENU_READ_HUM;
			}
			break;
		//Lectura de configuraciones de luz
		case ST_MENU_READ_LIG_CON:
			if (change_state) {
				//Inicializa scroll y display
				scroll_reset(p_task_menu_dta, 2);
				// 0 para minima 1 para horas de luz
				display_read_lig_con(p_task_menu_dta->scroll_idx);
			}
	    	if (p_task_menu_dta->event == EV_PRESS_SCROLL) {
	    		// Selección
	    		scrolling(p_task_menu_dta, value);
	    		display_read_lig_con(p_task_menu_dta->scroll_idx);
	        }
			if (p_task_menu_dta->event == EV_PRESS_BACK) {
				p_task_menu_dta->state = ST_MENU_READ_LIG;
			}
			break;
		//Lectura de muestras de luz
		case ST_MENU_READ_LIG_HIS:
			if (change_state) {
				p_task_menu_dta->scroll_idx = 0;
				p_task_menu_dta->scroll_max = data_count(); //Función que devuelve cantidad total de muestras
				display_read_lig_samples(p_task_menu_dta->scroll_idx);
			}
	    	if (p_task_menu_dta->event == EV_PRESS_SCROLL) {
	    		// Selección
	    		scrolling(p_task_menu_dta, value);
				display_read_lig_samples(p_task_menu_dta->scroll_idx);
	        }
			if (p_task_menu_dta->event == EV_PRESS_BACK) {
				p_task_menu_dta->state = ST_MENU_READ_LIG;
			}
			break;
		*/
		default:
			p_task_menu_dta->state = ST_MENU_INIT;
			p_task_menu_dta->flag  = false;
			break;
	    }
}


