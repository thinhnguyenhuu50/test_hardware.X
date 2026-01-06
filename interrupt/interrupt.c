
#include "interrupt.h"
#include "..\timer\timer.h"

void init_interrupt(void)
{
    INTCONbits.GIE = 1;  // Enable Global Interrupts
    RCONbits.IPEN = 1;   // Enable Interrupt Priorities
    INTCONbits.PEIE = 1; // Enable Peripheral/Low Priority Interrupts
}

void init_ext_int0(void)
{
    INTCON2bits.INTEDG0 = 0; // Interrupt0 on falling edge
    INTCONbits.INT0IF = 0;   // Clear Flag
    INTCONbits.INT0IE = 1;   // Enable INT0
}

//Map Reset Vector
#pragma code InterruptVectorLow = 0x18
void InterruptVectorLow(void)
{
    _asm
        GOTO low_isr
    _endasm
}

#pragma code InterruptVectorHigh = 0x08
void InterruptVectorHigh(void)
{
    _asm
        GOTO high_isr
    _endasm
}

//----------------------------------------------------------------------------
// HIGH PRIORITY ISR
// Handles: Timer 0, INT0, UART RX (Default High)
//----------------------------------------------------------------------------
#pragma code
#pragma interrupt high_isr
void high_isr(void)
{
    // --- TIMER 0 (Configured as High Priority in timer.c) ---
    if (INTCONbits.T0IF) 
    {
        INTCONbits.T0IF = 0;    // Clear Flag first
        stop_timer0();
        start_timer0();         // Reloads value
        timer0_isr();           // Run logic
    }

    // --- EXTERNAL INT 0 (Always High Priority on PIC18) ---
    if (INTCONbits.INT0IF) 
    {
        INTCONbits.INT0IF = 0;
    }

    // --- UART RECEIVE (Usually High Priority to prevent data loss) ---
    if (PIR1bits.RCIF) 
    {
        PIR1bits.RCIF = 0;
    }
}

//----------------------------------------------------------------------------
// LOW PRIORITY ISR
// Handles: Timer 1, Timer 2, Timer 3
//----------------------------------------------------------------------------
#pragma code
#pragma interruptlow low_isr
void low_isr(void)
{
    // --- TIMER 1 (Configured as Low Priority in timer.c) ---
    if (PIR1bits.TMR1IF) 
    {
        PIR1bits.TMR1IF = 0;    // Clear Flag
        stop_timer1();
        start_timer1();         // Reloads value
        timer1_isr();
    }

    // --- TIMER 2 (Configured as Low Priority in timer.c) ---
    if (PIR1bits.TMR2IF)
    {
        PIR1bits.TMR2IF = 0;    // Clear Flag (TMR2 auto-reloads via PR2)
        // Timer 2 does not need stop/start for reload
        timer2_isr();
    }

    // --- TIMER 3 (Configured as Low Priority in timer.c) ---
    if (PIR2bits.TMR3IF) 
    {
        PIR2bits.TMR3IF = 0;    // Clear Flag
        stop_timer3();
        start_timer3();         // Reloads value
        timer3_isr();
    }
}