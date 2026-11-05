

#include "ILI9341.h"
#include "screen.h"
#include "stdlib.h"

#define swap(a, b) { int16_t t = a; a = b; b = t; }

volatile uint16_t LCD_HEIGHT = ILI9341_SCREEN_HEIGHT;
volatile uint16_t LCD_WIDTH  = ILI9341_SCREEN_WIDTH;

// SPI helper - replaces duplicated CS/DC/SPI sequence
static inline void _lcd_send_byte(uint8_t dc, uint8_t byte)
{
    if (dc) DISP_DC_DATA; else DISP_DC_CMD;
    DISP_CS_SELECT;
    DISP_SPI->DR = byte;
    while(!(DISP_SPI->SR & SPI_SR_TXE));
    while(DISP_SPI->SR & SPI_SR_BSY);
    DISP_CS_UNSELECT;
}

#define LCD_SEND_CMD(cmd) _lcd_send_byte(0, cmd)
#define LCD_SEND_DATA(d)  _lcd_send_byte(1, d)

//>>>>>>>>>>Отправить команду (char) to LCD
void ILI9341_Write_Command(uint8_t Command)
{
	LCD_SEND_CMD(Command);
}

//>>>>>>>>>>Отправка данных (char) to LCD
void ILI9341_Write_Data(uint8_t Data)
{
	LCD_SEND_DATA(Data);
}

//>>>>>>>>>>Set Address - Location block - to draw into
void ILI9341_Set_Address(uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2)
{
    DISP_DC_CMD; DISP_CS_SELECT;
    DISP_SPI->DR = 0x2A;
    while(!(DISP_SPI->SR & SPI_SR_TXE));
    while(DISP_SPI->SR & SPI_SR_BSY);

    DISP_DC_DATA;
    DISP_SPI->DR = (uint8_t)(X1 >> 8);
    while(!(DISP_SPI->SR & SPI_SR_TXE));
    DISP_SPI->DR = (uint8_t)X1;
    while(!(DISP_SPI->SR & SPI_SR_TXE));
    DISP_SPI->DR = (uint8_t)(X2 >> 8);
    while(!(DISP_SPI->SR & SPI_SR_TXE));
    DISP_SPI->DR = (uint8_t)X2;
    while(!(DISP_SPI->SR & SPI_SR_TXE));
    while(DISP_SPI->SR & SPI_SR_BSY);

    DISP_DC_CMD;
    DISP_SPI->DR = 0x2B;
    while(!(DISP_SPI->SR & SPI_SR_TXE));
    while(DISP_SPI->SR & SPI_SR_BSY);

    DISP_DC_DATA;
    DISP_SPI->DR = (uint8_t)(Y1 >> 8);
    while(!(DISP_SPI->SR & SPI_SR_TXE));
    DISP_SPI->DR = (uint8_t)Y1;
    while(!(DISP_SPI->SR & SPI_SR_TXE));
    DISP_SPI->DR = (uint8_t)(Y2 >> 8);
    while(!(DISP_SPI->SR & SPI_SR_TXE));
    DISP_SPI->DR = (uint8_t)Y2;
    while(!(DISP_SPI->SR & SPI_SR_TXE));
    while(DISP_SPI->SR & SPI_SR_BSY);

    DISP_DC_CMD;
    DISP_SPI->DR = 0x2C;
    while(!(DISP_SPI->SR & SPI_SR_TXE));
    while(DISP_SPI->SR & SPI_SR_BSY);
    DISP_CS_UNSELECT;
}

//>>>>>>>>>>HARDWARE RESET
void ILI9341_Reset(void)
{
	DISP_RST_RESET;
	HAL_Delay(200);
	DISP_CS_SELECT;
	HAL_Delay(200);
	DISP_RST_WORK;
}

//>>>>>>>>>>Плавное вращение экрана - changes x0 and y0
void ILI9341_Set_Rotation(uint8_t Rotation)
{
	DISP_DC_CMD; DISP_CS_SELECT;
	DISP_SPI->DR = 0x36;
	while(!(DISP_SPI->SR & SPI_SR_TXE));
	while(DISP_SPI->SR & SPI_SR_BSY);
	DISP_CS_UNSELECT;

	static const struct { uint8_t data; uint16_t w; uint16_t h; } rot_table[] = {
		{ 0x40 | 0x08, 240, 320 },  // SCREEN_VERTICAL_1
		{ 0x20 | 0x08, 320, 240 },  // SCREEN_HORIZONTAL_1
		{ 0x80 | 0x08, 240, 320 },  // SCREEN_VERTICAL_2
		{ 0x40 | 0x80 | 0x20 | 0x08, 320, 240 }  // SCREEN_HORIZONTAL_2
	};

	if (Rotation < 4)
	{
		ILI9341_Write_Data(rot_table[Rotation].data);
		LCD_WIDTH  = rot_table[Rotation].w;
		LCD_HEIGHT = rot_table[Rotation].h;
	}
}


