/*
 * F-00009-01.c
 *
 * Created: 8.4.2016 12:42:17
 * Author : atom2
 */ 

#include <avr/io.h>

#include "inc/Define_Setup.h"
#include "inc/AllInit.h"
#include "inc/common_defs.h"
#include "inc/RS232.h"
#include <avr/boot.h>
#include <avr/fuse.h>
#define	F_CPU	14745600UL
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "inc/CRC_interface.h"
#include <avr/eeprom.h>



/************************************************************************/
/* Programovac� pojistky se mus� nastavit
BOOTRSZ = 0
BOOTSZ = "00"

																		*/
/************************************************************************/



/************************************************************************/
/* Definov�n� Bufferu, do kter�ho se vlo�� p�ijat� data                 */
/************************************************************************/

static uint8_t BufferFlash[SPM_PAGESIZE], BufferEeprom[PAGE_SIZE_EEPROM] ={0x01, 0x02, 0x03, 0x04};
static uint16_t	cnt=0;
static uint8_t	i=0;
static uint8_t	BootStatus=0;

// Sko�en� na adresu 0x0000
void	jumpaddress(void)
{
	asm("jmp 0x0000");
}

// Prvn� funkce, kter� zajist� odesl�n� informac�
void FirstFunction(void)
{
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
	// Pouze pro pozici Testov�n� simul�toru
	#warning	"Pouze Simulator";
	BootStatus = 1;
	
	if(!BootStatus)
	{
		jumpaddress();
	}
}

// P�e�ten� lock bit�
void ReadLockBits(void)
{
	
}

// Vymaz�n� cel� aplika�n� pam�ti
void ChipErase(void)
{
	uint16_t	address = 0x0000, konec = 0x0000;
	konec = END_APP_ADDRESS;
	// Ma�e od adresy 0x0000 po kone�nou adresu Bootloaderu
	while (konec > address)
	{
		// Vyma�e p��slu�nou adresu
		
		boot_page_erase(address);
		boot_spm_busy_wait();
		address += SPM_PAGESIZE;
	}
}

void EepromErase(void)
{
	uint16_t	address = 0x0000, konec = 0x0000;
	konec = END_EEPROM_ADDRESS;
	// Ma�e od adresy 0x0000 po kone�nou adresu Bootloaderu
	while (konec > address)
	{
		// Vyma�e p��slu�nou adresu
		if(eeprom_read_byte(address) != 0xFF)
		{
			eeprom_update_byte(address, 0xFF);
		}
		address++;
	}
}


// Zaps�n� Lock bit�
void WriteLockBits(void)
{
	// V t�to verzi nen� z�pis Lock bit� povolen, kv�li zamknut� MCU...
	//boot_lock_bits_set(RS232_Receive_Char());
	//boot_spm_busy_wait();
}

// Verify Flash pam�ti


// Verify EEPROM pam�ti

// Z�pis po pages do pam�ti Flash
void WriteFlashPages(uint16_t address, uint8_t	*Buffer)
{
	uint16_t i;
	uint16_t Data=0;
	for (i=0; i<=SPM_PAGESIZE; i+=2)
	{
		Data = *Buffer++;
		Data |= *Buffer++ << 8;
		// Pln� Page buffer (256 velikost u 644p)
		boot_page_fill (address + i, Data);
	}

	boot_page_write (address);     //Napln� buffer
	boot_spm_busy_wait();       // �ek� dokud se neuvoln�
}

// Z�pis po pages do pam�ti EEPROM
void WriteEepromPages(uint16_t address, uint8_t *Buffer)
{
	uint16_t cnt=0;
	for (cnt = 0; cnt < PAGE_SIZE_EEPROM; cnt++)
	{
// 		Data = *Buffer++;
// 		Data |= *Buffer++ << 8;
		eeprom_update_byte(address++, *Buffer++);
	}
}

// �ten� pam�ti Flash
void ReadFlashPages(uint8_t end)
{
	uint16_t First=0x0000, Last=0x0000, address=0x0000;
	uint16_t n=0;
	
	// Konec 0 - 0x0000 a� konec aplika�n� ��sti
	// Konec 1 - Za��tek Bootloader ��sti a� konec
	// Konec 2 - Cel� pam�
	
	if (end == 0)
	{
		First = START_APP_ADDRESS;
		Last = END_APP_ADDRESS;
	}
	else if (end == 1)
	{
		First = START_BOOT_ADDRESS;
		Last = END_ADDRESS;
	}
	else if(end == 2)
	{
		First = START_APP_ADDRESS;
		Last = END_ADDRESS;
	}
	else
	{
		First = START_APP_ADDRESS;
		Last = END_APP_ADDRESS;
	}
	// Odesl�n� po�et Byt� kolik bude muset celkov� p�ijmout
	n = FLASH / NUM_O_PAGES;
	RS232_Transmit_uint16(n);
	address = First;
	while (address > Last)
	{
		RS232_Transmit_uint16(address);
		for ( i = 0; i < SPM_PAGESIZE; i++)
		{
			RS232_Transmit_Char(pgm_read_byte(address + i));
		}
		address += SPM_PAGESIZE;
	}
}

void ReadEepromPages(void)
{
	uint16_t First=0x0000, Last=0x0000, address=0x0000;
	uint16_t n=0;
	
	// Odesl�n� po�et Byt� kolik bude muset celkov� p�ijmout
	n = FLASH / NUM_O_PAGES;
	RS232_Transmit_uint16(n);
	address = First;
	while (address > Last)
	{
		RS232_Transmit_uint16(address);
		for ( i = 0; i < SPM_PAGESIZE; i++)
		{
			RS232_Transmit_Char(pgm_read_byte(address + i));
		}
		address += SPM_PAGESIZE;
	}
}


