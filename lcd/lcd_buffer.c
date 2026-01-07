#include "lcd_buffer.h"
#include "lcd.h"
#include "../timer/timer.h" // To check timer flags if needed, or we pass time externally

// Buffer for what is currently ON SCREEN
unsigned char LcdScreen[LCD_ROWS][LCD_COLS];

// Animation Control Structure

struct {
    unsigned char mode; // ANIM_IDLE, ANIM_TYPEWRITER, etc.
    const rom char *targetStr; // The full string we want to show
    unsigned char currentRow; // Which row we are animating
    unsigned char currentIndex; // How many chars we have shown so far
    unsigned int speed; // Speed in ms
    unsigned int counter; // Internal counter for timing
    unsigned char blinkState; // For blinking (0 or 1)
} animState;

// Standard Init/Clear

void LcdClearS(void) {
    unsigned char i, j;
    for (i = 0; i < LCD_ROWS; i++) {
        for (j = 0; j < LCD_COLS; j++) {
            LcdScreen[i][j] = ' ';
        }
    }
    animState.mode = ANIM_IDLE;
    lcd_clear(); // Clear hardware immediately
}

// Direct print to buffer (No animation)

void LcdPrintStringS(unsigned char row, unsigned char col, const rom char *str) {
    while (*str && col < LCD_COLS) {
        LcdScreen[row][col++] = *str++;
    }
    // Force immediate update for static text
    lcd_set_cursor(row, 0);
    for (col = 0; col < LCD_COLS; col++) {
        lcd_print_char(LcdScreen[row][col]);
    }
}

// --- Animation Starters ---

void LcdAnimTypewriter(unsigned char row, const rom char *str, unsigned int speed_ms) {
    unsigned char i;

    animState.mode = ANIM_TYPEWRITER;
    animState.targetStr = str;
    animState.currentRow = row;
    animState.currentIndex = 0;
    animState.speed = speed_ms / 10; // Assuming LcdUpdate is called every 10ms (timer tick)
    animState.counter = 0;

    // Clear the row we are about to type on
    for (i = 0; i < LCD_COLS; i++);
}

void LcdAnimBlink(unsigned char row, const rom char *str, unsigned int speed_ms) {
    animState.mode = ANIM_BLINK_TEXT;
    animState.targetStr = str;
    animState.currentRow = row;
    animState.speed = speed_ms / 10;
    animState.counter = 0;
    animState.blinkState = 1;
}

// --- The Main Updater ---
// Call this function inside your main loop when a Timer Flag is set (e.g., every 10ms)

void LcdUpdate(void) {
    unsigned char i;

    if (animState.mode == ANIM_IDLE) return;

    animState.counter++;

    // Check if it is time to update (Software Timer)
    if (animState.counter >= animState.speed) {
        animState.counter = 0;

        switch (animState.mode) {
            case ANIM_TYPEWRITER:
                // If we haven't reached end of string or end of screen
                if (animState.targetStr[animState.currentIndex] != 0 && animState.currentIndex < LCD_COLS) {
                    // Update Buffer
                    LcdScreen[animState.currentRow][animState.currentIndex] = animState.targetStr[animState.currentIndex];

                    // Update Hardware (Optimization: Only write the new char)
                    lcd_set_cursor(animState.currentRow, animState.currentIndex);
                    lcd_print_char(LcdScreen[animState.currentRow][animState.currentIndex]);

                    animState.currentIndex++;
                } else {
                    animState.mode = ANIM_IDLE; // Done typing
                }
                break;

            case ANIM_BLINK_TEXT:
                animState.blinkState = !animState.blinkState;

                lcd_set_cursor(animState.currentRow, 0);
                if (animState.blinkState) {
                    // Show Text
                    const rom char *ptr = animState.targetStr;
                    while (*ptr) lcd_print_char(*ptr++);
                } else {
                    // Show Spaces
                    for (i = 0; i < LCD_COLS; i++) lcd_print_char(' ');
                }
                break;
        }
    }
}
