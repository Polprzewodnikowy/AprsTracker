/*
 * lcd.c
 *
 *  Created on: 08.01.2017
 *      Author: korgeaux
 */

#include "stm32f0xx.h"
#include "lcd.h"
#include "gpio.h"

static LcdCommand fifo[LCD_FIFO_SIZE];
static LcdCommand *fifoIn, *fifoOut;
static uint8_t backlightState = 1;

static void LcdWrite(uint8_t type, uint8_t mode, uint8_t data);
static void LcdWriteNibble(uint8_t data, uint8_t delay);
static void LcdFifoPush(LcdCommand c);
static LcdCommand LcdFifoPop(void);
static uint8_t LcdFifoIsEmpty(void);
static uint8_t LcdFifoIsFull(void);

static uint8_t dmaBuffer[7];

void LcdInit(void) {
    fifoIn = fifoOut = fifo;

    GpioConfig(GPIOB, 8, GPIO_AF1_OD_LOW_PULL_UP);
    GpioConfig(GPIOB, 9, GPIO_AF1_OD_LOW_PULL_UP);

    DMA1_Channel2->CMAR = (uint32_t) &dmaBuffer;
    DMA1_Channel2->CPAR = (uint32_t) &I2C1->TXDR;
    DMA1_Channel2->CNDTR = 7;
    DMA1_Channel2->CCR = DMA_CCR_MINC | DMA_CCR_DIR;

    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
    I2C1->CR2 = I2C_CR2_AUTOEND | (7 << I2C_CR2_NBYTES_Pos) | (LCD_I2C_ADDR << 1);
    I2C1->TIMINGR = 0x20503F5C;
    I2C1->CR1 = I2C_CR1_TXDMAEN | I2C_CR1_PE;

    //TIM7 | 1000Hz interrupt
    RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
    TIM7->ARR = (FREQ / 10000) - 1;
    TIM7->DIER = TIM_DIER_UIE;
    TIM7->CR1 = TIM_CR1_ARPE | TIM_CR1_URS | TIM_CR1_CEN;
    NVIC_SetPriority(TIM7_IRQn, 3);
    NVIC_EnableIRQ(TIM7_IRQn);

    for (int i = 0; i < 3; i++)
        LcdWriteNibble(0x03, 50);
    LcdWriteNibble(0x02, 10);
    LcdWriteCommand(HD44780_FUNCTION_SET | HD44780_FONT5x7 | HD44780_TWO_LINE | HD44780_4_BIT);
    LcdWriteCommand(HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_OFF);
    LcdWriteCommand(HD44780_CLEAR);
    LcdWriteCommand(HD44780_ENTRY_MODE | HD44780_EM_SHIFT_CURSOR | HD44780_EM_INCREMENT);
    LcdWriteCommand(HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_ON | HD44780_CURSOR_OFF | HD44780_CURSOR_NOBLINK);
}

void LcdSetBacklight(uint8_t s) {
    backlightState = s;
    LcdWriteCommand(0);
}

void LcdClear(void) {
    LcdCommand cmd;
    cmd.data = HD44780_CLEAR;
    cmd.type = LCD_COMMAND;
    cmd.mode = LCD_BYTE;
    cmd.delay = 2;
    LcdFifoPush(cmd);
}

void LcdHome(void) {
    LcdCommand cmd;
    cmd.data = HD44780_HOME;
    cmd.type = LCD_COMMAND;
    cmd.mode = LCD_BYTE;
    cmd.delay = 2;
    LcdFifoPush(cmd);
}

void LcdCursor(uint8_t x, uint8_t y) {
    LcdWriteCommand(HD44780_DDRAM_SET | (x + (0x40 * y)));
}

void LcdWriteData(uint8_t data) {
    LcdCommand cmd;
    cmd.data = data;
    cmd.type = LCD_DATA;
    cmd.mode = LCD_BYTE;
    cmd.delay = 0;
    LcdFifoPush(cmd);
}

