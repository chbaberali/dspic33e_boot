/*
 * File:   main.c
 * Author: root
 *
 * Created on October 6, 2016, 5:17 PM
 */

/*
 * Notes see section 1 notes.txt
 */

#include <xc.h>
#include "RS232_header.h"
#include "Flash_Setup.h"
#include "p33EP512MU810.h"


#pragma config GWRP = OFF               // General Segment Write-Protect bit (General Segment may be written)
#pragma config GSS = OFF                // General Segment Code-Protect bit (General Segment Code protect is disabled)
#pragma config GSSK = OFF               // General Segment Key bits (General Segment Write Protection and Code Protection is Disabled)

// FOSCSEL
#pragma config FNOSC = FRC              // Initial Oscillator Source Selection Bits (Internal Fast RC (FRC))
#pragma config IESO = OFF                // Two-speed Oscillator Start-up Enable bit (Start up with user-selected oscillator source)

// FOSC
//#pragma config POSCMD = EC              // Primary Oscillator Mode Select bits (EC (External Clock) Mode)
#pragma config POSCMD = XT              // Primary Oscillator Mode Select bits (EC (External Clock) Mode)
//#pragma config OSCIOFNC = ON            // OSC2 Pin Function bit (OSC2 is general purpose digital I/O pin)
#pragma config OSCIOFNC = OFF            // OSC2 Pin Function bit (OSC2 is general purpose digital I/O pin)
#pragma config IOL1WAY = OFF            // Peripheral pin select configuration (Allow multiple reconfigurations)
#pragma config FCKSM = CSECMD           // Clock Switching Mode bits (Clock switching is enabled,Fail-safe Clock Monitor is disabled)

// FWDT
#pragma config WDTPOST = PS16384        // Watchdog Timer Postscaler Bits (1:16,384)
#pragma config WDTPRE = PR32            // Watchdog Timer Prescaler bit (1:32)
#pragma config PLLKEN = ON             // PLL Lock Wait Enable bit (Clock switch to PLL source will wait until the PLL lock signal is valid.)
#pragma config WINDIS = OFF             // Watchdog Timer Window Enable bit (Watchdog Timer in Non-Window mode)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable bit (Watchdog timer enabled/disabled by user software)

// FPOR
#pragma config FPWRT = PWR128             // Power-on Reset Timer Value Select bits (Disabled)
#pragma config BOREN = ON              // Brown-out Reset (BOR) Detection Enable bit (BOR is disabled)
#pragma config ALTI2C1 = OFF            // Alternate I2C pins for I2C1 (SDA1/SCK1 pins are selected as the I/O pins for I2C1)
#pragma config ALTI2C2 = OFF            // Alternate I2C pins for I2C2 (SDA2/SCK2 pins are selected as the I/O pins for I2C2)

// FICD
#pragma config ICS = PGD1               // ICD Communication Channel Select bits (Communicate on PGEC1 and PGED1)
#pragma config RSTPRI = PF              // Reset Target Vector Select bit (Device will obtain reset instruction from Primary flash)
#pragma config JTAGEN = OFF             // JTAG Enable bit (JTAG is disabled)

// FAS -- need to read
#pragma config AWRP = OFF               // Auxiliary Segment Write-protect bit (Auxiliary program memory is not write-protected)
#pragma config APL = OFF                // Auxiliary Segment Code-protect bit (Aux Flash Code protect is disabled)
#pragma config APLK = OFF               // Auxiliary Segment Key bits (Aux Flash Write Protection and Code Protection is Disabled)


void TDelayms( unsigned t);
unsigned long temp1, temp2;

//unsigned char buffer[128*3 +1]; // receive buffer 128*3*8
//unsigned int BuffCount;
unsigned char Rcv_byte;
    
int main(void) {
    //unsigned char Result; 
    /*
     * configure Oscillator
     */
//    unsigned int i = 0; 
    
//    __builtin_write_OSCCONH( 0x02 );            // Initiate Clock Switch to External // NOSC = 0x02,
//    __builtin_write_OSCCONL( OSCCON || 0x01 );  // Start clock switching
//	while (OSCCONbits.COSC != 0x02);            // Wait for Shifting to new Oscillator
    
    PLLFBD=38;                          // M=40
	CLKDIVbits.PLLPOST= 0x0;			// N1=2
	CLKDIVbits.PLLPRE = 0x0; 			// N2=2
	__builtin_write_OSCCONH(0x03);		// New Oscillator Select HS(external)
	__builtin_write_OSCCONL(0x01);		// Enable Shift to External Oscillator
	while (OSCCONbits.COSC != 0b011);	// Wait for Shifting to new Oscillator
	while(OSCCONbits.LOCK != 1) {};     // Wait for PLL to lock
    RCONbits.SWDTEN = 0;      // Disable Watch Dog Timer
    /*
     * try to blink LED here
     */  
    TRISBbits.TRISB5 = 0;
//    for (i=0; i < sizeof(buffer)-1; i++) {
//        buffer[i] = 0x65; // this needs to be persistent so as to not step on persistent variables in user's program
//    }
////    FM_PageErase(0x022000);
//      FM_PageErase(0x02, 0x2000);
//      //FM_Page_Write (0x02, 0x2000, &buffer[0]);
//      FM_Single_Row_Prog (0x02, 0x2000, &buffer[0]);
//      temp1 = FM_MemRead(0x00, 0x0200);
//      temp2 = FM_MemRead(0x00, 0x0202);
    
    
    UART1Init();
    while (1) {
        LATBbits.LATB5 = 0;
        UART1TxString("dsPIC33ep512MU810\n");
//        Rcv_byte = UART1RxByte(200);
//        if (Rcv_byte)
//        {
//            UART1TxByte(Rcv_byte+0x30);
//        }
        TDelayms( 500);
        LATBbits.LATB5 = 1;
        TDelayms( 500);
    }
    return 0;
}

void TDelayms( unsigned t)
{
    T1CON = 0x8000;     // enable tmr1, Tcy, 1:1
    while (t--)
    {
        TMR1 = 0;
        while (TMR1<40000);
    }
    T1CON = 0x0000;
}

void BL_Receive_Cmd(void)
{

    Rcv_byte = UART1RxByte(200);
    switch (Rcv_byte)
    {
        case CMD_DEV_ID: 
            temp1 = FM_MemRead(0xFF, 0x0000);
            temp2 = FM_MemRead(0xFF, 0x0002);
            // send via UART
            break;
            
        case CMD_BL_VER: 
            temp1 = 0x01;
            temp2 = 0x00;
            break;
        case CMD_WRITE_FLASH: 
            // reserve last two pages of PM
            break;
        case CMD_READ_FLASH: 
            // read specific location using FM_MemRead(0xFF, 0x0000);
            break;
        case CMD_RESET_DEV: 
            asm("RESET");
            break;
        default:
            break;
    }
}