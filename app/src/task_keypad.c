#include <stdint.h>
#include "app.h"
#include "task_keypad.h"
#include "keypad.h"
#include "task_menu_attribute.h"
#include "logger.h"

/* ===================== CONFIGURACION ===================== */
#define TASK_KEYPAD_DEBOUNCE_TICKS   10
#define KEYPAD_SCAN_SAMPLES  10
#define MENU_EV_KEY_PRESSED 1
#define MENU_EV_KEY_RELEASED 0

#define G_TASK_KEYPAD_CNT_INIT			0ul
#define G_TASK_KEYPAD_TICK_CNT_INI		0ul

/* ========================================================= */

/* ===================== CONTADORES ======================== */
uint32_t g_task_keypad_cnt = 0;
volatile uint32_t g_task_keypad_tick_cnt = 0;
/* ========================================================= */

/* ===================== DATOS PRIVADOS ==================== */
const char *p_task_keypad 	= "Task Keypad (Keypad Statechart)";
const char *p_task_keypad_ 	= "Non-Blocking & Update By Time Code";

static task_keypad_cfg_t task_keypad_cfg_list[] = {
		{ .max_debounce = 4, .rows = 4, .cols = 4 }
};

#define KEYPAD_CFG_QTY (sizeof(task_keypad_cfg_list) / sizeof(task_keypad_cfg_t))

static task_keypad_dta_t  task_keypad_dta_list[] = {
		{ .state = ST_KEYPAD_FINDING, .event = EV_KEYPAD_IDLE, .row = 0, .col = 0, .key = KEY_NONE, .tick = 0 }
};

#define KEYPAD_DTA_QTY (sizeof(task_keypad_dta_list) / sizeof(task_keypad_dta_t))

/* ========================================================= */

/********************** internal functions declaration ***********************/
void task_keypad_statechart(shared_data_type * parameters);


/********************** external functions definition ************************/
void task_keypad_init(void *parameters) {
    (void) parameters;

	task_keypad_dta_t * p_task_keypad_dta;
	task_keypad_st_t state;
	task_keypad_ev_t event;

	/* Print out: Task Initialized */
	LOGGER_INFO(" ");
	LOGGER_INFO("  %s is running - %s", GET_NAME(task_keypad_init), p_task_keypad);
	LOGGER_INFO("  %s is a %s", GET_NAME(task_keypad), p_task_keypad_);

	/* Init & Print out: Task execution counter */
	g_task_keypad_cnt = G_TASK_KEYPAD_CNT_INIT;
	LOGGER_INFO("   %s = %lu", GET_NAME(g_task_keypad_cnt), g_task_keypad_cnt);


	for (uint32_t index = 0; index < KEYPAD_DTA_QTY; index++) 	{
		/* Update Task KEYPAD Data Pointer */
		p_task_keypad_dta = &task_keypad_dta_list[index];
		state = p_task_keypad_dta->state;
		event = p_task_keypad_dta->event;

		LOGGER_INFO(" ");
		LOGGER_INFO("   %s = %lu   %s = %lu   %s = %lu",
				    GET_NAME(index), index,
					GET_NAME(state), (uint32_t)state,
					GET_NAME(event), (uint32_t)event);
	}
}

void task_keypad_update(void *parameters) {
	bool b_time_update_required = false;

	/* Protect shared resource */
	__asm("CPSID i");	/* disable interrupts */
    if (g_task_keypad_tick_cnt > G_TASK_KEYPAD_TICK_CNT_INI) {
		/* Update Tick Counter */
    	g_task_keypad_tick_cnt--;
    	b_time_update_required = true;
    }
    __asm("CPSIE i");	/* enable interrupts */

    while (b_time_update_required) {
		/* Update Task Counter */
		g_task_keypad_cnt++;

		/* Run Task KEYPAD Statechart */
		shared_data_type * shared_data = (shared_data_type *) parameters;

		task_keypad_statechart(shared_data);

    	/* Protect shared resource */
		__asm("CPSID i");	/* disable interrupts */
		if (g_task_keypad_tick_cnt > G_TASK_KEYPAD_TICK_CNT_INI) {
			/* Update Tick Counter */
			g_task_keypad_tick_cnt--;
			b_time_update_required = true;
		} else {
			b_time_update_required = false;
		}
		__asm("CPSIE i");	/* enable interrupts */
    }
}

void task_keypad_statechart(shared_data_type * parameters) {
	const task_keypad_cfg_t * p_task_keypad_cfg;
	task_keypad_dta_t * p_task_keypad_dta;

	for (uint32_t index = 0; KEYPAD_DTA_QTY > index; index++) {
		/* Update Task Sensor Configuration & Data Pointer */
		p_task_keypad_cfg = &task_keypad_cfg_list[index];
		p_task_keypad_dta = &task_keypad_dta_list[index];

		/* Leo el botón actual de la botonera */
		keypad_key_t key_read = keypad_read_key(p_task_keypad_dta->row, p_task_keypad_dta->col);

		p_task_keypad_dta->event = key_read ? EV_KEYPAD_PRESSED : EV_KEYPAD_RELEASED;

		task_keypad_st_t state = p_task_keypad_dta->state;
		switch (state) {
		case ST_KEYPAD_FINDING:
			if (p_task_keypad_dta->event == EV_KEYPAD_PRESSED) {
				p_task_keypad_dta->state = ST_KEYPAD_DEBOUNCING;
				p_task_keypad_dta->key = key_read;
				p_task_keypad_dta->tick = 1;
			} else { /* Itero la matriz */
				p_task_keypad_dta->row = (p_task_keypad_dta->row + 1) % p_task_keypad_cfg->rows;
				if (p_task_keypad_dta->row == 0)
					p_task_keypad_dta->col = (p_task_keypad_dta->col + 1) % p_task_keypad_cfg->cols;
			}

			p_task_keypad_dta->event = EV_KEYPAD_IDLE;
			break;
		case ST_KEYPAD_DEBOUNCING:
			if (p_task_keypad_dta->event == EV_KEYPAD_RELEASED) {
				p_task_keypad_dta->state = ST_KEYPAD_FINDING;
				p_task_keypad_dta->event = EV_KEYPAD_IDLE;
				p_task_keypad_dta->tick = 0;
				break;
			}

			(p_task_keypad_dta->tick)++;

			if (p_task_keypad_dta->tick == p_task_keypad_cfg->max_debounce) {
				p_task_keypad_dta->state = ST_KEYPAD_PRESSED;
				p_task_keypad_dta->event = EV_KEYPAD_IDLE;
				p_task_keypad_dta->tick = 0;

                task_menu_push_event(MENU_EV_KEY_PRESSED, p_task_keypad_dta->key);
			}

			break;

		case ST_KEYPAD_PRESSED:
			if (p_task_keypad_dta->event == EV_KEYPAD_RELEASED) {
				p_task_keypad_dta->state = ST_KEYPAD_FINDING;
				p_task_keypad_dta->event = EV_KEYPAD_IDLE;

                task_menu_push_event(MENU_EV_KEY_RELEASED, p_task_keypad_dta->key);
			}

			break;

		default:
			p_task_keypad_dta->state = ST_KEYPAD_FINDING;
			p_task_keypad_dta->event = EV_KEYPAD_IDLE;

			break;
		}
	}
}

