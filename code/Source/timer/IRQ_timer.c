/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include <string.h>
#include "LPC17xx.h"
#include "timer.h"
#include "../GLCD/GLCD.h" 
#include "../TouchPanel/TouchPanel.h"
#include <stdio.h> /*for sprintf*/
#include "../CAN/CAN.h"

/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

extern uint8_t timer, gameCondition;
extern char stringa[10]; 
extern int i, pills, life, score;
extern int tempi[6];
int found, posX, posY, cnt, music_sample;
extern int matrice[28][24];
extern int dirBlinky, bliX, bliY, oldCell, blinkyCondition, blinkyTimer;

//const int freqs[8]={4240,3779,3367,3175,2834,2525,2249,2120};
/* 
131Hz		k=4240 C3
147Hz		k=3779
165Hz		k=3367
175Hz		k=3175
196Hz		k=2834		
220Hz		k=2525
247Hz		k=2249
262Hz		k=2120 C4
*/

const int background[8]={2120, 1684, 1417, 1263, 1417, 1684, 1890, 1592};
/*
262Hz	k=2120		c4
294Hz	k=1890		
330Hz	k=1684		
349Hz	k=1592		
392Hz	k=1417		
440Hz	k=1263		
494Hz	k=1125		
523Hz	k=1062		c5

*/

const int winning[8]={2120, 1592, 1062, 1062, 0, 0, 0, 0};
const int losing[8]={1062, 1592, 2120, 2120, 0, 0, 0, 0};

uint16_t SinTable[45] =                                       /* ÕýÏÒ±í                       */
{
    410, 467, 523, 576, 627, 673, 714, 749, 778,
    799, 813, 819, 817, 807, 789, 764, 732, 694, 
    650, 602, 550, 495, 438, 381, 324, 270, 217,
    169, 125, 87 , 55 , 30 , 12 , 2  , 0  , 6  ,   
    20 , 41 , 70 , 105, 146, 193, 243, 297, 353
};


void TIMER0_IRQHandler (void)
{
	if(LPC_TIM0->IR & 1){
		if (gameCondition==3){
			 move();
		}
	}
	else if(LPC_TIM0->IR & 2){
		
		if (gameCondition==0){
				LPC_RIT->RICOUNTER = 0;          // reset rit value to 0
				disable_timer(1);
				reset_timer(1);
				LPC_DAC->DACR = 0 <<6;
		}
		else if (gameCondition==1){
				cnt++;
				if (cnt>=2) cnt=0;
				if (cnt%2==0){
				LPC_RIT->RICOUNTER = 0;          // reset rit value to 0
				if (music_sample>=7) music_sample=0;
				else music_sample++;
				disable_timer(1);
				reset_timer(1);
				init_timer(1,0xFFFFFFFF,winning[music_sample]);
				enable_timer(1);
			}     
		}	
		else if (gameCondition==2){
				cnt++;
				if (cnt>=2) cnt=0;
				if (cnt%2==0){
				LPC_RIT->RICOUNTER = 0;          // reset rit value to 0
				if (music_sample>=7) music_sample=0;
				else music_sample++;
				disable_timer(1);
				reset_timer(1);
				init_timer(1,0xFFFFFFFF,losing[music_sample]);
				enable_timer(1);
		}
	}
		else if (gameCondition==3){
			
			if (timer>=40){
				if (cnt%3==0) moveBlinky();}
			else if (timer>=20 & timer<40){
				if (cnt%2==0) moveBlinky();}
			else if (timer<20){
				 moveBlinky();}
			
			move();
			cnt++;
			if (cnt%2==0){
				if (music_sample>=7) music_sample=0;
				else music_sample++;
				disable_timer(1);
				reset_timer(1);
				init_timer(1,0xFFFFFFFF,background[music_sample]);
				enable_timer(1);
			}
			if (cnt>=6){
				LPC_RIT->RICOUNTER = 0;          // reset rit value to 0
				cnt=0;
				timer--;
				if (blinkyCondition==2){
					blinkyTimer--;
					if (blinkyTimer==0) {
						blinkyCondition=0;
						bliX=12;												// Blinky initial position
						bliY=12;
						matrice[bliY][bliX]=5;
						LCD_DrawCircle(10*bliX, 10*(bliY) + 20, 5, Blue2);
					}}
				if (blinkyCondition==1){
					blinkyTimer--;
					if (blinkyTimer==0) blinkyCondition=0;}
				for (i=0; ( i<6 && tempi[i]>=timer ); i++){			// managing more pills at the same time
					if (tempi[i]==timer){
						found=0;																		// brute force approach and done flag
						while(!found){
							posY=rand() % (28+1);
							posX=rand() % (24+1);
							if (matrice[posY][posX]== 3){
								matrice[posY][posX]=4;
								found=1;
								LCD_DrawCircle(10*posX, 10*(posY) + 20, 3, Cyan);  //up date screen
							}
						}
					}
				}
				/* GUI_Text(0, 1, (uint8_t *) " Time: ", White, Black);
				sprintf(stringa, "%d", timer);
				GUI_Text(50, 1, (uint8_t *) "    ", White, Black);
				GUI_Text(50, 1, (uint8_t *) stringa, White, Black); */
				// now on can communication
				
					
				CAN_TxMsg.data[0] = (score & 0xFF);
				CAN_TxMsg.data[1] = (score & 0xFF00) >> 8;					// score LSB and MSB
				CAN_TxMsg.data[2] = life & 0xFF;										// life and time
				CAN_TxMsg.data[3] = timer & 0xFF;
				CAN_TxMsg.len = 4;
				CAN_TxMsg.id = 1;
				CAN_TxMsg.format = STANDARD_FORMAT;
				CAN_TxMsg.type = DATA_FRAME;
				CAN_wrMsg (1, &CAN_TxMsg);               /* transmit message */
				
				
				if (timer==0){																	// game over and victory condition
					
					gameCondition=2;
					disable_timer(0);
					upScreen();
					enable_timer(0);
					
					}
				}
			if (pills==0){
				
				gameCondition=1;
				disable_timer(0);
				upScreen();
				enable_timer(0);
				
			}
		}
	}
  LPC_TIM0->IR = 3;																		/* clear interrupt flag */
	return;
}

/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER1_IRQHandler (void)
{
	
	static int sineticks=0;
	/* DAC management */	
	static int currentValue; 
	currentValue = SinTable[sineticks];
	currentValue -= 410;
	currentValue /= 1;
	currentValue += 410;
	LPC_DAC->DACR = currentValue <<6;
	sineticks++;
	if(sineticks==45) sineticks=0;

	
  LPC_TIM1->IR = 1;			/* clear interrupt flag */
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
