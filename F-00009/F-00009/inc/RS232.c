/*

Program: Bronkhorst
Knihovna: RS232.c
Vlastn�k: Luk� Nov�k

*/
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/io.h>
#include "RS232.h"


//////////////////////////////////////////////////////////////////////
/*		Inicializa�n� procedura obsahuj�c� nastaven� odes�l�n�,		*/
/*		p��jem dat a povolen� p�eru�en� p��jmu po s�riov� lince.	*/
/*			D�le nastaven� R/T pomoc� 8 bit� a 1 stop bit.			*/
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
/*						Vytvo�enopro uC Atmega328P						*/
/*																		*/
//////////////////////////////////////////////////////////////////////////

//TXD0
//RXD0

void RS232_Init( unsigned int baud )
{
	
	UBRR0H = (unsigned char)(baud>>8);							//Registr pro nastaven� vrchn�ch byt� rychlosti s�riov� linky.
	UBRR0L = (unsigned char)baud;								//Registr pro nastaven� spodn�ch byt� rychlosti s�riov� linky.
	UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);		//Povolen� v registru odes�l�n�, p��jmu a povolen� p�eru�en� p��jmu
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);						//Nastaven� R/T 8 bit� data, 1 stop bitu, asynchronn� p�enos a bez parity.
}

//////////////////////////////////////////////////////////////////////////
/*				Procedura pro p��jem jednoho znaku a ukl�d�n�			*/
/*					z registru UDR do vr�cen� prom�nn�.					*/
//////////////////////////////////////////////////////////////////////////
uint8_t	RS232_Receive_Status(void)
{
	if(UCSR0A & (1<<RXC0))
	{
		return UDR0;
	}
	return 0;
}

unsigned char RS232_Receive_Char( void )
{
  unsigned char data;
  while ( !(UCSR0A & (1<<RXC0)) );	//�ek�n� dokud nen� dokon�en p�enos dat
  data=UDR0;							//�ten� dat z registru UDR a ukl�d�n� do prom�nn�
  return data;						//Vr�cen� p�ijmut�ho znaku
}

//////////////////////////////////////////////////////////////////////////
/*  		Procedura pro prijem vice znaku po s�riov� lince.			*/
//////////////////////////////////////////////////////////////////////////

void RS232_Receive_String( void )
{
	int RxBPtr=0;
	RxBPtr = 256;
	char RxB[RxBPtr];
	while (RS232_Receive_Char() != 0x0A)
	{
		RxB[RxBPtr] = RS232_Receive_Char();
		RS232_Transmit_Char(RxB[RxBPtr]);
		RxBPtr++;
		
	}
}

//////////////////////////////////////////////////////////////////////////
/*		Procedura pro odes�l�n� jednoho znaku po s�riov� lince.			*/
//////////////////////////////////////////////////////////////////////////

void RS232_Transmit_Char( unsigned char data )
{
  while ( !( UCSR0A & (1<<UDRE0)) );	//�ek�n� dokud nen� pr�zdn� buffer
  UDR0 = data;						//Nastaven� znaku do registru
}

void RS232_Transmit_Char_CR(void)
{
	while ( !( UCSR0A & (1<<UDRE0)) );	//�ek�n� dokud nen� pr�zdn� buffer
	UDR0 = 0x0D;						//Nastaven� znaku do registru
}

//////////////////////////////////////////////////////////////////////////
/*			Roz���en� procedury pro odes�l�n� jednoho znaku,			*/
/*						 na odes�l�n� pole znak�.						*/
//////////////////////////////////////////////////////////////////////////

void RS232_Transmit_String(char* StringPtr){
	
	while(*StringPtr != 0x00)			//Opakov�n� cyklu dokud se nevypr�zdn� buffer
	{
		RS232_Transmit_Char(*StringPtr);		//Vyu�it� procedury pro odesl�n� jednoho znaku
		StringPtr++;					//Posunut� o dal�� znak
	}
	
}

//////////////////////////////////////////////////////////////////////////
/*				 Upraven� procedura odes�l�n� pole znak�				*/
/*			s p�idan�m ukon�ovac�m znakem <CR> pro AT p��kazy.			*/
//////////////////////////////////////////////////////////////////////////

void RS232_Transmit_String_CR(char* StringPtr){
	
	while(*StringPtr != 0x00)			//Opakov�n� cyklu dokud se nevypr�zdn� buffer
	{
		RS232_Transmit_Char(*StringPtr);		//Vyu�it� procedury pro odes�l�n� jednoho znaku
		StringPtr++;					//Posunut� o dal�� znak
	}
	RS232_Transmit_Char(0x0D);				//Na konci odesl�n znak <CR>
}

void RS232_Transmit_String_CRLF(char* StringPtr){
	
	while(*StringPtr != 0x00)			//Opakov�n� cyklu dokud se nevypr�zdn� buffer
	{
		RS232_Transmit_Char(*StringPtr);		//Vyu�it� procedury pro odes�l�n� jednoho znaku
		StringPtr++;					//Posunut� o dal�� znak
	}
	RS232_Transmit_Char(0x0D);				//Na konci odesl�n znak <CR>
	RS232_Transmit_Char(0x0A);				//Na konci odesl�n znak <LF>
}


void RS232_Transmit_String_Buffer(char* StringPtr, int TXStringPtr)
{
	
//	while(*StringPtr != 0x0A)			//Opakov�n� cyklu dokud se nevypr�zdn� buffer
	while(TXStringPtr != 0)
	{
		RS232_Transmit_Char(*StringPtr);		//Vyu�it� procedury pro odes�l�n� jednoho znaku
		StringPtr++;					//Posunut� o dal�� znak
		TXStringPtr--;
	}
}

