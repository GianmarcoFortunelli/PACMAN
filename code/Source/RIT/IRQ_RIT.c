/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "LPC17xx.h"
#include "RIT.h"

/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

volatile int pressed=0;
extern uint8_t gameCondition;
extern volatile uint8_t timer;

int debug;

extern volatile int	life, i, j, dir, pacX, pacY, pills, n;
extern volatile score;
extern int matrice[28][24], field[28][24];
extern int temp;
extern int tempi[6];
extern int dirBlinky, blinkyCondition, bliX, bliY, blinkyTimer, oldCell, music_sample;
void RIT_IRQHandler (void)
	

{			
	debug++;
	LPC_RIT->RICTRL |= 0x1;					 /* clear interrupt flag */
	// joystick managment
	
	int up=0, down=0, left=0, right=0;   	   // variable to detect direction pressed
	
	if((LPC_GPIO1->FIOPIN & (1<<29)) == 0){	 // if press for first time change dir otw do nothing
		/* Joytick UP pressed */
		up++;
		switch(up){
			case 1:
				dir=3;
				break;
			default:
				break;
		}
	}
	else{
			up=0;																// if not press bring flag to zero
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<28)) == 0){	
		/* Joytick RIGTH pressed */
		right++;
		switch(right){
			case 1:
				dir=2;
				break;
			default:
				break;
		}
	}
	else{
			right=0;
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<27)) == 0){	
		/* Joytick LEFT pressed */
		left++;
		switch(left){
			case 1:
				dir=4;
				break;
			default:
				break;
		}
	}
	else{
			left=0;
	}
	
		if((LPC_GPIO1->FIOPIN & (1<<26)) == 0){	
		/* Joytick DOWN pressed */
		down++;
		switch(down){
			case 1:
				dir=1;
				break;
			default:
				break;
		}
	}
	else{
			down=0;
	}
	
	
	/* button management */
	if(pressed>=1){															  // pressed from IRQ button
		if((LPC_GPIO2->FIOPIN & (1<<10)) == 0){			// still pressed
			switch(pressed){				
				case 2:																	// for enough time, no debouncing
					switch(gameCondition){    					  //change game condition
						
						case(0):  												  // pause
							gameCondition=3; 								  // resume the game
							disable_timer(0);
							upScreen();
							enable_timer(0);
							break;
						
						
						case(1): 														// victory
							
							disable_timer(0);
					
							for(i=0; i<26;i++){
								for(j=0; j<28;j++){
									matrice[j][i]=field[j][i];    // reset matrix and initial values
								}
							}
							
							for (i=0; i<6; i++){
								tempi[i]=rand() % (60-20+1) + 20;    // generate random time for creation of power pills
							}
							
							for (i=0; i<6; i++){
								for (j=0; j<6-i-1;j++){
									if (tempi[j]< tempi[j+1]){
										temp = tempi[j];
										tempi[j] = tempi[j+1];
										tempi[j+1]=temp;
									}
								}
							}
							
							dir=0;
							timer=60;
							life=1;
							n=1;
							score=0;
							pills=240;
							pacX=12;
							pacY=22;
							gameCondition=3; 									// resume the game
							dirBlinky = 0;									// direction of blinky
							blinkyCondition = 0;						// follow mode
							bliX=12;												// Blinky initial position
							bliY=12;
							blinkyTimer=0;
							oldCell=0;
							music_sample=0;
							
							upScreen();
							
							enable_timer(0);
							break;
						
						case(2): 														// game over
							
							disable_timer(0);
						
							for(i=0; i<26;i++){
								for(j=0; j<28;j++){
									matrice[j][i]=field[j][i];		// reset matrix and initial values
								}
							}
							for (i=0; i<6; i++){											// generate random time for creation of power pills
								tempi[i]=rand() % (60-20+1) + 20;
							}
							
							for (i=0; i<6; i++){
								for (j=0; j<6-i-1;j++){
									if (tempi[j]< tempi[j+1]){
										temp = tempi[j];
										tempi[j] = tempi[j+1];
										tempi[j+1]=temp;}}}
						
						
							dir=0;
							timer=60;
							life=1;
							n=1;
							score=0;
							pills=240;
							pacX=12;
							pacY=22;
							gameCondition=3; 									// resume the game
							dirBlinky = 0;									// direction of blinky
							blinkyCondition = 0;						// follow mode
							bliX=12;												// Blinky initial position
							bliY=12;
							blinkyTimer=0;
							oldCell=0;
							music_sample=0;
							upScreen();
										
							enable_timer(0);
						
							break;
						
						case(3):
							//disable_timer(0);
							gameCondition=0; 									// pause
							upScreen();
						
							break;
						
						default:
							//disable_timer(0);
							gameCondition=5; 									// error screen
							upScreen();
							break;
					}
					break;
				default:
					break;}
			pressed++;}
		else {																		 /* button released */
			pressed=0;			
			NVIC_EnableIRQ(EINT0_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 20);     /* External interrupt 0 pin selection */
		}
	}
	  
	
	LPC_RIT->RICOUNTER = 0;          // Set count value to 0
  LPC_RIT->RICTRL |= 0x1;					 /* clear interrupt flag */
	
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
