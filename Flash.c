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


#define BYTES_PER_INSTRUCTION  3 
#define INSTR_PER_ROW 128
#define ROW_PER_PAGE   8
#define BYTES_PER_ROW (INSTR_PER_ROW * BYTES_PER_INSTRUCTION)
#define PM_ERASE_SIZE (INSTR_PER_ROW * BYTES_PER_INSTRUCTION * ROW_PER_PAGE)    //default erase size

/* *********************************************************************************
********************************************************************************** */

void FM_PageErase(unsigned char TablePage, unsigned int offset) // tested
{
    // Note: The program memory must be erased at an ?even? page address boundary (data sheet)
    asm ("push SR");
    asm("push    TBLPAG");
    // Set up the NVMADR registers to the starting address of the page (0x022000);
    
      NVMADRU = TablePage;
      NVMADR = offset;
    
    // Set up NVMCON to erase one page of Program Memory
    NVMCON  = FLASH_PAGE_ERASE_CODE;
    // Disable interrupts < priority 7 for next 5 instructions>
    asm("mov#0x00E0, W0");
    asm ("ior SR");
    // Write the KEY Sequence
    asm("mov #0x55,W0");
    asm("mov W0, NVMKEY");
    asm("mov #0xAA,W0");
    asm("mov W0,NVMKEY");
    // Start <erase> operation
    asm("bset NVMCON,#15");
    // Insert two NOPs after the erase cycle (required)
    asm("nop");
    asm("nop");
    while (NVMCON & 0x8000); //Wait for write end
    asm ("pop    SR");
    asm("pop     TBLPAG");
}

void FM_Single_Row_Prog (unsigned char TablePage, unsigned int offset, unsigned char *data)
{
    /*
     *  w0 = TablePage
     *  w1 = offset
     *  w2 = data
     */
      asm ("push SR");
      asm ("push TBLPAG");
      asm("mov w2,W8"); // save data address

    // Load the NVMADR register with the starting <programming address> 
      NVMADRU = TablePage;
      NVMADR = offset;
      
    // Setup NVMCON to write <1> row of program memory
    NVMCON = FLASH_ROW_PROG_CODE ;
    
    // Load the program memory write latches
      Row_WriteLatches();
      
    //Block all interrupt till write is complete
        asm("mov#0x00E0, W0");
        asm ("ior SR"); 
        // sequence 
   
        asm("mov #0x55,W0");
        asm("mov W0, NVMKEY");
        asm("mov #0xAA,W0");
        asm("mov W0,NVMKEY");
        // Start erase operation
        asm("bset NVMCON,#15");
        // Insert two NOPs after the erase cycle (required)
        asm("nop");
        asm("nop");
asm("wait:");
        asm("btsc    NVMCON, #15");
        asm("bra     wait");
        asm ("pop    SR");
        asm ("pop TBLPAG");
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
/*
 * see header file for detail
 */
void Row_WriteLatches(void)
{
    TBLPAG = 0xFA; // address of latch is 0xFA
    asm("mov #0x00,W7"); // Lower 16-bit of Write Latches starts from 0
    asm("MOV #128,W3");
asm("loop:");  
    asm("TBLWTH.b [W8++], [W7]");
    asm("TBLWTL.b [W8++], [W7++]");
    asm("TBLWTL.b [W8++], [W7++]");
    asm("DEC W3, W3");
    asm("BRA NZ, loop");
}
void FM_TWO_Word_Prog (unsigned char TablePage, unsigned int offset, unsigned int *data)
{
    /*
     *  w0 = TablePage
     *  w1 = offset
     *  w2 = data
     */
      asm ("push SR");
      asm ("push TBLPAG");

    // Load the NVMADR register with the starting <programming address> 
      NVMADRU = TablePage;
      NVMADR = offset;
      
    // Setup NVMCON to write <1> row of program memory
    NVMCON = FLASH_WORD_PROG_CODE ;
    
    // Load the program memory write latches
     TwoWord_WriteLatches();
      
    //Block all interrupt till write is complete
        asm("mov#0x00E0, W0");
        asm ("ior SR"); 
        // sequence 
   
        asm("mov #0x55,W0");
        asm("mov W0, NVMKEY");
        asm("mov #0xAA,W0");
        asm("mov W0,NVMKEY");
        // Start erase operation
        asm("bset NVMCON,#15");
        // Insert two NOPs after the erase cycle (required)
        asm("nop");
        asm("nop");
asm("wait1:");
        asm("btsc    NVMCON, #15");
        asm("bra     wait1");
        asm ("pop    SR");
        asm ("pop TBLPAG");
}
void TwoWord_WriteLatches(void)
{
    TBLPAG = 0xFA; // address of latch is 0xFA
    asm("mov #0x00,W7"); // Lower 16-bit of Write Latches starts from 0

    asm("TBLWTL [W2++], [W7]");
    asm("TBLWTH [W2++], [W7++]");
    asm("TBLWTL [W2++], [W7]");
    asm("TBLWTH [W2++], [W7++]");

}
