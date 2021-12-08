/*
 * Used datasheet given by sparkfun:
 * https://www.sparkfun.com/datasheets/LCD/Monochrome/Nokia5110.pdf?_ga=2.201112493.2012801648.1638834529-508216462.1635716272
 *
 * Used/analyzed parts of sparkfun's GitHub code to create driver
 * Link to header file that was compared with:
 * https://github.com/sparkfun/GraphicLCD_Nokia_5110/blob/46f85769969593150e52f02046f70cee568fceeb/Firmware/Nokia-5100-LCD-Example/LCD_Functions.h
 *
 * Used/analyzed parts from this file in order to better understand how to convert
 * the arduino example from sparkfun into code compatible with the atmega1284 microcontroller:
 * https://github.com/LittleBuster/avr-nokia5110/blob/8037ed6ef0b37a2021d064031fc2dc92e0084abd/nokia5110.c
 */

#define F_CPU 1000000UL

#include <avr/pgmspace.h>
#include <avr/io.h>
#include <util/delay.h>
#include "nokia5110_chars.h"
#include "nokia5110.h"

static struct {
    uint8_t screen[504];

    uint8_t cursor_x;
    uint8_t cursor_y;

} nokia_lcd = {
    .cursor_x = 0,
    .cursor_y = 0
};

void write(uint8_t bytes, uint8_t is_data) {
	PORT_LCD &= ~(1 << LCD_SCE);

	if (is_data) {
		PORT_LCD |= (1 << LCD_DC);
	}
	else {
		PORT_LCD &= ~(1 << LCD_DC);
        }

	for (register uint8_t i = 0; i < 8; i++) {
		if ((bytes >> (7-i)) & 0x01) {
			PORT_LCD |= (1 << LCD_DIN);
		}
		else {
			PORT_LCD &= ~(1 << LCD_DIN);
	        }

		PORT_LCD |= (1 << LCD_CLK);
		PORT_LCD &= ~(1 << LCD_CLK);
	}

	PORT_LCD |= (1 << LCD_SCE);
}

void write_cmd(uint8_t cmd) {
	write(cmd, 0);
}

void write_data(uint8_t data) {
	write(data, 1);
}

void nokia_lcd_init() {
	DDR_LCD |= (1 << LCD_SCE);
	DDR_LCD |= (1 << LCD_RST);
	DDR_LCD |= (1 << LCD_DC);
	DDR_LCD |= (1 << LCD_DIN);
	DDR_LCD |= (1 << LCD_CLK);

	PORT_LCD |= (1 << LCD_RST);
	PORT_LCD |= (1 << LCD_SCE);
	_delay_ms(10);
	PORT_LCD &= ~(1 << LCD_RST);
	_delay_ms(70);
	PORT_LCD |= (1 << LCD_SCE);

	PORT_LCD &= ~(1 << LCD_SCE);
	
	write_cmd(0x21);
	write_cmd(0x13);
	write_cmd(0x06);
	write_cmd(0xC2);
	write_cmd(0x20);
	write_cmd(0x09);

	write_cmd(0x80);
	write_cmd(LCD_CONTRAST);
	for (register uint8_t i = 0; i < 504; i++) {
		write_data(0x00);
	}

	write_cmd(0x08);
	write_cmd(0x0C);
}

void nokia_lcd_clear(void)
{
	write_cmd(0x80);
	write_cmd(0x40);
	nokia_lcd.cursor_x = 0;
	nokia_lcd.cursor_y = 0;
	for(register uint8_t i = 0;i < 504; i++) {
		nokia_lcd.screen[i] = 0x00;
	}
}

void nokia_lcd_set_pixel(uint8_t x, uint8_t y, uint8_t value) {
	uint8_t *byte = &nokia_lcd.screen[y/8*84+x];
	if (value) {
		*byte |= (1 << (y % 8));
	}
	else {
		*byte &= ~(1 << (y %8 ));
	}
}

void nokia_lcd_write_char(char code, uint8_t scale)
{
	for (register uint8_t x = 0; x < 5*scale; x++) {
		for (register uint8_t y = 0; y < 7*scale; y++) {
			if (pgm_read_byte(&CHARSET[code-32][x/scale]) & (1 << y/scale)) {
				nokia_lcd_set_pixel(nokia_lcd.cursor_x + x, nokia_lcd.cursor_y + y, 1);
			}
			else {
				nokia_lcd_set_pixel(nokia_lcd.cursor_x + x, nokia_lcd.cursor_y + y, 0);
			}
	        }
	}

	nokia_lcd.cursor_x += 5*scale + 1;
	if (nokia_lcd.cursor_x >= 84) {
		nokia_lcd.cursor_x = 0;
		nokia_lcd.cursor_y += 7*scale + 1;
	}
	if (nokia_lcd.cursor_y >= 48) {
		nokia_lcd.cursor_x = 0;
		nokia_lcd.cursor_y = 0;
	}
}

void nokia_lcd_write_string(const char *str, uint8_t scale)
{
	while(*str) {
		nokia_lcd_write_char(*str++, scale);
	}
}

void nokia_lcd_set_cursor(uint8_t x, uint8_t y)
{
	nokia_lcd.cursor_x = x;
	nokia_lcd.cursor_y = y;
}

void nokia_lcd_display()
{
	write_cmd(0x80);
	write_cmd(0x40);

	for (register uint8_t i = 0; i < 504; i++) {
		write_data(nokia_lcd.screen[i]);
	}
}
