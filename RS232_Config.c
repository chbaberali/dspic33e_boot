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
     * 
     * UARTs is connected in this manner:
     * U1TX on RB5	(RP65)
     * U1RX on RB6	(RPI76)
     */
    
//    TRISDbits.TRISD1 = 0;                   // configure as output
//    TRISDbits.TRISD12 = 1;                  // configure as input
 //*************************************************************
    // Unlock Registers

   
//    RPOR0bits.RP65R = 0b00001;              //  Assign U1Tx To Pin RP65 -> pin 76
//    RPINR18bits.U1RXR = 76;                 // Assign U1Rx To Pin RPI_76 (input)


 //*************************************************************

    U1MODE = 0x0;
    U1BRG = BAUD;                // 40Mhz osc, 9600 Baud
    U1STA = 0x0;
//    IPC7 = 0x4400;              // Mid Range Interrupt Priority level, no urgent reason
    IFS0bits.U1TXIF = 0;        // Clear the Transmit Interrupt Flag
    IEC0bits.U1TXIE = 1;        // Enable Transmit Interrupts
    IFS0bits.U1RXIF = 0;        // Clear the Recieve Interrupt Flag
    IEC0bits.U1RXIE = 0;        // Enable Recieve Interrupts
    _IOLOCK = 0;
    RPOR9 = 0;
    RPOR9bits.RP101R = 1;       //RF5/RP101 as U1TX
    RPINR18bits.U1RXR = 100;    //RP100/RF4 as U1RX
    _IOLOCK = 1;
    U1MODEbits.UARTEN = 1;      // And turn the peripheral on
    U1STAbits.UTXEN = 1;
    IFS0bits.U1TXIF = 0;
}
 
// writes a byte to UART1
void UART1TxByte(char byte)
{
	if(U1STAbits.UTXBF) 
		while(!U1STAbits.TRMT);	
    U1TXREG = byte;
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

void __attribute__((__interrupt__,no_auto_psv)) _U1TXInterrupt(void)
{
    IFS0bits.U1TXIF = 0;                     // Clear TX Interrupt flag
}

