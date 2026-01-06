/* 
 * File:   main.c
 * Author: thinh
 *
 * This project is used to test virtually all the peripheral devices on the pic kit
 */

#include "main.h"

#define     LED     PORTB
#define     ON      1
#define     OFF     0

unsigned char arrayMapOfOutput [8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
unsigned char statusOutput[8] = {0, 0, 0, 0, 0, 0, 0, 0};

void init_system(void);

void delay_ms(int value);

void OpenOutput(int index);
void CloseOutput(int index);
void ReverseOutput(int index);

void main(void) {
    init_system();
    
    while (1) {
        if (flag_timer0 == 1) {
            flag_timer0 = 0;
            ReverseOutput(0);
        }

        if (flag_timer1 == 1) {
            flag_timer1 = 0;
            ReverseOutput(1);
        }

        if (flag_timer3 == 1) {
            flag_timer3 = 0;
            ReverseOutput(2);
        }
    }
}

void init_system(void) {
    TRISB = 0x00;
    LED = 0x00;
    
    init_interrupt();
    
    init_timer0(5000);
    init_timer1(5000);
//    init_timer2(156);
    init_timer3(5000);
    
    SetTimer0_ms(500);
    SetTimer1_ms(1000);
//    SetTimer2_ms(2000);
    SetTimer3_ms(2000);
}

void delay_ms(int value) {
    int i, j;
    for (i = 0; i < value; i++)
        for (j = 0; j < 260; j++);
}

void OpenOutput(int index) {
    if (index >= 0 && index <= 7) {
        LED = LED | arrayMapOfOutput[index];
    }
}

void CloseOutput(int index) {
    if (index >= 0 && index <= 7) {
        LED = LED & ~arrayMapOfOutput[index];
    }
}

void ReverseOutput(int index) {
    if (statusOutput[index] == ON) {
        CloseOutput(index);
        statusOutput[index] = OFF;
    } else {
        OpenOutput(index);
        statusOutput[index] = ON;
    }
}
