// lcd_init.h

#ifndef LCD_INIT_H
#define LCD_INIT_H

#include <stdint.h>

void lcd_init(void);
void lcd_send_command(uint8_t cmd);
void lcd_send_data(uint8_t data);
void lcd_write_byte(uint8_t data);  // ✅ HINZUGEFÜGT

#endif // LCD_INIT_H
