/*
 * num_buffer.h
 *
 *  Created on: Feb 27, 2026
 *      Author: Euteo
 */


/********************** inclusions *******************************************/
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "display.h"
#include "num_buffer.h"
#include "task_clock.h"
#include "task_controller.h"
#include "task_display.h"
#include "ext_memory.h"

/********************** macros and definitions *******************************/

/********************** Functions definition ***********************/
void display_num(num_buffer_t *v_num_buf) {
	char pushed_num[NUM_BUFFER_SIZE + 1];
	num_buffer_to_str(v_num_buf, pushed_num);

	task_display_request_write(NULL, pushed_num);

}


void display_num_OK(num_buffer_t *v_num_buf) {
    char pushed_num[NUM_BUFFER_SIZE + 1];
    char num_str[NUM_BUFFER_SIZE + 6];

    num_buffer_to_str(v_num_buf, pushed_num);
    snprintf(num_str, sizeof(num_str), "%s   OK", pushed_num);

    displayClearScreen();
    task_display_request_write(NULL, num_str);
}

//Display del estado init modificable por scroll
void display_initial(uint32_t idx) {
	displayClearScreen();

	if (idx == 1) {
		task_display_request_write("Lectura de", "datos");

	}
	else  {
		task_display_request_write("Configurar", "parametros");
	}
}

//Display del estado config modificable por scroll
void display_config(uint32_t idx) {
	char aux[DISPLAY_CHAR_WIDTH + 1];

	switch (idx) {
	case 0:
		strcpy(aux, "temporal");
		break;
	case 1:
		strcpy(aux, "temperatura");
		break;
	case 2:
		strcpy(aux, "humedad");
		break;
	case 3:
		strcpy(aux, "luminosidad");
		break;
	case 4:
		strcpy(aux, "Limpiar logs");
		break;
	}

	displayClearScreen();
	task_display_request_write("Configuracion", aux);
}

//Display del estado config_time modificable por scroll
void display_cfg_time(uint32_t idx) {
	displayClearScreen();
	switch (idx) {
	case 0:
		task_display_request_write("Fijar anio", NULL);
		break;
	case 1:
		task_display_request_write("Fijar mes", NULL);
		break;
	case 2:
		task_display_request_write("Fijar dia", NULL);
		break;
	case 3:
		task_display_request_write("Fijar hora", NULL);
		break;
	case 4:
		task_display_request_write("Fijar minuto", NULL);
		break;
	case 5:
		task_display_request_write("Fijar muestreo", NULL);
		break;
	}
}

//Display del estado config_temp modificable por scroll
void display_cfg_temp(uint32_t idx) {
	displayClearScreen();
	switch (idx) {
	case 0:
		task_display_request_write("Diurna minima", NULL);
		break;
	case 1:
		task_display_request_write("Diurna maxima", NULL);
		break;
	case 2:
		task_display_request_write("Nocturna minima", NULL);
		break;
	case 3:
		task_display_request_write("Nocturna maxima", NULL);
		break;
	}
}

//Display del estado config_hum modificable por scroll
void display_cfg_hum(uint32_t idx) {
	displayClearScreen();

	switch (idx) {
	case 0:
		task_display_request_write("Humedad minima", NULL);
		break;
	case 1:
		task_display_request_write("Humedad maxima", NULL);
		break;
	}
}

//Display del estado config_lig modificable por scroll
void display_cfg_lig(uint32_t idx) {
	displayClearScreen();
	switch (idx) {
	case 0:
		task_display_request_write("Luminosidad min", NULL);
		break;
	case 1:
		task_display_request_write("Horas de luz", NULL);
		break;
	}
}

void display_cfg_confirm_delete(uint32_t idx) {
	char line2[DISPLAY_CHAR_WIDTH + 1];
	snprintf(line2, sizeof(line2), "%cNo %cSi", idx == 0 ? '>' : ' ', idx == 1 ? '>' : ' ');

	displayClearScreen();
	task_display_request_write("Eliminar logs?", line2);
}

//Display del estado read modificable por scroll
void display_read(uint32_t idx) {
	char line2[DISPLAY_CHAR_WIDTH];

	switch (idx) {
	case 0:
		strcpy(line2, "historicos");
		break;
	case 1:
		strcpy(line2, "temporales");
		break;
	case 2:
		strcpy(line2, "temperatura");
		break;
	case 3:
		strcpy(line2, "humedad");
		break;
	case 4:
		strcpy(line2, "luminosidad");
		break;
	}

	displayClearScreen();
	task_display_request_write("Lectura de datos", line2);

}

