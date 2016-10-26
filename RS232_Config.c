/*
 * File:   RS232_Config.c
 * Author: root
 *
 * Created on October 6, 2016, 5:33 PM
 */


#include "xc.h"
#include "RS232_header.h"

/********************
 UART  Functions
********************/
 
// initializes UART1 at specified baud rate
// FCY defined in Header file
void UART1Init()
{
    /*
     * RPPORx register to control output pin 
     * RPINRx 
     * 
     * RP is remap-able peripheral and n is pin number  
     * 
     * Therefore, when configuring the RPn pin for input, the corresponding bit in the
     * TRISx register must be configured for input
     * 
     * example for UART 1
     * 
     * RPINR18bits.U1RXR = 0x10;  // Assign U1Rx To Pin RP16 (input)
     * RPINR18bits.U1CTSR = 0x11; //  Assign U1CTS To Pin RP17
     * 
     * RPOR1bits.RP66 = 1; //  Assign U1Tx To Pin RP66 
     * RPOR1bits.RP67 = 2; //  Assign U1RTS To Pin RP67
     */
    
    TRISDbits.TRISD1 = 0;                   // configure as output
    TRISDbits.TRISD12 = 1;                  // configure as input
    
    RPOR0bits.RP65R0 = 1;                   //  Assign U1Tx To Pin RP65
    RPINR18bits.U1RXR = 76;                 // Assign U1Rx To Pin RPI_76 (input)
    
    U1MODEbits.STSEL = 0;                    // 1-Stop bit
    U1MODEbits.PDSEL = 0;                    // No Parity, 8-Data bits
    U1MODEbits.ABAUD = 0;                    // Auto-Baud disabled
    U1MODEbits.BRGH  = 0;                    // Standard-Speed mode, for fast mode set this bit to 1 and the constant to 4
    U1BRG            = BAUD;                 // set baud rate generator Baud Rate setting for 9600
    U1STAbits.UTXISEL0 = 0;                  // Interrupt after one TX character is transmitted
    U1STAbits.UTXISEL1 = 0;
    IFS0bits.U1TXIF    = 0;                  // Clear UART Tx interrupt flag
    IEC0bits.U1TXIE    = 1;                  // Enable UART TX interrupt
    U1STAbits.UTXEN    = 1;                  // Enable UART TX
    U1MODEbits.UARTEN  = 1;                  // Enable UART
    
}
 
// writes a byte to UART1
void UART1TxByte(char byte)
{
    U1TXREG = byte;
	if(U1STAbits.UTXBF) 
		while(!U1STAbits.TRMT);	
}

// reads a byte from UART 1 polling 
char UART1RxByte(unsigned int timeout)
{
    if (U1STAbits.OERR == 1)
    {
        U1STAbits.OERR = 0; // Clear overrun error 
        return -1;
    }
    // check here for appropriate bits
    while (!U1STAbits.URXDA && timeout > 0)	// wait for data to be available
    timeout--;
    return U1RXREG;		// read the character from the receive buffer			// return data byte
}
// writes a string from RAM to UART1
void UART1TxString(char *str)
{
    int i = 0;
    while(str[i] != 0) 
    {
        UART1TxByte(str[i++]);
    }
}
void EnableU1RxInterrupt()
{
	IEC0bits.U1RXIE = 1; // Enable UART Rx interrupt
}
void DisableU1Rxinterrupt()
{	
	IEC0bits.U1RXIE = 0; // dsiable  UART Rx interrupt	
}

void __attribute__((__interrupt__,auto_psv)) _U1TXInterrupt(void)
{
    IFS0bits.U1TXIF = 0;                     // Clear TX Interrupt flag
}

