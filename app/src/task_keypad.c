#include <stdint.h>
#include "app.h"
#include "task_keypad.h"
#include "keypad.h"
#include "task_menu_attribute.h"
#include "logger.h"

/* ===================== CONFIGURACION ===================== */
#define TASK_KEYPAD_DEBOUNCE_TICKS   20
#define TASK_KEYPAD_KEYS_QTY         16
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

/* ========================================================= */

/* ===================== DATOS PRIVADOS ==================== */
const char *p_task_keypad 	= "Task Keypad (Keypad Statechart)";
const char *p_task_keypad_ 	= "Non-Blocking & Update By Time Code";

static keypad_ctrl_t keypad_ctrl[TASK_KEYPAD_KEYS_QTY];
/* ========================================================= */

/********************** internal functions declaration ***********************/
void task_keypad_statechart(shared_data_type * parameters);


/********************** external functions definition ************************/
void task_keypad_init(void *parameters) {
    (void) parameters;

	//uint32_t index;
	//keypad_state_t state;

	/* Print out: Task Initialized */
	LOGGER_INFO(" ");
	LOGGER_INFO("  %s is running - %s", GET_NAME(task_keypad_init), p_task_keypad);
	LOGGER_INFO("  %s is a %s", GET_NAME(task_keypad), p_task_keypad_);

	/* Init & Print out: Task execution counter */
	g_task_keypad_cnt = G_TASK_KEYPAD_CNT_INIT;
	LOGGER_INFO("   %s = %lu", GET_NAME(g_task_keypad_cnt), g_task_keypad_cnt);

	/*
	for (index = 0; CLOCK_DTA_QTY > index; index++)
	{
		* Update Task CLOCK Data Pointer *
		p_task_clock_dta = &task_clock_dta_list[index];
		state = p_task_clock_dta->state;
		event = p_task_clock_dta->event;

		LOGGER_INFO(" ");
		LOGGER_INFO("   %s = %lu   %s = %lu   %s = %lu",
				    GET_NAME(index), index,
					GET_NAME(state), (uint32_t)state,
					GET_NAME(event), (uint32_t)event);
	}
	*/

    for (uint8_t i = 0; i < TASK_KEYPAD_KEYS_QTY; i++) {
        keypad_ctrl[i].key   = keypad_key_map[i];
        keypad_ctrl[i].state = ST_UP;
        keypad_ctrl[i].tick  = 0;
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
    /* Botonera matricial clásica unibotón */
    keypad_key_t key_read = keypad_scan();

    for (uint8_t i = 0; i < TASK_KEYPAD_KEYS_QTY; i++)
    {
        keypad_ctrl_t *s = &keypad_ctrl[i];
        uint8_t pressed = (key_read == s->key);

        switch (s->state)
        {
            case ST_UP:
                if (pressed) {
                    s->state = ST_FALLING;
                    s->tick = 0;
                }
                break;

            case ST_FALLING:
                if (pressed) {
                    if (++s->tick >= TASK_KEYPAD_DEBOUNCE_TICKS) {
                        s->state = ST_DOWN;
                        task_menu_push_event(MENU_EV_KEY_PRESSED, s->key);
                    }
                } else {
                    s->state = ST_UP;
                    s->tick = 0;
                }
                break;

            case ST_DOWN:
                if (!pressed) {
                    s->state = ST_RISING;
                    s->tick = 0;
                }
                break;

            case ST_RISING:
                if (!pressed) {
                    if (++s->tick >= TASK_KEYPAD_DEBOUNCE_TICKS) {
                        s->state = ST_UP;
                        task_menu_push_event(MENU_EV_KEY_RELEASED, s->key);
                    }
                } else {
                    s->state = ST_DOWN;
                    s->tick = 0;
                }
                break;
        }
    }


}



