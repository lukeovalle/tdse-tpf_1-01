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

/********************** macros and definitions *******************************/
#define G_TASK_MEN_CNT_INI			0ul
#define G_TASK_MEN_TICK_CNT_INI		0ul

#define DEL_MEN_XX_MIN				0ul
#define DEL_MEN_XX_MED				50ul
#define DEL_MEN_XX_MAX				500ul

#define EVENT_UNDEFINED	(255)
#define MAX_EVENTS		(16)
#define KEY_VALUE_INVALID 10

/********************** internal data declaration ****************************/
task_menu_dta_t task_menu_dta =
	{DEL_MEN_XX_MIN, ST_MENU_INIT, ST_MENU_INIT, EV_PRESS_BACK, KEY_VALUE_INVALID, false};

#define MENU_DTA_QTY	(sizeof(task_menu_dta)/sizeof(task_menu_dta_t))

motor_cfg_t motor_cfg[] = {
    {false, 0, RIGHT},
    {false, 0, RIGHT}
};

#define MOTOR_CFG_QTY (sizeof(motor_cfg)/sizeof(motor_cfg_t))

uint32_t current_motor = 0;
motor_cfg_type_t current_motor_cfg_type = POWER;
motor_cfg_t temp_motor_cfg;

/********************** internal functions declaration ***********************/
motor_cfg_type_t next_motor_cfg(motor_cfg_type_t current);
void change_current_cfg(motor_cfg_t * cfg, motor_cfg_type_t type);
void text_info_motor_in_row(char str[], uint8_t index, motor_cfg_t * cfg);
void text_select_motor(char str[], uint8_t index);
void text_select_config(char str[], motor_cfg_type_t type);
void text_select_value(char str[],motor_cfg_t * cfg, motor_cfg_type_t type);
void print_text_in_row(const char str[], uint8_t row);
void task_menu_statechart(void);

/********************** internal data definition *****************************/
const char *p_task_menu 		= "Task Menu (Interactive Menu)";
const char *p_task_menu_ 		= "Non-Blocking & Update By Time Code";
const char header_text[]        = "Enter/Next/Back";
const char empty_line[]			= "                ";

/********************** external data declaration ****************************/
uint32_t g_task_menu_cnt;
volatile uint32_t g_task_menu_tick_cnt;

/********************** external functions definition ************************/
void task_menu_init(void *parameters)
{
	char menu_str[DISPLAY_CHAR_WIDTH + 1];
	task_menu_dta_t *p_task_menu_dta;
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

	task_keypad_init();

	g_task_menu_tick_cnt = G_TASK_MEN_TICK_CNT_INI;
}

void task_menu_update(void *parameters)
{
	task_menu_dta_t *p_task_menu_dta;
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

    	if (DEL_MEN_XX_MIN < p_task_menu_dta->tick) {
			p_task_menu_dta->tick--;
            continue;
		}

        p_task_menu_dta->tick = DEL_MEN_XX_MAX;

        task_keypad_update();
        if (true == any_event_task_menu())
        {
            p_task_menu_dta->flag = true;
            p_task_menu_dta->event = get_event_task_menu();
        }

        /* Run Task Menu Statechart */
        task_menu_statechart()
	}
}

motor_cfg_type_t next_motor_cfg(motor_cfg_type_t current) {
    switch (current) {
    case POWER:
        return SPEED;
    case SPEED:
        return SPIN;
    case SPIN:
        return POWER;
    default:
        return POWER;
    }
}

void change_current_cfg(motor_cfg_t * cfg, motor_cfg_type_t type) {
    switch (type) {
    case POWER:
        cfg->power = !cfg->power;
        break;

    case SPEED:
        cfg->speed++;
        if (cfg->speed > MAX_SPEED)
            cfg->speed = 0;
        break;

    case SPIN:
        if (cfg->spin == LEFT)
            cfg->spin = RIGHT;
        else
            cfg->spin = LEFT;
        break;

    default:
        break;
    }
}


