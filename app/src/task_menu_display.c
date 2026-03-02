/*
 * num_buffer.h
 *
 *  Created on: Feb 27, 2026
 *      Author: Euteo
 */


/********************** inclusions *******************************************/
#include <stdint.h>
#include <stdio.h>
#include "display.h"
#include "num_buffer.h"
#include "task_clock.h"
#include "ext_memory.h"

/********************** macros and definitions *******************************/
#define NUM_BUFFER_SIZE 4


/********************** Functions definition ***********************/
void display_num(num_buffer_t *v_num_buf) {
	char pushed_num[NUM_BUFFER_SIZE + 1];
	num_buffer_to_str(v_num_buf, pushed_num);

	displayCharPositionWrite(0, 1);
	displayStringWrite(pushed_num);
}


void display_num_OK(num_buffer_t *v_num_buf) {
    char pushed_num[NUM_BUFFER_SIZE + 1];
    char num_str[NUM_BUFFER_SIZE + 6];

    num_buffer_to_str(v_num_buf, pushed_num);
    snprintf(num_str, sizeof(num_str), "%s   OK", pushed_num);

    displayCharPositionWrite(0, 1);
    displayStringWrite(num_str);
}

//Display del estado init modificable por scroll
void display_initial(uint32_t idx) {
	displayClearScreen();

	if (idx == 1) {
		displayCharPositionWrite(0, 0);
		displayStringWrite("Lectura de");
		displayCharPositionWrite(0, 1);
		displayStringWrite("datos");
	}
	else  {
		displayCharPositionWrite(0, 0);
		displayStringWrite("Configurar");
		displayCharPositionWrite(0, 1);
		displayStringWrite("parametros");
	}
}

//Display del estado config modificable por scroll
void display_config(uint32_t idx) {
	displayClearScreen();

	displayCharPositionWrite(0, 0);
	displayStringWrite("Configuracion");
	displayCharPositionWrite(0, 1);
	switch (idx) {
		case 0:
			displayStringWrite("temporal");
			break;
		case 1:
			displayStringWrite("temperatura");
			break;
		case 2:
			displayStringWrite("humedad");
			break;
		case 3:
			displayStringWrite("luminocidad");
			break;
	}
}

//Display del estado config_time modificable por scroll
void display_cfg_time(uint32_t idx) {
	displayClearScreen();

	switch (idx) {
		case 0:
			displayCharPositionWrite(0, 0);
			displayStringWrite("Fijar día");
			break;
		case 1:
			displayCharPositionWrite(0, 0);
			displayStringWrite("Fijar mes");
			break;
		case 2:
			displayCharPositionWrite(0, 0);
			displayStringWrite("Fijar año");
			break;
		case 3:
			displayCharPositionWrite(0, 0);
			displayStringWrite("Fijar hora");
			break;
		case 4:
			displayCharPositionWrite(0, 0);
			displayStringWrite("Fijar minuto");
			break;
		case 5:
			displayCharPositionWrite(0, 0);
			displayStringWrite("Freq muestreo");
			break;
	}
}

//Display del estado config_temp modificable por scroll
void display_cfg_temp(uint32_t idx) {
	displayClearScreen();

	displayCharPositionWrite(0, 0);
	switch (idx) {
		case 0:
			displayStringWrite("Diurna minima");
			break;
		case 1:
			displayStringWrite("Diurna maxima");
			break;
		case 2:
			displayStringWrite("Nocturna minima");
			break;
		case 3:
			displayStringWrite("Nocturna maxima");
			break;
	}
}

//Display del estado config_hum modificable por scroll
void display_cfg_hum(uint32_t idx) {
	displayClearScreen();

	displayCharPositionWrite(0, 0);
	switch (idx) {
		case 0:
			displayStringWrite("Humedad minima");
			break;
		case 1:
			displayStringWrite("Humedad maxima");
			break;
	}
}

