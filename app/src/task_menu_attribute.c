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

/********************** macros and definitions *******************************/
#define EVENT_UNDEFINED	(255)
#define MAX_EVENTS		(16)
#define KEY_VALUE_INVALID 10


/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/
/********************** external data declaration ****************************/

/********************** external functions definition ************************/
//Funcion de transformacion de boton presionado a valor numerico y evento correspondientes
void task_menu_push_event(bool btn_pressed, keypad_key_t key) {
	 if (!btn_pressed || key == KEY_NONE)
		 return;

    task_menu_ev_t event = EVENT_UNDEFINED;
    uint32_t key_value = KEY_VALUE_INVALID;

    switch(key)
    {
        case KEY_0: key_value = 0; event = EV_PRESS_NUM; break;
        case KEY_1: key_value = 1; event = EV_PRESS_NUM; break;
        case KEY_2: key_value = 2; event = EV_PRESS_NUM; break;
        case KEY_3: key_value = 3; event = EV_PRESS_NUM; break;
        case KEY_4: key_value = 4; event = EV_PRESS_NUM; break;
        case KEY_5: key_value = 5; event = EV_PRESS_NUM; break;
        case KEY_6: key_value = 6; event = EV_PRESS_NUM; break;
        case KEY_7: key_value = 7; event = EV_PRESS_NUM; break;
        case KEY_8: key_value = 8; event = EV_PRESS_NUM; break;
        case KEY_9: key_value = 9; event = EV_PRESS_NUM; break;

        case KEY_STAR: event = EV_PRESS_SCROLL; key_value = 0; break;
        case KEY_HASH: event = EV_PRESS_SCROLL; key_value = 1; break;

        case KEY_A: event = EV_PRESS_BACK; break;
        case KEY_B: event = EV_PRESS_NEXT; break;
        case KEY_C: event = EV_PRESS_ENTER; break;

        default: return;
    }

    put_event_task_menu(event, key_value); //Encolar evento y valor
}

/********************** end of file ******************************************/
