/*
 * button.c
 *
 *  Created on: 3 мая 2026 г.
 *      Author: dima
 */

#include "stm32l1xx_hal.h"
#include "main.h"
#include "button.h"
#include "nrf24.h"
#include "ILI9341.h"
#include "touch.h"
#include <stdio.h>
#include <string.h>

extern uint8_t rx_data;
extern uint8_t falstart_enabled;
extern uint16_t pressed_btn_team;
extern uint8_t timer_running;
extern uint8_t flag_press;
extern uint32_t time_press;
extern uint16_t x;
extern uint16_t y;
extern game_screen screen;
extern uint8_t g_timer_settings_erudit;
extern char buf[64];

extern int16_t scores[8];							//количество очков команд [0-7]

extern volatile int g_timer_seconds;				//Начальное значение таймера
extern volatile uint8_t reset_timer;				//старт/стоп таймера
extern uint16_t teams;								//номер команды
extern uint16_t teams_answer_state[8];         		// Teams answer state
extern uint16_t teams_fs_state[8];          		// Teams false start state
extern uint16_t answer;								//положительный или отрицательный ответ
extern uint8_t question_id;							// question id
// macros to check the coordinates in range
#define IS_WITHIN(x, y, x1, y1, x2, y2) ((x) > (x1)) && ((x) < (x2)) && ((y) > (y1)) && ((y) < (y2))

// Set to GPIO_PIN_RESET if using Pull-Up (Pressed = GND)
// Set to GPIO_PIN_SET   if using Pull-Down (Pressed = VCC)
#define BUTTON_PRESSED_LEVEL   GPIO_PIN_RESET

extern TIM_HandleTypeDef htim2;
extern UART_HandleTypeDef huart1;

//=======Обработчик левой кнопки KEY1==================
uint8_t Button_Read_left(void)
{
	static uint32_t last_time = 0;
	static uint8_t last_state = 0;
	uint8_t current_state;
	current_state = HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_PIN_LEFT);
	if (current_state != last_state)
	{
		if (HAL_GetTick() - last_time > 20) // 20 мс антидребезг
		{
			last_time = HAL_GetTick();
			last_state = current_state;
			if (current_state != GPIO_PIN_SET)
				return 1; // кнопка нажата
		}
	}
	return 0;
}
//======= Center button handler KEY2==================
uint8_t Button_Read_center(void)
{
	typedef enum
	{
		BTN_IDLE, BTN_DEBOUNCE, BTN_PRESSED, BTN_LONG_HELD
	} BTN_State_t;

	static BTN_State_t state = BTN_IDLE;
	static uint32_t timer = 0;

	uint32_t now = HAL_GetTick();
	uint8_t is_pressed = (HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_PIN_CENTER) == BUTTON_PRESSED_LEVEL);

	switch (state)
	{
	case BTN_IDLE:
		if (is_pressed)
		{
			timer = now;
			state = BTN_DEBOUNCE;
		}
		break;

	case BTN_DEBOUNCE:
		if (!is_pressed)
		{
			// Contact bounce noise – ignore
			state = BTN_IDLE;
		}
		else if ((now - timer) >= 30) // 30 ms debounce filter
		{
			// Valid press confirmed
			timer = now;
			state = BTN_PRESSED;
		}
		break;

	case BTN_PRESSED:
		if (!is_pressed)
		{
			// Released before 1000 ms -> Short Press
			state = BTN_IDLE;
			return 1;
		}
		else if ((now - timer) >= 1000)
		{
			// Held for 1000 ms -> Long Press
			state = BTN_LONG_HELD;
			return 2;
		}
		break;

	case BTN_LONG_HELD:
		if (!is_pressed)
		{
			// Button finally released after long press
			state = BTN_IDLE;
		}
		break;
	}

	return 0; // No event
}
//=======Обработчик правой кнопки KEY3==================
uint8_t Button_Read_right(void)
{
	static uint32_t last_time = 0;
	static uint8_t last_state = 0;
	uint8_t current_state;
	current_state = HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_PIN_RIGHT);
	if (current_state != last_state)
	{
		if (HAL_GetTick() - last_time > 20) // 20 мс антидребезг
		{
			last_time = HAL_GetTick();
			last_state = current_state;
			if (current_state == GPIO_PIN_SET)
				return 1; // кнопка нажата
		}
	}
	return 0;
}
//=======Обработчик кнопок передатчиков===============
void Reset_answer_state(void)
{
	for (uint8_t i = 0; i < 8; i++)
		teams_answer_state[i] = 0;
}

void Reset_falstart_state(void)
{
	for (uint8_t i = 0; i < 8; i++)
		teams_fs_state[i] = 0;
}


void Touchscreen_handler(void)
{
	if (HAL_GPIO_ReadPin(GPIOB, T_IRQ_Pin) == GPIO_PIN_RESET && flag_press) //если нажат тачскрин
	{
		x = 0;
		y = 0;
		if (ILI9341_TouchGetCoordinates(&x, &y))
		{
			flag_press = 0;
			//>>>>>>>>>>вывод координат в UART для отладки
			snprintf(buf, 64, "X = %d, Y = %d\n", x, y);
			HAL_UART_Transmit(&huart1, (uint8_t*) buf, strlen(buf), 100);
			buf[strlen(buf) - 1] = '\0';
			//ILI9341_WriteString(10, 120, buf, Font_11x18, WHITE, MYFON);//вывод координат на экран для отладки
		}
		if (IS_WITHIN(x, y, 20, 60, 100, 120))
					{

					}
	}

	if (!flag_press && (HAL_GetTick() - time_press) > 200) { // задержка до следующего нажатия
		flag_press = 1;
	}
}

