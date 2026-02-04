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
#include "display.h"

/********************** macros and definitions *******************************/
#define G_TASK_MEN_CNT_INI			0ul
#define G_TASK_MEN_TICK_CNT_INI		0ul

#define DEL_MEN_XX_MIN				0ul
#define DEL_MEN_XX_MED				50ul
#define DEL_MEN_XX_MAX				500ul

/********************** internal data declaration ****************************/
task_menu_dta_t task_menu_dta =
	{DEL_MEN_XX_MIN, ST_MENU_XX_MAIN, EV_MEN_ENT_IDLE, false};

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
	task_menu_dta_t *p_task_menu_dta;
	task_menu_ST_MENU_t	state;
	task_menu_ev_t	event;
	bool b_event;
	char menu_str[DISPLAY_CHAR_WIDTH + 1];

	/* Print out: Task Initialized */
	LOGGER_LOG("  %s is running - %s\r\n", GET_NAME(task_menu_init), p_task_menu);
	LOGGER_LOG("  %s is a %s\r\n", GET_NAME(task_menu), p_task_menu_);

	g_task_menu_cnt = G_TASK_MEN_CNT_INI;

	/* Print out: Task execution counter */
	LOGGER_LOG("   %s = %lu\r\n", GET_NAME(g_task_menu_cnt), g_task_menu_cnt);

	init_queue_event_task_menu();

	/* Update Task Actuator Configuration & Data Pointer */
	p_task_menu_dta = &task_menu_dta;

	/* Print out: Task execution FSM */
	state = p_task_menu_dta->state;
	LOGGER_LOG("   %s = %lu", GET_NAME(state), (uint32_t)state);

	event = p_task_menu_dta->event;
	LOGGER_LOG("   %s = %lu", GET_NAME(event), (uint32_t)event);

	b_event = p_task_menu_dta->flag;
	LOGGER_LOG("   %s = %s\r\n", GET_NAME(b_event), (b_event ? "true" : "false"));

	cycle_counter_init();
	cycle_counter_reset();

	displayInit( DISPLAY_CONNECTION_GPIO_4BITS );

    // Imprime el texto del menú principal, en dos filas

    text_info_motor_in_row(menu_str, 0, motor_cfg + 0);
    print_text_in_row(menu_str, 0);

    text_info_motor_in_row(menu_str, 1, motor_cfg + 1);
    print_text_in_row(menu_str, 1);

    // pongo el motor 0 a configurar por defecto.
    temp_motor_cfg = motor_cfg[0];

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

        if (true == any_event_task_menu())
        {
            p_task_menu_dta->flag = true;
            p_task_menu_dta->event = get_event_task_menu();
        }

        switch (p_task_menu_dta->state) {
            case ST_MENU_XX_MAIN:
                if (!p_task_menu_dta->flag)
                    break;

                p_task_menu_dta->flag = false;

                // Imprime el texto del menú principal, en dos filas
                text_info_motor_in_row(menu_str, 0, motor_cfg + 0);
                print_text_in_row(menu_str, 0);

                text_info_motor_in_row(menu_str, 1, motor_cfg + 1);
                print_text_in_row(menu_str, 1);


                switch (p_task_menu_dta->event) {
                    case EV_MEN_ENT_ACTIVE:
                        p_task_menu_dta->state = ST_MENU_XX_SELECT_MOTOR;
                        break;

                    default:
                        break;
                }

                break;

            case ST_MENU_XX_SELECT_MOTOR:
                if (!p_task_menu_dta->flag)
                    break;

                p_task_menu_dta->flag = false;

                print_text_in_row(header_text, 0);

                text_select_motor(menu_str, current_motor);
                print_text_in_row(menu_str, 1);

                switch (p_task_menu_dta->event) {
                    case EV_MEN_ENT_ACTIVE:
                        p_task_menu_dta->state = ST_MENU_XX_SELECT_CONFIG;
                        break;

                    case EV_MEN_NEX_ACTIVE:
                        // cambiar entre motores
                        current_motor++;
                        if (current_motor >= MOTOR_CFG_QTY)
                            current_motor = 0;

                        temp_motor_cfg = motor_cfg[current_motor];
                        break;

                    case EV_MEN_ESC_ACTIVE:
                    	// devuelve el motor seleccionado al por defecto
                    	current_motor = 0;
                    	temp_motor_cfg = motor_cfg[0];

                        p_task_menu_dta->state = ST_MENU_XX_MAIN;
                        break;

                    default:
                        break;
                }

                break;

            case ST_MENU_XX_SELECT_CONFIG:
                if (!p_task_menu_dta->flag)
                    break;

                p_task_menu_dta->flag = false;

                print_text_in_row(header_text, 0);

                text_select_config(menu_str, current_motor_cfg_type);
                print_text_in_row(menu_str, 1);

                switch (p_task_menu_dta->event) {
                    case EV_MEN_ENT_ACTIVE:
                        p_task_menu_dta->state = ST_MENU_XX_SELECT_VALUE;
                        break;

                    case EV_MEN_NEX_ACTIVE:
                        // cambia entre power, speed y spin
                        current_motor_cfg_type = next_motor_cfg(current_motor_cfg_type);
                        break;

                    case EV_MEN_ESC_ACTIVE:
                    	// devuelve la selección de configuración a por defecto
                    	current_motor_cfg_type = POWER;

                        p_task_menu_dta->state = ST_MENU_XX_SELECT_MOTOR;
                        break;

                    default:
                        break;
                }

                break;

            case ST_MENU_XX_SELECT_VALUE:
                if (!p_task_menu_dta->flag)
                    break;

                p_task_menu_dta->flag = false;

                print_text_in_row(header_text, 0);

                text_select_value(menu_str, &temp_motor_cfg, current_motor_cfg_type);
                print_text_in_row(menu_str, 1);

                switch (p_task_menu_dta->event) {
                    case EV_MEN_ENT_ACTIVE:
                        // guardar valor seleccionado
                        motor_cfg[current_motor] = temp_motor_cfg;

                        // reinicia las variables de selección de configuración
                        current_motor = 0;
                        current_motor_cfg_type = POWER;
                        temp_motor_cfg = motor_cfg[0];

                        p_task_menu_dta->state = ST_MENU_XX_MAIN;

                        break;

                    case EV_MEN_NEX_ACTIVE:
                        // cambia el valor de la configuración selecionada
                        change_current_cfg(&temp_motor_cfg, current_motor_cfg_type);
                        break;

                    case EV_MEN_ESC_ACTIVE:
                        p_task_menu_dta->state = ST_MENU_XX_SELECT_CONFIG;
                        break;

                    default:
                        break;
                }

                break;

            default:

                p_task_menu_dta->tick  = DEL_MEN_XX_MIN;
                p_task_menu_dta->state = ST_MENU_XX_MAIN;
                p_task_menu_dta->event = EV_MEN_ENT_IDLE;
                p_task_menu_dta->flag  = false;

                break;
		}
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


