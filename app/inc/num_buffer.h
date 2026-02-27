/*
 * num_buffer.h
 *
 *  Created on: Feb 24, 2026
 *      Author: Euteo
 */

/********************** inclusions *******************************************/
#include <stdint.h>

/********************** macros ***********************************************/
#define NUM_BUFFER_SIZE 4

/********************** typedef **********************************************/
//Budder numerico de 4 digitos para numeros tipeados por usuario
typedef struct {
    uint8_t buf[NUM_BUFFER_SIZE];
    uint8_t count;
} num_buffer_t;

/********************** external data declaration ****************************/

/********************** external functions declaration ***********************/
void num_buffer_reset(num_buffer_t *d);
void num_buffer_push(num_buffer_t *d, uint8_t digit);
uint16_t num_buffer_to_int(num_buffer_t *d);
char* num_buffer_to_str(num_buffer_t *d, char *out);
