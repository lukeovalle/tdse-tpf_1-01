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



/*    Maquina de estados  ESTADO ACTUAL--EVENTO--GUARDA--ESTADO SIGUIENTE--ACCIONES
ST_CONFIG			ST_CONFIG_TIME
ST_CONFIG_TIME	EV_PRESS_ENTER	Fecha valida	ST_CONFIG_TIME	Guarda dia/mes/año
ST_CONFIG_TIME	EV_PRESS_BACK		ST_CONFIG_TIME
ST_CONFIG_TIME	EV_PRESS_NEXT		ST_CONFIG_DAY
ST_CONFIG_DAY	EV_PRESS_BACK		ST_CONFIG_TIME
ST_CONFIG_DAY	EV_PRESS_ENTER	Horario valido	ST_CONFIG_DAY	Guarda hora/minuto
ST_CONFIG_DAY	EV_PRESS_NEXT	Informacion de fecha y hora existentes	ST_CONFIG_TEMP
ST_CONFIG_TEMP	EV_PRESS_NEXT		ST_CONFIG_HUM
ST_CONFIG_HUM	EV_PRESS_NEXT		ST_CONFIG_LIG
ST_CONFIG_LIG	EV_PRESS_NEXT		ST_CONFIG_TEMP
ST_CONFIG_TEMP	EV_PRESS_ENTER		ST_CONFIG_TEMP_READ
ST_CONFIG_TEMP_READ	EV_PRESS_NEXT		ST_CONFIG_TEMP_WRITE
ST_CONFIG_TEMP_WRITE	EV_PRESS_NEXT		ST_CONFIG_TEMP_READ
ST_CONFIG_TEMP_READ/WRITE	EV_PRESS_BACK		ST_CONFIG_TEMP
ST_CONFIG_TEMP_READ	EV_PRESS_ENTER		ST_CONFIG_TEMP_READ_CUR
ST_CONFIG_TEMP_READ_CUR	EV_PRESS_NEXT		ST_CONFIG_TEMP_READ_MAX
ST_CONFIG_TEMP_READ_MAX	EV_PRESS_NEXT		ST_CONFIG_TEMP_READ_MIN
ST_CONFIG_TEMP_READ_MIN	EV_PRESS_NEXT		ST_CONFIG_TEMP_READ_CUR
ST_CONFIG_TEMP_READ_CUR/MAX/MIN	EV_PRESS_BACK		ST_CONFIG_TEMP_READ
ST_CONFIG_TEMP_READ_CUR	EV_PRESS_ENTER		ST_CONFIG_TEMP_READ_CUR	Muestra valor actual
ST_CONFIG_TEMP_READ_CUR	EV_PRESS_NEXT		ST_CONFIG_TEMP_READ_CUR
ST_CONFIG_TEMP_READ_MAX	EV_PRESS_ENTER		ST_CONFIG_TEMP_READ_MAX	Muestra valor maximo
ST_CONFIG_TEMP_READ_MAX	EV_PRESS_NEXT		ST_CONFIG_TEMP_READ_MAX
ST_CONFIG_TEMP_READ_MIN	EV_PRESS_ENTER		ST_CONFIG_TEMP_READ_MIN	Muestra valor minimo
ST_CONFIG_TEMP_READ_MIN	EV_PRESS_NEXT		ST_CONFIG_TEMP_READ_MIN
ST_CONFIG_TEMP_WRITE	EV_PRESS_ENTER		ST_CONFIG_TEMP_WRITE_MAX
ST_CONFIG_TEMP_WRITE_MAX	EV_PRESS_NEXT		ST_CONFIG_TEMP_WRITE_MIN
ST_CONFIG_TEMP_WRITE_MAX/MIN	EV_PRESS_BACK		ST_CONFIG_TEMP_WRITE
ST_CONFIG_TEMP_WRITE_MAX	EV_PRESS_ENTER	Valor númerico tipeado	ST_CONFIG_TEMP_WRITE_MAX	Setea valor maximo, devuelve OK
ST_CONFIG_TEMP_WRITE_MIN	EV_PRESS_ENTER	Valor númerico tipeado	ST_CONFIG_TEMP_WRITE_MIN	Setea valor minimo, devuelve OK
ST_CONFIG_HUM	EV_PRESS_ENTER		ST_CONFIG_HUM_READ
ST_CONFIG_HUM_READ	EV_PRESS_NEXT		ST_CONFIG_HUM_WRITE
ST_CONFIG_HUM_WRITE	EV_PRESS_NEXT		ST_CONFIG_HUM_READ
ST_CONFIG_HUM_READ/WRITE	EV_PRESS_BACK		ST_CONFIG_HUM
ST_CONFIG_HUM_READ	EV_PRESS_ENTER		ST_CONFIG_HUM_READ_CUR
ST_CONFIG_HUM_READ_CUR	EV_PRESS_NEXT		ST_CONFIG_HUM_READ_MAX
ST_CONFIG_HUM_READ_MAX	EV_PRESS_NEXT		ST_CONFIG_HUM_READ_MIN
ST_CONFIG_HUM_READ_MIN	EV_PRESS_NEXT		ST_CONFIG_HUM_READ_CUR
ST_CONFIG_HUM_READ_CUR/MAX/MIN	EV_PRESS_BACK		ST_CONFIG_HUM_READ
ST_CONFIG_HUM_READ_CUR	EV_PRESS_ENTER		ST_CONFIG_HUM_READ_CUR	Muestra valor actual
ST_CONFIG_HUM_READ_CUR	EV_PRESS_NEXT		ST_CONFIG_HUM_READ_CUR
ST_CONFIG_HUM_READ_MAX	EV_PRESS_ENTER		ST_CONFIG_HUM_READ_MAX	Muestra valor maximo
ST_CONFIG_HUM_READ_MAX	EV_PRESS_NEXT		ST_CONFIG_HUM_READ_MAX
ST_CONFIG_HUM_READ_MIN	EV_PRESS_ENTER		ST_CONFIG_HUM_READ_MIN	Muestra valor minimo
ST_CONFIG_HUM_READ_MIN	EV_PRESS_NEXT		ST_CONFIG_HUM_READ_MIN
ST_CONFIG_HUM_WRITE	EV_PRESS_ENTER		ST_CONFIG_HUM_WRITE_MAX
ST_CONFIG_HUM_WRITE_MAX	EV_PRESS_NEXT		ST_CONFIG_HUM_WRITE_MIN
ST_CONFIG_HUM_WRITE_MAX/MIN	EV_PRESS_BACK		ST_CONFIG_HUM_WRITE
ST_CONFIG_HUM_WRITE_MAX	EV_PRESS_ENTER	Valor númerico tipeado	ST_CONFIG_HUM_WRITE_MAX	Setea valor maximo, devuelve OK
ST_CONFIG_HUM_WRITE_MIN	EV_PRESS_ENTER	Valor númerico tipeado	ST_CONFIG_HUM_WRITE_MIN	Setea valor minimo, devuelve OK
ST_CONFIG_LIG	EV_PRESS_ENTER		ST_CONFIG_LIG_READ
ST_CONFIG_LIG_READ	EV_PRESS_NEXT		ST_CONFIG_LIG_WRITE
ST_CONFIG_LIG_WRITE	EV_PRESS_NEXT		ST_CONFIG_LIG_READ
ST_CONFIG_LIG_READ/WRITE	EV_PRESS_BACK		ST_CONFIG_LIG
ST_CONFIG_LIG_READ	EV_PRESS_ENTER		ST_CONFIG_LIG_READ_CUR
ST_CONFIG_LIG_READ_CUR	EV_PRESS_NEXT		ST_CONFIG_LIG_READ_MAX
ST_CONFIG_LIG_READ_MAX	EV_PRESS_NEXT		ST_CONFIG_LIG_READ_MIN
ST_CONFIG_LIG_READ_MIN	EV_PRESS_NEXT		ST_CONFIG_LIG_READ_CUR
ST_CONFIG_LIG_READ_CUR/MAX/MIN	EV_PRESS_BACK		ST_CONFIG_LIG_READ
ST_CONFIG_LIG_READ_CUR	EV_PRESS_ENTER		ST_CONFIG_LIG_READ_CUR	Muestra valor actual
ST_CONFIG_LIG_READ_CUR	EV_PRESS_NEXT		ST_CONFIG_LIG_READ_CUR
ST_CONFIG_LIG_READ_MAX	EV_PRESS_ENTER		ST_CONFIG_LIG_READ_MAX	Muestra valor maximo
ST_CONFIG_LIG_READ_MAX	EV_PRESS_NEXT		ST_CONFIG_LIG_READ_MAX
ST_CONFIG_LIG_READ_MIN	EV_PRESS_ENTER		ST_CONFIG_LIG_READ_MIN	Muestra valor minimo
ST_CONFIG_LIG_READ_MIN	EV_PRESS_NEXT		ST_CONFIG_LIG_READ_MIN
ST_CONFIG_LIG_WRITE	EV_PRESS_ENTER		ST_CONFIG_LIG_WRITE_MAX
ST_CONFIG_LIG_WRITE_MAX	EV_PRESS_NEXT		ST_CONFIG_LIG_WRITE_MIN
ST_CONFIG_LIG_WRITE_MAX/MIN	EV_PRESS_BACK		ST_CONFIG_LIG_WRITE
ST_CONFIG_LIG_WRITE_MAX	EV_PRESS_ENTER	Valor númerico tipeado	ST_CONFIG_LIG_WRITE_MAX	Setea valor maximo, devuelve OK
ST_CONFIG_LIG_WRITE_MIN	EV_PRESS_ENTER	Valor númerico tipeado	ST_CONFIG_LIG_WRITE_MIN	Setea valor minimo, devuelve OK
*/

