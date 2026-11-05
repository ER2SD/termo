/*
 * screen.c
 *
 *  Created on: 6 апр. 2026 г.
 *      Author: dima
 */
#include "main.h"
#include "screen.h"
#include "ILI9341.h"
#include "touch.h"

#include "stdio.h"
#include "fonts.h"

extern game_screen screen;
extern uint16_t teams;
extern uint16_t pressed_btn_team;
extern int16_t scores[8];
extern uint16_t answer;
extern uint8_t question_id;
char scope_buf[16];

#define TEAM_LABEL_X      7
#define TEAM_SCORE_X      135
#define TEAM_START_Y      51
#define TEAM_ROW_STEP     20
#define MAX_TEAMS         8

static inline void draw_team_name(uint8_t idx, uint16_t color)
{
	snprintf(scope_buf, sizeof(scope_buf), "Team %u", idx + 1);
	ILI9341_WriteString(TEAM_LABEL_X, TEAM_START_Y + idx * TEAM_ROW_STEP, scope_buf, Font_11x18, color, MYFON);
}

//============= координатная сетка =====================
void show_coordinate_grid()
{
	for (uint16_t z = 20; z < 240; z += 20)
		ILI9341_Draw_Horizontal_Line(0, z, 320, WHITE);
	for (uint16_t h = 20; h < 320; h += 20)
		ILI9341_Draw_Vertical_Line(h, 0, 240, WHITE);
}

//============= Турнирная таблица =====================
void tournament_table() {
	for (uint16_t z = 50; z < 220; z += 20)
		ILI9341_Draw_Horizontal_Line(120, z, 80, RED);
	for (uint16_t h = 120; h < 201; h += 80)
		ILI9341_Draw_Vertical_Line(h, 50, 160, RED);
}
// simplified table
void tournament_table_simple() {
	for (uint16_t z = 50; z < 220; z += 20)
			ILI9341_Draw_Horizontal_Line(120, z, 80, RED);
	for (uint16_t h = 120; h < 201; h += 80)
			ILI9341_Draw_Vertical_Line(h, 50, 160, RED);
}
//==============установка цвета названия комманд для следующего вопроса===========
void reset_color(void)
{
	for (uint8_t i = 0; i < MAX_TEAMS; i++)
	{
		uint16_t color = (i < teams) ? WHITE : BLACK;
		draw_team_name(i, color);
	}
}

//============= экран выбора игры =====================
void screen_menu()
{
	screen = MAIN_MENU;
	ILI9341_Fill_Screen(YELLOW);
	ILI9341_Draw_Filled_Rectangle_Coord(10, 10, 310, 230, RED);
	ILI9341_WriteString(128, 12, "CMJI", Font_16x26, NAVY, RED);
	ILI9341_Draw_Filled_Rectangle_Coord(20, 60, 100, 120, BLUE);
	ILI9341_Draw_Filled_Rectangle_Coord(120, 60, 200, 120, BLUE);
	ILI9341_Draw_Filled_Rectangle_Coord(220, 60, 300, 120, BLUE);
	ILI9341_Draw_Filled_Rectangle_Coord(20, 150, 300, 210, BLUE);
	ILI9341_WriteString(32, 82, "Brain", Font_11x18, ORANGE, BLUE);
	ILI9341_WriteString(128, 82, "Simple", Font_11x18, ORANGE, BLUE);
	ILI9341_WriteString(228, 82, "Erudit", Font_11x18, ORANGE, BLUE);
	ILI9341_WriteString(100, 167, "Settings", Font_16x26, ORANGE, BLUE);
}




