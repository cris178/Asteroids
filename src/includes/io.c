#define F_CPU 8000000UL // Clock Definition 8MHz

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>
#include "io.h"

#define SET_BIT(p,i) ((p) |= (1 << (i)))
#define CLR_BIT(p,i) ((p) &= ~(1 << (i)))
#define GET_BIT(p,i) ((p) & (1 << (i)))


/*-------------------------------------------------------------------------*/

#define DATA_BUS PORTB		// port connected to pins 7-14 of LCD display
#define CONTROL_BUS PORTA	// port connected to pins 4 and 6 of LCD disp.
#define RS 0			// pin number of uC connected to pin 4 of LCD disp.
#define E 1	 // pin number of uC connected to pin 6 of LCD disp.

/*-------------------------------------------------------------------------*/


void LCD_ClearScreen(void) {
	LCD_WriteCommand(0x01);
}

void LCD_init(void) {
	_delay_ms(20);
	LCD_WriteCommand(0x33); // 4-bit initialization process
	LCD_WriteCommand(0x32); // 4-bit initialization process
	LCD_WriteCommand(0x28); // Function Set: 4-bit data, 2 lines of LCD, 5x8 font
	LCD_WriteCommand(0x0C); // Display Set: Display on, Cursor off, Blinking cursor off
	LCD_WriteCommand(0x06); // Entry Mode: Increment DDRAM by 1 when a character is written to or read from DDRAM
	LCD_WriteCommand(0x01); // Clear display and set DDRAM address to 0
	_delay_ms(2);
}

void LCD_WriteCommand (unsigned char Command) {
	CLR_BIT(CONTROL_BUS,RS);
	DATA_BUS = (DATA_BUS & 0x0F) | (Command & 0xF0);
	SET_BIT(CONTROL_BUS,E);
	_delay_us(1);
	CLR_BIT(CONTROL_BUS,E);
	_delay_us(200);
	
	CLR_BIT(CONTROL_BUS,RS);
	DATA_BUS = (DATA_BUS & 0x0F) | ((Command << 4) & 0xF0);
	SET_BIT(CONTROL_BUS,E);
	_delay_us(1);
	CLR_BIT(CONTROL_BUS,E);
	_delay_ms(2); // ClearScreen requires 1.52ms to execute
}

void LCD_WriteData(unsigned char Data) {
	SET_BIT(CONTROL_BUS,RS);
	DATA_BUS = (DATA_BUS & 0x0F) | (Data & 0xF0);
	SET_BIT(CONTROL_BUS,E);
	_delay_us(1);
	CLR_BIT(CONTROL_BUS,E);
	_delay_us(200);
	
	SET_BIT(CONTROL_BUS,RS);
	DATA_BUS = (DATA_BUS & 0x0F) | ((Data << 4) & 0xF0);
	SET_BIT(CONTROL_BUS,E);
	_delay_us(1);
	CLR_BIT(CONTROL_BUS,E);
	_delay_ms(1);
}

void LCD_CreateCustom(unsigned char index, const unsigned char Data[8]) {
	if(index < 8)
	{
		LCD_WriteCommand(0x40 + (index * 8));
		for(unsigned char i = 0; i < 8; i++)
		{
			LCD_WriteData(Data[i]);
		}
		_delay_us(1);
		LCD_WriteCommand(0x80);
		_delay_ms(2);
	}
}

void LCD_Cursor(unsigned char column) {
	if ( column < 17 )
	{
		// 16x1 LCD: column < 9
		// 16x2 LCD: column < 17
		LCD_WriteCommand(0x80 + column - 1);
	}
	else
	{
		LCD_WriteCommand(0xB8 + column - 9);
		// 16x1 LCD: column - 1
		// 16x2 LCD: column - 9
	}
}

void LCD_DisplayString( unsigned char column, const unsigned char* string) {
	LCD_ClearScreen();
	unsigned char c = column;
	while(*string)
	{
		LCD_Cursor(c++);
		LCD_WriteData(*string++);
	}
}