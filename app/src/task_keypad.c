#include <task_keypad.h>
#include "keypad.h"
#include "task_menu.h"

/* ===================== CONFIGURACION ===================== */
#define TASK_KEYPAD_DEBOUNCE_TICKS   10
#define TASK_KEYPAD_KEYS_QTY         16
// #define KEYPAD_SCAN_SAMPLES  10  elimino esto para no hacer un x10 en lecturas ya que tengo filtro con la FSM
/* ========================================================= */

/* ===================== CONTADORES ======================== */
uint32_t g_task_keypad_cnt = 0;
volatile uint32_t g_task_keypad_tick_cnt = 0;
/* ========================================================= */

/* ===================== FSM PRIVADA ======================= */
typedef enum {
    ST_UP,
    ST_FALLING,
    ST_DOWN,
    ST_RISING
} keypad_state_t;

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
//static keypad_ctrl_t keypad_ctrl[TASK_KEYPAD_KEYS_QTY];
static keypad_fsm_t keypad;
/* ========================================================= */

void task_keypad_init(void *parameters)
{
    (void) parameters;
    /*
    for (uint8_t i = 0; i < TASK_KEYPAD_KEYS_QTY; i++) {
        keypad_ctrl[i].key   = keypad_key_map[i];
        keypad_ctrl[i].state = ST_UP;
        keypad_ctrl[i].tick  = 0;
    }
    */
    keypad.key = KEY_NONE;
    keypad.state = ST_UP;
    keypad.tick = 0;

}

void task_keypad_update(void *parameters)
{
    (void) parameters;

    /* Botonera matricial clasica uniboton */
    keypad_key_t key_read = keypad_scan();

    /* No es necesario hacer muchas muestas
    for (uint8_t i = 0; i < KEYPAD_SCAN_SAMPLES; i++)
    {
    	keypad_key_t key_read_next = keypad_scan();
    	if (key_read_next == KEY_NONE) break;
    	key_read = key_read_next;
    }
    */

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
                }
                break;
        }
    }
    */
}
