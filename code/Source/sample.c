/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The GLCD application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-11-7
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             Paolo Bernardi
** Modified date:           03/01/2020
** Version:                 v2.0
** Descriptions:            basic program for LCD and Touch Panel teaching
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "LPC17xx.h"
#include "GLCD/GLCD.h" 
#include "timer/timer.h"
#include "RIT/RIT.h"
#include "button_EXINT/button.h"
#include "joystick/joystick.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "time.h"
#include "CAN/CAN.h"


#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif


typedef struct {
    int x, y;
} Point;


typedef struct{
	Point point;
	int g, h, f; // position, distance from source, target and sum of them 
} Node;

void move( void );
void moveBlinky( void );
void upScreen ( void );
int heuristic(int x1, int y1, int x2, int y2);
bool isValid(int x, int y, int matrix[28][24]);
int aStar(Point start, Point target, int matrix[28][24]);
int reconstructFirstMove(Point parent[28][24], Point start, Point target);




volatile uint8_t gameCondition, timer;
volatile int score;
char stringa[10]; 
int matrice[28][24];
int temp;
int tempi[6];
volatile int	life, i, j, dir, pacX, pacY, pills, n, dirBlinky, bliX, bliY, oldCell, blinkyCondition, blinkyTimer;
extern int music_sample;

int gCost[28][24], fCost[28][24];
int8_t evaluated[28][24];
Point parent[28][24];
int dRow[] = {0, 0, -1, 1};
int dCol[] = {1, -1, 0, 0};
Point openList[28*24];		// list of visited points
int openCount = 0;
int dirBli;
int lowestIndex;
int iterazione=0;
Point current;
int newX, newY, tentativeG;
bool inOpenList;


/*int field[28][24] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1},
    {1, 3, 1, 1, 1, 0, 0, 1, 1, 1, 3, 1, 1, 1, 0, 3, 1, 1, 1, 3, 1, 1, 3, 1},
    {1, 3, 1, 0, 3, 3, 3, 3, 3, 1, 3, 0, 0, 1, 0, 3, 3, 1, 3, 3, 0, 1, 3, 1},
    {1, 3, 1, 0, 1, 3, 0, 1, 1, 1, 3, 3, 3, 3, 3, 3, 3, 1, 3, 0, 0, 1, 3, 1},
    {1, 3, 1, 0, 1, 3, 0, 0, 0, 3, 3, 3, 1, 0, 1, 1, 3, 1, 3, 0, 1, 1, 3, 1},
    {1, 3, 3, 3, 3, 3, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 3, 0, 3, 0, 0, 1, 3, 1},
    {1, 3, 1, 0, 0, 3, 1, 1, 0, 3, 0, 0, 1, 0, 1, 0, 0, 1, 3, 0, 0, 1, 0, 1},
    {1, 1, 1, 1, 0, 3, 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 3, 3, 3, 1},
    {1, 3, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 3, 3, 3, 3, 1},
    {1, 3, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 3, 3, 3, 3, 1, 3, 3, 3, 1, 1, 1, 1},
    {1, 3, 1, 1, 1, 3, 3, 1, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 1},
    {1, 0, 1, 0, 0, 3, 0, 1, 1, 3, 3, 0, 5, 3, 3, 3, 1, 0, 1, 3, 1, 0, 3, 1},
    {1, 0, 0, 3, 3, 3, 3, 1, 1, 3, 1, 1, 1, 1, 1, 3, 1, 3, 1, 0, 1, 0, 3, 1},
    {0, 0, 1, 1, 1, 1, 3, 1, 1, 3, 1, 0, 0, 0, 1, 3, 1, 3, 1, 3, 3, 3, 3, 0},
    {1, 0, 0, 0, 3, 0, 0, 1, 3, 3, 1, 1, 1, 1, 1, 3, 1, 3, 3, 3, 0, 1, 1, 1},
    {1, 3, 1, 0, 3, 1, 1, 1, 0, 3, 0, 0, 0, 0, 0, 3, 1, 0, 1, 1, 0, 0, 3, 1},
    {1, 3, 1, 0, 3, 3, 0, 1, 0, 3, 3, 1, 1, 1, 0, 1, 1, 0, 1, 3, 3, 3, 3, 1},
    {1, 3, 1, 0, 3, 3, 1, 1, 1, 0, 3, 1, 3, 3, 3, 3, 1, 3, 1, 3, 1, 1, 3, 1},
    {1, 3, 1, 1, 1, 3, 3, 1, 3, 3, 3, 1, 3, 1, 1, 1, 1, 3, 1, 3, 0, 0, 3, 1},
    {1, 3, 0, 0, 1, 3, 0, 1, 3, 1, 3, 1, 3, 0, 1, 0, 1, 3, 1, 1, 1, 1, 3, 1},
    {1, 3, 1, 0, 1, 3, 0, 1, 3, 1, 3, 1, 3, 0, 1, 0, 1, 3, 3, 3, 3, 3, 3, 1},
    {1, 3, 1, 0, 1, 3, 0, 1, 3, 1, 3, 0, 2, 0, , 0, 1, 0, 3, 1, 1, 1, 3, 1},
		{1, 3, 1, 0, 1, 3, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 3, 1, 3, 1},
		{1, 3, 1, 0, 0, 3, 0, 1, 3, 1, 3, 0, 0, 1, 0, 1, 0, 0, 0, 1, 3, 1, 3, 1},
		{1, 3, 1, 1, 1, 1, 0, 1, 3, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 3, 1, 3, 1},		
		{1, 3, 3, 3, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 3, 3, 3, 3, 3, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};*/

