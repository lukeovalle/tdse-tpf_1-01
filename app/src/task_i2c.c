/*
 * task_i2c.c
 *
 *  Created on: Dec 10, 2025
 *      Author: Hikar
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
#include "adc.h"

/********************** macros and definitions *******************************/
#define G_TASK_I2C_CNT_INIT           0ul
#define G_TASK_I2C_TICK_CNT_INI  		0ul

/* Number of ticks for the i2c measurement and the starting value */
#define DEL_I2C_TICK_MAX			50ul
#define DEL_I2C_TICK_INIT			0ul

/********************** external data declaration ****************************/
uint32_t g_task_i2c_cnt;
volatile uint32_t g_task_i2c_tick_cnt;
extern ADC_HandleTypeDef hadc1;

/********************** internal data declaration ****************************/
const task_i2c_cfg_t task_i2c_cfg_list[] = {
		{ }	// 1010 0000: primer nibble 1010 hardcodeado, segundo nibble 0ABC, AB = dir_página, C = R/W
};

#define I2C_CFG_QTY	(sizeof(task_i2c_cfg_list)/sizeof(task_i2c_cfg_t))

task_i2c_dta_t task_i2c_dta_list[] = {
	{ .state = ST_I2C_IDLE, .event = EV_I2C_IDLE, .addr = 0 }
};

#define I2C_DTA_QTY	(sizeof(task_i2c_dta_list)/sizeof(task_i2c_dta_t))

bool tr_finished = false;

/********************** internal functions declaration ***********************/
void task_i2c_statechart(shared_data_type * parameters);

/********************** internal data definition *****************************/
const char *p_task_i2c 		= "Task I2C (I2C Statechart)";
const char *p_task_i2c_ 	= "Non-Blocking & Update By Time Code";



/********************** external functions definition ************************/
void task_i2c_init(void *parameters)
{
	uint32_t index;
	task_i2c_dta_t *p_task_i2c_dta;
	task_i2c_st_t state;
	task_i2c_ev_t event;

	/* Print out: Task Initialized */
	LOGGER_INFO(" ");
	LOGGER_INFO("  %s is running - %s", GET_NAME(task_i2c_init), p_task_i2c);
	LOGGER_INFO("  %s is a %s", GET_NAME(task_i2c), p_task_i2c_);

	/* Init & Print out: Task execution counter */
	g_task_i2c_cnt = G_TASK_I2C_CNT_INIT;
	LOGGER_INFO("   %s = %lu", GET_NAME(g_task_i2c_cnt), g_task_i2c_cnt);

	for (index = 0; I2C_DTA_QTY > index; index++)
	{
		/* Update Task I2C Data Pointer */
		p_task_i2c_dta = &task_i2c_dta_list[index];

		LOGGER_INFO(" ");
		LOGGER_INFO("   %s = %lu   %s = %lu   %s = %lu",
				    GET_NAME(index), index,
					GET_NAME(state), (uint32_t)state,
					GET_NAME(event), (uint32_t)event);
	}
}

void task_i2c_update(void *parameters) {
	bool b_time_update_required = false;

	/* Protect shared resource */
	__asm("CPSID i");	/* disable interrupts */
    if (g_task_i2c_tick_cnt > G_TASK_I2C_TICK_CNT_INI) {
		/* Update Tick Counter */
    	g_task_i2c_tick_cnt--;
    	b_time_update_required = true;
    }
    __asm("CPSIE i");	/* enable interrupts */

    while (b_time_update_required) {
		/* Update Task Counter */
		g_task_i2c_cnt++;

		/* Run Task I2C Statechart */
		shared_data_type * shared_data = (shared_data_type *) parameters;


    	task_i2c_statechart(shared_data);

    	/* Protect shared resource */
		__asm("CPSID i");	/* disable interrupts */
		if (g_task_i2c_tick_cnt > G_TASK_I2C_TICK_CNT_INI) {
			/* Update Tick Counter */
			g_task_i2c_tick_cnt--;
			b_time_update_required = true;
		} else {
			b_time_update_required = false;
		}
		__asm("CPSIE i");	/* enable interrupts */
    }
}

void task_i2c_statechart(shared_data_type * parameters) {
	uint32_t index;
	const task_i2c_cfg_t * p_task_i2c_cfg;
	task_i2c_dta_t * p_task_i2c_dta;

	for (index = 0; I2C_DTA_QTY > index; index++) {
		/* Update Task I2C Configuration & Data Pointer */
		p_task_i2c_cfg = &task_i2c_cfg_list[index];
		p_task_i2c_dta = &task_i2c_dta_list[index];

		task_i2c_st_t state = p_task_i2c_dta->state;
		switch (state) {
		case ST_I2C_IDLE:
			if (p_task_i2c_dta->event == EV_I2C_IDLE)
				p_task_i2c_dta->state = ST_I2C_IDLE;
			else if (p_task_i2c_dta->event == EV_I2C_WRITE) {
				HAL_I2C_Master_Transmit_IT(hi2c, p_task_i2c_dta->addr, p_task_i2c_dta->data,
						p_task_i2c_dta->data_size);
				p_task_i2c_dta->state = ST_I2C_SENDING;
			} else if (p_task_i2c_dta->event == EV_I2C_READ) {
				HAL_I2C_Master_Receive_IT(hi2c, p_task_i2c_dta->addr, p_task_i2c_dta->data,
						p_task_i2c_dta->data_size);
				p_task_i2c_dta->state = ST_I2C_RECEIVING;
			}
			break;HAL_I2C_Mem_Read_IT(hi2c, DevAddress, MemAddress, MemAddSize, pData, Size)

		case ST_I2C_SENDING:
			if (tr_finished) {
				tr_finished = false;
				p_task_i2c_dta->state = ST_I2C_IDLE;
			}
			break;

		case ST_I2C_RECEIVING:
			if (tr_finished) {
				tr_finished = false;
				p_task_i2c_dta->state = ST_I2C_IDLE;
			}
			break;

		default:
			p_task_i2c_dta->tick_cnt = DEL_I2C_TICK_INIT;
			p_task_i2c_dta->event = EV_I2C_IDLE;
			p_task_i2c_dta->state = ST_I2C_IDLE;
			break;
		}
	}
}

void HAL_I2C_MasterRxCpltCallback (I2C_HandleTypeDef * hi2c) {
	tr_finished = true;
}

void HAL_I2C_MasterTxCpltCallback (I2C_HandleTypeDef * hi2c) {
	tr_finished = true;
}


/********************** end of file ******************************************/
