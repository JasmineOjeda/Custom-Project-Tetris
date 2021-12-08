#ifndef __NOKIA5110_H__
#define __NOKIA5110_H__

#include <avr/pgmspace.h>
#include <stdint.h>

/*
 * LCD's port
 */
#define PORT_LCD PORTD
#define DDR_LCD DDRD

#define LCD_SCE PD5
#define LCD_RST PD4
#define LCD_DC PD3
#define LCD_DIN PD2
#define LCD_CLK PD1

#define LCD_CONTRAST 0x40

void nokia_lcd_init();
void nokia_lcd_clear();
void nokia_lcd_power(uint8_t on);
void nokia_lcd_set_pixel(uint8_t x, uint8_t y, uint8_t value);
void nokia_lcd_write_char(char code, uint8_t scale);
void nokia_lcd_write_string(const char *str, uint8_t scale);
void nokia_lcd_set_cursor(uint8_t x, uint8_t y);
void nokia_lcd_display();

#endif