//Display del estado config_lig modificable por scroll
void display_cfg_lig(uint32_t idx) {
	displayClearScreen();

	displayCharPositionWrite(0, 0);
	switch (idx) {
		case 0:
			displayStringWrite("Luminocidad minima");
			break;
		case 1:
			displayStringWrite("Horas de luz");
			break;
	}
}

//Display del estado read modificable por scroll
void display_read(uint32_t idx) {
	displayClearScreen();

	displayCharPositionWrite(0, 0);
	displayStringWrite("Lectura de datos");
	displayCharPositionWrite(0, 1);
	switch (idx) {
		case 0:
			displayStringWrite("temporales");
			break;
		case 1:
			displayStringWrite("temperatura");
			break;
		case 2:
			displayStringWrite("humedad");
			break;
		case 3:
			displayStringWrite("luminocidad");
			break;
	}
}

//Falta implementar conversión de uint y enum meses a str
void display_read_time() {
	displayClearScreen();

    date_time_t clk = clock_get_time();
    char line1[17];
    char line2[17];

    snprintf(line1, sizeof(line1), "%02u/%02u/%04u", clk.day, clk.month + 1, clk.year);
    snprintf(line2, sizeof(line2), "%02u:%02u", clk.hours, clk.minutes);

    displayCharPositionWrite(0, 0);
    displayStringWrite(line1);
    displayCharPositionWrite(0, 1);
    displayStringWrite(line2);
}


void display_read_parameters(uint32_t idx) {
	displayClearScreen();

    displayCharPositionWrite(0, 0);
    displayStringWrite("Lectura de ");
    displayCharPositionWrite(0, 1);

	if (idx==0)
		displayStringWrite("Configuraciones");
	else
		displayStringWrite("Muestras");
}

void display_read_con(mem_type_cfg_t mem) {
	displayClearScreen();

    char line1[17];
    char line2[17];
	mem_cfg_t * conf = controller_get_config();

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
			snprintf(line2, sizeof(line2), "Minimo %04u ", (uint16_t) conf->light_threshold);
			break;

		case MEM_CFG_SAVE_FREQ:
			snprintf(line1, sizeof(line1), "Horas entre toma");
			snprintf(line2, sizeof(line2), "de muestras %03u", (uint16_t) conf->save_freq);
			break;

		default:
			break;
		}

    displayCharPositionWrite(0, 0);
    displayStringWrite(line1);
    displayCharPositionWrite(0, 1);
    displayStringWrite(line2);
}

void display_read_his(mem_type_cfg_t mem, uint32_t idx) {
	displayClearScreen();

	uint8_t samples_max = memory_log_size();
	uint32_t sample_idx = (samples_max - 1) - idx;
	mem_log_t sample;
	memory_read_log_range(sample_idx, 1, &sample);

	date_time_t time = timestamp_to_datetime(sample.timestamp);

    char line1[17];
    char line2[17];

    switch (mem) {
			case MEM_CFG_TEMP_DAY_MIN:
				uint16_t temp = (uint16_t) sample.temperature;
				snprintf(line1, sizeof(line1), "Temperatura %02u", temp);
				snprintf(line2, sizeof(line2), "%02u/%02u/%04u %02u:%02u", time.day, time.month, time.year, time.hours, time.minutes);
				break;

			case MEM_CFG_HUMIDITY_MIN:
				snprintf(line1, sizeof(line1), "Humedad %03u %%", (uint16_t) sample.humidity);
				snprintf(line2, sizeof(line2), "%02u/%02u/%04u %02u:%02u", time.day, time.month, time.year, time.hours, time.minutes);
				break;

			case MEM_CFG_LIGHT_THRESHOLD:
				snprintf(line1, sizeof(line1), "Luminocidad %02u", (uint16_t) sample.light);
				snprintf(line2, sizeof(line2), "%02u/%02u/%04u %02u:%02u", time.day, time.month, time.year, time.hours, time.minutes);
				break;

			default:
				break;
	}
}
