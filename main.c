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
#include "p33EP512MU810.h"

#pragma config GWRP = OFF               // General Segment Write-Protect bit (General Segment may be written)
#pragma config GSS = OFF                // General Segment Code-Protect bit (General Segment Code protect is disabled)
#pragma config GSSK = OFF               // General Segment Key bits (General Segment Write Protection and Code Protection is Disabled)

// FOSCSEL
#pragma config FNOSC = FRC              // Initial Oscillator Source Selection Bits (Internal Fast RC (FRC))
#pragma config IESO = ON                // Two-speed Oscillator Start-up Enable bit (Start up device with FRC, then switch to user-selected oscillator source)

// FOSC
#pragma config POSCMD = EC              // Primary Oscillator Mode Select bits (EC (External Clock) Mode)
#pragma config OSCIOFNC = ON            // OSC2 Pin Function bit (OSC2 is general purpose digital I/O pin)
#pragma config IOL1WAY = OFF            // Peripheral pin select configuration (Allow multiple reconfigurations)
#pragma config FCKSM = CSECME           // Clock Switching Mode bits (Both Clock switching and Fail-safe Clock Monitor are enabled)

// FWDT
#pragma config WDTPOST = PS16384        // Watchdog Timer Postscaler Bits (1:16,384)
#pragma config WDTPRE = PR32            // Watchdog Timer Prescaler bit (1:32)
#pragma config PLLKEN = OFF             // PLL Lock Wait Enable bit (Clock switch to PLL source will wait until the PLL lock signal is valid.)
#pragma config WINDIS = OFF             // Watchdog Timer Window Enable bit (Watchdog Timer in Non-Window mode)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable bit (Watchdog timer enabled/disabled by user software)

// FPOR
#pragma config FPWRT = PWR1             // Power-on Reset Timer Value Select bits (Disabled)
#pragma config BOREN = OFF              // Brown-out Reset (BOR) Detection Enable bit (BOR is disabled)
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

// _PERSISTENT char buffer[128*3]; // this needs to be persistent so as to not step on persistent variables in user's program

int main(void) {
    unsigned char Result; 
    /*
     * configure Oscillator
     */
    __builtin_write_OSCCONH( 0x02 );            // Initiate Clock Switch to External // NOSC = 0x02,
    __builtin_write_OSCCONL( OSCCON || 0x01 );  // Start clock switching
	while (OSCCONbits.COSC != 0x02);       // Wait for Shifting to new Oscillator
    
    /*
     * try to blink LED here
     */
    
    UART1Init();
    
    while (1) {
        UART1TxString("dsPIC33ep512MU810 \n");
    }
    return 0;
}

void __attribute__((__interrupt__,auto_psv)) _U1TXInterrupt(void)
{
    IFS0bits.U1TXIF = 0;                     // Clear TX Interrupt flag
}