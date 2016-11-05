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
#pragma config POSCMD = XT              // Primary Oscillator Mode Select bits (EC (External Clock) Mode)
//#pragma config OSCIOFNC = ON            // OSC2 Pin Function bit (OSC2 is general purpose digital I/O pin)
#pragma config OSCIOFNC = OFF            // OSC2 Pin Function bit (OSC2 is general purpose digital I/O pin)
#pragma config IOL1WAY = OFF            // Peripheral pin select configuration (Allow multiple reconfigurations)
#pragma config FCKSM = CSECMD           // Clock Switching Mode bits (Clock switching is enabled,Fail-safe Clock Monitor is disabled)

// FWDT
#pragma config WDTPOST = PS32768        // Watchdog Timer Postscaler Bits (1:16,384)
#pragma config WDTPRE = PR128            // Watchdog Timer Prescaler bit (1:32)
#pragma config PLLKEN = ON             // PLL Lock Wait Enable bit (Clock switch to PLL source will wait until the PLL lock signal is valid.)
#pragma config WINDIS = OFF             // Watchdog Timer Window Enable bit (Watchdog Timer in Non-Window mode)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable bit (Watchdog timer enabled/disabled by user software)

// FPOR
#pragma config FPWRT = PWR128             // Power-on Reset Timer Value Select bits (Disabled)
#pragma config BOREN = ON             // Brown-out Reset (BOR) Detection Enable bit (BOR is disabled)
#pragma config ALTI2C1 = OFF            // Alternate I2C pins for I2C1 (SDA1/SCK1 pins are selected as the I/O pins for I2C1)
#pragma config ALTI2C2 = OFF            // Alternate I2C pins for I2C2 (SDA2/SCK2 pins are selected as the I/O pins for I2C2)

// FICD
#pragma config ICS = PGD3               // ICD Communication Channel Select bits (Communicate on PGEC1 and PGED1)
#pragma config RSTPRI = PF              // Reset Target Vector Select bit (Device will obtain reset instruction from Primary flash)
#pragma config JTAGEN = OFF             // JTAG Enable bit (JTAG is disabled)

// FAS -- need to read
#pragma config AWRP = OFF               // Auxiliary Segment Write-protect bit (Auxiliary program memory is not write-protected)
#pragma config APL = OFF                // Auxiliary Segment Code-protect bit (Aux Flash Code protect is disabled)
#pragma config APLK = OFF               // Auxiliary Segment Key bits (Aux Flash Write Protection and Code Protection is Disabled)

void TDelayms( unsigned t);
void InitClock(void);
void InitPorts(void);

char buffer[128*3 +1];
unsigned long temp1, temp2, temp3;

int main(void) {
    
//    char Rcv_byte; 
    unsigned int loop;
    InitClock();
    UART1Init();
    InitPorts();

    for (loop = 0; loop < (128*3); loop++)
    {
        buffer[loop]= 0xAB;
    }
    
    UART1TxString("Flashing Page \n");
    FM_PageErase(0x02, 0x2000);
    UART1TxString("Page Erased \n");
    TDelayms(500);
    
    UART1TxString("Writing Latches \n");
    Row_WriteLatches(&buffer[0]);
    UART1TxString("Latches Written \n");
    
    
    UART1TxString("Row Programming starts \n");
    FM_Single_Row_Prog (0x02, 0x2000);
    UART1TxString("Row Programming Ends \n");
    TDelayms(500);
    
 // last data byte written address will increment by 2 -> 128*2 = 254, new row address is 0x2100   
    temp1 = FM_MemRead(0x02, 0x2100); // new row address
    temp2 = FM_MemRead(0x02, 0x20FE); // last
    temp3 = FM_MemRead(0x02, 0x20FC); // 2nd last
    
    temp1 = FM_MemRead(0xFF, 0x0000); // new row address
    temp2 = FM_MemRead(0xFF, 0x0002); // last
    
    while (1) {
        LATBbits.LATB5 = 0;
        TDelayms(500);
        LATBbits.LATB5 = 1;
        TDelayms(
                500);
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