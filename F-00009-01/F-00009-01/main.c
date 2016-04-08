/*
 * F-00009-01.c
 *
 * Created: 8.4.2016 12:42:17
 * Author : atom2
 */ 

#include <avr/io.h>
#include "inc/RS232.h"
#include "inc/AllInit.h"
#include "inc/common_defs.h"
#include <avr/boot.h>
#include <avr/fuse.h>
#define	F_CPU	14745600
#include <util/delay.h>
#include <avr/interrupt.h>
/************************************************************************/
/* Programovací pojistky se musí nastavit
BOOTRSZ = 0
BOOTSZ = "00"

																		*/
/************************************************************************/


#define SIGNAT_BYTE_0	0x00
#define SIGNAT_BYTE_1	0x02
#define	SIGNAT_BYTE_2	0x04
#define	BOOTLOADER_VERSION	"00"
//#define	BOOT_SIZE	512
//#define	START_ADDRESS	0x7E00
//#define	PAGES	4
//#define	BOOT_SIZE	1024
//#define	START_ADDRESS	0x7C00
//#define	PAGES	8
//#define	BOOT_SIZE	2048
//#define	START_ADDRESS	0x7800
//#define	PAGES	16


// Velikost BOOTLOADER sekce pøi urèitém nastavení (Datasheet str.290)
#define	BOOTLOADER	4096
//#define	PAGES	32
// Jedno slovo je veliké 16 bitù (2 BYTES)
#define ONE_WORDS	2
// Poèet slov pro velikost BOOTLOADER sekce 4096 (Datasheet str.290)
#define	BOOT_WORDS	128
// Poèet PAGES (viz Datasheet 290) 256 PAGES
#define	NUM_O_PAGES	256
// Velikost celé FLASH pamìti 64kBytes
#define FLASH		65536
// Poèet stránek (Datasheet str.290) 256 PAGES
#define PAGE_SIZE		(FLASH / NUM_O_PAGES)
// Velikost BOOTLOADER èásti o velikosti PAGES (Stránek) 32 PAGES
#define	BOOT_SIZE	(BOOTLOADER / BOOT_WORDS)
// Velikost APPLICATION PAGES (Applikaèních stránek) 224 PAGES
#define APP_SIZE	(PAGE_SIZE - BOOT_SIZE)

#define START_APP_ADDRESS	0x0000
#define	END_APP_ADDRESS		((APP_SIZE * BOOT_WORDS) - 1)
#define	START_BOOT_ADDRESS	(APP_SIZE * BOOT_WORDS)
#define END_ADDRESS			(PAGE_SIZE * BOOT_WORDS)

void	jumpaddress(void)
{
	asm("jmp 0x0000");
}

void ChipErase(void)
{
	uint16_t	address = 0x0000;
	// Maže od adresy 0x0000 po koneènou adresu Bootloaderu
	while (START_BOOT_ADDRESS > address)
	{
		// Vymaže pøíslušnou adresu
		boot_page_erase(address);
		boot_spm_busy_wait();
		address += SPM_PAGESIZE;
	}
}

void WriteLockBits(void)
{

}

void WriteFlashPages(uint16_t page, uint8_t	*buf)
{
	uint16_t i;
	uint8_t sreg;
	
	for (i=0; i<SPM_PAGESIZE; i+=2)
	{
		// Set up little-endian word.

		uint16_t w = *buf++;
		w += (*buf++) << 8;
		
		boot_page_fill (page + i, w);
	}

	boot_page_write (page);     // Store buffer in flash page.
	boot_spm_busy_wait();       // Wait until the memory is written.
}

int main(void)
{
	
    /* Replace with your application code */
	uint8_t	i=0;
	uint8_t	BootStatus=0;
	uint8_t	cnt=0;
	uint8_t ID_Data=0;
	uint8_t	Buffer[SPM_PAGESIZE] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
	uint16_t	address=0x0000;
	cli();
	
	RS232_Init(RS232_115200);
	
	//for (cnt = 0; cnt < 255; cnt++)
	#warning "Pouze Simulator"
	for (cnt = 0; cnt < 2; cnt++)
	{
		if (RS232_Receive_Status() == 'P')
		{
			RS232_Transmit_String_CR("AtomTraceBootLoader");
			BootStatus = 1;
			break;
		}
		_delay_ms(1);
	}
	// Pouze pro pozici Testování simulátoru
	#warning	"Pouze Simulator";
	BootStatus = 1;
	
	if(!BootStatus)
	{
		jumpaddress();
	}
	
    while (1) 
    {
		
		//ID_Data = RS232_Receive_Char();
		#warning "Pouze Simulator"
		ID_Data = 'B';
		
		switch (ID_Data)
		{
			// Enter Programming Mode
			case 'P':
				RS232_Transmit_Char_CR();
				break;
			// Auto increment Address
			case 'a':
				break;
			// Set Adress
			case 'A':
				RS232_Transmit_Char_CR();
				break;
			// Write Program Memory, Low Byte
			case 'c':
				RS232_Transmit_Char_CR();
				break;
			// Write Program Memory, High Byte
			case 'C':
				RS232_Transmit_Char_CR();
				break;
			// Issue Page Write
			case 'm':
				RS232_Transmit_Char_CR();
				break;
			// Read Lock Bits
			case 'r':
				RS232_Transmit_Char(boot_lock_fuse_bits_get(GET_LOCK_BITS));
				break;
			// Read Program Memory
			case 'R':
				
				break;
			// Read Data Memory
			case 'd':
				
				break;
			// Write Data Memory
			case 'D':
				RS232_Transmit_Char_CR();
				break;
			// Chip Erase
			case 'e':
				ChipErase();
				RS232_Transmit_Char_CR();
				break;
			// Write Lock Bits
			case 'l':
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
				
			// 
			case 'L':
				RS232_Transmit_Char_CR();
				break;
				
				
			//Read Signature
			case 's':
				RS232_Transmit_Char(boot_signature_byte_get(SIGNAT_BYTE_0));
				RS232_Transmit_Char(boot_signature_byte_get(SIGNAT_BYTE_1));				
				RS232_Transmit_Char(boot_signature_byte_get(SIGNAT_BYTE_2));				
				break;
			case 'V':
				RS232_Transmit_String(BOOTLOADER_VERSION);
				break;
			case 'E':
				RS232_Transmit_String_CR("Exit from BOOTLOADER. BYE");
				jumpaddress();
				break;
			case 'B':
				// Èást pro naprogramování FLASH
				WriteFlashPages(address, Buffer);
				// Èást pro naprogramování EEPROM
				break;
			
		}
    }
}