int field[28][24] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1},
    {1, 3, 1, 1, 1, 0, 0, 1, 1, 1, 3, 1, 1, 1, 0, 3, 1, 1, 1, 3, 1, 1, 3, 1},
    {1, 3, 1, 0, 3, 3, 3, 3, 3, 1, 3, 0, 0, 1, 0, 3, 3, 1, 3, 3, 0, 1, 3, 1},
    {1, 3, 1, 0, 1, 3, 0, 1, 1, 1, 3, 3, 3, 3, 3, 3, 3, 1, 3, 0, 0, 1, 3, 1},
    {1, 3, 1, 0, 1, 3, 0, 0, 0, 3, 3, 3, 1, 0, 1, 1, 3, 1, 3, 0, 1, 1, 3, 1},
    {1, 3, 3, 3, 3, 3, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 3, 0, 3, 0, 0, 1, 3, 1},
    {1, 3, 1, 0, 0, 3, 1, 1, 0, 3, 0, 0, 1, 0, 1, 0, 0, 1, 3, 0, 0, 1, 0, 1},
    {1, 1, 1, 1, 0, 3, 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 3, 3, 3, 1},
    {1, 3, 0, 0, 0, 1, 1, 3, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 3, 3, 3, 3, 1},
    {1, 3, 1, 0, 0, 0, 1, 3, 1, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1},
    {1, 3, 1, 1, 1, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 1},
    {1, 0, 1, 0, 0, 3, 0, 0, 1, 3, 3, 0, 5, 3, 3, 3, 0, 0, 1, 3, 1, 0, 3, 1},
    {1, 0, 0, 3, 3, 3, 3, 1, 1, 3, 1, 1, 1, 1, 1, 3, 3, 3, 1, 0, 1, 0, 3, 1},
    {0, 0, 1, 1, 1, 1, 3, 3, 1, 3, 1, 0, 0, 0, 1, 3, 1, 3, 1, 3, 3, 3, 3, 0},
    {1, 0, 0, 0, 3, 0, 0, 3, 3, 3, 1, 1, 1, 1, 1, 3, 1, 3, 3, 3, 0, 1, 1, 1},
    {1, 3, 1, 0, 3, 1, 1, 1, 0, 3, 0, 0, 0, 0, 0, 3, 1, 0, 1, 1, 0, 0, 3, 1},
    {1, 3, 1, 0, 3, 3, 0, 0, 0, 3, 3, 1, 1, 1, 0, 1, 1, 0, 1, 3, 3, 3, 3, 1},
    {1, 3, 1, 0, 3, 3, 1, 1, 1, 0, 3, 1, 3, 3, 3, 3, 3, 3, 1, 3, 1, 1, 3, 1},
    {1, 3, 1, 1, 1, 3, 3, 3, 3, 3, 3, 1, 3, 1, 1, 1, 1, 3, 1, 3, 0, 0, 3, 1},
    {1, 3, 0, 0, 1, 3, 0, 1, 3, 1, 3, 1, 3, 0, 1, 0, 1, 3, 1, 1, 1, 1, 3, 1},
    {1, 3, 1, 0, 1, 3, 0, 1, 3, 1, 3, 1, 3, 0, 1, 0, 1, 3, 3, 3, 3, 3, 3, 1},
    {1, 3, 1, 0, 1, 3, 0, 1, 3, 1, 3, 0, 2, 0, 0, 0, 0, 0, 3, 1, 1, 1, 3, 1},
		{1, 3, 1, 0, 1, 3, 0, 1, 3, 1, 3, 1, 1, 1, 0, 1, 1, 0, 0, 1, 3, 1, 3, 1},
		{1, 3, 1, 0, 0, 3, 0, 1, 3, 1, 3, 0, 0, 1, 0, 1, 0, 0, 0, 1, 3, 1, 3, 1},
		{1, 3, 1, 1, 1, 1, 0, 1, 3, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 3, 1, 3, 1},		
		{1, 3, 3, 3, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 3, 3, 3, 3, 3, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};



int main(void){
	srand(time(NULL));
	for(i=0; i<26;i++){							// copy field in matrice
				for(j=0; j<28;j++){
					matrice[j][i]=field[j][i];			// create a copy of the field
		}
	}
	dir=0;													// direction of pac-man	
	gameCondition = 0;							// pause, resume, victory, game over
	dirBlinky = 0;									// direction of blinky
	blinkyCondition = 0;						// follow mode
	bliX=12;												// Blinky initial position
	bliY=12;
	blinkyTimer=0;
	oldCell=0;
	timer=60;												// final coutdown
	life=1;													// number of lifes
	score=0;												// number of points
	pills=240;												// initial value 240
	pacX=12;												// inital position of pacman in the matrix
	pacY=22;
	n=1; 															// number of 1000 pills to earn next life
	music_sample=0;
	
	
	for (i=0; i<6; i++){
		tempi[i]=rand() % (60-20+1) + 20;    // generate random time for creation of power pills
	}

	for (i=0; i<6; i++){									// sorting the vector
		for (j=0; j<6-i-1;j++){
			if (tempi[j]< tempi[j+1]){
				temp = tempi[j];
				tempi[j] = tempi[j+1];
				tempi[j+1]=temp;
			}
		}
	}
	
  SystemInit(); 												/* System Initialization (i.e., PLL)  */
	init_timer(0, 0xBEBC20, 0x3F940A); 		/* Initialization for  interrupt on match and reset each 0.16 s, mr1 not used */
	
	BUTTON_init();												/* BUTTON Initialization              */
	joystick_init();											/* Joystick Initialization            */
	init_RIT(0x004C4B40);									/* RIT Initialization 50 msec         */
	LCD_Initialization();									/* LCD Initialization 							  */
	CAN_Init();														/* CAN Initialization 							  */
	
	upScreen();														/* update of the screen               */
	
	enable_RIT();													/* RIT enabled												*/	
	
	
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= ~(0x2);	

	LPC_PINCON->PINSEL1 |= (1<<21);				// dac and speaker set up
	LPC_PINCON->PINSEL1 &= ~(1<<20);
	LPC_GPIO0->FIODIR |= (1<<26);
	
	
  while (1)	
  {
		__ASM("wfi");
  }
}


