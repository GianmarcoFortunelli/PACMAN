/*----------------------------------------------------------------------------
 * Name:    Can.c
 * Purpose: CAN interface for for LPC17xx with MCB1700
 * Note(s): see also http://www.port.de/engl/canprod/sv_req_form.html
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2009 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include <LPC17xx.h>                  /* LPC17xx definitions */
#include "CAN.h"                      /* LPC17xx CAN adaption layer */
#include "../GLCD/GLCD.h"
#include <stdio.h> /*for sprintf*/

extern uint8_t icr ; 										//icr and result must be global in order to work with both real and simulated landtiger.
extern uint32_t result;
extern char stringa[10];
extern CAN_msg       CAN_TxMsg;    /* CAN message for sending */
extern CAN_msg       CAN_RxMsg;    /* CAN message for receiving */                                

uint16_t rem_life;

/*----------------------------------------------------------------------------
  CAN interrupt handler
 *----------------------------------------------------------------------------*/
void CAN_IRQHandler (void)  {

  /* check CAN controller 1 */
	icr = 0;
  icr = (LPC_CAN1->ICR | icr) & 0xFF;               /* clear interrupts */
	
  if (icr & (1 << 0)) {                          		/* CAN Controller #1 meassage is received */
		CAN_rdMsg (1, &CAN_RxMsg);	                		/* Read the message */
    LPC_CAN1->CMR = (1 << 2);                    		/* Release receive buffer */

  }
	if (icr & (1 << 1)) {                         /* CAN Controller #1 meassage is transmitted */
		// do nothing in this example
	}
		
	/* check CAN controller 2 */
	icr = 0;
	icr = (LPC_CAN2->ICR | icr) & 0xFF;             /* clear interrupts */

	if (icr & (1 << 0)) {                          	/* CAN Controller #2 meassage is received */
		CAN_rdMsg (2, &CAN_RxMsg);	                		/* Read the message */
    LPC_CAN2->CMR = (1 << 2);                    		/* Release receive buffer */
		
		GUI_Text(0, 1, (uint8_t *) " Time: ", White, Black);
		sprintf(stringa, "%d", CAN_RxMsg.data[3]);
		GUI_Text(50, 1, (uint8_t *) "    ", White, Black);
		GUI_Text(50, 1, (uint8_t *) stringa, White, Black);						//time
		
		rem_life = (CAN_RxMsg.data[1]<<8)|CAN_RxMsg.data[0];
		GUI_Text(100, 1, (uint8_t *) " Score: ", White, Black);  			//score
		sprintf(stringa, "%d", rem_life); 
		GUI_Text(160, 1, (uint8_t *) stringa, White, Black);
		LCD_DrawLine(0, 19, 260, 19, White);
	
		GUI_Text(0, 304, (uint8_t *) " Life: ", White, Black);  			// n. life
		sprintf(stringa, "%d", CAN_RxMsg.data[3]); 
		GUI_Text(50, 304, (uint8_t *) stringa, White, Black);
		LCD_DrawLine(0, 300, 260, 300, White);
		
	}
	if (icr & (1 << 1)) {                         /* CAN Controller #2 meassage is transmitted */
		// do nothing in this example
	}
}