void LcdWriteCommand(uint8_t data) {
    LcdCommand cmd;
    cmd.data = data;
    cmd.type = LCD_COMMAND;
    cmd.mode = LCD_BYTE;
    cmd.delay = 0;
    LcdFifoPush(cmd);
}

void LcdWriteText(char *text) {
    while (*text)
        LcdWriteData(*text++);
}

void TIM7_IRQHandler(void) {
    TIM7->SR = ~TIM_SR_UIF;
    static uint8_t delay = 0;
    if (delay) {
        --delay;
    } else {
        if ((!(DMA1_Channel2->CCR & DMA_CCR_EN) || (DMA1->ISR & DMA_ISR_TCIF2)) && !LcdFifoIsEmpty()) {
            DMA1->IFCR = DMA_IFCR_CTCIF2;
            LcdCommand cmd = LcdFifoPop();
            delay = cmd.delay;
            LcdWrite(cmd.type, cmd.mode, cmd.data);
        }
    }
}

static void LcdWrite(uint8_t type, uint8_t mode, uint8_t data) {
    if (mode == LCD_NIBBLE)
        data <<= 4;

    uint8_t tmp, size;

    tmp = 0;
    if (data & 0x10)
        tmp |= LCD_DB4_BIT;
    if (data & 0x20)
        tmp |= LCD_DB5_BIT;
    if (data & 0x40)
        tmp |= LCD_DB6_BIT;
    if (data & 0x80)
        tmp |= LCD_DB7_BIT;

    dmaBuffer[0] = ((type == LCD_DATA ? 1 : 0) << LCD_RS_POS) | ((backlightState ? 1 : 0) << LCD_BL_POS);
    dmaBuffer[1] = dmaBuffer[0] | LCD_E_BIT;
    dmaBuffer[2] = dmaBuffer[1] | tmp;
    dmaBuffer[3] = dmaBuffer[2] & ~LCD_E_BIT;

    if (mode == LCD_BYTE) {
        tmp = 0;
        if (data & 0x01)
            tmp |= LCD_DB4_BIT;
        if (data & 0x02)
            tmp |= LCD_DB5_BIT;
        if (data & 0x04)
            tmp |= LCD_DB6_BIT;
        if (data & 0x08)
            tmp |= LCD_DB7_BIT;

        dmaBuffer[4] = dmaBuffer[3] | LCD_E_BIT;
        dmaBuffer[5] = (dmaBuffer[4] & ~LCD_DB_MASK) | tmp;
        dmaBuffer[6] = dmaBuffer[5] & ~LCD_E_BIT;
        size = 7;
    } else {
        size = 4;
    }

    DMA1_Channel2->CCR &= ~DMA_CCR_EN;
    I2C1->CR2 = I2C_CR2_AUTOEND | (size << I2C_CR2_NBYTES_Pos) | (LCD_I2C_ADDR << 1);
    DMA1_Channel2->CNDTR = size;
    DMA1_Channel2->CCR |= DMA_CCR_EN;
    I2C1->CR2 |= I2C_CR2_START;
}

static void LcdWriteNibble(uint8_t data, uint8_t delay) {
    LcdCommand cmd;
    cmd.data = 0x03;
    cmd.type = LCD_COMMAND;
    cmd.mode = LCD_NIBBLE;
    cmd.delay = delay > 0x3F ? 0x3F : delay;
    LcdFifoPush(cmd);
}

static void LcdFifoPush(LcdCommand cmd) {
    while (LcdFifoIsFull())
        ;
    *fifoIn++ = cmd;
    if (fifoIn >= (fifo + LCD_FIFO_SIZE))
        fifoIn = fifo;
}

static LcdCommand LcdFifoPop(void) {
    LcdCommand data = *fifoOut++;
    if (fifoOut >= (fifo + LCD_FIFO_SIZE))
        fifoOut = fifo;
    return data;
}

static uint8_t LcdFifoIsEmpty(void) {
    return fifoIn == fifoOut;
}

static uint8_t LcdFifoIsFull(void) {
    return (((fifoOut == fifo) && (fifoIn == (fifo + LCD_FIFO_SIZE - 1))) || (fifoIn == (fifoOut - 1)));
}
