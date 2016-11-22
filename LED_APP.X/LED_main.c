/*
 * File:   main.c
 * Author: root
 *
 * Created on November 8, 2016, 8:03 AM
 */



#include "xc.h"
#include "p33EP512MU810.h"
void TDelayms(unsigned );
void InitClock(void);
void inittimer1(void);
void inittimer2(void);
void TDelayms( unsigned t);

unsigned char pwmCounter = 0;
unsigned char ledActualBrightness = 0;
unsigned char ledTargetBrightness = 0;
unsigned char fadeCounter = 0;


int main(void) {
    InitClock();
    inittimer1();
    inittimer2();
    TRISBbits.TRISB4 = 0;
    TRISBbits.TRISB3 = 0;
    while (1) {
       ledTargetBrightness = 19;
		
		// Pause for a bit
		TDelayms(2000);
		
		// Off
		ledTargetBrightness = 0;
		
		// Pause for a bit
		TDelayms(2000);
    }
    return 0;
}
void inittimer1(void){
    T1CON = 0x0000;
    T1CONbits.TCKPS = 0b011;    // 1:256 
    TMR1 = 0x00;            // Clear timer register
    PR1  = 39063;          // Load the period value for 250ms
    //IPC0bits.T1IP = 0x01;   // Set Timer 1 Interrupt Priority Level
    IFS0bits.T1IF = 0;      // Clear Timer 1 Interrupt Flag
    _GIE = 1;
    IEC0bits.T1IE = 1;      // Enable Timer1 interrupt
    T1CONbits.TON = 1;      // Start Timer
}
void inittimer2(void){
    T2CON = 0x0000;
    T2CONbits.TCKPS = 0b000;    // 1:256 
    TMR2 = 0x00;            // Clear timer register
    PR2  = 40000;          // Load the period value for 250ms
    //IPC0bits.T1IP = 0x01;   // Set Timer 1 Interrupt Priority Level
    IFS0bits.T2IF = 0;      // Clear Timer 1 Interrupt Flag
    _GIE = 1;
    IEC0bits.T2IE = 1;      // Enable Timer1 interrupt
    T2CONbits.TON = 1;      // Start Timer
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
void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
{
    LATBbits.LATB4 = ~LATBbits.LATB4;
    IFS0bits.T1IF = 0; //Clear Timer1 interrupt flag
}
void __attribute__((__interrupt__, auto_psv)) _T2Interrupt(void)
{
    // Perform the PWM brightness control
		if (ledActualBrightness > pwmCounter)
			LATBbits.LATB3 = 1; else LATBbits.LATB3 = 0;
        pwmCounter++;
		 if (pwmCounter > 19) pwmCounter = 0; 
        
        // Perform fading control
		if (ledTargetBrightness >= ledActualBrightness)
			ledActualBrightness = ledTargetBrightness;
		else
		{
			fadeCounter++;
			if (fadeCounter == 24)
			{
				ledActualBrightness--;
				fadeCounter = 0;
			}	
		}
    IFS0bits.T2IF = 0; //Clear Timer1 interrupt flag
}

void TDelayms( unsigned t)
{
    T3CON = 0x8000;     // enable tmr1, Tcy, 1:1
    while (t--)
    {
        TMR3 = 0;
        while (TMR3<40000);
    }
    T3CON = 0x0000;
}