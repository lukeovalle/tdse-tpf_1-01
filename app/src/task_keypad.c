#include <task_keypad.h>
#include "keypad.h"
#include "task_menu.h"

/* ===================== CONFIGURACION ===================== */
#define TASK_KEYPAD_DEBOUNCE_TICKS   20
#define TASK_KEYPAD_KEYS_QTY         16
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

typedef struct {
    keypad_key_t key;
    keypad_state_t state;
    uint32_t tick;
} keypad_ctrl_t;
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
static keypad_ctrl_t keypad_ctrl[TASK_KEYPAD_KEYS_QTY];
/* ========================================================= */

void task_keypad_init(void *parameters)
{
    (void) parameters;

    for (uint8_t i = 0; i < TASK_KEYPAD_KEYS_QTY; i++) {
        keypad_ctrl[i].key   = keypad_key_map[i];
        keypad_ctrl[i].state = ST_UP;
        keypad_ctrl[i].tick  = 0;
    }
}

void task_keypad_update(void *parameters)
{
    (void) parameters;

    keypad_key_t key_read = keypad_scan();
    g_task_keypad_cnt++;

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
}
