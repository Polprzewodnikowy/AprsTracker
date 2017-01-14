/*
 * lcd.h
 *
 *  Created on: 08.01.2017
 *      Author: korgeaux
 */

#ifndef MISC_LCD_H_
#define MISC_LCD_H_

#define LCD_FIFO_SIZE   128

#define LCD_I2C_ADDR    0x27

#define LCD_RS_BIT      (1 << 0)
#define LCD_RW_BIT      (1 << 1)
#define LCD_E_BIT       (1 << 2)
#define LCD_BL_BIT      (1 << 3)
#define LCD_DB4_BIT     (1 << 4)
#define LCD_DB5_BIT     (1 << 5)
#define LCD_DB6_BIT     (1 << 6)
#define LCD_DB7_BIT     (1 << 7)

#define LCD_RS_POS      0
#define LCD_RW_POS      1
#define LCD_E_POS       2
#define LCD_BL_POS      3
#define LCD_DB4_POS     4
#define LCD_DB5_POS     5
#define LCD_DB6_POS     6
#define LCD_DB7_POS     7

#define LCD_DB_MASK     (LCD_DB4_BIT | LCD_DB5_BIT | LCD_DB6_BIT | LCD_DB7_BIT)

enum {
    LCD_COMMAND = 0, LCD_DATA = 1,
};

enum {
    LCD_BYTE = 0, LCD_NIBBLE = 1,
};

typedef struct {
    uint8_t data;
    uint8_t type :1;
    uint8_t mode :1;
    uint8_t delay :6;
} LcdCommand;

#define HD44780_CLEAR                   0x01

#define HD44780_HOME                    0x02

#define HD44780_ENTRY_MODE              0x04
#define HD44780_EM_SHIFT_CURSOR         0
#define HD44780_EM_SHIFT_DISPLAY        1
#define HD44780_EM_DECREMENT            0
#define HD44780_EM_INCREMENT            2

#define HD44780_DISPLAY_ONOFF           0x08
#define HD44780_DISPLAY_OFF             0
#define HD44780_DISPLAY_ON              4
#define HD44780_CURSOR_OFF              0
#define HD44780_CURSOR_ON               2
#define HD44780_CURSOR_NOBLINK          0
#define HD44780_CURSOR_BLINK            1

#define HD44780_DISPLAY_CURSOR_SHIFT    0x10
#define HD44780_SHIFT_CURSOR            0
#define HD44780_SHIFT_DISPLAY           8
#define HD44780_SHIFT_LEFT              0
#define HD44780_SHIFT_RIGHT             4

#define HD44780_FUNCTION_SET            0x20
#define HD44780_FONT5x7                 0
#define HD44780_FONT5x10                4
#define HD44780_ONE_LINE                0
#define HD44780_TWO_LINE                8
#define HD44780_4_BIT                   0
#define HD44780_8_BIT                   16

#define HD44780_CGRAM_SET               0x40

#define HD44780_DDRAM_SET               0x80

void LcdInit(void);
void LcdSetBacklight(uint8_t s);
void LcdClear(void);
void LcdHome(void);
void LcdCursor(uint8_t x, uint8_t y);
void LcdWriteData(uint8_t data);
void LcdWriteCommand(uint8_t data);
void LcdWriteText(char *text);

#endif /* MISC_LCD_H_ */
