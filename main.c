/*
 * File:   newmainXC16.c
 * Author: root
 *
 * Created on October 26, 2016, 3:42 AM
 */


#include <xc.h>
#include "RS232_header.h"
#include "p33EP512MU810.h"
#include "Flash_Setup.h"


#pragma config GWRP = OFF               // General Segment Write-Protect bit (General Segment may be written)
#pragma config GSS = OFF                // General Segment Code-Protect bit (General Segment Code protect is disabled)
#pragma config GSSK = OFF               // General Segment Key bits (General Segment Write Protection and Code Protection is Disabled)

// FOSCSEL
#pragma config FNOSC = FRC            // Initial Oscillator Source Selection Bits
#pragma config IESO = ON                // Two-speed Oscillator Start-up Enable bit (Start up with user-selected oscillator source)

// FOSC
//#pragma config POSCMD = EC              // Primary Oscillator Mode Select bits (EC (External Clock) Mode)
//#pragma config OSCIOFNC = ON            // OSC2 Pin Function bit (OSC2 is general purpose digital I/O pin)
#pragma config POSCMD = XT              // Primary Oscillator Mode Select bits (XT Crystal Oscillator Mode)
#pragma config OSCIOFNC = OFF           // OSC2 Pin Function bit (OSC2 is clock output)
#pragma config IOL1WAY = OFF            // Peripheral pin select configuration (Allow multiple reconfigurations)
#pragma config FCKSM = CSECMD           // Clock Switching Mode bits (Clock switching is enabled,Fail-safe Clock Monitor is disabled)


// FWDT
#pragma config WDTPOST = PS32768        // Watchdog Timer Postscaler Bits (1:32,768)
#pragma config WDTPRE = PR128           // Watchdog Timer Prescaler bit (1:128)
#pragma config PLLKEN = ON              // PLL Lock Wait Enable bit (Clock switch to PLL source will wait until the PLL lock signal is valid.)
#pragma config WINDIS = OFF             // Watchdog Timer Window Enable bit (Watchdog Timer in Non-Window mode)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable bit (Watchdog timer enabled/disabled by user software)

// FPOR
#pragma config FPWRT = PWR128           // Power-on Reset Timer Value Select bits (128ms)
#pragma config BOREN = ON               // Brown-out Reset (BOR) Detection Enable bit (BOR is enabled)
#pragma config ALTI2C1 = OFF            // Alternate I2C pins for I2C1 (SDA1/SCK1 pins are selected as the I/O pins for I2C1)
#pragma config ALTI2C2 = OFF            // Alternate I2C pins for I2C2 (SDA2/SCK2 pins are selected as the I/O pins for I2C2)

// FICD
#pragma config ICS = PGD3               // ICD Communication Channel Select bits (Communicate on PGEC3 and PGED3)
#pragma config RSTPRI = AF              // Reset Target Vector Select bit (Device will obtain reset instruction from Aux flash)
#pragma config JTAGEN = OFF             // JTAG Enable bit (JTAG is disabled)

// FAS
#pragma config AWRP = OFF               // Auxiliary Segment Write-protect bit (Auxiliary program memory is not write-protected)
#pragma config APL = OFF                // Auxiliary Segment Code-protect bit (Aux Flash Code protect is disabled)
#pragma config APLK = OFF               // Auxiliary Segment Key bits (Aux Flash Write Protection and Code Protection is Disabled)

/********************
        Defines
********************/

//#define BYTES_PER_INSTRUCTION  2 
//#define INSTR_PER_ROW 128
//#define ROW_PER_PAGE   8
//#define BYTES_PER_ROW (INSTR_PER_ROW * BYTES_PER_INSTRUCTION)
//#define PM_ERASE_SIZE (INSTR_PER_ROW * BYTES_PER_INSTRUCTION * ROW_PER_PAGE)    //default erase size

#define BL_START_Table_page     0x0002
#define BL_Table_Page_offset    0x2000

/********************
        Functions
********************/
void TDelayms( unsigned t);
void InitClock(void);
void InitPorts(void);
void Bootloader_cmd(char);

/********************
        Variables
********************/
char buffer[128*3 + 2] = {0xFF};
char str[20];
unsigned int loop_var,high,low; 
unsigned long temp1, temp2, temp3;
unsigned char Command,Interrupt,done;
unsigned int byteloop,bytecounter,pagecounter, rcv_counter;

