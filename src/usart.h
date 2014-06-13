/*
 * usart.h
 *
 *  Created on: 25 May 2014
 *      Author: wynands
 */

#ifndef USART_H_
#define USART_H_

uint8_t usart_read(void);
uint8_t usart_available(void);
void usart_print( char *msg );
void usart_init(void);

void RecvString(char *);

#endif /* USART_H_ */
