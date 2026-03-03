/*
 * num_buffer.h
 *
 *  Created on: Feb 27, 2026
 *      Author: Euteo
 */

/********************** inclusions *******************************************/
#include <stdint.h>


/********************** internal functions declaration ***********************/
void display_initial(uint32_t idx);
void display_config(uint32_t idx);
void display_cfg_time(uint32_t idx);
void display_cfg_temp(uint32_t idx);
void display_cfg_hum(uint32_t idx);
void display_cfg_lig(uint32_t idx);
void display_num(num_buffer_t *v_num_buf) ;
void display_num_OK(num_buffer_t *v_num_buf) ;
void display_read(uint32_t idx);
void display_read_time();
void display_read_parameters(uint32_t idx);
void display_read_con(mem_type_cfg_t mem);
void display_request_log(mem_log_t * log, uint32_t idx);
void display_read_hist(mem_log_t * curr_log);