void move( void )
	/* function that move pacman accroding to position, direction and wanted cell*/
{
	if (matrice[pacY][pacX]==2){
		if (pacX==0 & pacY==14 & dir==4){										// teleport to left condition
			matrice[pacY][23]=2;
			matrice[pacY][0]=0;
		
			LCD_DrawCircle(10*23, 10*pacY + 20, 5, Yellow);  	// screen update
			LCD_DrawCircle(10*0, 10*pacY + 20, 5, Black);
			pacX=23;
		}
		else if (pacX==23 & pacY==14 & dir==2){							// teleport to right condition
			matrice[pacY][0]=2;
			matrice[pacY][23]=0;
		
			LCD_DrawCircle(10*0, 10*(pacY) + 20, 5, Yellow);  //screen update
			LCD_DrawCircle(10*23, 10*pacY + 20, 5, Black);
			pacX=0;
		}
		else{																								// if not special case check direction
			switch(dir){
		case(0):																					 	// if 0 not move
			break;
		case(1):																						// if 1 try to move down
			switch(matrice[pacY+1][pacX]){		
				case(0):																				// empty cell
					matrice[pacY+1][pacX]=2;
					matrice[pacY][pacX]=0;
				
					LCD_DrawCircle(10*pacX, 10*(pacY+1) + 20, 5, Yellow);  //screen update
					LCD_DrawCircle(10*pacX, 10*pacY + 20, 5, Black);
				
					pacY++;																				// position upgrade
					break;
				
				case(1):																				// if wall present
					dir=0;																				// stop movement
					break;
				
				case(3):																				// if normal pills
					matrice[pacY+1][pacX]=2;
					matrice[pacY][pacX]=0;
				
					LCD_DrawCircle(10*pacX, 10*(pacY+1) + 20, 5, Yellow);  //screen update
					LCD_DrawCircle(10*pacX, 10*pacY + 20, 5, Black);
				
					pacY++;
				
					score+=10;																		// increment score
					pills--;																			// recude pill counter
					if (score>=n*1000){ 														// check if increment lifes
						life++;
						n++;
						/*sprintf(stringa, "%d", life); 
						GUI_Text(50, 304, (uint8_t *) stringa, White, Black);*/
					}
					/*GUI_Text(100, 1, (uint8_t *) " Score: ", White, Black);			// update screen
					sprintf(stringa, "%d", score); 
					GUI_Text(160, 1, (uint8_t *) stringa, White, Black);*/
					break;
				
				case(4):																			// if power pill
					matrice[pacY+1][pacX]=2;										
					matrice[pacY][pacX]=0;
				
					LCD_DrawCircle(10*pacX, 10*(pacY+1) + 20, 5, Yellow);		// screen update
					LCD_DrawCircle(10*pacX, 10*pacY + 20, 5, Black);
				
					pacY++;
				
					score+=50;																		// increment score 
					blinkyCondition=1;
					blinkyTimer=10;
					pills--;																			// decrement winning condition
					if (score>=n*1000){ 														// condition to increment lifes
						life++;
						n++;
						/*sprintf(stringa, "%d", life); 
						GUI_Text(50, 304, (uint8_t *) stringa, White, Black);*/
					}
					/*GUI_Text(100, 1, (uint8_t *) " Score: ", White, Black);
					sprintf(stringa, "%d", score); 
					GUI_Text(160, 1, (uint8_t *) stringa, White, Black);*/
					break;
				case(5):																				// if ghost
					if (blinkyCondition==0){
						life--;
						pacX=12;																						 // inital position of pacman in the matrix
						pacY=22;
						matrice[pacY][pacX]=2;
						LCD_DrawCircle(10*pacX, 10*(pacY) + 20, 5, Yellow);  //screen update
						if (life==0) {
							gameCondition=2;
							disable_timer(0);
							upScreen();
							enable_timer(0);
							return;}
						}
					else if(blinkyCondition==1){
						matrice[pacY+1][pacX]=2;										
						matrice[pacY][pacX]=0;
						LCD_DrawCircle(10*pacX, 10*(pacY+1) + 20, 5, Yellow);		// screen update
						LCD_DrawCircle(10*pacX, 10*pacY + 20, 5, Black);
						
						score+=100;	
						
						blinkyCondition=2;
						blinkyTimer=3;
						pacY++;
					}
					break;
			}	
			break;
		case(2):
			switch(matrice[pacY][pacX+1]){		//right
							case(0):
								matrice[pacY][pacX+1]=2;			// se vuoto sposto
								matrice[pacY][pacX]=0;
							
								LCD_DrawCircle(10*(pacX+1), 10*pacY + 20, 5, Yellow);  //disegno
								LCD_DrawCircle(10*pacX, 10*pacY + 20, 5, Black);
							
								pacX++;												// nuova posizione
								break;
							
							case(1):
								dir=0;												// se muro fermo
								break;
							
							case(3):
								matrice[pacY][pacX+1]=2;			// se vuoto sposto
								matrice[pacY][pacX]=0;
							
								LCD_DrawCircle(10*(pacX+1), 10*pacY + 20, 5, Yellow);  //disegno
								LCD_DrawCircle(10*pacX, 10*pacY + 20, 5, Black);
							
								pacX++;												// nuova posizione
							
								score+=10;										// se pill incremento pure score
								pills--;
								if (score>=n*1000){ 
									life++;
									n++;
									/*sprintf(stringa, "%d", life); 
									GUI_Text(50, 304, (uint8_t *) stringa, White, Black);*/
								}
								/*GUI_Text(100, 1, (uint8_t *) " Score: ", White, Black); 
								sprintf(stringa, "%d", score); 
								GUI_Text(160, 1, (uint8_t *) stringa, White, Black);*/
								break;
							
							case(4):
								matrice[pacY][pacX+1]=2;			// se vuoto sposto
								matrice[pacY][pacX]=0;
							
								LCD_DrawCircle(10*(pacX+1), 10*pacY + 20, 5, Yellow);  //disegno
								LCD_DrawCircle(10*pacX, 10*pacY + 20, 5, Black);
							
								pacX++;												// nuova posizione
							
								score+=50;
								blinkyCondition=1;
								blinkyTimer=10;
								pills--;
								if (score>=n*1000){ 
									life++;
									n++;
									/*sprintf(stringa, "%d", life); 
									GUI_Text(50, 304, (uint8_t *) stringa, White, Black);*/
								}
								/*GUI_Text(100, 1, (uint8_t *) " Score: ", White, Black);
								sprintf(stringa, "%d", score); 
								GUI_Text(160, 1, (uint8_t *) stringa, White, Black);*/
								break;
							case(5):
								if (blinkyCondition==0){
									life--;
									pacX=12;																						 // inital position of pacman in the matrix
									pacY=22;
									matrice[pacY][pacX]=2;
									LCD_DrawCircle(10*pacX, 10*(pacY) + 20, 5, Yellow);  //screen update
									
									if (life==0) {
										gameCondition=2;
										disable_timer(0);
										upScreen();
										enable_timer(0);
										return;}
									}
								else if(blinkyCondition==1){
									matrice[pacY][pacX+1]=2;										
									matrice[pacY][pacX]=0;
									LCD_DrawCircle(10*(pacX+1), 10*pacY + 20, 5, Yellow);  //disegno
									LCD_DrawCircle(10*pacX, 10*pacY + 20, 5, Black);
									
									score+=100;	
									
									blinkyCondition=2;
									blinkyTimer=3;
									pacX++;}
								break;
						}	
			break;
		case(3):
			switch(matrice[pacY-1][pacX]){		//up
							case(0):
								matrice[pacY-1][pacX]=2;			// se vuoto sposto
								matrice[pacY][pacX]=0;
							
								LCD_DrawCircle(10*pacX, 10*(pacY-1) + 20, 5, Yellow);  //disegno
								LCD_DrawCircle(10*pacX, 10*pacY + 20, 5, Black);
							
								pacY--;												// nuova posizione
								break;
							
							case(1):
								dir=0;												// se muro fermo
								break;
							
							case(3):
								matrice[pacY-1][pacX]=2;			// sposto
								matrice[pacY][pacX]=0;
							
								LCD_DrawCircle(10*pacX, 10*(pacY-1) + 20, 5, Yellow);  //disegno
								LCD_DrawCircle(10*pacX, 10*pacY + 20, 5, Black);
							
								pacY--;
							
								score+=10;										// se pill incremento pure score
								pills--;
								if (score>=n*1000){ 
									life++;
									n++;
									/*sprintf(stringa, "%d", life); 
									GUI_Text(50, 304, (uint8_t *) stringa, White, Black);*/
								}
								/*GUI_Text(100, 1, (uint8_t *) " Score: ", White, Black); 
								sprintf(stringa, "%d", score); 
								GUI_Text(160, 1, (uint8_t *) stringa, White, Black);*/
								break;
							
							case(4):
								matrice[pacY-1][pacX]=2;			// sposto
								matrice[pacY][pacX]=0;
							
								LCD_DrawCircle(10*pacX, 10*(pacY-1) + 20, 5, Yellow);  //disegno
								LCD_DrawCircle(10*pacX, 10*pacY + 20, 5, Black);
							
								pacY--;
							
								score+=50;
								blinkyCondition=1;
								blinkyTimer=10;
								pills--;
								if (score>=n*1000){ 
									life++;
									n++;
									/*sprintf(stringa, "%d", life); 
									GUI_Text(50, 304, (uint8_t *) stringa, White, Black);*/
								}
								/*GUI_Text(100, 1, (uint8_t *) " Score: ", White, Black);
								sprintf(stringa, "%d", score); 
								GUI_Text(160, 1, (uint8_t *) stringa, White, Black);*/
								break;
								
							case(5):
								if (blinkyCondition==0){
									life--;
									pacX=12;																						 // inital position of pacman in the matrix
									pacY=22;
									matrice[pacY][pacX]=2;
									LCD_DrawCircle(10*pacX, 10*(pacY) + 20, 5, Yellow);  //screen update
									if (life==0) {
										gameCondition=2;
										disable_timer(0);
										upScreen();
										enable_timer(0);
										return;}
									}
								else if(blinkyCondition==1){
									matrice[pacY-1][pacX]=2;										
									matrice[pacY][pacX]=0;
									
									LCD_DrawCircle(10*pacX, 10*(pacY-1) + 20, 5, Yellow);  //disegno
									LCD_DrawCircle(10*pacX, 10*pacY + 20, 5, Black);
									
									score+=100;	
									
									blinkyCondition=2;
									blinkyTimer=3;
									pacY--;}
								break;
						}	
			break;
		case(4):
			switch(matrice[pacY][pacX-1]){		//left
							case(0):
								matrice[pacY][pacX-1]=2;			// se vuoto sposto
								matrice[pacY][pacX]=0;
							
								LCD_DrawCircle(10*(pacX-1), 10*pacY + 20, 5, Yellow);  //disegno
								LCD_DrawCircle(10*pacX, 10*pacY + 20, 5, Black);
							
								pacX--;												// nuova posizione
								break;
							
							case(1):
								dir=0;												// se muro fermo
								break;
							
							case(3):
								matrice[pacY][pacX-1]=2;			// se vuoto sposto
								matrice[pacY][pacX]=0;
							
								LCD_DrawCircle(10*(pacX-1), 10*pacY + 20, 5, Yellow);  //disegno
								LCD_DrawCircle(10*pacX, 10*pacY + 20, 5, Black);
							
								pacX--;												// nuova posizione
							
								score+=10;										// se pill incremento pure score
								pills--;
								if (score>=n*1000){ 
									life++;
									n++;
									/*sprintf(stringa, "%d", life); 
									GUI_Text(50, 304, (uint8_t *) stringa, White, Black);*/
								}
								/*GUI_Text(100, 1, (uint8_t *) " Score: ", White, Black); 
								sprintf(stringa, "%d", score); 
								GUI_Text(160, 1, (uint8_t *) stringa, White, Black);*/
								break;
							
							case(4):
								matrice[pacY][pacX-1]=2;			// se vuoto sposto
								matrice[pacY][pacX]=0;
							
								LCD_DrawCircle(10*(pacX-1), 10*pacY + 20, 5, Yellow);  //disegno
								LCD_DrawCircle(10*pacX, 10*pacY + 20, 5, Black);
							
								pacX--;												// nuova posizione
							
								score+=50;
								blinkyCondition=1;
								blinkyTimer=10;
								pills--;
								if (score>=n*1000){ 
									life++;
									n++;
									/*sprintf(stringa, "%d", life); 
									GUI_Text(50, 304, (uint8_t *) stringa, White, Black);*/
								}
								/*GUI_Text(100, 1, (uint8_t *) " Score: ", White, Black);
								sprintf(stringa, "%d", score); 
								GUI_Text(160, 1, (uint8_t *) stringa, White, Black);*/
								break;
								
							case(5):
								if (blinkyCondition==0){
									life--;
									pacX=12;																						 // inital position of pacman in the matrix
									pacY=22;
									matrice[pacY][pacX]=2;
									LCD_DrawCircle(10*pacX, 10*(pacY) + 20, 5, Yellow);  //screen update
									if (life==0) {
										gameCondition=2;
										disable_timer(0);
										upScreen();
										enable_timer(0);
										return;}
									}
								else if(blinkyCondition==1){
									matrice[pacY][pacX-1]=2;										
									matrice[pacY][pacX]=0;
									LCD_DrawCircle(10*(pacX-1), 10*pacY + 20, 5, Yellow);  //disegno
									LCD_DrawCircle(10*pacX, 10*pacY + 20, 5, Black);
									
									score+=100;	
									blinkyCondition=2;
									blinkyTimer=3;
									pacX--;}
								break;
						}	
			break;
	}}}
	else {													// error message if no pacman found
		gameCondition=5;
		upScreen();
	}
}

