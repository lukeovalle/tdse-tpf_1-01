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

/********************** macros and definitions *******************************/
#define EVENT_UNDEFINED	(255)
#define MAX_EVENTS		(16)

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/
struct
{
	uint32_t	head;
	uint32_t	tail;
	uint32_t	count;
	task_menu_ev_t	queue[MAX_EVENTS];
} queue_task_a;

/********************** external data declaration ****************************/

/********************** external functions definition ************************/
void key_to_event(bool btn_pressed, keypad_key_t key) {
	if (!btn_pressed) return;

	task_menu_st_t	state;

}
/********************** end of file ******************************************/
