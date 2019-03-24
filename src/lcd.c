#include <avr/io.h>
#include "io.c"
#include "eeprom.c"


unsigned char highscore = 0;
unsigned char score = 0;
unsigned char previous = 0;
unsigned char check = 0;


void LCD_CreateOmega(unsigned char index)
{
	unsigned char Omega[8] = // ~
	{
		  0x00,
		  0x0E,
		  0x11,
		  0x11,
		  0x11,
		  0x0A,
		  0x1B,
		  0x1B
	};
	
	LCD_CreateCustom(index, Omega);
}


void WriteScore(unsigned char score)
{
	unsigned char hundreds = 0;
	unsigned char tens = 0;
	unsigned char ones = 0;
	
	while(score >= 100)
	{
		score-=100;
		hundreds++;
	}
	while(score >= 10)
	{
		score-=10;
		tens++;
	}
	while(score > 0)
	{
		score--;
		ones++;
	}
	if(hundreds > 0)
	{
		LCD_WriteData(('0' + hundreds));
		LCD_WriteData(('0' + tens));
		LCD_WriteData(('0' + ones));
	}
	else if(tens > 0)
	{
		LCD_WriteData((' '));
		LCD_WriteData(('0' + tens));
		LCD_WriteData(('0' + ones));
	}
	else
	{
		LCD_WriteData((' '));
		LCD_WriteData((' '));
		LCD_WriteData(('0' + ones));
	}
}

enum States{start,wait, display,gameover} state;

void tick(){
	switch(state){
		case start:
			state = wait;
			break;
		case wait:
			if(highscore > previous){
				state = display;
			}else{
				state = wait;
			}
			break;
		case display:
			previous = highscore;	
			//Only update LCD when highscore is greater than previous saved highscore
			//if( highscore <= EEPROM_Read(0x00)){
				//LCD_WriteData(('0' + EEPROM_Read(0x00))); //Read from Saved High score address
			//}else{
				check = 1;
				LCD_Cursor(25);
				LCD_WriteData(('0' + highscore));
			//}
			state = wait;
			break;
		case gameover:
			//if(check = 1){
				//EEPROM_Write(0x00,highscore);	
			//}
			state = wait;
			break;
		default:
			state = wait;
			break;
	}
	switch(state){
		case start:
			break;
		case wait:
			break;
		case display:
			break;
		case gameover:
			break;
		default:
			break;
	}
}

int main(void)
{
	DDRB = 0xFF; PORTB = 0x00; // LCD data lines
	DDRA = 0xFF; PORTA = 0x00; // LCD control lines
	DDRC = 0x00; PORTC = 0xFF; //Input from Highscore from other Microcontroller

	
	  
	  
	//Read if what is saved in EEPROM is a previous save, if not set it to 0
	if(EEPROM_Read(0x00) > 100){ //starting value
		EEPROM_Write(0x00,0);
	}
	
	highscore = EEPROM_Read(0x00);
	// Initializes the LCD display
	 LCD_init();
	 
	 // Starting at position 1 on the LCD screen, writes Hello World
	LCD_ClearScreen();
	
	
	LCD_CreateOmega(0);
	LCD_Cursor(1);
	LCD_WriteData(0);
	
	LCD_Cursor(2);
	LCD_WriteData('H');
	LCD_WriteData('i');
	LCD_WriteData('g');
	LCD_WriteData('h');
	LCD_WriteData(' ');
	LCD_WriteData('S');
	LCD_WriteData('c');
	LCD_WriteData('o');
	LCD_WriteData('r');
	LCD_WriteData('e');
	LCD_WriteData(':');
	LCD_WriteData(' ');
	WriteScore(highscore);
	
	LCD_Cursor(17);
	LCD_WriteData('S');
	LCD_WriteData('c');
	LCD_WriteData('o');
	LCD_WriteData('r');
	LCD_WriteData('e');
	LCD_WriteData(':');
	LCD_WriteData(' ');
	WriteScore(score);
	
	
	while(1) {
		score = PINC;
		if(score > previous){
			previous = score;
			LCD_Cursor(24);
			WriteScore(score);
			if(score > highscore){
				EEPROM_Write(0x00,score);
			}
		}else{
			
		}
		
	}
}