void upScreen( void )
	/* function that upgrade screen according to game condition and actual field*/
{	
		switch(gameCondition){
		case(0):																												//pause
			LCD_Clear(Black);
			GUI_Text(0, 140, (uint8_t *) " Press key 1 to start the game ", Yellow, Black);
			break;
		case(1):																												//victory
			LCD_Clear(Black);
			GUI_Text(0, 140, (uint8_t *) " Victory !! ", Green, Black);
			break;
		case(2):																												//game over
			LCD_Clear(Black);
			GUI_Text(0, 140, (uint8_t *) " Game Over :( ", Yellow, Black);
			break;
		case(3):																												// game screen
			LCD_Clear(Black);
			/*GUI_Text(0, 1, (uint8_t *) " Time: ", White, Black);
			sprintf(stringa, "%d", timer);
			GUI_Text(50, 1, (uint8_t *) stringa, White, Black);						//time
		
			GUI_Text(100, 1, (uint8_t *) " Score: ", White, Black);  			//score
			sprintf(stringa, "%d", score); 
			GUI_Text(160, 1, (uint8_t *) stringa, White, Black);
			LCD_DrawLine(0, 19, 260, 19, White);
		
			GUI_Text(0, 304, (uint8_t *) " Life: ", White, Black);  			// n. life
			sprintf(stringa, "%d", life); 
			GUI_Text(50, 304, (uint8_t *) stringa, White, Black);*/
			LCD_DrawLine(0, 300, 260, 300, White);
		
			for(i=0; i<26;i++){																						// field
				for(j=0; j<28;j++){
					switch(matrice[j][i]){
						case 1:
								LCD_DrawSquare(10*i,10*j+20,10,White);
							break;
						case 2:
								LCD_DrawCircle(10*i,10*j+20, 5, Yellow);
							break;
						case 3:
								LCD_DrawCircle(10*i,10*j+20, 2, White);
							break;
						case 4:
								LCD_DrawCircle(10*i,10*j+20, 3, Cyan);
							break;
					
						case 5:
								LCD_DrawCircle(10*i,10*j+20, 5, Red);
							break;
				}
				}
			}
		
			
		
		break;
		default:																												//error message otw
			LCD_Clear(Black);
			GUI_Text(0, 140, (uint8_t *) " ERROR ", Red, Black);
			
	}

}



