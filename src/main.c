#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
//#include <keypad.h>
#include <timer.h>
//#include "io.c"
#include "bit.h"

//Build a type for the Tasks
typedef struct _task
{
	int state;				   // Task's current state
	unsigned long period;	  // Task period
	unsigned long elapsedTime; // Time elapsed since last task tick
	int (*TickFct)(int);	   // Task tick function
} task;



//------------Global Variables
//Missle
unsigned char missleLeft = 0;
unsigned char missleRight = 0;
unsigned char finishFire = 1;
unsigned char missleCol = 0;
unsigned char missleRow = 0;
unsigned char button = 0;


//score
unsigned char counting = 0;
unsigned char gameOver = 0;
unsigned char placeUP = 3;
unsigned char placeDown = 3;

unsigned char score = 0;

//Asteroids Fired-Shot
unsigned char ran = 0;
unsigned char fired = 0;
unsigned char shotR = 0; //Row
unsigned char shotC = 0; //Column

unsigned char firedL = 0;
unsigned char shotRL = 0; //Row
unsigned char shotCL = 0; //Column



//Matrix
unsigned char shipUPDown = 4; //Increase my multiples of 2 to move up down.
unsigned char shipRIGHTDOWN = 0xF7;

//analogue 
unsigned char up = 0;
unsigned char down = 0;
unsigned char left = 0;
unsigned char right = 0;
unsigned short holder = 0;
unsigned short u = 0;
unsigned short d = 0;
unsigned short r = 0;
unsigned short l = 0;
unsigned char shiftLeft = 0;;

//analog to digital function
void A2D_init()
{
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);

	// ADEN: Enables analog-to-digital conversion
	// ADSC: Starts analog-to-digital conversion
	// ADATE: Enables auto-triggering, allowing for constant
	// analog to digital conversions.
}

void Set_A2D_Pin(unsigned char pinNum) {
	ADMUX = (pinNum <= 0x07) ? pinNum : ADMUX;
	// Allow channel to stabilize
	static unsigned char i = 0;
	for ( i=0; i<15; i++ ) { asm("nop"); }
}

//void TaskManage()
//{
	
//}



//State Machines State List
enum AnalogueStickStates{StartA,unMoved,UP,UPHOLD,LEFTHOLD,RIGHTHOLD,RIGHT,DOWNHOLD,DOWN,LEFT,Held} Astate = -1;
enum ButtonStates{StartB,Wait,Pressed,PressedHeld} Bstate = -1;
enum DodgePoints{StartC,Count,Finished} Cstate = -1;
enum asteroid{StartD, WaitD, Fire} Dstate = -1;
enum asteroidL{StartE, WaitE, FireL} Estate = -1;



//Start of State Machines
int TickButton(int Bstate)
{
		static unsigned int i = 0;
		switch (Bstate){
		case -1:
			Bstate = Wait;
			break;
		case Wait:
			if(gameOver){
				Bstate = Wait;
				missleCol = 0;
				missleRow = 0;
				break;
			}
			Bstate = Wait;
			/*if(button == 0x80){
				i = 0;
				missleRow = shipUPDown;
				Bstate = Pressed;
			}else{
				Bstate = Wait;
			}*/
			break;
		case Pressed:
			/*if(button == 0x80){
				Bstate = PressedHeld;
			}else{
				Bstate = Wait;
			}
			break;
		case PressedHeld:
			if(button == 0x80){
				PORTB = missleRow;
				PORTC = 0x00;	
				Bstate = PressedHeld;
				button = 0;
			}else{
				Bstate = Wait;
			}*/
			break;
		default:
			Bstate = Wait;
			break;
	}
	switch (Bstate){
		case -1:
			break;
		case Wait:
			break;
		case Pressed:
			break;
		default:
			break;
	}
	return Bstate;
}

