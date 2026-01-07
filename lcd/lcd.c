#include "lcd.h"
#include <delays.h>

/*
 * MPLAB C18 Compiler LCD Driver for PIC18F4620
 * Cleaned and Refactored
 */

// Internal Delay Helpers (Adjust based on Fosc if needed)
// Assuming Fosc around 4MHz to 20MHz.
// Delay1KTCYx(x) delays for x * 1000 instruction cycles.
// For 20MHz, 1 instr = 0.2us. 1000 instr = 200us.

void lcd_delay_ms(unsigned char ms) {
    // delay_ms(1) at 20MHz roughly requires 5000 cycles.
    // We will use a safe approximation loop.
    while(ms--) {
        Delay1KTCYx(5); // ~1ms at 20MHz, ~5ms at 4MHz (Safe for LCD)
    }
}

void lcd_delay_us(unsigned char us) {
    // LCD needs short pulses. 
    // Delay10TCYx(1) is 10 cycles. 
    Delay10TCYx(us); 
}

void lcd_pulse_enable(void) {
    LCD_EN = 1;
    lcd_delay_us(5); // Enable pulse width > 450ns
    LCD_EN = 0;
    lcd_delay_us(5); // Hold time
}

/* * Writes a nibble (4 bits) to the LCD data pins.
 * Handles the masking of the port to preserve other pins on PORTB.
 */
void lcd_write_nibble(unsigned char nibble) {
    // Clear the data bits (RB4-RB7)
    LCD_LAT &= 0x0F; 
    // Set the data bits (Shift nibble to upper 4 bits)
    LCD_LAT |= (nibble & 0xF0);
    
    lcd_pulse_enable();
}

/*
 * Sends a command byte to the LCD
 * Handles splitting byte into two nibbles
 */
void lcd_write_cmd(unsigned char cmd) {
    LCD_RS = 0; // Select Command Register
    
    // High Nibble
    lcd_write_nibble(cmd & 0xF0);
    
    // Low Nibble (Shifted to upper position for the port)
    lcd_write_nibble(cmd << 4);
    
    // Delays based on command type
    if((cmd == 0x01) || (cmd == 0x02)) {
        lcd_delay_ms(2); // Clear Display and Return Home need > 1.52ms
    } else {
        lcd_delay_us(5); // Others need > 37us
    }
}

/*
 * Sends data (character) to the LCD
 */
void lcd_write_data(unsigned char data) {
    LCD_RS = 1; // Select Data Register
    
    // High Nibble
    lcd_write_nibble(data & 0xF0);
    
    // Low Nibble
    lcd_write_nibble(data << 4);
    
    lcd_delay_us(5); // Write execution time > 37us
}

/*
 * Initialization Sequence (4-bit mode)
 * Follows HD44780 datasheet strictly
 */
void init_lcd(void) {
    // Set Data direction to Output
    LCD_TRIS &= 0x0F; // RB4-RB7 as Output
    LCD_TRIS_RS = 0;
    LCD_TRIS_EN = 0;
    
    LCD_RS = 0;
    LCD_EN = 0;
    LCD_LAT &= 0x0F; // Clear data lines

    // Power-on delay > 15ms
    lcd_delay_ms(20);

    // Sequence to enter 4-bit mode
    // 1. Send 0x03
    lcd_write_nibble(0x30);
    lcd_delay_ms(5); // Wait > 4.1ms

    // 2. Send 0x03 again
    lcd_write_nibble(0x30);
    lcd_delay_us(15); // Wait > 100us

    // 3. Send 0x03 again
    lcd_write_nibble(0x30);
    lcd_delay_us(15); 

    // 4. Set 4-bit mode (Send 0x02)
    lcd_write_nibble(0x20);
    lcd_delay_us(15);

    // Interface is now 4-bit. Configure display:
    lcd_write_cmd(0x28); // Function Set: 4-bit, 2 lines, 5x8 font
    lcd_write_cmd(0x0C); // Display ON, Cursor OFF, Blink OFF
    lcd_write_cmd(0x06); // Entry Mode: Increment, No Shift
    lcd_write_cmd(0x01); // Clear Display
    lcd_delay_ms(2);     // Wait for clear
}

/* Helper wrappers */
void lcd_putchar(char c) {
    lcd_write_data(c);
}

void lcd_print_char(char c) {
    lcd_write_data(c);
}

void lcd_print_str(const rom char *str) {
    while (*str) {
        lcd_write_data(*str++);
    }
}

void lcd_set_cursor(unsigned char row, unsigned char col) {
    unsigned char address;
    
    if (row == 0)
        address = 0x80 + col;
    else
        address = 0xC0 + col; // 0xC0 is start of 2nd line for most 16x2
        
    lcd_write_cmd(address);
}

void lcd_clear(void) {
    lcd_write_cmd(0x01);
    lcd_delay_ms(2);
}

/* * Optimized number printing 
 * Replaces the recursive/power functions for better stack usage
 */
void lcd_print_num(long num) {
    char buffer[12]; // Long can be -2147483648 (11 chars + null)
    unsigned char i = 0;
    unsigned char j;
    
    if (num == 0) {
        lcd_write_data('0');
        return;
    }
    
    if (num < 0) {
        lcd_write_data('-');
        num = -num;
    }
    
    // Extract digits in reverse order
    while (num > 0) {
        buffer[i++] = (num % 10) + '0';
        num /= 10;
    }
    
    // Print in correct order
    for (j = i; j > 0; j--) {
        lcd_write_data(buffer[j-1]);
    }
}