void task_menu_statechart(void)
{
	task_menu_dta_t *p_task_menu_dta;

    /* Update Task Menu Data Pointer */
	p_task_menu_dta = &task_menu_dta;

	if (true == any_event_task_menu())
	{
		p_task_menu_dta->flag = true;
		p_task_menu_dta->event = get_event_task_menu();
	}

	switch (p_task_menu_dta->state)
	{
		case ST_MENU_INIT:

	    	if (p_task_menu_dta->flag && p_task_menu_dta->event == EV_PRESS_HASH)
	            {
	    			/* Selección*/
	    			p_task_menu_dta->flag = false;
	            }
	        else if (p_task_menu_dta->flag && p_task_menu_dta->event == EV_PRESS_NEXT)
	            {
	        		if (config_selected())
	        			p_task_menu_dta->state = ST_MENU_CONFIG;
	                else if (!config_selected() && fecha_valida())
	                	p_task_menu_dta->state = ST_MENU_READ;

	        		p_task_menu_dta->flag = false;
	            }
	    	break;

		case ST_MENU_CONFIG:

			if (p_task_menu_dta->flag && p_task_menu_dta->event == EV_PRESS_HASH)
	            {
	                /* Selección*/
					p_task_menu_dta->flag = false;
	            }
	        else if (p_task_menu_dta->flag && p_task_menu_dta->event == EV_PRESS_BACK)
	            {
	                p_task_menu_dta->state = ST_MENU_INIT;
	                p_task_menu_dta->flag = false;
	            }
	        else if (p_task_menu_dta->flag && p_task_menu_dta->event == EV_PRESS_NEXT)
	            {
	                if (cfg_time_selected())      p_task_menu_dta->state = ST_MENU_CONFIG_TIME;
	                else if (cfg_temp_selected()) p_task_menu_dta->state = ST_MENU_CONFIG_TEMP;
	                else if (cfg_hum_selected())  p_task_menu_dta->state = ST_MENU_CONFIG_HUM;
	                else if (cfg_lig_selected())  p_task_menu_dta->state = ST_MENU_CONFIG_LIG;

	                p_task_menu_dta->flag = false;
	            }
	        break;

	        /* ===================== CONFIG TIME ===================== */
		case ST_MENU_CONFIG_TIME:

			if (p_task_menu_dta->flag && p_task_menu_dta->event == EV_PRESS_HASH)
	            {
					/* Selección*/
	                p_task_menu_dta->flag = false;
	            }
	        else if (p_task_menu_dta->flag && p_task_menu_dta->event == EV_PRESS_ENTER)
	            {
	                if (fecha_valida())
	                    guardar_fecha();

	                p_task_menu_dta->flag = false;
	            }
	        else if (p_task_menu_dta->flag && p_task_menu_dta->event == EV_PRESS_BACK)
	            {
	                p_task_menu_dta->state = ST_MENU_CONFIG;
	                p_task_menu_dta->flag = false;
	            }
	        break;

	        /* ===================== CONFIG TEMP ===================== */
		case ST_MENU_CONFIG_TEMP:

			if (p_task_menu_dta->flag && p_task_menu_dta->event == EV_PRESS_HASH)
	            {
					/* Selección*/
	                p_task_menu_dta->flag = false;
	            }
			else if (p_task_menu_dta->flag && p_task_menu_dta->event == EV_PRESS_ENTER)
	            {
	                if (temp_valida())
	                    guardar_temp();

	                p_task_menu_dta->flag = false;
	            }
			else if (p_task_menu_dta->flag && p_task_menu_dta->event == EV_PRESS_BACK)
	            {
	                p_task_menu_dta->state = ST_MENU_CONFIG;
	                p_task_menu_dta->flag = false;
	            }
			break;

	        /* ===================== READ ===================== */
		case ST_MENU_READ:

			if (p_task_menu_dta->flag && p_task_menu_dta->event == EV_PRESS_HASH)
	            {
	            	/* Selección*/
	                p_task_menu_dta->flag = false;
	            }
			else if (p_task_menu_dta->flag && p_task_menu_dta->event == EV_PRESS_BACK)
	            {
	                p_task_menu_dta->state = ST_MENU_INIT;
	                p_task_menu_dta->flag = false;
	            }
			else if (p_task_menu_dta->flag && p_task_menu_dta->event == EV_PRESS_NEXT)
	            {
	                if (read_time_selected()) p_task_menu_dta->state = ST_MENU_READ_TIME;
	                else if (read_temp_selected()) p_task_menu_dta->state = ST_MENU_READ_TEMP;
	                else if (read_hum_selected())  p_task_menu_dta->state = ST_MENU_READ_HUM;
	                else if (read_lig_selected())  p_task_menu_dta->state = ST_MENU_READ_LIG;

	                p_task_menu_dta->flag = false;
	            }
			break;

	        /* ===================== READ TEMP ===================== */
		case ST_MENU_READ_TEMP:

			if (p_task_menu_dta->flag && p_task_menu_dta->event == EV_PRESS_HASH)
	            {
					/* Selección*/
	                p_task_menu_dta->flag = false;
	            }
			else if (p_task_menu_dta->flag && p_task_menu_dta->event == EV_PRESS_NEXT)
	            {
	                if (temp_config_selected())
	                    p_task_menu_dta->state = ST_MENU_READ_TEMP_CON;
	                else
	                    p_task_menu_dta->state = ST_MENU_READ_TEMP_HIS;

	                p_task_menu_dta->flag = false;
	            }
			break;

	        /* ===================== DEFAULT ===================== */
		default:
			p_task_menu_dta->state = ST_MENU_INIT;
			p_task_menu_dta->flag  = false;
			break;
	    }
}