int TickAsteroid(int Dstate)
{
	unsigned char buttons = 0;
	static unsigned char powers[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
	switch (Dstate){
		case -1:
			Dstate = WaitD;
			break;
		case WaitD:
		if(gameOver){
			Dstate = WaitD;
			fired = 0;
			break;
		}
			if(fired == 0){
				Dstate = Fire;
			}else{//Something Fired
				if(shotC == 0x80){ //Finished Animation
					Dstate = Fire;
				}else{//Move animation
					shotC = shotC << 1;
					PORTC = (~shotC);
					PORTB = (shotR);
					Dstate = WaitD;
					if(shotC == shipRIGHTDOWN && shotR == shipUPDown){
						gameOver = 1;
						Dstate = WaitD;
					}
				}
			}
			break;
		case Fire:
			shotC = 0x01;
			ran = (rand() %8);
			shotR = powers[ran];
			PORTC = (~shotC);
			PORTB = shotR;
			fired = 1;
			Dstate = WaitD;
			break;
		default:
			Dstate = WaitD;
			break;
	}
	switch (Dstate){
		case -1:
			break;
		case WaitD:
			break;
		case Fire:
			break;
		default:
			break;
	}
	return Dstate;
}


int TickAsteroidL(int Estate)
{
	static unsigned char powers[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
	switch (Estate){
		case -1:
		Estate = WaitE;
		break;
		case WaitE:
		if(gameOver){
			Estate = WaitE;
			firedL = 0;
			break;
		}
		if(firedL == 0){
			Estate = FireL;
		}else{//Something Fired
			if(shotCL == 0x01){ //Finished Animation
				Estate = FireL;
			}else{//Move animation
				shotCL = shotCL >> 1;
				PORTC = (~shotCL);
				PORTB = (shotRL);
				Estate = WaitE;
				
				//Check if collision
				if(shotCL == shipRIGHTDOWN && shotRL == shipUPDown){
					gameOver = 1;
					Estate = WaitE;
				}
			}
		}
		break;
		case FireL:
			shotCL = 0x80;
			ran = (rand() %8);
			shotRL = powers[ran];
			PORTC = (~shotCL);
			PORTB = shotRL;
			firedL = 1;
			Estate = WaitE;
			break;
		default:
			Estate = WaitE;
			break;
	}
	switch (Estate){
		case -1:
			break;
		case WaitE:
			break;
		case FireL:
			break;
		default:
			break;
	}
	return Estate;
}


int DodgePoints(int Cstate){
	
	switch (Cstate){
		case -1:
			Cstate = Count;
			break;
		case Count:
			if(gameOver){
				Cstate = Finished;
				break;
			}else if(counting < 20){
				Cstate = Count;
			}else{
				gameOver = 1;
				Cstate = Finished;
			}
			break;
		case Finished:
			if(gameOver){
				Cstate = Finished;
				score = 0;
				PORTD = score;
				PORTB = 0xFF;
				PORTC = 0x00;
				//if(button){	//Reset by hitting button
					//Cstate = Count;
					//gameOver = 0;
				//}
			}else{
				counting = 0;
				Cstate = Count;
			}
			break;
		default:
			Cstate = Wait;
			break;
	}
	switch (Cstate){
		case -1:
			break;
		case Count:
			counting++;
			break;
		case Finished:
			break;
		default:
			break;
	}
	return Cstate;
}

int TickAnalogueStick(int Astate)
{

	//Holds 10bit value, won't be erased after each function call thanks to static
	//Original Voltages with no LCD are 10v&& 15 original
	//Voltages with LCD are
	//UP (holder < 520 && holder > 500)
	static unsigned int i = 0;
	//holder = PINA;
	switch (Astate)
	{
		case -1:
			Astate = unMoved;
			break;
		case unMoved:
			if(gameOver){
				Astate = unMoved;
				break;
			}
			PORTC = (~shipRIGHTDOWN); //column to delete
			PORTB = shipUPDown;
			missleRight = 0;
			missleLeft = 0;
			holder = 0;
			right = 0;
			left = 0;
			up = 0;
			down = 0;
			Set_A2D_Pin(0);
			holder = ADC;//up 
			u = holder;
			if(holder < 540 && holder >500){ //recognizes up and up right in 540 - 520 range
				//PORTB = 0x02;
				up = 1;
			}else{
				up = 0;
			}
			
			holder = 0;
			Set_A2D_Pin(1);
			holder = ADC;//down
			d = holder;
			if(holder < 540 && holder >500){
				down = 1;
				//PORTB = 1;
			}else{
				down = 0;
			}
			holder = 0;
			Set_A2D_Pin(2);
			holder = ADC;//right
			r = holder;
			if(holder < 540 && holder >500){
				right = 1;
			}else{
				right = 0;
			}
			holder = 0;
			Set_A2D_Pin(3);
			holder = ADC;//left
			l = holder;
			if(holder < 540 && holder >500){
				left = 1;
			}else{
				left = 0;
			}
			if(up == 0 & down == 0 & left == 0 & right == 0){
				Astate = unMoved;
			}else if(up + down + right + left > 1){
				Astate = unMoved;
			}else if(u >540 && l > 540 && d > 540 && r > 540){
				Astate = unMoved;
			}else if(up == 1 && down ==0 && right == 0 && left == 0){
				Astate = UP;
			}else if(up == 0 && down ==1 && right == 0 && left == 0){
				Astate = DOWN;
			}else if(up == 0 && down == 0 && right == 1 && left == 0){
				Astate = RIGHT;
			}else if(up == 0 && down == 0 && right == 0 && left == 1){
				Astate = LEFT;
			}else{
				Astate = unMoved;
			}
			break;
		case UP:
			holder = 0;
			Set_A2D_Pin(0);
			holder = ADC;//up
			if(holder < 900 && holder > 30){
				Astate = UPHOLD;
				//PORTB = 0x04;
			}else{
				Astate = unMoved;	
			}
			break;
		case UPHOLD:
			if(gameOver){
				Astate = unMoved;
				break;
			}
			holder = 0;
			Set_A2D_Pin(0);
			holder = ADC;//up
			if(holder < 40 && holder > 30){
				
				Astate = UPHOLD;
				//PORTB = 0x08;
				if(i == 150){
					if(shipUPDown < 127){
						shipUPDown = shipUPDown * 2;
					}
					i = 0;	
				}else{
					i++;
				}
				PORTC = (~shipRIGHTDOWN); //column to delete
				PORTB = shipUPDown;
				
				
			}else{
				Astate = unMoved;
			}
			//scoring check
			if(placeUP != 1 && shipUPDown == 128){
				placeUP = 1;
				placeDown = 0;
				score = score + 1;
			}
			break;
		case RIGHT:
			holder = 0;
			Set_A2D_Pin(2);
			holder = ADC;//right
			if(holder < 900 && holder > 30){
				Astate = RIGHTHOLD;
			}else{
				Astate = unMoved;
			}
			break;
		case RIGHTHOLD:
			if(gameOver){
				Astate = unMoved;
				break;
			}
			holder = 0;
			Set_A2D_Pin(2);
			holder = ADC;//right
			if(holder < 40 && holder > 30){
				Astate = RIGHTHOLD;
				if(i == 150){
					if(shipRIGHTDOWN != 0x01){
						shipRIGHTDOWN = (shipRIGHTDOWN >> 1);
						PORTC = (~shipRIGHTDOWN);
					}
					i = 0;
					}else{
					i++;
				}
				PORTC = (~shipRIGHTDOWN); //column to delete
				PORTB = shipUPDown;
				}else{
				Astate = unMoved;
			}
			break;
		case DOWN:
			holder = 0;
			Set_A2D_Pin(1);
			holder = ADC;//up
			if(holder < 900 && holder > 30){
				Astate = DOWNHOLD;
			}else{
				Astate = unMoved;
			}
			break;
		case DOWNHOLD:
			if(gameOver){
				Astate = unMoved;
				break;
			}
			holder = 0;
			Set_A2D_Pin(1);
			holder = ADC;//down
			if(holder < 40 && holder > 30){
				Astate = DOWNHOLD;
				if(i == 150){
					if(shipUPDown > 1){
						shipUPDown = shipUPDown / 2;
					}
					i = 0;
				}else{
					i++;
				}
				PORTC = (~shipRIGHTDOWN); //column to delete
				PORTB = shipUPDown;
			}else{
				Astate = unMoved;
			}
			if(placeDown != 1 && shipUPDown == 1){
				placeDown = 1;
				placeUP = 0;
				score = score + 1;
			}
			break;
		case LEFT:
			holder = 0;
			Set_A2D_Pin(3);
			holder = ADC;//up
			if(holder < 900 && holder > 30){
				Astate = LEFTHOLD;
				}else{
				Astate = unMoved;
			}
			break;
		case LEFTHOLD:
			if(gameOver){
				Astate = unMoved;
				break;
			}
			holder = 0;
			Set_A2D_Pin(3);
			holder = ADC;//up
			u = holder;
			if(holder < 40 && holder > 30){
				Astate = LEFTHOLD;
				if(i == 150){
					if(shipRIGHTDOWN != 0x80){
						shipRIGHTDOWN = (shipRIGHTDOWN << 1);
						PORTC = (~shipRIGHTDOWN);
					}
					i = 0;
					}else{
					i++;
				}
				PORTC = (~shipRIGHTDOWN); //column to delete
				PORTB = shipUPDown;
				}else{
				Astate = unMoved;
			}
		break;
		default:
			Astate = unMoved;
			break;
	}
		switch (Astate){
			case -1:
				break;
			case unMoved:
				if(!gameOver){
					PORTD = score;
				}
				break;
			case UP:
				break;
			case UPHOLD:
				break;
			case RIGHT:
				break;
			case RIGHTHOLD:
				
				break;
			case DOWN:
				break;
			case DOWNHOLD:
				break;
			case LEFT:
				break;
			case LEFTHOLD:
				
				break;
			default:
				break;
		}
		return Astate;
}

	//End of State Machines

int main(void)
{
		A2D_init();
		
		
		DDRA = 0x00; PORTA = 0xFF; //PORTA set to input JOYSTICK ATMEGA REQUIREMENT
		DDRC = 0xFF; PORTC = 0x00; //PORTC set to input BUTTONS
		DDRB = 0xFF; PORTB = 0x00; // PORTB set to output, outputs init 0?
		DDRD = 0xFF; PORTD = 0x00; // PORTD set to output send to LCD Score
		
		shipRIGHTDOWN = 0x08;
		PORTC = (~shipRIGHTDOWN); //column to delete
		PORTB = shipUPDown; //rows to light
			
		
		const unsigned long timerPeriod = 1; //1 ms Global Timer Period
		//const unsigned short NUMTASK = 4;
		
		static task task1, task2,task3,task4,task5;


		task *tasks[] = {&task1,&task2,&task3,&task4,&task5};
		const unsigned short numTasks = sizeof(tasks)/sizeof(task*);


		//Initialize Array with appropriate Tick Functions
		unsigned char i = 0;
		task1.state = -1;
		task1.period = 1;
		task1.elapsedTime = 0;
		task1.TickFct = &TickAnalogueStick;
		//i++;
		task2.state = -1;
		task2.period = 1;
		task2.elapsedTime = 0;
		task2.TickFct = &TickButton;
		//i++
		task3.state = -1;
		task3.period = 1000;
		task3.elapsedTime = 0;
		task3.TickFct = &DodgePoints;
		//i++
		task4.state = -1;
		task4.period = 100;
		task4.elapsedTime = 0;
		task4.TickFct = &TickAsteroid;
		//i++
		task5.state = -1;
		task5.period = 150;
		task5.elapsedTime = 0;
		task5.TickFct = &TickAsteroidL;
		//i++
		

		TimerSet(timerPeriod);
		TimerOn();
	
		
		while (1)
		{
			//button = ~PIND & 0x80;
			
			for (unsigned char i = 0; i < numTasks; ++i)
			{ // Heart of the scheduler code
				//Runs through each task, checks when they have to run, then runs them.
				if (tasks[i]->elapsedTime >= tasks[i]->period)
				{ // Ready
					tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
					tasks[i]->elapsedTime = 0;
				}
				tasks[i]->elapsedTime += timerPeriod;
			}
			
			while (!TimerFlag);
			TimerFlag = 0;
		}
}