//Falta implementar conversión de uint y enum meses a str
void display_read_time() {
	displayClearScreen();

    date_time_t clk = clock_get_time();
    char line1[DISPLAY_CHAR_WIDTH + 1];
    char line2[DISPLAY_CHAR_WIDTH + 1];

    snprintf(line1, sizeof(line1), "%02u/%02u/%04u", clk.day, clk.month + 1, clk.year);
    snprintf(line2, sizeof(line2), "%02u:%02u:%02u", clk.hours, clk.minutes, clk.seconds);

	task_display_request_write(line1, line2);
}


void display_read_parameters(uint32_t idx) {
	displayClearScreen();
	if (idx==0)
		task_display_request_write("Lectura de", "Configuraciones");
	else
		task_display_request_write("Lectura de", "Muestras");
}

void display_read_con(mem_type_cfg_t mem) {
    char line1[DISPLAY_CHAR_WIDTH + 1];
    char line2[DISPLAY_CHAR_WIDTH + 1];
	const mem_cfg_t * conf = controller_get_config();

	switch (mem) {
	case MEM_CFG_TEMP_DAY_MIN:
		snprintf(line1, sizeof(line1), "Temp Diurna");
		snprintf(line2, sizeof(line2), "Minima %02u C", (uint16_t) conf->temp_day_min);
		break;

	case MEM_CFG_TEMP_DAY_MAX:
		snprintf(line1, sizeof(line1), "Temp Diurna");
		snprintf(line2, sizeof(line2), "Maxima %02u C", (uint16_t) conf->temp_day_max);
		break;

	case MEM_CFG_TEMP_NIGHT_MIN:
		snprintf(line1, sizeof(line1), "Temp Nocturna");
		snprintf(line2, sizeof(line2), "Minima %02u C", (uint16_t) conf->temp_night_min);
		break;

	case MEM_CFG_TEMP_NIGHT_MAX:
		snprintf(line1, sizeof(line1), "Temp Nocturna");
		snprintf(line2, sizeof(line2), "Maxima %02u C", (uint16_t) conf->temp_night_max);
		break;

	case MEM_CFG_HUMIDITY_MIN:
		snprintf(line1, sizeof(line1), "Humedad Minima");
		snprintf(line2, sizeof(line2), "%02u %%", (uint16_t) conf->humidity_min);
		break;

	case MEM_CFG_HUMIDITY_MAX:
		snprintf(line1, sizeof(line1), "Humedad Maxima");
		snprintf(line2, sizeof(line2), "%02u %%", (uint16_t) conf->humidity_max);
		break;

	case MEM_CFG_LIGHT_HOURS_NEEDED:
		snprintf(line1, sizeof(line1), "Minimo de horas");
		snprintf(line2, sizeof(line2), "de luz %02u ", (uint16_t) conf->light_hours_needed);
		break;

	case MEM_CFG_LIGHT_THRESHOLD:
		snprintf(line1, sizeof(line1), "Umbral de luz");
		snprintf(line2, sizeof(line2), "Minimo %5u ", (uint16_t) conf->light_threshold);
		break;

	case MEM_CFG_SAVE_FREQ:
		snprintf(line1, sizeof(line1), "Horas entre toma");
		snprintf(line2, sizeof(line2), "de muestras %3u", ((uint16_t) conf->save_freq)% 1000);
		break;

	default:
		break;
	}

	displayClearScreen();
	task_display_request_write(line1, line2);
}

void display_request_log(mem_log_t * log, uint32_t idx) {
	if (!log)
		return;

	uint8_t samples_max = memory_log_size();
	uint32_t sample_idx = (samples_max - 1) - idx;

	memory_read_log_range(sample_idx, 1, log);
}

void display_read_hist(mem_log_t * curr_log) {
	displayClearScreen();

	date_time_t time = timestamp_to_datetime(curr_log->timestamp);

    char line1[DISPLAY_CHAR_WIDTH + 1];
    char line2[DISPLAY_CHAR_WIDTH + 1];

	snprintf(line1, sizeof(line1), "%02u/%02u/%04u %02u:%02u",
			time.day%100, (time.month + 1)%100, time.year%10000, time.hours%100, time.minutes%100);
	snprintf(line2, sizeof(line2), "%5uL %02u\xDF" "C %02u%%", // luz, temp, humedad: "xxxxxL yy°C zz%"
			(uint16_t) curr_log->light,
			(uint16_t) curr_log->temperature%100,
			(uint16_t) curr_log->humidity%100
	);

	task_display_request_write(line1, line2);
}

void display_reading_data(void) {
	displayClearScreen();

	task_display_request_write("Leyendo...", NULL);
}
