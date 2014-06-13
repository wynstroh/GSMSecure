/*
 * usart.c
 *
 *  Created on: 25 May 2014
 *      Author: wynands
 */

#include "stm32l1xx_conf.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

#define CR 13
#define OChar 79
#define KChar 75

void usart_write(uint8_t ch)
{
      USART_SendData(USART1, (uint8_t) ch);
      while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
      {
      }
}

uint8_t usart_read(void){
     while ( USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
        return (uint8_t)USART_ReceiveData(USART1);
}

uint8_t usart_available(void)
{
	if ( USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET )
		return 1;

	return 0;
}

void RecvString(char* data)
{
	unsigned char d = 0x00;
	unsigned int i = 0;
	unsigned int crcount =0;
	do
	{
		while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);    // Wait for data to be received
		d = (unsigned char) (USART_ReceiveData(USART1) & 0x7F);
		if (d != 0x00)
		{
			data[i] = d;
			i++;

			if (d==CR)
				crcount++;
		}

		// Increment the string pointer
	}
	while ((data[i-1] != 'K' && data[i-2] != 'O') || crcount<4); // d!=CR); (crcount<4);

}

void usart_print( char *msg )
{
	int len = strlen( msg );

	for ( int c = 0; c < len; c++ )
		usart_write( (uint8_t)*msg++ );
}

void usart_init(void)
{
	GPIO_InitTypeDef gpio_init;

	USART_InitTypeDef usart_init;
	USART_ClockInitTypeDef usart_clk_init;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	// PA9 = Tx, PA10 = Rx
	gpio_init.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	gpio_init.GPIO_Mode = GPIO_Mode_AF;
	gpio_init.GPIO_Speed = GPIO_Speed_40MHz;
	gpio_init.GPIO_OType = GPIO_OType_PP;
	gpio_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &gpio_init);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

	USART_ClockStructInit(&usart_clk_init);
	USART_ClockInit(USART1, &usart_clk_init);

	usart_init.USART_BaudRate =            19200;
	usart_init.USART_WordLength =          USART_WordLength_8b;
	usart_init.USART_StopBits =            USART_StopBits_1;
	usart_init.USART_Parity =              USART_Parity_No ;
	usart_init.USART_Mode =                USART_Mode_Rx | USART_Mode_Tx;
	usart_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART1, &usart_init);
	USART_Cmd(USART1,ENABLE);

	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}

}