int heuristic(int x1, int y1, int x2, int y2){
	int dis;
	dis = abs(x1-x2)+abs(y1-y2); 		// distance to target
	return dis;
}



bool isValid(int x, int y, int matrix[28][24]){
	bool inside, noWall;
	inside = x>=1 & x<=22 & y>=1 & y<=26;		// check if inside matrix and no border
	noWall = matrice[y][x]!=1;							// if there isn't a wall
	return inside&noWall;
}

int aStar(Point start, Point target, int matrix[28][24]){
		lowestIndex=0;
		openCount=0;
		
		for (i=0; i<24; i++){   // initialization to inf
			for (j=0; j<28; j++){
				gCost[j][i]=99999;
				fCost[j][i]=99999;
				evaluated[j][i]=0;
			}
		}
		
		gCost[start.y][start.x] = 0;
		fCost[start.y][start.x] = heuristic(start.x, start.y, target.x, target.y);   // first point values
		
		openList[openCount++]= start;
		
		while(openCount > 0){
			
			for(i=1; i<openCount; i++){		// look for visited point that has lower fcost
				Point p1 = openList[i];
				Point p2 = openList[lowestIndex];
				if (fCost[p1.y][p1.x]< fCost[p2.y][p2.x]) lowestIndex=i;
			}
	
			current = openList[lowestIndex];			// select to evalute
			openList[lowestIndex] = openList[--openCount];		// remove from visited list
			
			if (current.x == target.x && current.y == target.y) { // finish condition

				dirBli = reconstructFirstMove(parent, start, target);
				return dirBli;
				
			}
			evaluated[current.x][current.y] = 1;				// otherwise put in evaluated list
			
			for (i = 0; i < 4; i++) {											// visit the 4 neighboor
				newX = current.x + dRow[i];
				newY = current.y + dCol[i];

				if (!isValid(newX, newY, matrice) || evaluated[newX][newY]==1) { // only if valid
						continue;
				}
				
				tentativeG = gCost[current.y][current.x] + 1;							// determine new source distance

				if (tentativeG < gCost[newY][newX]) { // if current point gives shorter route to source
						parent[newY][newX] = current;				// set as parent and update gCost and fCost of evaluating point
						gCost[newY][newX] = tentativeG;
						fCost[newY][newX] = gCost[newY][newX] + heuristic(newX, newY, target.x, target.y);
					
					
						inOpenList = false;
						for ( j = 0; j < openCount; j++) {	// look for evaluation point in open list
								if (openList[j].x == newX && openList[j].y == newY) {
										inOpenList = true;
										break;
								}
						}
						if (!inOpenList) {									// otherwise add it
								openList[openCount++] = (Point){newX, newY};
						}
				}
			}
		}
		
	return 0;			// if no path ghost stay still
}