/********************** inclusions *******************************************/
/* Project includes */
#include "main.h"

/* Demo includes */
#include "logger.h"
#include "dwt.h"

/* Application & Tasks includes */
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
	{DEL_MEN_XX_MIN, ST_MEN_XX_IDLE, EV_MEN_ENT_IDLE, false};

#define MENU_DTA_QTY	(sizeof(task_menu_dta)/sizeof(task_menu_dta_t))

/********************** internal functions declaration ***********************/
void task_menu_statechart(void);

/********************** internal data definition *****************************/
const char *p_task_menu 		= "Task Menu (Interactive Menu)";
const char *p_task_menu_ 		= "Non-Blocking & Update By Time Code";

/********************** external data declaration ****************************/
uint32_t g_task_menu_cnt;
volatile uint32_t g_task_menu_tick_cnt;

/********************** external functions definition ************************/
void task_menu_init(void *parameters)
{
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
	LOGGER_INFO("   %s = %lu", GET_NAME(g_task_menu_cnt), g_task_menu_cnt);

	init_queue_event_task_menu();

	/* Update Task Actuator Configuration & Data Pointer */
	p_task_menu_dta = &task_menu_dta;

	/* Init & Print out: Task execution FSM */
	state = ST_MEN_XX_IDLE;
	p_task_menu_dta->state = state;

	event = EV_MEN_ENT_IDLE;
	p_task_menu_dta->event = event;

	b_event = false;
	p_task_menu_dta->flag = b_event;

	LOGGER_INFO(" ");
	LOGGER_INFO("   %s = %lu   %s = %lu   %s = %s",
				 GET_NAME(state), (uint32_t)state,
				 GET_NAME(event), (uint32_t)event,
				 GET_NAME(b_event), (b_event ? "true" : "false"));

	/* Init & Print out: LCD Display */
	displayInit( DISPLAY_CONNECTION_GPIO_4BITS );

    displayCharPositionWrite(0, 0);
	displayStringWrite("TdSE Bienvenidos");

	displayCharPositionWrite(0, 1);
	displayStringWrite("Test Nro: ");
}

