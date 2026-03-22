#include <stdint.h>
#include "app.h"
#include "task_keypad.h"
#include "keypad.h"
#include "task_menu_attribute.h"
#include "logger.h"

/* ===================== CONFIGURACION ===================== */
#define TASK_KEYPAD_DEBOUNCE_TICKS   10
#define TASK_KEYPAD_KEYS_QTY         16
// #define KEYPAD_SCAN_SAMPLES  10  elimino esto para no hacer un x10 en lecturas ya que tengo filtro con la FSM
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

/* ===================== FSM PRIVADA ======================= */

// Utilizo una sola FSM para toda la botonera, con un array de control para cada tecla, asi no tengo que hacer 16 FSM independientes
/*
typedef struct {
    keypad_key_t key;
    keypad_state_t state;
    uint32_t tick;
} keypad_ctrl_t;
*/

typedef struct {
    keypad_key_t key;
    keypad_state_t state;
    uint32_t tick;
} keypad_fsm_t;

static keypad_fsm_t keypad;
/* ========================================================= */

/* ===================== MAPEO DE TECLAS =================== */
static const keypad_key_t keypad_key_map[TASK_KEYPAD_KEYS_QTY] = {
    KEY_1, KEY_2, KEY_3, KEY_A,
    KEY_4, KEY_5, KEY_6, KEY_B,
    KEY_7, KEY_8, KEY_9, KEY_C,
    KEY_STAR, KEY_0, KEY_HASH, KEY_D
};
/* ========================================================= */

/* ===================== DATOS PRIVADOS ==================== */
const char *p_task_keypad 	= "Task Keypad (Keypad Statechart)";
const char *p_task_keypad_ 	= "Non-Blocking & Update By Time Code";

//static keypad_ctrl_t keypad_ctrl[TASK_KEYPAD_KEYS_QTY];
static keypad_fsm_t keypad;

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

    keypad.key = KEY_NONE;
    keypad.state = ST_UP;
    keypad.tick = 0;

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

    g_task_keypad_cnt++;

    switch (keypad.state)
    {
    case ST_UP:
        if (key_read != KEY_NONE) {
            keypad.state = ST_FALLING;
            keypad.tick = 0;
            keypad.key = key_read;
        }
        break;

    case ST_FALLING:
        if (key_read == keypad.key) {
            if (++keypad.tick >= TASK_KEYPAD_DEBOUNCE_TICKS) {
                keypad.state = ST_DOWN;
                task_menu_push_event(MENU_EV_KEY_PRESSED, keypad.key);
            }
        } else {
            keypad.state = ST_UP;
        }
        break;

    case ST_DOWN:
        if (key_read != keypad.key) {
            keypad.state = ST_RISING;
            keypad.tick = 0;
        }
        break;

    case ST_RISING:
        if (key_read != keypad.key) {
            if (++keypad.tick >= TASK_KEYPAD_DEBOUNCE_TICKS) {
                keypad.state = ST_UP;
                task_menu_push_event(MENU_EV_KEY_RELEASED, keypad.key);
            }
        } else {
            keypad.state = ST_DOWN;
        }
        break;

    default: // Por si por algun motivo quedo en un estado invalido, lo reseteo
        keypad.state = ST_UP;
        keypad.tick = 0;
        keypad.key = KEY_NONE;
        break;
    }
    
    /*
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
    */
}

