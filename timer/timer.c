#include "timer.h"

// Global variables for Timer management
unsigned int timer0_value = 0;
unsigned int timer1_value = 0;
unsigned char timer2_value = 0;
unsigned int timer3_value = 0;

// Application flags
unsigned char flag_timer0 = 0, flag_timer1 = 0, flag_timer2 = 0, flag_timer3 = 0;

// Internal counters
unsigned int timer0_cnt = 0, timer1_cnt = 0, timer2_cnt = 0, timer3_cnt = 0;

// Multipliers (Soft-scalers)
unsigned int time0_MUL = 1, time1_MUL = 1, time2_MUL = 1, time3_MUL = 1;

//--------------------TIMER0 (High Priority)-------------------------
void init_timer0(unsigned int count)
{
    T0CONbits.TMR0ON = 0;       // Stop
    timer0_value = count;
    T0CONbits.T08BIT = 0;       // 16-bit mode
    
    TMR0H = (65535 - timer0_value) / 256;
    TMR0L = (65535 - timer0_value) % 256;
    
    T0CONbits.T0CS = 0;         // Internal instruction cycle clock
    T0CONbits.PSA = 1;          // No Prescaler (1:1)
    
    INTCONbits.T0IE = 1;        // Enable Interrupt
    INTCONbits.T0IF = 0;        // Clear Flag
    INTCON2bits.TMR0IP = 1;     // High Priority
    
    T0CONbits.TMR0ON = 1;       // Start
}

void start_timer0(void)
{
    // Reload Count
    TMR0H = (65535 - timer0_value) / 256;
    TMR0L = (65535 - timer0_value) % 256;
    T0CONbits.TMR0ON = 1;
}

void stop_timer0(void)
{
    T0CONbits.TMR0ON = 0;
}

//--------------------TIMER1 (Low Priority)-------------------------
void init_timer1(unsigned int count)
{
    T1CONbits.TMR1ON = 0;
    timer1_value = count;

    TMR1H = (65535 - timer1_value) / 256;
    TMR1L = (65535 - timer1_value) % 256;

    PIE1bits.TMR1IE = 1;        // Enable Interrupt
    PIR1bits.TMR1IF = 0;        // Clear Flag
    IPR1bits.TMR1IP = 0;        // Low Priority

    T1CONbits.RD16 = 1;         // 16-bit RW operations
    T1CONbits.T1CKPS0 = 0;      // Prescaler 1:1
    T1CONbits.T1CKPS1 = 0;
    T1CONbits.T1OSCEN = 0;      // Oscillator off
    T1CONbits.TMR1CS = 0;       // Internal clock
    
    T1CONbits.TMR1ON = 1;       // Start
}

void start_timer1(void)
{
    TMR1H = (65535 - timer1_value) / 256;
    TMR1L = (65535 - timer1_value) % 256;
    T1CONbits.TMR1ON = 1;
}

void stop_timer1(void)
{
    T1CONbits.TMR1ON = 0;
}

//--------------------TIMER2 (Low Priority)-------------------------
// Timer 2 is 8-bit and uses PR2 for period matching (Auto-reload)
void init_timer2(unsigned char count)
{
    T2CONbits.TMR2ON = 0;
    TMR2 = 0x00;
    timer2_value = count;
    PR2 = timer2_value;         // Load period register

    PIE1bits.TMR2IE = 1;        // Enable Interrupt
    PIR1bits.TMR2IF = 0;        // Clear Flag
    IPR1bits.TMR2IP = 0;        // Low Priority

// --- Use Prescaler ---
    // T2CON bits 1-0: 00=1, 01=4, 1x=16
    T2CONbits.T2CKPS0 = 1;  
    T2CONbits.T2CKPS1 = 1;  // Prescaler is now 1:16

    // --- Use Postscaler ---
    // T2CON bits 6-3: 1001 = 1:10 Postscale
    T2CONbits.T2OUTPS3 = 0;
    T2CONbits.T2OUTPS2 = 0;
    T2CONbits.T2OUTPS1 = 0;
    T2CONbits.T2OUTPS0 = 1;

    T2CONbits.TMR2ON = 1;
}

void start_timer2(void)
{
    // Timer 2 auto-reloads from PR2, just turn ON
    T2CONbits.TMR2ON = 1;
}

void stop_timer2(void)
{
    T2CONbits.TMR2ON = 0;
}

//--------------------TIMER3 (Low Priority)-------------------------
void init_timer3(unsigned int count)
{
    T3CONbits.TMR3ON = 0;
    timer3_value = count;

    // FIX: Write High Byte FIRST
    TMR3H = (65535 - timer3_value) / 256;
    TMR3L = (65535 - timer3_value) % 256;

    PIE2bits.TMR3IE = 1;        // Enable Interrupt
    PIR2bits.TMR3IF = 0;        // Clear Flag
    IPR2bits.TMR3IP = 0;        // Low Priority

    T3CONbits.RD16 = 1;         // 16-bit mode
    T3CONbits.T3CKPS0 = 0;      // Prescaler 1:1
    T3CONbits.T3CKPS1 = 0;
    T3CONbits.TMR3CS = 0;       // Internal clock
    
    T3CONbits.TMR3ON = 1;       // Start
}

void start_timer3(void)
{
    TMR3H = (65535 - timer3_value) / 256;
    TMR3L = (65535 - timer3_value) % 256;
    T3CONbits.TMR3ON = 1;
}

void stop_timer3(void)
{
    T3CONbits.TMR3ON = 0;
}

//------------------------------------------------------------------
// INTERRUPT SERVICE LOGIC (Called by Dispatcher)
//------------------------------------------------------------------

void timer0_isr(void)
{
    timer0_cnt++;
    if (timer0_cnt >= time0_MUL)
    {
        timer0_cnt = 0;
        flag_timer0 = 1;
        // High priority tasks here
    }
}

void timer1_isr(void)
{
    timer1_cnt++;
    if (timer1_cnt >= time1_MUL)
    {
        timer1_cnt = 0;
        flag_timer1 = 1;
    }
}

void timer2_isr(void)
{
    timer2_cnt++;
    if (timer2_cnt >= time2_MUL)
    {
        timer2_cnt = 0;
        flag_timer2 = 1;
    }
}

void timer3_isr(void)
{
    timer3_cnt++;
    if (timer3_cnt >= time3_MUL)
    {
        timer3_cnt = 0;
        flag_timer3 = 1;
    }
}

//------------------------------------------------------------------
// CONFIGURATION FUNCTIONS
//------------------------------------------------------------------

void SetTimer0_ms(int time)
{
    time0_MUL = time;
    timer0_cnt = 0;
    flag_timer0 = 0;
}

void SetTimer1_ms(int time)
{
    time1_MUL = time;
    timer1_cnt = 0;
    flag_timer1 = 0;
}

void SetTimer2_ms(int time)
{
    // Since Timer 2 interrupts happen very fast (it's only 8-bit),
    // you might need a larger multiplier here depending on your init logic.
    time2_MUL = time; 
    timer2_cnt = 0;
    flag_timer2 = 0;
}

void SetTimer3_ms(int time)
{
    time3_MUL = time;
    timer3_cnt = 0;
    flag_timer3 = 0;
}