void task_menu_update(void *parameters)
{
	bool b_time_update_required = false;

	/* Protect shared resource */
	__asm("CPSID i");	/* disable interrupts */
    if (G_TASK_MEN_TICK_CNT_INI < g_task_menu_tick_cnt)
    {
		/* Update Tick Counter */
    	g_task_menu_tick_cnt--;
    	b_time_update_required = true;
    }
    __asm("CPSIE i");	/* enable interrupts */

    while (b_time_update_required)
    {
		/* Update Task Counter */
		g_task_menu_cnt++;

		/* Run Task Menu Statechart */
    	task_menu_statechart();

    	/* Protect shared resource */
		__asm("CPSID i");	/* disable interrupts */
		if (G_TASK_MEN_TICK_CNT_INI < g_task_menu_tick_cnt)
		{
			/* Update Tick Counter */
			g_task_menu_tick_cnt--;
			b_time_update_required = true;
		}
		else
		{
			b_time_update_required = false;
		}
		__asm("CPSIE i");	/* enable interrupts */
	}
}

void task_menu_statechart(void)
{
	task_menu_dta_t *p_task_menu_dta;
	char menu_str[8];

    /* Update Task Menu Data Pointer */
	p_task_menu_dta = &task_menu_dta;

	if (true == any_event_task_menu())
	{
		p_task_menu_dta->flag = true;
		p_task_menu_dta->event = get_event_task_menu();
	}

	switch (p_task_menu_dta->state)
	{
		case ST_MEN_XX_IDLE:

			if ((true == p_task_menu_dta->flag) && (EV_MEN_ENT_ACTIVE == p_task_menu_dta->event))
			{
				p_task_menu_dta->tick = DEL_MEN_XX_MAX;
				p_task_menu_dta->flag = false;
				p_task_menu_dta->state = ST_MEN_XX_ACTIVE;
			}

			break;

		case ST_MEN_XX_ACTIVE:

			if ((true == p_task_menu_dta->flag) && (EV_MEN_ENT_IDLE == p_task_menu_dta->event))
			{
				p_task_menu_dta->flag = false;
				p_task_menu_dta->state = ST_MEN_XX_IDLE;
			}
			else
			{
				p_task_menu_dta->tick--;
				if (DEL_MEN_XX_MIN == p_task_menu_dta->tick)
				{
					p_task_menu_dta->tick = DEL_MEN_XX_MAX;

					/* Print out: LCD Display */
					snprintf(menu_str, sizeof(menu_str), "%lu", (g_task_menu_cnt/1000ul));
					displayCharPositionWrite(10, 1);
					displayStringWrite(menu_str);
				}
			}

			break;

		default:

			p_task_menu_dta->tick  = DEL_MEN_XX_MIN;
			p_task_menu_dta->state = ST_MEN_XX_IDLE;
			p_task_menu_dta->event = EV_MEN_ENT_IDLE;
			p_task_menu_dta->flag  = false;

			break;
	}
}