int main(void)
{
	
    /* Replace with your application code */

	uint8_t ID_Data=0;
	uint16_t	address=0x0000;
	uint8_t	all=0x00;
	
	cli();
	RS232_Init(RS232_115200);
	
	//FirstFunction();
	
    while (1) 
    {
		
		//ID_Data = RS232_Receive_Char();
		#warning "Pouze Simulator"
		ID_Data = 'g';
		
		switch (ID_Data)
		{
			// Read Lock Bits
			case 'r':
				RS232_Transmit_Char(boot_lock_fuse_bits_get(GET_LOCK_BITS));
				RS232_Transmit_Char_CR();
				break;
			// Chip Erase
			case 'e':
				// P�ijme 1 Byte, kter� rozhodne jestli EEPROM, Flash, nebo oboje
				ID_Data = RS232_Receive_Char();
				
				//#warning "Pouze Simulator"
				//ID_Data = 'F';
				// Chceme Flash nebo EEProm Vymazat
				switch(ID_Data)
				{
					case 'F':
						ChipErase();
						RS232_Transmit_Char(ACK);
						RS232_Transmit_Char_CR();
						break;
					case 'E':
						EepromErase();
						RS232_Transmit_Char(ACK);
						RS232_Transmit_Char_CR();
						break; 
					case 'A':
						ChipErase();
						EepromErase();
						RS232_Transmit_Char(ACK);
						RS232_Transmit_Char_CR();
						break;
					default:
						RS232_Transmit_Char(NAK);
						RS232_Transmit_Char_CR();
					break;
				}
				break;
			// Write Lock Bits
			case 'l':
				WriteLockBits();
				RS232_Transmit_Char(NAK);
				RS232_Transmit_Char_CR();
				break;
			//Read LOW FUSE
			case 'F':
				RS232_Transmit_Char(boot_lock_fuse_bits_get(GET_LOW_FUSE_BITS));
				RS232_Transmit_Char_CR();
				break;
			//Read High FUSE
			case 'N':
				RS232_Transmit_Char(boot_lock_fuse_bits_get(GET_HIGH_FUSE_BITS));
				RS232_Transmit_Char_CR();
				break;
			//Read Extended FUSE
			case 'Q':
				RS232_Transmit_Char(boot_lock_fuse_bits_get(GET_EXTENDED_FUSE_BITS));
				RS232_Transmit_Char_CR();
				break;	
			//Read Signature
			case 's':
				RS232_Transmit_Char(boot_signature_byte_get(SIGNAT_BYTE_0));
				RS232_Transmit_Char(boot_signature_byte_get(SIGNAT_BYTE_1));				
				RS232_Transmit_Char(boot_signature_byte_get(SIGNAT_BYTE_2));				
				RS232_Transmit_Char_CR();
				break;
			// Return Software Identifier
			case 'S':
				RS232_Transmit_String("VA000001");
				RS232_Transmit_Char_CR();
				break;
			// Return Software Version
			case 'V':
				RS232_Transmit_String(BOOTLOADER_VERSION);
				RS232_Transmit_Char_CR();
				break;
			// Exit from Bootloader
			case 'E':
				RS232_Transmit_String("Exit from BOOTLOADER. BYE");
				RS232_Transmit_Char_CR();
				jumpaddress();
				break;
			// Check Block Support (Vytvo�en� Verify pam�ti)
			case 'b':
				VerifyFlash();
				break;
			// Write Data into Flash memory with specific Address
			case 'B':
				// P�ijme 1 Byte, kter� rozhodne jestli 
				ID_Data = RS232_Receive_Char();
				address = RS232_Receive_Char() << 8;
				address |= RS232_Receive_Char();
				// Chceme Flash nebo EEProm naplnit daty
				switch(ID_Data)
				{
					// ��st pro naprogramov�n� FLASH
					case 'F':
						// Zapln� buffer o velikosti 1 str�nky			
						for (cnt = 0; cnt <= SPM_PAGESIZE; cnt++)
						{
							BufferFlash[cnt] = RS232_Receive_Char();
						}
						if (address > (END_APP_ADDRESS - 256))
						{
							RS232_Transmit_Char(NAK);
							RS232_Transmit_Char_CR();
						} 
						else
						{
							WriteFlashPages(address, BufferFlash);
							RS232_Transmit_Char(ACK);
							RS232_Transmit_Char_CR();
						}
						break;
					// ��st pro naprogramov�n� EEPROM
					case 'E':
						for (cnt = 0; cnt < PAGE_SIZE_EEPROM; cnt++)
						{
							//BufferEeprom[cnt] = cnt;
							BufferEeprom[cnt] = RS232_Receive_Char();
						}
						if (address > (END_EEPROM_ADDRESS - 256))
						{
							RS232_Transmit_Char(NAK);
							RS232_Transmit_Char_CR();
						}
						else
						{
							WriteEepromPages(address, BufferEeprom);
							RS232_Transmit_Char(ACK);
							RS232_Transmit_Char_CR();
						}
						break;
				}
				break;
			case 'g':
				// P�ijme 1 Byte, kter� rozhodne jestli 
				ID_Data = RS232_Receive_Char();
				all = RS232_Receive_Char();
				address = RS232_Receive_Char() << 8;
				address |= RS232_Receive_Char();
				switch(ID_Data)
				{
					case 'F':
						// P�e�ten� cel� pam�ti dle stavov�ho automatu
						ReadFlashPages(all, address);	
						break;
					case 'E':
						ReadEepromPages(all, address);
						break;
				}
				break;
		}
    }
}

