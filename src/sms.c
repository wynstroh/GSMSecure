/*
 * sms.c
 *
 *  Created on: 25 May 2014
 *      Author: wynands
 */

#include "stm32l1xx_conf.h"
#include "discoverylcd.h"
#include "stm32l1xx_rtc.h"
#include "usart.h"
#include <string.h>
#include "mini-printf.h"
#include <time.h>

#define CTRL_Z 26
#define CR 13
#define NL 10
#define Q 34

char data[256];
char date_s[17];
int count;

int SendSMS(char PhoneNumer, char Message)
{
	char buffer[256];
	mini_snprintf(buffer ,255 ,"AT+CMGS=\"%s\"%c", PhoneNumer, CR);

	usart_print("AT+CMGF=1\r\n");

	timer_sleep(1000);

	mini_snprintf(buffer ,255 ,"AT+CMGS=\"%s\"%c", PhoneNumer, CR);

	usart_print( buffer );

	timer_sleep(1000);

	mini_snprintf(buffer ,255 ,"%s%c", Message, CTRL_Z);

	usart_print( buffer );

	return 0;
}

int SetRTCClock()
{
	struct tm result;

	usart_print( "AT+CCLK?\r" );

	RecvString(data);

	char buffer[4];
	mini_snprintf(buffer ,255 ," %c%c%c", Q, CR, NL);

	char *tok;
	count =0;

	tok = strtok(data,buffer);

	while (tok != NULL)
	{
		count ++;

		if (count == 3)
			strncpy(date_s, tok, sizeof(date_s));

		tok = strtok(NULL,buffer);
	}

	if(strptime(date_s, "%y / %m / %d , %H:%M:%S ",&result) == NULL)
	{

	}
	// Set the date (BCD)
	RTC_DateTypeDef RTC_DateStructure;
	RTC_DateStructure.RTC_Year = IntToBCD((result.tm_year - 100)); //(result.tm_year - 100);
	RTC_DateStructure.RTC_Month = IntToBCD((result.tm_mon + 1)); //(result.tm_mon + 1);
	RTC_DateStructure.RTC_Date = IntToBCD(result.tm_mday);
	RTC_DateStructure.RTC_WeekDay = IntToBCD(result.tm_wday);

	RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);

	// Set the time (BCD)
	RTC_TimeTypeDef RTC_TimeStructure;
	if (result.tm_hour > 12)
	{
		RTC_TimeStructure.RTC_H12     = RTC_H12_PM;
	}
	else
	{
		RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
	}
	RTC_TimeStructure.RTC_Hours   = IntToBCD(result.tm_hour); // 0x20
	RTC_TimeStructure.RTC_Minutes = IntToBCD(result.tm_min);
	RTC_TimeStructure.RTC_Seconds = IntToBCD(result.tm_sec);
	RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);

	return 0;
}

int IntToBCD(unsigned int intValue)
{
	return ((intValue/10)<<4)+(intValue%10);
}


int dayofweek(int d, int m, int y)
{
    static int t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
    y -= m < 3;
    return ( y + y/4 - y/100 + y/400 + t[m-1] + d) % 7;
}
