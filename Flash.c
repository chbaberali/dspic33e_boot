/*
 * File:   Flash.c
 * Author: root
 *
 * Created on October 9, 2016, 8:14 PM
 */


#include <p33EP512MU810.h>
#include "xc.h"
#include "Flash_Setup.h"

#define FLASH_WORD_PROG_CODE    0x4001
#define FLASH_ROW_PROG_CODE     0x4002 //write row
#define FLASH_PAGE_ERASE_CODE   0x4003 //erase entire page


//#define BYTES_PER_INSTRUCTION  2 
//#define INSTR_PER_ROW 128
//#define ROW_PER_PAGE   8
//#define BYTES_PER_ROW (INSTR_PER_ROW * BYTES_PER_INSTRUCTION)
//#define PM_ERASE_SIZE (INSTR_PER_ROW * BYTES_PER_INSTRUCTION * ROW_PER_PAGE)    //default erase size
/* *********************************************************************************
********************************************************************************** */
void FM_PageErase(unsigned int TablePage, unsigned int offset) // tested
{
    // Note: The program memory must be erased at an even page address boundary (data sheet)
    // Set up the NVMADR registers to the starting address of the page (0x022000);
    asm("push    TBLPAG");
    NVMADRU = TablePage;
    NVMADR = offset;
    
    // Set up NVMCON to erase one page of Program Memory
    NVMCON  = FLASH_PAGE_ERASE_CODE;
    // Disable interrupts < priority 7 for next 5 instructions>
    asm  ("DISI #06");
    // Write the KEY Sequence
    __builtin_write_NVM();
    NVMCONbits.WR = 1; // start the erase cycle
    __builtin_nop();
    __builtin_nop(); 
    while(NVMCONbits.WR); // wait until erase cycle is complete
    asm("pop     TBLPAG");
}
void FM_Single_Row_Prog (unsigned int TablePage, unsigned int offset) // tested
{
    // Load the NVMADR register with the starting <programming address>
    NVMADRU = TablePage;
    NVMADR = offset;
    // Setup NVMCON to write <1> row of program memory
    NVMCON = FLASH_ROW_PROG_CODE ;
    //Block all interrupt till write is complete
    asm volatile ("DISI #06");
    // sequence 
    __builtin_write_NVM();
    NVMCONbits.WR = 1; // Start the programming sequence
    __builtin_nop();
    __builtin_nop(); 
    while(NVMCONbits.WR); // wait until programming sequence is complete
}
unsigned long FM_MemRead(unsigned int TablePage, unsigned int TableOffset) //tested
{
    /*
     *     data1 = FM_MemRead(0xF8, 0x0006); // tested
     */
    unsigned long Temp;
    TBLPAG = TablePage;
    Temp = __builtin_tblrdh(TableOffset);
    Temp = (Temp << 16 ) | __builtin_tblrdl(TableOffset);
    return Temp;
}
void Row_WriteLatches(char *data)
{
    asm ("mov W0, W2"); // w2 points to the address of data to be written
    asm("push    TBLPAG");
    TBLPAG = 0xFA; // address of latch is 0xFA
    asm("mov #0,W1"); // Lower 16-bit of Write Latches starts from 0
    asm("MOV #128,W3");
    asm("loop:");  
    asm("TBLWTH.b [W2++], [W1++]");  // TBLWTH Ws,Wd -> Write Ws 7:0 to Prog 23:16
    asm("TBLWTL.b [W2++], [W1--]"); //y TBLWTH Ws,Wd -> Write Ws to Prog 15:0
    asm("TBLWTL.b [W2++], [W1]");
    asm("INC2 W1, W1");
    asm("DEC W3, W3");
    asm("BRA NZ, loop");
    asm("pop     TBLPAG");
}