/********************** end of file ******************************************/


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
	{DEL_MEN_XX_MIN, ST_MEN_XX_MAIN, EV_MEN_ENT_IDLE, false};

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
	task_menu_st_t	state;
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
            case ST_MEN_XX_MAIN:
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
                        p_task_menu_dta->state = ST_MEN_XX_SELECT_MOTOR;
                        break;

                    default:
                        break;
                }

                break;

            case ST_MEN_XX_SELECT_MOTOR:
                if (!p_task_menu_dta->flag)
                    break;

                p_task_menu_dta->flag = false;

                print_text_in_row(header_text, 0);

                text_select_motor(menu_str, current_motor);
                print_text_in_row(menu_str, 1);

                switch (p_task_menu_dta->event) {
                    case EV_MEN_ENT_ACTIVE:
                        p_task_menu_dta->state = ST_MEN_XX_SELECT_CONFIG;
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

                        p_task_menu_dta->state = ST_MEN_XX_MAIN;
                        break;

                    default:
                        break;
                }

                break;

            case ST_MEN_XX_SELECT_CONFIG:
                if (!p_task_menu_dta->flag)
                    break;

                p_task_menu_dta->flag = false;

                print_text_in_row(header_text, 0);

                text_select_config(menu_str, current_motor_cfg_type);
                print_text_in_row(menu_str, 1);

                switch (p_task_menu_dta->event) {
                    case EV_MEN_ENT_ACTIVE:
                        p_task_menu_dta->state = ST_MEN_XX_SELECT_VALUE;
                        break;

                    case EV_MEN_NEX_ACTIVE:
                        // cambia entre power, speed y spin
                        current_motor_cfg_type = next_motor_cfg(current_motor_cfg_type);
                        break;

                    case EV_MEN_ESC_ACTIVE:
                    	// devuelve la selección de configuración a por defecto
                    	current_motor_cfg_type = POWER;

                        p_task_menu_dta->state = ST_MEN_XX_SELECT_MOTOR;
                        break;

                    default:
                        break;
                }

                break;

            case ST_MEN_XX_SELECT_VALUE:
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

                        p_task_menu_dta->state = ST_MEN_XX_MAIN;

                        break;

                    case EV_MEN_NEX_ACTIVE:
                        // cambia el valor de la configuración selecionada
                        change_current_cfg(&temp_motor_cfg, current_motor_cfg_type);
                        break;

                    case EV_MEN_ESC_ACTIVE:
                        p_task_menu_dta->state = ST_MEN_XX_SELECT_CONFIG;
                        break;

                    default:
                        break;
                }

                break;

            default:

                p_task_menu_dta->tick  = DEL_MEN_XX_MIN;
                p_task_menu_dta->state = ST_MEN_XX_MAIN;
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
