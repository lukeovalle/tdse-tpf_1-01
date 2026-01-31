#include "keypad.h"
#include "stm32fxxx_hal.h"
#include "dwt.h"

/* === CONFIGURACION HARDWARE === */
GPIO_TypeDef* ROW_PORT[4] = {GPIOA,         GPIOA,      GPIOA,      GPIOA};
uint16_t ROW_PIN[4]       = {GPIO_PIN_0,    GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3};

GPIO_TypeDef* COL_PORT[4] = {GPIOB,         GPIOB,      GPIOB,      GPIOB};
uint16_t COL_PIN[4]       = {GPIO_PIN_0,    GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3};
/* ============================== */

static const keypad_key_t key_map[4][4] = {
    {KEY_1, KEY_2, KEY_3, KEY_A},
    {KEY_4, KEY_5, KEY_6, KEY_B},
    {KEY_7, KEY_8, KEY_9, KEY_C},
    {KEY_STAR, KEY_0, KEY_HASH, KEY_D}
};

void display_delay_us(uint32_t delay_us)
{
	uint32_t now = cycle_counter_time_us();
	uint32_t then = delay_us + now;

	while (now < then)
		now = cycle_counter_time_us();
}

keypad_key_t keypad_scan(void)
{
    for (uint8_t i = 0; i < 4; i++) HAL_GPIO_WritePin(ROW_PORT[i], ROW_PIN[i], GPIO_PIN_SET);

    for (uint8_t row = 0; row < 4; row++)
    {
        HAL_GPIO_WritePin(ROW_PORT[row], ROW_PIN[row], GPIO_PIN_RESET);
        display_delay_us(5);

        for (uint8_t col = 0; col < 4; col++)
            if (HAL_GPIO_ReadPin(COL_PORT[col], COL_PIN[col]) == GPIO_PIN_RESET)
                return key_map[row][col];

        HAL_GPIO_WritePin(ROW_PORT[row], ROW_PIN[row], GPIO_PIN_SET);
    }

    return KEY_NONE;
}