int reconstructFirstMove(Point parent[28][24], Point start, Point target) {
    current = target;

    // Rebuilt the path
    while (!(parent[current.y][current.x].x == start.x && parent[current.y][current.x].y == start.y)) {
        current = parent[current.y][current.x];
    }

    // find direction
    if (current.y > start.y) return 1; // Giù
    if (current.y < start.y) return 3; // Su
    if (current.x < start.x) return 4; // Sinistra
    if (current.x > start.x) return 2; // Destra

    return 0; // if error stay still
}


void moveBlinky( void )
	/* function that move blinky accroding to pacman position and game condition*/
{
		if(blinkyCondition==2) return;
		else if(blinkyCondition==0) dirBlinky = aStar((Point){bliX,bliY}, (Point){pacX,pacY}, matrice); // ricerco pacman
		else if(blinkyCondition==1) {			// ricerco fuga da pacman
			int dx, dy;
			dx=bliX-pacX;
			dy=bliY-pacY;
			if (dx>0 && dy>0)																// se sul terzo "quadrante" allora vado in angolino basso destra
				if((bliY==1 && bliX==1) || (bliY==26 && bliX==22) || (bliY==1 && bliX==22) || (bliY==26 && bliX==1)) dirBlinky=0;
				else dirBlinky = aStar((Point){bliX,bliY}, (Point){22,26}, matrice);
			else if(dx>0 && dy==0)													// se sinistra allora punto centrale 
				dirBlinky = aStar((Point){bliX,bliY}, (Point){22,13}, matrice);
			else if(dx>0 && dy<0)													// se sul secondo "qaudrante" allora alto destra
				if((bliY==1 && bliX==1) || (bliY==26 && bliX==22) || (bliY==1 && bliX==22) || (bliY==26 && bliX==1)) dirBlinky=0;
				else dirBlinky = aStar((Point){bliX,bliY}, (Point){22,1}, matrice);
			
			else if (dx<0 && dy>0)
				if((bliY==1 && bliX==1) || (bliY==26 && bliX==22) || (bliY==1 && bliX==22) || (bliY==26 && bliX==1)) dirBlinky=0;// se sul terzo "quadrante" allora vado in angolino basso sinistra
				else dirBlinky = aStar((Point){bliX,bliY}, (Point){1,26}, matrice);
			else if(dx<0 && dy==0)													// se destra allora punto centrale 
				dirBlinky = aStar((Point){bliX,bliY}, (Point){1,13}, matrice);
			else if(dx<0 && dy<0)													// se primo quadrante allora alto sinistra
				if((bliY==1 && bliX==1) || (bliY==26 && bliX==22) || (bliY==1 && bliX==22) || (bliY==26 && bliX==1)) dirBlinky=0;
				else dirBlinky = aStar((Point){bliX,bliY}, (Point){1,1}, matrice);
			else if (dx==0 && dy>0)																// se sul terzo "quadrante" allora vado in angolino basso sinistra
				dirBlinky = aStar((Point){bliX,bliY}, (Point){12,26}, matrice);
			else if(dx==0 && dy<0)																// se primo quadrante allora alto sinistra
				dirBlinky = aStar((Point){bliX,bliY}, (Point){12,1}, matrice);
		}
		if (matrice[bliY][bliX]!=5) gameCondition=5;				// se non trovo blinky errore
		else{switch(dirBlinky){
		case(0):																					 	// if 0 not move
			break;
		case(1):																						// if 1 try to move down
			matrice[bliY][bliX]=oldCell;
			LCD_DrawCircle(10*bliX,10*bliY+20, 5, Black);
			switch(oldCell){																	// ridisegno cella precedente
					case 3:
							LCD_DrawCircle(10*bliX,10*bliY+20, 2, White);
						break;
					case 4:
							LCD_DrawCircle(10*bliX,10*bliY+20, 3, Cyan);
						break;}
		
			switch(matrice[bliY+1][bliX]){										// down
				case(0):																				// empty cell
	
					matrice[bliY+1][bliX]=5;
					oldCell=0;
					
					if (blinkyCondition==0) LCD_DrawCircle(10*bliX, 10*(bliY+1) + 20, 5, Red);  //screen update
					if (blinkyCondition==1) LCD_DrawCircle(10*bliX, 10*(bliY+1) + 20, 5, Blue2);  //screen update
				
					bliY++;																				// position upgrade
					break;
				
				case(1):																				// if wall present
					gameCondition=5;
					break;
				
				case(2):																				// if pacman
					oldCell=0;
					
					if (blinkyCondition==0) {
						matrice[bliY+1][bliX]=5;
						LCD_DrawCircle(10*bliX, 10*(bliY+1) + 20, 5, Red);   //screen update
						life--;
						pacX=12;																						 // inital position of pacman in the matrix
						pacY=22;
						matrice[pacY][pacX]=2;
						LCD_DrawCircle(10*pacX, 10*(pacY) + 20, 5, Yellow);  //screen update
						bliY++;																				// position upgrade
						if (life==0) {
							gameCondition=2;
							disable_timer(0);
							upScreen();
							enable_timer(0);
							return;}
						}
					if (blinkyCondition==1){
						blinkyCondition=2;
						blinkyTimer=3;
						bliX=12;												// Blinky initial position
						bliY=12;
						matrice[bliY][bliX]=5;
						LCD_DrawCircle(10*bliX, 10*(bliY) + 20, 5, Blue2);
					}
				
					break;
				
				case(3):																				// if normal pills
					matrice[bliY+1][bliX]=5;
					oldCell=3;
					
					if (blinkyCondition==0) LCD_DrawCircle(10*bliX, 10*(bliY+1) + 20, 5, Red);  //screen update
					if (blinkyCondition==1) LCD_DrawCircle(10*bliX, 10*(bliY+1) + 20, 5, Blue2);  //screen update
					bliY++;																				// position upgrade
					break;
				
				case(4):																			// if power pill
					matrice[bliY+1][bliX]=5;										
					oldCell=4;
				
					if (blinkyCondition==0) LCD_DrawCircle(10*bliX, 10*(bliY+1) + 20, 5, Red);  //screen update
					if (blinkyCondition==1) LCD_DrawCircle(10*bliX, 10*(bliY+1) + 20, 5, Blue2);  //screen update
					bliY++;	
					break;
			}	
			break;
		case(2):
			matrice[bliY][bliX]=oldCell;
			LCD_DrawCircle(10*bliX,10*bliY+20, 5, Black);
			switch(oldCell){																	// ridisegno cella precedente
					case 3:
							LCD_DrawCircle(10*bliX,10*bliY+20, 2, White);
						break;
					case 4:
							LCD_DrawCircle(10*bliX,10*bliY+20, 3, Cyan);
						break;}
			switch(matrice[bliY][bliX+1]){										//right
							case(0):							
								matrice[bliY][bliX+1]=5;										
								oldCell=0;
				
								if (blinkyCondition==0) LCD_DrawCircle(10*(bliX+1), 10*(bliY) + 20, 5, Red);  //screen update
								if (blinkyCondition==1) LCD_DrawCircle(10*(bliX+1), 10*(bliY) + 20, 5, Blue2);  //screen update
								bliX++;	
							
								break;
							
							case(1):
								gameCondition=5;
								break;
							
							case(2):
								oldCell=0;
								
								if (blinkyCondition==0) {
									LCD_DrawCircle(10*(bliX+1), 10*(bliY) + 20, 5, Red);   //screen update
									life--;
									pacX=12;																						 // inital position of pacman in the matrix
									pacY=22;
									matrice[pacY][pacX]=2;
									bliX++;																								// position upgrade
									LCD_DrawCircle(10*pacX, 10*(pacY) + 20, 5, Yellow);  //screen update
									if (life==0) {
										gameCondition=2;
										disable_timer(0);
										upScreen();
										enable_timer(0);
										return;}
									}
								if (blinkyCondition==1){
									blinkyCondition=2;
									blinkyTimer=3;
									bliX=12;												// Blinky initial position
									bliY=12;
									matrice[bliY][bliX]=5;
									LCD_DrawCircle(10*bliX, 10*(bliY) + 20, 5, Blue2);
								}
								
								break;
							
							case(3):
								matrice[bliY][bliX+1]=5;										
								oldCell=3;
				
								if (blinkyCondition==0) LCD_DrawCircle(10*(bliX+1), 10*(bliY) + 20, 5, Red);  //screen update
								if (blinkyCondition==1) LCD_DrawCircle(10*(bliX+1), 10*(bliY) + 20, 5, Blue2);  //screen update
								bliX++;	
								break;
							
							case(4):
								matrice[bliY][bliX+1]=5;										
								oldCell=4;
				
								if (blinkyCondition==0) LCD_DrawCircle(10*(bliX+1), 10*(bliY) + 20, 5, Red);  //screen update
								if (blinkyCondition==1) LCD_DrawCircle(10*(bliX+1), 10*(bliY) + 20, 5, Blue2);  //screen update
								bliX++;	
								break;
						}	
			break;
		case(3):
			matrice[bliY][bliX]=oldCell;
			LCD_DrawCircle(10*bliX,10*bliY+20, 5, Black);
			switch(oldCell){																	// ridisegno cella precedente
					case 3:
							LCD_DrawCircle(10*bliX,10*bliY+20, 2, White);
						break;
					case 4:
							LCD_DrawCircle(10*bliX,10*bliY+20, 3, Cyan);
						break;}
			switch(matrice[bliY-1][bliX]){										//up
							case(0):
								matrice[bliY-1][bliX]=5;										
								oldCell=0;
				
								if (blinkyCondition==0) LCD_DrawCircle(10*(bliX), 10*(bliY-1) + 20, 5, Red);  //screen update
								if (blinkyCondition==1) LCD_DrawCircle(10*(bliX), 10*(bliY-1) + 20, 5, Blue2);  //screen update
								bliY--;	
								break;
							
							case(1):
								dir=0;												// se muro fermo
								break;
							
							case(2):
								oldCell=0;
								
								if (blinkyCondition==0) {
									matrice[bliY-1][bliX]=5;
									LCD_DrawCircle(10*(bliX), 10*(bliY-1) + 20, 5, Red);   //screen update
									life--;
									pacX=12;																						 // inital position of pacman in the matrix
									pacY=22;
									matrice[pacY][pacX]=2;
									LCD_DrawCircle(10*pacX, 10*(pacY) + 20, 5, Yellow);  //screen update
									bliY--;																				// position upgrade
									if (life==0) {
										gameCondition=2;
										disable_timer(0);
										upScreen();
										enable_timer(0);
										return;}
									}
								if (blinkyCondition==1){
									blinkyCondition=2;
									blinkyTimer=3;
									bliX=12;												// Blinky initial position
									bliY=12;
									matrice[bliY][bliX]=5;
									LCD_DrawCircle(10*bliX, 10*(bliY) + 20, 5, Blue2);
								}
							
								break;
							
							case(3):
								matrice[bliY-1][bliX]=5;										
								oldCell=3;
				
								if (blinkyCondition==0) LCD_DrawCircle(10*(bliX), 10*(bliY-1) + 20, 5, Red);  //screen update
								if (blinkyCondition==1) LCD_DrawCircle(10*(bliX), 10*(bliY-1) + 20, 5, Blue2);  //screen update
								bliY--;	
								break;
							
							case(4):
								matrice[bliY-1][bliX]=5;										
								oldCell=4;
				
								if (blinkyCondition==0) LCD_DrawCircle(10*(bliX), 10*(bliY-1) + 20, 5, Red);  //screen update
								if (blinkyCondition==1) LCD_DrawCircle(10*(bliX), 10*(bliY-1) + 20, 5, Blue2);  //screen update
								bliY--;	
								break;
						}	
			break;
		case(4):
			matrice[bliY][bliX]=oldCell;
			LCD_DrawCircle(10*bliX,10*bliY+20, 5, Black);
			switch(oldCell){																	// ridisegno cella precedente
					case 3:
							LCD_DrawCircle(10*bliX,10*bliY+20, 2, White);
						break;
					case 4:
							LCD_DrawCircle(10*bliX,10*bliY+20, 3, Cyan);
						break;}
			
			switch(matrice[bliY][bliX-1]){										//left
							case(0):
								matrice[bliY][bliX-1]=5;										
								oldCell=0;
				
								if (blinkyCondition==0) LCD_DrawCircle(10*(bliX-1), 10*(bliY) + 20, 5, Red);  //screen update
								if (blinkyCondition==1) LCD_DrawCircle(10*(bliX-1), 10*(bliY) + 20, 5, Blue2);  //screen update
								bliX--;	
								break;
							
							case(1):
								dir=0;												// se muro fermo
								break;
							
							case(2):
								oldCell=0;
								
								if (blinkyCondition==0) {
									matrice[bliY][bliX-1]=5;
									LCD_DrawCircle(10*(bliX-1), 10*(bliY) + 20, 5, Red);   //screen update
									life--;
									pacX=12;																						 // inital position of pacman in the matrix
									pacY=22;
									matrice[pacY][pacX]=2;
									LCD_DrawCircle(10*pacX, 10*(pacY) + 20, 5, Yellow);  //screen update
									bliX--;																				// position upgrade
									if (life==0) {
										gameCondition=2;
										disable_timer(0);
										upScreen();
										enable_timer(0);
										return;}
									}
								if (blinkyCondition==1){
									blinkyCondition=2;
									blinkyTimer=3;
									bliX=12;												// Blinky initial position
									bliY=12;
									matrice[bliY][bliX]=5;
									LCD_DrawCircle(10*bliX, 10*(bliY) + 20, 5, Blue2);
								}
							
								break;
							
							case(3):
								matrice[bliY][bliX-1]=5;										
								oldCell=3;
				
								if (blinkyCondition==0) LCD_DrawCircle(10*(bliX-1), 10*(bliY) + 20, 5, Red);  //screen update
								if (blinkyCondition==1) LCD_DrawCircle(10*(bliX-1), 10*(bliY) + 20, 5, Blue2);  //screen update
								bliX--;	
								break;
							
							case(4):
								matrice[bliY][bliX-1]=5;										
								oldCell=4;
				
								if (blinkyCondition==0) LCD_DrawCircle(10*(bliX-1), 10*(bliY) + 20, 5, Red);  //screen update
								if (blinkyCondition==1) LCD_DrawCircle(10*(bliX-1), 10*(bliY) + 20, 5, Blue2);  //screen update
								bliX--;	
								break;
						}	
			break;
		}
	}
}


/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
