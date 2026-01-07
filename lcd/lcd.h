#ifndef _LCD_H_
#define _LCD_H_

#include <p18f4620.h>

/* * Hardware Configuration
 * Assumes 4-bit High Mode (Data on RB4-RB7)
 */

// Using LAT registers for Output to prevent Read-Modify-Write issues on PIC18
#define LCD_LAT         LATB
#define LCD_TRIS        TRISB

// Control Pin Definitions (Modify bit numbers if hardware differs)
#define LCD_RS          LATBbits.LATB2
#define LCD_TRIS_RS     TRISBbits.TRISB2

#define LCD_EN          LATBbits.LATB3
#define LCD_TRIS_EN     TRISBbits.TRISB3

// Note: RW pin support removed. 
// Library assumes RW is grounded (Write-Only) for efficiency.

// Command Definitions
#define LCD_CMD_CLEAR           0x01
#define LCD_CMD_HOME            0x02
#define LCD_CMD_ENTRY_MODE      0x06
#define LCD_CMD_DISPLAY_ON      0x0C
#define LCD_CMD_FUNCTION_SET    0x28

// Function Prototypes
void init_lcd(void);
void lcd_write_cmd(unsigned char cmd);
void lcd_write_data(unsigned char data);

// Print Helpers
void lcd_putchar(char c);
void lcd_print_char(char c);
void lcd_print_str(const rom char *str); // Note: 'rom' is C18 specific
void lcd_print_num(long num);

// Control Helpers
void lcd_clear(void);
void lcd_set_cursor(unsigned char row, unsigned char col);

// Delay Helpers
void lcd_delay_ms(unsigned char ms);

#endif // _LCD_H_