void text_conf(task_menu_dta_t *p) {
	if (p->state == ST_INIT && p->event == EV_PRESS_SCROLL) {
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

void text_info_motor_in_row(char str[], uint8_t index, motor_cfg_t * cfg) {
    snprintf(str, DISPLAY_CHAR_WIDTH + 1, "Motor %1.1d %s %1.1d %c",
            index,
            (cfg->power) ? "ON" : "OFF",
            cfg->speed,
            ((cfg->spin == LEFT) ? 'L' : 'R'));
}

void text_select_motor(char str[], uint8_t index) {
    snprintf(str, DISPLAY_CHAR_WIDTH + 1, "> Motor %d", index);
}

void text_select_config(char str[], motor_cfg_type_t type) {
    char *aux;

    switch (type) {
    case POWER:
        aux = "Power";
        break;
    case SPEED:
        aux = "Speed";
        break;
    case SPIN:
        aux = "Spin";
        break;
    default:
        break;
    }
    snprintf(str, DISPLAY_CHAR_WIDTH + 1, "> %s", aux);
}

void text_select_value(char str[],motor_cfg_t * cfg, motor_cfg_type_t type) {
    switch (type) {
    case POWER:
        snprintf(str, DISPLAY_CHAR_WIDTH + 1, "> %s",
                cfg->power ? "ON" : "OFF");
        break;
    case SPEED:
        snprintf(str, DISPLAY_CHAR_WIDTH + 1, "> %d", cfg->speed);
        break;
    case SPIN:
        snprintf(str, DISPLAY_CHAR_WIDTH + 1, "> %s",
                cfg->spin == LEFT ? "LEFT" : "RIGHT");
        break;
    default:
        break;
    }

}

void print_text_in_row(const char str[], uint8_t row) {
    displayCharPositionWrite(0, row);
    displayStringWrite(empty_line);
    displayCharPositionWrite(0, row);
    displayStringWrite(str);
}

/*Falta crear funciones de acciones, guardas y display (condicional cambiar los elif por switch anidados con if para guardas) */
void task_menu_statechart(void)
{
	task_menu_dta_t *p_task_menu_dta;

    /* Update Task Menu Data Pointer */
	p_task_menu_dta = &task_menu_dta;

	/*Use for numerical values and scroll identification */
	uint32_t value = KEY_VALUE_INVALID;

	if (true == any_event_task_menu())
	{
		p_task_menu_dta->flag = true;
		p_task_menu_dta->event = get_event_task_menu(&value);
		/*Codificar acciones con EV_PRESS_NUM mas adelante*/
	}
	else return;

	/*El flag se vuelve redundante para la maquina de estados*/
	p_task_menu_dta->flag = false;

	switch (p_task_menu_dta->state) {
		case ST_MENU_INIT:
			/*Mejorar dysplay para que se altere con cada scroll*/
			display_init(p_task_menu_dta);

	    	if (p_task_menu_dta->event == EV_PRESS_SCROLL) {
	    		/* Selección*/
	    		scroll_init();
	        }
	        else if (p_task_menu_dta->event == EV_PRESS_NEXT) {
	        	if (config_selected())
	        		p_task_menu_dta->state = ST_MENU_CONFIG;
	            else if (!config_selected() && valid_date())
	                p_task_menu_dta->state = ST_MENU_READ;
	        }
	    	break;

		case ST_MENU_CONFIG:
			if (p_task_menu_dta->event == EV_PRESS_SCROLL) {
	            /* Selección*/
				scroll_modes();
	        }
	        else if (p_task_menu_dta->event == EV_PRESS_BACK) {
	                p_task_menu_dta->state = ST_MENU_INIT;
	        }
	        else if (p_task_menu_dta->event == EV_PRESS_NEXT) {
	        	if (time_selected())      p_task_menu_dta->state = ST_MENU_CONFIG_TIME;
	        	else if (temp_selected()) p_task_menu_dta->state = ST_MENU_CONFIG_TEMP;
	        	else if (hum_selected())  p_task_menu_dta->state = ST_MENU_CONFIG_HUM;
	            else if (lig_selected())  p_task_menu_dta->state = ST_MENU_CONFIG_LIG;
	        }
	        break;

		case ST_MENU_CONFIG_TIME:
			if (p_task_menu_dta->event == EV_PRESS_SCROLL) {
				/* Selección*/
				scroll_time();
	        }
	        else if (p_task_menu_dta->event == EV_PRESS_ENTER) {
	            if (valid_date()) save_date();
	        }
	        else if (p_task_menu_dta->event == EV_PRESS_BACK) {
	            p_task_menu_dta->state = ST_MENU_CONFIG;
	        }
	        else if (p_task_menu_dta->event == EV_PRESS_NUM) {
	        	config_time(value);
	        }
	        break;

		case ST_MENU_CONFIG_TEMP:
			if (p_task_menu_dta->event == EV_PRESS_SCROLL) {
				/* Selección*/
				scroll_parameters();
	       	}
			else if (p_task_menu_dta->event == EV_PRESS_ENTER) {
	        	if (valid_temp()) save_temp();
	        }
			else if (p_task_menu_dta->event == EV_PRESS_BACK) {
	        	p_task_menu_dta->state = ST_MENU_CONFIG;
	        }
	        else if (p_task_menu_dta->event == EV_PRESS_NUM) {
	        	config_temp(value);
	        }
			break;

		case ST_MENU_CONFIG_HUM:
			if (p_task_menu_dta->event == EV_PRESS_SCROLL) {
				/* Selección*/
				scroll_parameters();
	       	}
			else if (p_task_menu_dta->event == EV_PRESS_ENTER) {
	        	if (valid_hum()) save_hum();
	        }
			else if (p_task_menu_dta->event == EV_PRESS_BACK) {
				p_task_menu_dta->state = ST_MENU_CONFIG;
	        }
	        else if (p_task_menu_dta->event == EV_PRESS_NUM) {
	        	config_hum(value);
	        }
			break;

		case ST_MENU_CONFIG_LIG:
			if (p_task_menu_dta->event == EV_PRESS_SCROLL) {
				/* Selección*/
				scroll_parameters();
	       	}
			else if (p_task_menu_dta->event == EV_PRESS_ENTER) {
	        	if (valid_lig()) save_lig();
	        }
			else if (p_task_menu_dta->event == EV_PRESS_BACK) {
				p_task_menu_dta->state = ST_MENU_CONFIG;
	        }
	        else if (p_task_menu_dta->event == EV_PRESS_NUM) {
	        	config_lig(value);
	        }
			break;

		case ST_MENU_READ:
			if (p_task_menu_dta->event == EV_PRESS_SCROLL) {
	            /* Selección*/
				scroll_modes();
	        }
			else if (p_task_menu_dta->event == EV_PRESS_BACK){
	            p_task_menu_dta->state = ST_MENU_INIT;
	        }
			else if (p_task_menu_dta->event == EV_PRESS_NEXT) {
				if (time_selected()) p_task_menu_dta->state = ST_MENU_READ_TIME;
				else if (temp_selected()) p_task_menu_dta->state = ST_MENU_READ_TEMP;
				else if (hum_selected())  p_task_menu_dta->state = ST_MENU_READ_HUM;
				else if (lig_selected())  p_task_menu_dta->state = ST_MENU_READ_LIG;
	        }
			break;

		case ST_MENU_READ_TIME:
			display_date();
			if (p_task_menu_dta->event == EV_PRESS_BACK) {
				p_task_menu_dta->state = ST_MENU_READ;
			}
			break;

		case ST_MENU_READ_TEMP:
			if (p_task_menu_dta->event == EV_PRESS_SCROLL) {
				/* Selección*/
				scroll_lectures();
	        }
			else if (p_task_menu_dta->event == EV_PRESS_NEXT) {
				if (!history_selected()) p_task_menu_dta->state = ST_MENU_READ_TEMP_CON;
	            else p_task_menu_dta->state = ST_MENU_READ_TEMP_HIS;
	        }
			else if (p_task_menu_dta->event == EV_PRESS_BACK) {
				p_task_menu_dta->state = ST_MENU_READ;
			}
			break;

		case ST_MENU_READ_HUM:
			if (p_task_menu_dta->event == EV_PRESS_SCROLL) {
				/* Selección*/
				scroll_lectures();
	        }
			else if (p_task_menu_dta->event == EV_PRESS_NEXT) {
				if (!history_selected()) p_task_menu_dta->state = ST_MENU_READ_HUM_CON;
	            else p_task_menu_dta->state = ST_MENU_READ_HUM_HIS;
	        }
			else if (p_task_menu_dta->event == EV_PRESS_BACK) {
				p_task_menu_dta->state = ST_MENU_READ;
			}
			break;

		case ST_MENU_READ_LIG:
			if (p_task_menu_dta->event == EV_PRESS_SCROLL) {
				/* Selección*/
				scroll_lectures();
	        }
			else if (p_task_menu_dta->event == EV_PRESS_NEXT) {
				if (!history_selected()) p_task_menu_dta->state = ST_MENU_READ_LIG_CON;
	            else p_task_menu_dta->state = ST_MENU_READ_LIG_HIS;
	        }
			else if (p_task_menu_dta->event == EV_PRESS_BACK) {
				p_task_menu_dta->state = ST_MENU_READ;
			}
			break;

		case ST_MENU_READ_TEMP_CON:
			display_cfg_temp();
			if (p_task_menu_dta->event == EV_PRESS_BACK) {
				p_task_menu_dta->state = ST_MENU_READ_TEMP;
			}
			break;

		case ST_MENU_READ_TEMP_HIS:
			display_data_temp();
			if (p_task_menu_dta->event == EV_PRESS_SCROLL) {
				/* Selección de medición*/
				scroll_history();
	        }
			if (p_task_menu_dta->event == EV_PRESS_BACK) {
				p_task_menu_dta->state = ST_MENU_READ_TEMP;
			}
			break;

		case ST_MENU_READ_HUM_CON:
			display_cfg_hum();
			if (p_task_menu_dta->event == EV_PRESS_BACK) {
				p_task_menu_dta->state = ST_MENU_READ_HUM;
			}
			break;

		case ST_MENU_READ_HUM_HIS:
			display_data_hum();
			if (p_task_menu_dta->event == EV_PRESS_SCROLL) {
				/* Selección de medición*/
				scroll_history();
	        }
			if (p_task_menu_dta->event == EV_PRESS_BACK) {
				p_task_menu_dta->state = ST_MENU_READ_HUM;
			}
			break;

		case ST_MENU_READ_LIG_CON:
			display_cfg_lig();
			if (p_task_menu_dta->event == EV_PRESS_BACK) {
				p_task_menu_dta->state = ST_MENU_READ_LIG;
			}
			break;

		case ST_MENU_READ_LIG_HIS:
			display_data_lig();
			if (p_task_menu_dta->event == EV_PRESS_SCROLL) {
				/* Selección de medición*/
				scroll_history();
	        }
			if (p_task_menu_dta->event == EV_PRESS_BACK) {
				p_task_menu_dta->state = ST_MENU_READ_LIG;
			}
			break;

		default:
			p_task_menu_dta->state = ST_MENU_INIT;
			p_task_menu_dta->flag  = false;
			break;
	    }
}


