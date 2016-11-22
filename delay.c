/*
 * File:   delay.c
 * Author: root
 *
 * Created on November 19, 2016, 3:23 PM
 */


#include "xc.h"
#include "delay.h"

void Tdelaysec(unsigned char Time)// 32 bit timer combination of timer <8> and <9>
{
	unsigned long TimeRegister = 0;
	T9CONbits.TON = 0; 		// Stop any 16-bit Timer9 operation
	T8CONbits.TON = 0; 		// Stop any 16/32-bit Timer8 operation
	T8CONbits.T32 = 1; 		// Enable 32-bit Timer mode
	T8CONbits.TCS = 0; 		// Select internal instruction cycle clock
	T8CONbits.TGATE = 0; 	// Disable Gated Timer mode
	T8CONbits.TCKPS = 0b00;	// Select 1:1 Pre-scaler
	TMR9 = 0x00; 			// Clear 32-bit Timer (msw)
	TMR8 = 0x00; 			// Clear 32-bit Timer (lsw)
 	TimeRegister = Time * OneSecValue; 
 	PR9 = (TimeRegister>>16) & 0xFFFF; 	// Load 32-bit period value (msw)
	PR8 = TimeRegister & 0xFFFF; 		// Load 32-bit period value (lsw)
	//IPC13bits.T9IP = 0x01; 	// Set Timer9 Interrupt Priority Level
	IFS3bits.T9IF = 0; 		// Clear Timer9 Interrupt Flag
	IEC3bits.T9IE = 1; 		// Enable Timer9 interrupt
	T8CONbits.TON = 1; 		// Start 32-bit Timer
}
void TimerOff(void)
{
	T9CONbits.TON = 0; 		// Stop any 16-bit Timer3 operation
	T8CONbits.TON = 0; 		// Stop any 16/32-bit Timer3 operation
	IFS3bits.T9IF = 0; 		// Clear Timer3 Interrupt Flag
	IEC3bits.T9IE = 0; 		// Enable Timer3 interrupt
	TMR9 = 0x00; 			// Clear 32-bit Timer (msw)
	TMR8 = 0x00; 			// Clear 32-bit Timer (lsw)
	PR9  = 0x0000; 			// Load 32-bit period value (msw)
	PR8  = 0x0000; 			// Load 32-bit period value (lsw)
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