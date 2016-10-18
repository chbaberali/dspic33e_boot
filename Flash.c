/*
 * File:   Flash.c
 * Author: root
 *
 * Created on October 9, 2016, 8:14 PM
 */


#include "xc.h"
#include "Flash_Setup.h"

#define FLASH_ROW_PROG_CODE     0x4002 //write double word
#define FLASH_PAGE_ERASE_CODE   0x4003 //erase entire page


#define BYTES_PER_INSTRUCTION  3 
#define INSTR_PER_ROW 128
#define ROW_PER_PAGE   8
#define BYTES_PER_ROW (INSTR_PER_ROW * BYTES_PER_INSTRUCTION)
#define PM_ERASE_SIZE (INSTR_PER_ROW * BYTES_PER_INSTRUCTION * ROW_PER_PAGE)    //default erase size

/* *********************************************************************************
********************************************************************************** */

void FM_PageErase(unsigned int TablePage, unsigned int Addrhi, unsigned int  Addrlow)
{
    // Note: The program memory must be erased at an ?even? page address boundary (data sheet)
    asm ("push    TBLPAG");
    TBLPAG = TablePage;
    NVMADRU = Addrhi;
    NVMADR  = Addrlow;
    NVMCON  = FLASH_PAGE_ERASE_CODE;
    asm("disi #06");
    asm("mov #0x55,W0");
    asm("mov W0, NVMKEY");
    asm("mov #0xAA,W0");
    asm("mov W0,NVMKEY");
    // Start erase operation
    asm("bset NVMCON,#15");
    // Insert two NOPs after the erase cycle (required)
    asm("nop");
    asm("nop");
    while (NVMCONbits.WR == 0); //Wait for write end
	asm ("pop     TBLPAG");
}

void FM_PageWrite(unsigned int TablePage, unsigned int  Addrlow, unsigned char *data)
{
    
}
unsigned long FM_MemRead(unsigned int TablePage, unsigned int TableOffset)
{
    unsigned long Temp;
    TBLPAG = TablePage;
    Temp = __builtin_tblrdh(TableOffset);
    Temp = (Temp << 16 ) | __builtin_tblrdl(TableOffset);
    return Temp;
}
/*
 * see header file for detail
 */
void WriteLatches(unsigned int TablePage,  unsigned int offset, char *data)
{
    
    TBLPAG = TablePage; // address of latch is 0xFA
   //void __builtin_tblwtl(unsigned int offset, unsigned int data);
    __builtin_tblwtl(offset , *data); //equivalent to asm("  tblwtl W3,[W1]")  asm ("mov #0 , W1");
    __builtin_tblwth(offset , *data); //equivalent to asm("  tblwth W2,[W1]")
}
unsigned long ReadLatches(unsigned int TablePage, unsigned int TableOffset)
{
    unsigned long Temp;
    TBLPAG = TablePage;
    Temp = __builtin_tblrdh(TableOffset);
    Temp = (Temp << 16 ) | __builtin_tblrdl(TableOffset);
    return Temp;
}

void FlashPageRead()
{
    asm(" push    TBLPAG ");
    TBLPAG = 0;
    
    
}
void Pagewrite()
{
    
}
