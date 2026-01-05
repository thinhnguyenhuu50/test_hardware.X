/* 
 * File:   main.c
 * Author: thinh
 *
 * This project is used to test virtually all the peripheral devices on the pic kit
 */

#include "main.h"

#define     LED     PORTB

void init_system(void);
void delay_ms(int value);

void main(void) {
    init_system();
    
    while(1) {
        LED = 0;
        delay_ms(500);
        LED = 0xFF;
        delay_ms(500);
    }
}

void init_system(void)
{
    TRISB = 0x00;
}

void delay_ms(int value)
{
	int i,j;
	for(i = 0; i < value; i++)
		for(j = 0; j < 260; j++);
}