//>>>>>>>>>>Initialize LCD display
void ILI9341_Init(void)
{
	DISP_RST_WORK;
	DISP_CS_SELECT;
	ILI9341_Reset();

	ILI9341_Write_Command(0x01);
	HAL_Delay(1000);

	ILI9341_Write_Command(0xCB); ILI9341_Write_Data(0x39); ILI9341_Write_Data(0x2C); ILI9341_Write_Data(0x00); ILI9341_Write_Data(0x34); ILI9341_Write_Data(0x02);
	ILI9341_Write_Command(0xCF); ILI9341_Write_Data(0x00); ILI9341_Write_Data(0xC1); ILI9341_Write_Data(0x30);
	ILI9341_Write_Command(0xE8); ILI9341_Write_Data(0x85); ILI9341_Write_Data(0x00); ILI9341_Write_Data(0x78);
	ILI9341_Write_Command(0xEA); ILI9341_Write_Data(0x00); ILI9341_Write_Data(0x00);
	ILI9341_Write_Command(0xED); ILI9341_Write_Data(0x64); ILI9341_Write_Data(0x03); ILI9341_Write_Data(0x12); ILI9341_Write_Data(0x81);
	ILI9341_Write_Command(0xF7); ILI9341_Write_Data(0x20);
	ILI9341_Write_Command(0xC0); ILI9341_Write_Data(0x23);
	ILI9341_Write_Command(0xC1); ILI9341_Write_Data(0x10);
	ILI9341_Write_Command(0xC5); ILI9341_Write_Data(0x3E); ILI9341_Write_Data(0x28);
	ILI9341_Write_Command(0xC7); ILI9341_Write_Data(0x86);
	ILI9341_Write_Command(0x36); ILI9341_Write_Data(0x48);
	ILI9341_Write_Command(0x3A); ILI9341_Write_Data(0x55);
	ILI9341_Write_Command(0xB1); ILI9341_Write_Data(0x00); ILI9341_Write_Data(0x18);
	ILI9341_Write_Command(0xB6); ILI9341_Write_Data(0x08); ILI9341_Write_Data(0x82); ILI9341_Write_Data(0x27);
	ILI9341_Write_Command(0xF2); ILI9341_Write_Data(0x00);
	ILI9341_Write_Command(0x26); ILI9341_Write_Data(0x01);
	ILI9341_Write_Command(0xE0); ILI9341_Write_Data(0x0F); ILI9341_Write_Data(0x31); ILI9341_Write_Data(0x2B); ILI9341_Write_Data(0x0C); ILI9341_Write_Data(0x0E); ILI9341_Write_Data(0x08); ILI9341_Write_Data(0x4E); ILI9341_Write_Data(0xF1); ILI9341_Write_Data(0x37); ILI9341_Write_Data(0x07); ILI9341_Write_Data(0x10); ILI9341_Write_Data(0x03); ILI9341_Write_Data(0x0E); ILI9341_Write_Data(0x09); ILI9341_Write_Data(0x00);
	ILI9341_Write_Command(0xE1); ILI9341_Write_Data(0x00); ILI9341_Write_Data(0x0E); ILI9341_Write_Data(0x14); ILI9341_Write_Data(0x03); ILI9341_Write_Data(0x11); ILI9341_Write_Data(0x07); ILI9341_Write_Data(0x31); ILI9341_Write_Data(0xC1); ILI9341_Write_Data(0x48); ILI9341_Write_Data(0x08); ILI9341_Write_Data(0x0F); ILI9341_Write_Data(0x0C); ILI9341_Write_Data(0x31); ILI9341_Write_Data(0x36); ILI9341_Write_Data(0x0F);

	ILI9341_Write_Command(0x11);
	HAL_Delay(120);

	ILI9341_Write_Command(0x29);

	ILI9341_Set_Rotation(SCREEN_HORIZONTAL_2);
}


