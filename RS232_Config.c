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
    U1MODEbits.STSEL = 0;                    // 1-Stop bit
    U1MODEbits.PDSEL = 0;                    // No Parity, 8-Data bits
    U1MODEbits.ABAUD = 0;                    // Auto-Baud disabled
    U1MODEbits.BRGH  = 0;                    // Standard-Speed mode, for fast mode set this bit to 1 and the constant to 4
    U1BRG            = BAUD;                 // set baud rate generator Baud Rate setting for 9600
    U1STAbits.UTXISEL0 = 0;                  // Interrupt after one TX character is transmitted
    U1STAbits.UTXISEL1 = 0;
    IEC0bits.U1TXIE    = 1;                  // Enable UART TX interrupt
    U1MODEbits.UARTEN  = 1;                  // Enable UART
    U1STAbits.UTXEN    = 1;                  // Enable UART TX
}
 
// writes a byte to UART1
void UART1TxByte(char byte)
{
    U1TXREG = byte;
	if(U1STAbits.UTXBF) 
		while(!U1STAbits.TRMT);	
}

// reads a byte from UART 1
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
