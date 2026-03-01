/*
 * num_buffer.c
 *
 *  Created on: Feb 24, 2026
 *      Author: Euteo
 */

#include "num_buffer.h"

num_buffer_t num_buff;

//Reseteo tras cambio de estado
void num_buffer_reset(num_buffer_t *d) {
    d->count = 0;
}

//Agregar numero insertado
void num_buffer_push(num_buffer_t *d, uint8_t digit) {
    if (d->count < NUM_BUFFER_SIZE) {
        d->buf[d->count++] = digit;
    }
}

//Convertir los valores del buffer en un número
uint16_t num_buffer_to_int(num_buffer_t *d) {
    uint16_t result = 0;

    for (uint8_t i = 0; i < d->count; i++) {
        result = result * 10 + d->buf[i];
    }

    return result;
}

//Convertir los valores del buffer en un string
char* num_buffer_to_str(num_buffer_t *d, char *out) {
    for (uint8_t i = 0; i < d->count; i++) {
        out[i] = d->buf[i] + '0';  // convierte a ACSII
    }

    out[d->count] = '\0'; // EOL
    return out;
}