//ВНУТРЕННЯЯ ФУНКЦИЯ БИБЛИОТЕКИ, ИСПОЛЬЗОВАНИЕ КОТОРОЙ НЕ РЕКОМЕНДУЕТСЯ, ВМЕСТО ЭТОГО ИСПОЛЬЗУЙТЕ Draw_Pixel
//>>>>>>>>>>Отправляет информацию о цвете одного пикселя на ЖК-дисплей
void ILI9341_Draw_Colour(uint16_t Colour)
{
	DISP_DC_DATA;
	DISP_CS_SELECT;
	DISP_SPI->DR = (Colour >> 8);
	while(!(DISP_SPI->SR & SPI_SR_TXE));
	DISP_SPI->DR = Colour;
	while(!(DISP_SPI->SR & SPI_SR_TXE));
	while(DISP_SPI->SR & SPI_SR_BSY);
	DISP_CS_UNSELECT;
}

//ВНУТРЕННЯЯ ФУНКЦИЯ БИБЛИОТЕКИ
//>>>>>>>>>>Отправляет информацию о цвете блока на ЖК-дисплей
void ILI9341_Draw_Colour_Burst(uint16_t Colour, uint32_t Size)
{
	DISP_DC_DATA;
	DISP_CS_SELECT;

	while(Size > 0)
	{
		DISP_SPI->DR = (Colour >> 8);
		while(!(DISP_SPI->SR & SPI_SR_TXE));
		DISP_SPI->DR = Colour;
		while(!(DISP_SPI->SR & SPI_SR_TXE));
		Size--;
	}

	while(!(DISP_SPI->SR & SPI_SR_TXE));
	while(DISP_SPI->SR & SPI_SR_BSY);
	DISP_CS_UNSELECT;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// USER FUNCTION //////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//>>>>>>>>>>ЗАПОЛНИТЕ ВЕСЬ ЭКРАН ВЫБРАННЫМ ЦВЕТОМ (either #define-d ones or custom 16bit)
void ILI9341_Fill_Screen(uint16_t Colour)
{
	ILI9341_Set_Address(0, 0, LCD_WIDTH, LCD_HEIGHT);
	ILI9341_Draw_Colour_Burst(Colour, LCD_WIDTH * LCD_HEIGHT);
}

//>>>>>>>>>>НАРИСУЙТЕ ПИКСЕЛЬ В ПОЗИЦИИ XY ВЫБРАННЫМ ЦВЕТОМ
void ILI9341_Draw_Pixel(uint16_t X, uint16_t Y, uint16_t Colour)
{
	if((X >= LCD_WIDTH) || (Y >= LCD_HEIGHT)) return;

	DISP_DC_CMD;
	DISP_CS_SELECT;
	DISP_SPI->DR = 0x2A;
	while(!(DISP_SPI->SR & SPI_SR_TXE));
	while(DISP_SPI->SR & SPI_SR_BSY);

	DISP_DC_DATA;
	DISP_SPI->DR = (X >> 8);
	while(!(DISP_SPI->SR & SPI_SR_TXE));
	DISP_SPI->DR = X;
	while(!(DISP_SPI->SR & SPI_SR_TXE));
	DISP_SPI->DR = ((X + 1) >> 8);
	while(!(DISP_SPI->SR & SPI_SR_TXE));
	DISP_SPI->DR = (X + 1);
	while(!(DISP_SPI->SR & SPI_SR_TXE));
	while(DISP_SPI->SR & SPI_SR_BSY);

	DISP_DC_CMD;
	DISP_SPI->DR = 0x2B;
	while(!(DISP_SPI->SR & SPI_SR_TXE));
	while(DISP_SPI->SR & SPI_SR_BSY);

	DISP_DC_DATA;
	DISP_SPI->DR = (Y >> 8);
	while(!(DISP_SPI->SR & SPI_SR_TXE));
	DISP_SPI->DR = Y;
	while(!(DISP_SPI->SR & SPI_SR_TXE));
	DISP_SPI->DR = ((Y + 1) >> 8);
	while(!(DISP_SPI->SR & SPI_SR_TXE));
	DISP_SPI->DR = (Y + 1);
	while(!(DISP_SPI->SR & SPI_SR_TXE));
	while(DISP_SPI->SR & SPI_SR_BSY);

	DISP_DC_CMD;
	DISP_SPI->DR = 0x2C;
	while(!(DISP_SPI->SR & SPI_SR_TXE));
	while(DISP_SPI->SR & SPI_SR_BSY);

	DISP_DC_DATA;
	DISP_SPI->DR = (Colour >> 8);
	while(!(DISP_SPI->SR & SPI_SR_TXE));
	DISP_SPI->DR = Colour;
	while(!(DISP_SPI->SR & SPI_SR_TXE));
	while(DISP_SPI->SR & SPI_SR_BSY);

	DISP_CS_UNSELECT;
}

//>>>>>>>>>>НАРИСУЙТЕ ПРЯМОУГОЛЬНИК ЗАДАННОГО РАЗМЕРА И ВЫСОТЫ В ТОЧКАХ X и Y ПРОИЗВОЛЬНЫМ ЦВЕТОМ
void ILI9341_Draw_Rectangle(uint16_t X, uint16_t Y, uint16_t Width, uint16_t Height, uint16_t Colour)
{
	if ((X >= LCD_WIDTH) || (Y >= LCD_HEIGHT))
		return;

	if ((X + Width - 1) >= LCD_WIDTH)
		Width = LCD_WIDTH - X;
	if ((Y + Height - 1) >= LCD_HEIGHT)
		Height = LCD_HEIGHT - Y;

	ILI9341_Set_Address(X, Y, X + Width - 1, Y + Height - 1);
	ILI9341_Draw_Colour_Burst(Colour, Height * Width);
}

//>>>>>>>>>>ПРОВЕДИТЕ ЛИНИЮ ОТ ТОЧКИ X, Y до ТОЧКИ X+ Ширина, Y
void ILI9341_Draw_Horizontal_Line(uint16_t X, uint16_t Y, uint16_t Width, uint16_t Colour)
{
	if ((X >= LCD_WIDTH) || (Y >= LCD_HEIGHT))
		return;
	if ((X + Width - 1) >= LCD_WIDTH)
		Width = LCD_WIDTH - X;

	ILI9341_Set_Address(X, Y, X + Width - 1, Y);
	ILI9341_Draw_Colour_Burst(Colour, Width);
}

//>>>>>>>>>>ПРОВЕДИТЕ ЛИНИЮ ОТ ТОЧКИ X, Y до ТОЧКИ X, Y+ Высота
void ILI9341_Draw_Vertical_Line(uint16_t X, uint16_t Y, uint16_t Height, uint16_t Colour)
{
	if ((X >= LCD_WIDTH) || (Y >= LCD_HEIGHT))
		return;
	if ((Y + Height - 1) >= LCD_HEIGHT)
		Height = LCD_HEIGHT - Y;

	ILI9341_Set_Address(X, Y, X, Y + Height - 1);
	ILI9341_Draw_Colour_Burst(Colour, Height);
}

//>>>>>>>>>>Нарисуйте полый круг в точках X,Y с заданным радиусом и цветом
void ILI9341_Draw_Hollow_Circle(uint16_t X, uint16_t Y, uint16_t Radius, uint16_t Colour)
{
	int x = Radius - 1;
	int y = 0;
	int dx = 1, dy = 1, err = dx - (Radius << 1);

	while (x >= y)
	{
		ILI9341_Draw_Pixel(X + x, Y + y, Colour);
		ILI9341_Draw_Pixel(X + y, Y + x, Colour);
		ILI9341_Draw_Pixel(X - y, Y + x, Colour);
		ILI9341_Draw_Pixel(X - x, Y + y, Colour);
		ILI9341_Draw_Pixel(X - x, Y - y, Colour);
		ILI9341_Draw_Pixel(X - y, Y - x, Colour);
		ILI9341_Draw_Pixel(X + y, Y - x, Colour);
		ILI9341_Draw_Pixel(X + x, Y - y, Colour);

		if (err <= 0)
		{
			y++;
			err += dy;
			dy += 2;
		}
		if (err > 0)
		{
			x--;
			dx += 2;
			err += (-Radius << 1) + dx;
		}
	}
}

//>>>>>>>>>>Нарисуйте закрашенный круг в точках X,Y с заданным радиусом и цветом
void ILI9341_Draw_Filled_Circle(uint16_t X, uint16_t Y, uint16_t Radius, uint16_t Colour)
{
	int x = Radius, y = 0;
	int xChange = 1 - (Radius << 1), yChange = 0, radiusError = 0;

	while (x >= y)
	{
		for (int i = X - x; i <= X + x; i++)
		{
			ILI9341_Draw_Pixel(i, Y + y, Colour);
			ILI9341_Draw_Pixel(i, Y - y, Colour);
		}
		for (int i = X - y; i <= X + y; i++)
		{
			ILI9341_Draw_Pixel(i, Y + x, Colour);
			ILI9341_Draw_Pixel(i, Y - x, Colour);
		}

		y++;
		radiusError += yChange;
		yChange += 2;
		if (((radiusError << 1) + xChange) > 0)
		{
			x--;
			radiusError += xChange;
			xChange += 2;
		}
	}
}

//>>>>>>>>>>Нарисуйте полый прямоугольник между позициями X0,Y0 и X1,Y1 заданным цветом
void ILI9341_Draw_Hollow_Rectangle_Coord(uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1, uint16_t Colour)
{
	uint16_t x_len, y_len;

	if (X1 >= X0)
		x_len = X1 - X0;
	else
	{
		x_len = X0 - X1;
		uint16_t t = X0;
		X0 = X1;
		X1 = t;
	}
	if (Y1 >= Y0)
		y_len = Y1 - Y0;
	else
	{
		y_len = Y0 - Y1;
		uint16_t t = Y0;
		Y0 = Y1;
		Y1 = t;
	}

	ILI9341_Draw_Horizontal_Line(X0, Y0, x_len, Colour);
	ILI9341_Draw_Horizontal_Line(X0, Y1, x_len, Colour);
	ILI9341_Draw_Vertical_Line(X0, Y0, y_len, Colour);
	ILI9341_Draw_Vertical_Line(X1, Y0, y_len, Colour);

	if ((x_len > 0) || (y_len > 0))
		ILI9341_Draw_Pixel(X1, Y1, Colour);
}

//>>>>>>>>>>Нарисуйте заполненный прямоугольник между позициями X0,Y0 и X1,Y1 заданным цветом
void ILI9341_Draw_Filled_Rectangle_Coord(uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1, uint16_t Colour)
{
	uint16_t x_len, y_len;

	if (X1 >= X0)
		x_len = X1 - X0;
	else
	{
		x_len = X0 - X1;
		uint16_t t = X0;
		X0 = X1;
		X1 = t;
	}
	if (Y1 >= Y0)
		y_len = Y1 - Y0;
	else
	{
		y_len = Y0 - Y1;
		uint16_t t = Y0;
		Y0 = Y1;
		Y1 = t;
	}

	ILI9341_Draw_Rectangle(X0, Y0, x_len, y_len, Colour);
}

//////////////////////////////////////////////////////////////////////////////////////////////
void ILI9341_Random_line(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
	int16_t steep = abs(y2 - y1) > abs(x2 - x1);
	if (steep)
	{
		swap(x1, y1);
		swap(x2, y2);
	}
	if (x1 > x2)
	{
		swap(x1, x2);
		swap(y1, y2);
	}

	int16_t dx = x2 - x1, dy = abs(y2 - y1), err = dx / 2, ystep = (y1 < y2) ? 1 : -1;

	for (; x1 <= x2; x1++)
	{
		if (steep)
			ILI9341_Draw_Pixel(y1, x1, color);
		else
			ILI9341_Draw_Pixel(x1, y1, color);
		err -= dy;
		if (err < 0)
		{
			y1 += ystep;
			err += dx;
		}
	}
}

/////////////////////////////////////// Картинка из массива ///////////////////////////////////////////////////
void ILI9341_Draw_Image(const char *image_array, uint16_t x_coordinat, uint16_t y_coordinat, uint16_t img_width, uint16_t img_height,
		uint32_t s_img)
{
	ILI9341_Set_Address(x_coordinat, y_coordinat, img_width + x_coordinat - 1, img_height + y_coordinat - 1);

	DISP_DC_DATA;
	DISP_CS_SELECT;

	for (uint32_t i = 0; i < s_img; i++)
	{
		DISP_SPI->DR = image_array[i];
		while (!(DISP_SPI->SR & SPI_SR_TXE))
			;
	}

	while (!(DISP_SPI->SR & SPI_SR_TXE))
		;
	while (DISP_SPI->SR & SPI_SR_BSY)
		;
	DISP_CS_UNSELECT;
}

// Вывод символа
static void ILI9341_WriteChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor)
{
	uint32_t i, b, j;

	ILI9341_Set_Address(x, y, x + font.width - 1, y + font.height - 1);

	for (i = 0; i < font.height; i++)
	{
		b = font.data[(ch - 32) * font.height + i];

		for (j = 0; j < font.width; j++)
		{
			if ((b << j) & 0x8000)
			{
				ILI9341_Write_Data(color >> 8);
				ILI9341_Write_Data(color & 0xFF);
			}
			else
			{
				ILI9341_Write_Data(bgcolor >> 8);
				ILI9341_Write_Data(bgcolor & 0xFF);
			}
		}
	}
}

//>>>>>>>>>>Вывод строки
void ILI9341_WriteString(uint16_t x, uint16_t y, const char *str, FontDef font, uint16_t color, uint16_t bgcolor)
{
	while (*str)
	{
		if (x + font.width >= ILI9341_SCREEN_WIDTH)
		{
			x = 0;
			y += font.height;

			if (y + font.height >= ILI9341_SCREEN_HEIGHT)
				break;

			if (*str == ' ')
			{
				str++;
				continue;
			}
		}

		ILI9341_WriteChar(x, y, *str, font, color, bgcolor);
		x += font.width;
		str++;
	}
}


