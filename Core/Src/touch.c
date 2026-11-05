/*
 * xpt2046_touch.c
 *
 *  Created on: 5 апр. 2020 г.
 *      Author: dima
 */

#include "main.h"
#include "touch.h"
#include "ILI9341.h"
#include "nrf24.h"
#include "stdio.h"

#include "button.h"
#include "screen.h"

#define TOUCH_SCALE_X 320
#define TOUCH_SCALE_Y 240

#define TOUCH_MIN_RAW_X 1500
#define TOUCH_MAX_RAW_X 30000
#define TOUCH_MIN_RAW_Y 2500
#define TOUCH_MAX_RAW_Y 30000

#define EDIT_ZONE_X_MIN 140
#define EDIT_ZONE_X_MAX 170
#define EDIT_ZONE_START_Y 51
#define EDIT_ZONE_ROW_STEP 20
#define MAX_TEAMS 8

#define BUTTON_PLUS_X 20
#define BUTTON_PLUS_Y 220
#define BUTTON_PLUS_W 40
#define BUTTON_PLUS_H 20

#define BUTTON_SAVE_X 130
#define BUTTON_SAVE_Y 220
#define BUTTON_SAVE_W 60
#define BUTTON_SAVE_H 20

#define BUTTON_MINUS_X 260
#define BUTTON_MINUS_Y 220
#define BUTTON_MINUS_W 40
#define BUTTON_MINUS_H 20

#define BUTTON_Y 225

extern TIM_HandleTypeDef htim2;

static const uint8_t cmd_read_x = 0x90;
static const uint8_t cmd_read_y = 0xD0;
static const uint8_t zeroes_tx[] =
{ 0x00, 0x00 };

extern uint16_t edit_score;
extern int16_t scores[8];
extern uint16_t screen;
extern uint16_t x;
extern uint16_t y;
extern uint8_t flag_press;
extern uint16_t teams;
extern char scope_buf[6];

static inline void int16_to_str(int16_t val, char *buf, uint8_t maxlen)
{
    if (maxlen == 0)
        return;



    int32_t v = (int32_t)val;   /* widen so that -INT16_MIN (-32768) is safe */
    char digits[6];
    uint8_t n = 0;
    uint8_t i = 0;

    if (v < 0) {
        v = -v;
        if (i + 1 < maxlen)
            buf[i++] = '-';
    }

    do {
        if (n < 5)
            digits[n++] = '0' + (v % 10);
        v /= 10;
    } while (v > 0);

    while (n > 0 && i < maxlen - 1)
        buf[i++] = digits[--n];
    buf[i] = '\0';
}


uint8_t ILI9341_TouchGetCoordinates(uint16_t *x, uint16_t *y)
{
	if (HAL_GPIO_ReadPin(GPIOB, T_IRQ_Pin) != GPIO_PIN_RESET)
		return 0;
	uint32_t avg_x = 0;
	uint32_t avg_y = 0;
	HAL_SPI_Transmit(TOUCH_SPI_PTR, (uint8_t*) &cmd_read_y, 1, 1000);
	uint8_t y_raw[2] =
	{ 0, };
	HAL_SPI_TransmitReceive(TOUCH_SPI_PTR, (uint8_t*) zeroes_tx, y_raw, 2, 1000);
	HAL_SPI_Transmit(TOUCH_SPI_PTR, (uint8_t*) &cmd_read_x, 1, 1000);
	uint8_t x_raw[2] =
	{ 0, };
	HAL_SPI_TransmitReceive(TOUCH_SPI_PTR, (uint8_t*) zeroes_tx, x_raw, 2, 1000);
	avg_x = (((uint16_t) x_raw[0]) << 8) | ((uint16_t) x_raw[1]);
	avg_y = (((uint16_t) y_raw[0]) << 8) | ((uint16_t) y_raw[1]);
	if (avg_x < TOUCH_MIN_RAW_X)
		avg_x = TOUCH_MIN_RAW_X;
	if (avg_x > TOUCH_MAX_RAW_X)
		avg_x = TOUCH_MAX_RAW_X;

	if (avg_y < TOUCH_MIN_RAW_Y)
		avg_y = TOUCH_MIN_RAW_Y;
	if (avg_y > TOUCH_MAX_RAW_Y)
		avg_y = TOUCH_MAX_RAW_Y;

	*x = (avg_x - TOUCH_MIN_RAW_X) * TOUCH_SCALE_X / (TOUCH_MAX_RAW_X - TOUCH_MIN_RAW_X);
	*y = (avg_y - TOUCH_MIN_RAW_Y) * TOUCH_SCALE_Y / (TOUCH_MAX_RAW_Y - TOUCH_MIN_RAW_Y);
	return 1;
}

