/*
 * F-00009.c
 *
 * Created: 6.4.2016 9:16:04
 * Author : atom2
 */ 

#include <avr/io.h>
//#include "inc/AllInit.h"
//#include "inc/common_defs.h"
//#include "inc/RS232.h"
//#include <avr/boot.h>
//#include <avr/interrupt.h>
//#include <avr/fuse.h>
//#include <stdio.h>
//#include <avr/pgmspace.h>
//#include <avr/fuse.h>


#define	F_CPU	14745600
//#include <util/delay.h>

//#include <util/twi.h>

/*
Základní nastavení 
BOOTSZ = '00'
BOOTRST = '0'
*/
/*
#define		SIGNAT_BYTE0			0x00
#define		SIGNAT_BYTE1			0x02
#define		SIGNAT_BYTE2			0x04
#define		BOOTLOADER_VERSION		"00"
//Velikos Bootloader sekce
//#define	BOOT_SIZE	512
//#define	BOOT_SIZE	1024
//#define	BOOT_SIZE	2048
#define		BOOT_SIZE	4096
#define		BOOT_WORDS	128
#define		PAGES		(BOOT_SIZE / BOOT_WORDS)


//Definování 1 Stránky
#define APP_PAGES ((2*8192 / PAGESIZE)- (2*128 / PAGESIZE ))
//Konec stránky
#define APP_END APP_PAGES * PAGESIZE
*/
//Nastavení rychlosti
/*
void jumpaddress(void)
{
	asm("jmp 0x0000");
}

void ChipErase(void)
{
	
}

void WriteLockBits(void)
{
	
}
*/



int main(void)
{
	//cli();
	//Natavení promìnných
	uint16_t	Adress=0;
	uint8_t		device=0;
	uint8_t		ID_Data=0;
	uint8_t		BOOT_STATUS=0;
	//RS232_Init(RS232_115200);
    
	uint8_t	cnt=0;
	/*while (cnt--)
	{
		// Enter programming Mode
		if (RS232_Receive_Status() == 'P')
		{
			RS232_Transmit_String_CR("AtomTraceBootloader");
			BOOT_STATUS = 1;
			break;
		}
		_delay_ms(1);
	}
	if (!BOOT_STATUS)
	{
		//jumpaddress();
	} 
	ID_Data = 0x30;
	*/
	/* Replace with your application code */
    while (1) 
    {
		ID_Data++;
		if (ID_Data == 255)
		{
			ID_Data = 0;
		}
		
		//ID_Data = RS232_Receive_Char();
		/*
		switch (ID_Data)
		{
			case 'P':
				RS232_Transmit_Char_CR();
				break;
				//Read LOW FUSE
			case 'F':
				RS232_Transmit_Char(boot_lock_fuse_bits_get(GET_LOW_FUSE_BITS));
				break;
				//Read High FUSE
			case 'N':
				RS232_Transmit_Char(boot_lock_fuse_bits_get(GET_HIGH_FUSE_BITS));
				break;
				//Read Extended FUSE
			case 'Q':
				RS232_Transmit_Char(boot_lock_fuse_bits_get(GET_EXTENDED_FUSE_BITS));
				break;
				//Read Signature
			case 's':
				RS232_Transmit_Char(boot_signature_byte_get(SIGNAT_BYTE0));
				RS232_Transmit_Char(boot_signature_byte_get(SIGNAT_BYTE1));				
				RS232_Transmit_Char(boot_signature_byte_get(SIGNAT_BYTE2));				
				break;
			case 'V':
				RS232_Transmit_String(BOOTLOADER_VERSION);
				break;
			case 'r':
				RS232_Transmit_Char(boot_lock_fuse_bits_get(GET_LOCK_BITS));
				break;
			case 'E':
				RS232_Transmit_String_CR("Exit from BOOTLOADER. BYE");
				jumpaddress();
				break;
			case 'e':
				RS232_Transmit_Char_CR();
				// Smazání èipu
				break;
			
		}
		*/
		
    }
}

