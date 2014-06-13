#include "stm32l1xx_conf.h"
//#include "discoverylcd.h"
#include "nokialcd.h"
#include "mini-printf.h"
#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

#include "usart.h"
#include "sms.h"

int _sent = 0;
char strDisp[20] ;

void delay( int a )
{
	volatile int i, j;

	for ( i = 0; i < a; i++ )
	{
		j++;
	}
}

void EnableClock()
{

  // divide HCLK / 2
  RCC_HCLKConfig(RCC_SYSCLK_Div2);

  // enable HSI
  RCC_HSICmd(ENABLE);
  RCC_PLLCmd(DISABLE);
  // wait for HSI to get ready
  while (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);

  // configure PLL - x4 /2
  RCC_PLLConfig( RCC_PLLSource_HSI, RCC_PLLMul_8,  RCC_PLLDiv_4 );
  RCC_PLLCmd(ENABLE);

  while ( RCC_GetFlagStatus( RCC_FLAG_PLLRDY ) == RESET );

  // set hsi as clock
  //RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);

  // set pll as clock
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

  // setup lse for rtc and lcd
  /* Allow access to the RTC */
  PWR_RTCAccessCmd(ENABLE);

  /* Reset RTC Backup Domain */
  RCC_RTCResetCmd(ENABLE);
  RCC_RTCResetCmd(DISABLE);

  /* LSE Enable */
  RCC_LSEConfig(RCC_LSE_ON);

  /* Wait until LSE is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);

   /* RTC Clock Source Selection */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

  /* Enable the RTC */
  RCC_RTCCLKCmd(ENABLE);

  RTC_WaitForSynchro();

}

void RCC_Configuration(void)
{

  /* Enable comparator, LCD and PWR mngt clocks */
  // RCC_APB1Periph_COMP |
  RCC_APB1PeriphClockCmd( RCC_APB1Periph_LCD | RCC_APB1Periph_PWR,ENABLE);

  /* Enable ADC & SYSCFG clocks */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_SYSCFG , ENABLE);
}

int main(void) {

	RCC_Configuration();

	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);

	EnableClock();

	//LCD_GLASS_Configure_GPIO();
	//LCD_GLASS_Init();

	SysTick_Config((RCC_Clocks.SYSCLK_Frequency / 2) / 1000); // Cannot exceed 16,777,215

	/* Set SysTick Preemption Priority, it's a system handler rather than a regular interrupt */
	//NVIC_SetPriority(SysTick_IRQn, 0x04);

	lcdInit();
	lcdClear();
	lcdXY( 2, 5 );
	lcdStr( "Test");
	//lcdBender();

	// init rtc
	RTC_InitTypeDef rtcInit;
	rtcInit.RTC_HourFormat = RTC_HourFormat_24;
	rtcInit.RTC_AsynchPrediv = 0x7F;
	rtcInit.RTC_SynchPrediv = 0xFF;
	RTC_Init(&rtcInit);

	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef RTC_DateStructure;

	usart_init();

 	SetRTCClock();

 	GPIO_InitTypeDef gpio_btn;

 	gpio_btn.GPIO_Pin = GPIO_Pin_0;
 	gpio_btn.GPIO_Mode = GPIO_Mode_IN;
 	gpio_btn.GPIO_PuPd = GPIO_PuPd_UP;

 	GPIO_Init(GPIOA, &gpio_btn);

 	int prevSecond = -1;

 	while (1) {

       	//uint8_t __status = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);

       	if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 0x00 && !_sent)
       	{
       		_sent = 1;

       		mini_snprintf( strDisp, 22,  "20%02d/%02d/%02d %02d:%02d:%02d Hallo !!!", RTC_DateStructure.RTC_Year, RTC_DateStructure.RTC_Month, RTC_DateStructure.RTC_Date, RTC_TimeStructure.RTC_Hours, RTC_TimeStructure.RTC_Minutes, RTC_TimeStructure.RTC_Seconds);

       		SendSMS("0836325001",strDisp);
       	}

       	if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) != 0x00 && _sent)
       	{
       		_sent = 0;
       	}

       	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure );
       	RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);



       	if ( RTC_TimeStructure.RTC_Seconds != prevSecond )
        {
       		lcdClear();
       		mini_snprintf( strDisp, 11,  "20%02d/%02d/%02d", RTC_DateStructure.RTC_Year, RTC_DateStructure.RTC_Month, RTC_DateStructure.RTC_Date);
       		lcdXY( 0, 0 );
       		lcdStr( strDisp);
       		mini_snprintf( strDisp, 10,  "%02d:%02d:%02d", RTC_TimeStructure.RTC_Hours, RTC_TimeStructure.RTC_Minutes, RTC_TimeStructure.RTC_Seconds );
       		lcdXY( 0, 1 );
       		lcdStr( strDisp);
        	//LCD_GLASS_Clear();
        	//LCD_GLASS_DisplayString( strDisp );
        	prevSecond = RTC_TimeStructure.RTC_Seconds;
        }

			/*if ( usart_available() ) // data available
			{
				//usart_print( "Data Available: " );
				uint8_t ch = usart_read();
				//usart_write(ch);
				//usart_print( "\r\n" );
				SendSMS("836325001","Wynand");
			}*/

  		}


        return 0;
}

