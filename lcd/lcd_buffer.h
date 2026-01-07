/* 
 * File:   lcd_buffer.h
 * Author: thinh
 */
#ifndef _LCD_BUFFER_H_
#define _LCD_BUFFER_H_

// Animation States
#define ANIM_IDLE           0
#define ANIM_TYPEWRITER     1
#define ANIM_BLINK_TEXT     2
#define ANIM_SCROLL_LEFT    3

// Configuration
#define LCD_COLS 16
#define LCD_ROWS 2

// Function Prototypes
void LcdClearS(void);
void LcdUpdate(void); // Call this in your main loop or timer check
void LcdPrintStringS(unsigned char row, unsigned char col, const rom char *str);

// New Animation Functions
void LcdAnimTypewriter(unsigned char row, const rom char *str, unsigned int speed_ms);
void LcdAnimBlink(unsigned char row, const rom char *str, unsigned int speed_ms);

#endif