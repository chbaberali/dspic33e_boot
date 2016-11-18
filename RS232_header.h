/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef RS232_header_H
#define	RS232_header_H

#include <xc.h> // include processor files - each processor file is guarded.  

// TODO Insert appropriate #include <>
#define BaudRate 115200
//#define FCY 50000000
#define FCY 40000000UL
#define BAUD ((FCY/BaudRate)/16)-1

#define CMD_DEV_ID      0x00 
#define CMD_BL_VER      0x01
#define CMD_WRITE_FLASH 0x02
#define CMD_READ_FLASH  0x03
#define CMD_RESET_DEV   0x04


// TODO Insert C++ class definitions if appropriate

// TODO Insert declarations
void UART1Init(void);
void UART1TxByte(char byte);
char UART1RxByte(unsigned int);
void UART1TxString(char *str);
void BL_Receive_Cmd(void);
char hexDigit(unsigned n);

// Comment a function and leverage automatic documentation with slash star star
/**
    <p><b>void UART1Init(long baud):</b></p>
  
    <p><b>Configures UART port at specifies baud rate </b></p>

    <p><b>Description:</b></p>

    <p><b>Set the oscillator value :</b></p>

    <p><b>Put in desired baud rate:</b></p>

    <p><b>Nothing:</b></p>

    <p><b>UART1Init(9600):</b></p>
    <code>
 
    </code>

    <p><b>Remarks:</b></p>
 */
// TODO Insert declarations or function prototypes (right here) to leverage 
// live documentation

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