/********************
        Main
********************/
int main(void) {
    InitClock();
    UART1Init();
    InitPorts();
    UART1TxString ("hello world \n");
    while (1) {
        
        LATBbits.LATB5 = ~LATBbits.LATB5;
        TDelayms(500);
        if (Interrupt)
        {
            Interrupt = 0;
            Bootloader_cmd(buffer[0]);
        }
            
    }
    return 0;
}
void TDelayms( unsigned t)
{
    T2CON = 0x8000;     // enable tmr1, Tcy, 1:1
    while (t--)
    {
        TMR2 = 0;
        while (TMR2<40000);
    }
    T2CON = 0x0000;
}
void InitClock(void)
{
    PLLFBD=38;                          // M=40
	CLKDIVbits.PLLPOST= 0x0;			// N1=2
	CLKDIVbits.PLLPRE = 0x0; 			// N2=2
    RCONbits.SWDTEN = 0;                // Disable Watch Dog Timer
    // at this point processor should be running at 40MHz
	__builtin_write_OSCCONH(0x03);		// New Oscillator Select XT(external)
	__builtin_write_OSCCONL( OSCCON || 0x01 );		// Enable Shift to External Oscillator
	while (OSCCONbits.COSC != 0b011);	// Wait for Shifting to new Oscillator
	while(OSCCONbits.LOCK != 1) {};     // Wait for PLL to lock
}
void InitPorts(void)
{
    TRISBbits.TRISB5 = 0;
}
void Bootloader_cmd(char Command)
{
    switch(Command)
    {
        case 0x01:
            rcv_counter = 0;
            temp2 = FM_MemRead(0xFF, 0x0002); // read devid
            UART1TxString("Device ID is 0x");
            UART1TxByte( ((temp2 >> 12) & 0x0F)+0x30 );
            UART1TxByte( ((temp2 >> 8) & 0x0F) +0x30 );
            UART1TxByte( ((temp2 >> 4) & 0x0F) +0x30 );
            UART1TxByte( ((temp2 ) & 0x0F) +0x30 );
            UART1TxByte(13);
            break;
        case 0x02: 
            UART1TxString("Resetting Device\n");
            TDelayms(500);
            asm ("reset"); 
            break;
        case 0x03:
            for (loop_var = 0; loop_var < 512; loop_var+=2)
            {
                temp2 = FM_MemRead(BL_START_Table_page, BL_Table_Page_offset + loop_var); // new row address
                UART1TxString("data is 0x");
                UART1TxByte( hexDigit( (temp2 >> 20) & 0x0F)  );
                UART1TxByte( hexDigit( (temp2 >> 16) & 0x0F)  );
                UART1TxByte( hexDigit( (temp2 >> 12) & 0x0F)  );
                UART1TxByte( hexDigit( (temp2 >> 8) & 0x0F)  );
                UART1TxByte( hexDigit( (temp2 >> 4) & 0x0F)  );
                UART1TxByte( hexDigit( temp2 & 0x0F));
                UART1TxByte(13);
                UART1TxByte(10);
            }
            rcv_counter = 0;
            break;
        case 0x04:
            byteloop = 0;
            bytecounter = 0x0;
            pagecounter = 0x0;
            rcv_counter = 0; 
            done = 0;
     
            FM_PageErase(BL_START_Table_page, BL_Table_Page_offset);
            UART1TxString("OK\n");  // tell pyton device is ready
            while (done != 1)
            {
                while (rcv_counter != 384); // wait until a complete row is not received

                    Row_WriteLatches(&buffer[0]);
                    FM_Single_Row_Prog ((BL_START_Table_page + pagecounter), (BL_Table_Page_offset + bytecounter));
                    
                    bytecounter += 256;
                    if(bytecounter == 2048) // if one page is written /* need to check the maths again must be 0xFFFF*/
                        {
                            pagecounter += 1;
                            bytecounter = 0x0000;
                            FM_PageErase(BL_START_Table_page + pagecounter, BL_Table_Page_offset + bytecounter);
                        }
                    if(buffer[384] == 'F') // F = finish
                    {
                        done = 1;
                        UART1TxString("KO\n");
                    }
                    else
                    {
                        UART1TxString("OK\n");  // tell host that we have written 1 row
                    }
                    rcv_counter = 0;
            }
            break;
        case 0x05: 
            rcv_counter = 0; 
            asm("goto 0x22000");              // jump to application address for example 0x220000
            asm("nop");
            break;
        default:
            rcv_counter = 0;
            break;
    }
}
void __attribute__((__interrupt__,no_auto_psv)) _Aux_Interrupt(void)
{
    if( IFS0bits.U1RXIF )
    {
      Interrupt = 1;
      buffer[rcv_counter++] = U1RXREG & 0xFF;
      IFS0bits.U1RXIF = 0;                     // Clear RX Interrupt flag
    }
    if (IFS0bits.U1TXIF)
    {
       IFS0bits.U1TXIF = 0;                     // Clear TX Interrupt flag
    }
}


