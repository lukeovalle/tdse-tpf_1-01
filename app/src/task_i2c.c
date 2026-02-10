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
#include "task_i2c.h"
#include "task_i2c_attribute.h"
#include "memory_buffer.h"
#include "ext_memory.h"

/********************** macros and definitions *******************************/
#define G_TASK_I2C_CNT_INIT			0ul
#define G_TASK_I2C_TICK_CNT_INI		0ul

/* Number of ticks for the i2c measurement and the starting value */
#define DEL_I2C_TICK_MAX			50ul
#define DEL_I2C_TICK_INIT			0ul

/********************** external data declaration ****************************/
uint32_t g_task_i2c_cnt;
volatile uint32_t g_task_i2c_tick_cnt;
extern I2C_HandleTypeDef hi2c1;

/********************** internal data declaration ****************************/
const task_i2c_cfg_t task_i2c_cfg_list[] = {
		{ }
};

#define I2C_CFG_QTY	(sizeof(task_i2c_cfg_list)/sizeof(task_i2c_cfg_t))

task_i2c_dta_t task_i2c_dta_list[] = {
	{ .state = ST_I2C_IDLE, .event = EV_I2C_IDLE }
};

#define I2C_DTA_QTY	(sizeof(task_i2c_dta_list)/sizeof(task_i2c_dta_t))

static bool tr_finished = false;

static mem_data_t data_aux;

/********************** internal functions declaration ***********************/
void task_i2c_statechart(shared_data_type * parameters);
HAL_StatusTypeDef start_page_write(mem_data_t * data);
HAL_StatusTypeDef start_page_read(mem_data_t * data);
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

	HAL_StatusTypeDef status = HAL_I2C_IsDeviceReady(&hi2c1, DEVICE_ADDRESS_8BIT, 3, 3);
	if (status == HAL_OK) {
		LOGGER_INFO("   I2C preparado");
	} else {
		LOGGER_INFO("   Error en I2C");
	}

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
	HAL_StatusTypeDef status;

	for (index = 0; I2C_DTA_QTY > index; index++) {
		/* Update Task I2C Configuration & Data Pointer */
		p_task_i2c_cfg = &task_i2c_cfg_list[index];
		p_task_i2c_dta = &task_i2c_dta_list[index];

		task_i2c_st_t state = p_task_i2c_dta->state;
		switch (state) {
		case ST_I2C_IDLE:
			data_aux = mem_buffer_dequeue();
			if (data_aux.size)
				p_task_i2c_dta->event = data_aux.write_mode ? EV_I2C_WRITE : EV_I2C_READ;

			switch (p_task_i2c_dta->event) {
			case EV_I2C_IDLE:
				p_task_i2c_dta->state = ST_I2C_IDLE;
				break;
			case EV_I2C_WRITE:
				p_task_i2c_dta->state = ST_I2C_WRITING;
				p_task_i2c_dta->event = EV_I2C_IDLE;
				break;
			case EV_I2C_READ:
				p_task_i2c_dta->state = ST_I2C_READING;
				p_task_i2c_dta->event = EV_I2C_IDLE;
				break;
			default:
				break;
			}
			break;

		case ST_I2C_WRITING:
			status = start_page_write(&data_aux);
			if (status == HAL_OK) {
				p_task_i2c_dta->state = ST_I2C_WAITING;
			} else {
				LOGGER_LOG("Error en escritura de datos: %u\n", status);
				p_task_i2c_dta->state = ST_I2C_IDLE;
			}

			break;

		case ST_I2C_READING:
			status = start_page_read(&data_aux);
			if (status == HAL_OK) {
				p_task_i2c_dta->state = ST_I2C_WAITING;
			} else {
				LOGGER_LOG("Error en lectura de datos: %u\n", status);
				p_task_i2c_dta->state = ST_I2C_IDLE;
			}

			break;

		case ST_I2C_WAITING:
			if (tr_finished) {
				while (HAL_I2C_IsDeviceReady(&hi2c1, DEVICE_ADDRESS_8BIT, 10, 5) != HAL_OK);
				p_task_i2c_dta->state = ST_I2C_IDLE;
				tr_finished = false;
			}
			break;

		default:
			p_task_i2c_dta->event = EV_I2C_IDLE;
			p_task_i2c_dta->state = ST_I2C_IDLE;
			break;
		}
	}
}

/********************** internal functions definition ************************/

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef * hi2c) {
	tr_finished = true;
}

void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef * hi2c) {
	tr_finished = true;
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c) {
    tr_finished = true; // para no quedar colgado
    LOGGER_LOG("HAL_I2C_ErrorCallback: sr = %08lx\n", hi2c->ErrorCode);
}

HAL_StatusTypeDef start_page_write(mem_data_t * data) {
    return HAL_I2C_Mem_Write_IT(&hi2c1, data->dev_addr, data->dir, data->mem_addr_size, data->data, data->size);
}

HAL_StatusTypeDef start_page_read(mem_data_t * data) {
	return HAL_I2C_Mem_Read_IT(&hi2c1, data->dev_addr, data->dir, data->mem_addr_size, data->data, data->size);
}

/********************** end of file ******************************************/
