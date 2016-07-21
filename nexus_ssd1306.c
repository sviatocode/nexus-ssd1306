/*
 *******************************************************************************
 * Copyright © 2016 Sviatoslav Semchyshyn
 *******************************************************************************
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *******************************************************************************
 */
/* Includes */
#include "nexus_ssd1306.h"
#include <string.h>

#define HAL_I2C_TIMEOUT_MAX                             0x1000

#define SSD1306_ADDR                                    0x78
#define SSD1306_COMMAND                                 0x00
#define SSD1306_DATA                                    0xC0
#define SSD1306_DATA_CONTINUE                           0x40

// SSD1306 Commandset
// ------------------
// Fundamental Commands
#define SSD1306_SET_CONTRAST_CONTROL                    0x81
#define SSD1306_DISPLAY_ALL_ON_RESUME                   0xA4
#define SSD1306_DISPLAY_ALL_ON                          0xA5
#define SSD1306_NORMAL_DISPLAY                          0xA6
#define SSD1306_INVERT_DISPLAY                          0xA7
#define SSD1306_DISPLAY_OFF                             0xAE
#define SSD1306_DISPLAY_ON                              0xAF
#define SSD1306_NOP                                     0xE3
// Scrolling Commands
#define SSD1306_HORIZONTAL_SCROLL_RIGHT                 0x26
#define SSD1306_HORIZONTAL_SCROLL_LEFT                  0x27
#define SSD1306_HORIZONTAL_SCROLL_VERTICAL_AND_RIGHT    0x29
#define SSD1306_HORIZONTAL_SCROLL_VERTICAL_AND_LEFT     0x2A
#define SSD1306_DEACTIVATE_SCROLL                       0x2E
#define SSD1306_ACTIVATE_SCROLL                         0x2F
#define SSD1306_SET_VERTICAL_SCROLL_AREA                0xA3
// Addressing Setting Commands
#define SSD1306_SET_LOWER_COLUMN                        0x00
#define SSD1306_SET_HIGHER_COLUMN                       0x10
#define SSD1306_MEMORY_ADDR_MODE                        0x20
#define SSD1306_SET_COLUMN_ADDR                         0x21
#define SSD1306_SET_PAGE_ADDR                           0x22
// Hardware Configuration Commands
#define SSD1306_SET_START_LINE                          0x40
#define SSD1306_SET_SEGMENT_REMAP                       0xA0
#define SSD1306_SET_MULTIPLEX_RATIO                     0xA8
#define SSD1306_COM_SCAN_DIR_INC                        0xC0
#define SSD1306_COM_SCAN_DIR_DEC                        0xC8
#define SSD1306_SET_DISPLAY_OFFSET                      0xD3
#define SSD1306_SET_COM_PINS                            0xDA
#define SSD1306_CHARGE_PUMP                             0x8D
// Timing & Driving Scheme Setting Commands
#define SSD1306_SET_DISPLAY_CLOCK_DIV_RATIO             0xD5
#define SSD1306_SET_PRECHARGE_PERIOD                    0xD9
#define SSD1306_SET_VCOM_DESELECT                       0xDB

/* Variables */
#define ssd1306_buflen (SSD1306_WIDTH*SSD1306_HEIGHT/8)
static uint8_t ssd1306_buffer[SSD1306_HEIGHT/8][SSD1306_WIDTH];

/* Functions */
void ssd1306_setPixel(size_t x, size_t y, color_t color)
{
    if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) return;
    switch (color)
    {
    case COLOR_BLACK:
        ssd1306_buffer[y/8][x] &= ~(1<<(y%8));
        break;
    case COLOR_WHITE:
        ssd1306_buffer[y/8][x] |= (1<<(y%8));
        break;
    case COLOR_INV:
        ssd1306_buffer[y/8][x] ^= (1<<(y%8));
        break;
    default:
        break;
    }
}

color_t ssd1306_getPixel(size_t x, size_t y)
{
    if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) return COLOR_INV;
    if ((ssd1306_buffer[y/8][x]&(1<<(y%8))) != 0) return COLOR_WHITE;
	else return COLOR_BLACK;
}

void ssd1306_sendCmd(I2C_HandleTypeDef* hi2c, uint8_t cmd)
{
    uint8_t data[1];
    data[0] = cmd;
    HAL_I2C_Mem_Write(hi2c, SSD1306_ADDR, SSD1306_COMMAND, I2C_MEMADD_SIZE_8BIT,
                      data, 1, HAL_I2C_TIMEOUT_MAX);
}

void ssd1306_update(I2C_HandleTypeDef* hi2c)
{
    ssd1306_sendCmd(hi2c, SSD1306_SET_COLUMN_ADDR);
    ssd1306_sendCmd(hi2c, 0);
    ssd1306_sendCmd(hi2c, 127);

    ssd1306_sendCmd(hi2c, SSD1306_SET_PAGE_ADDR);
    ssd1306_sendCmd(hi2c, 0);
    ssd1306_sendCmd(hi2c, 7);

    HAL_I2C_Mem_Write(hi2c, SSD1306_ADDR, SSD1306_DATA_CONTINUE,
                      I2C_MEMADD_SIZE_8BIT,
                      &ssd1306_buffer[0][0], ssd1306_buflen, HAL_I2C_TIMEOUT_MAX);
}

void ssd1306_clear()
{
    memset(&ssd1306_buffer[0][0], 0, ssd1306_buflen);
}

void ssd1306_init(I2C_HandleTypeDef* hi2c)
{
    ssd1306_sendCmd(hi2c, SSD1306_DISPLAY_OFF);
    ssd1306_sendCmd(hi2c, SSD1306_SET_DISPLAY_CLOCK_DIV_RATIO);
    ssd1306_sendCmd(hi2c, 0x80);
    ssd1306_sendCmd(hi2c, SSD1306_SET_MULTIPLEX_RATIO);
    ssd1306_sendCmd(hi2c, 0x3F);
    ssd1306_sendCmd(hi2c, SSD1306_SET_DISPLAY_OFFSET);
    ssd1306_sendCmd(hi2c, 0x0);
    ssd1306_sendCmd(hi2c, SSD1306_SET_START_LINE | 0x0);
    ssd1306_sendCmd(hi2c, SSD1306_CHARGE_PUMP);
	ssd1306_sendCmd(hi2c, 0x14);
    ssd1306_sendCmd(hi2c, SSD1306_MEMORY_ADDR_MODE);
    ssd1306_sendCmd(hi2c, 0x00);
    ssd1306_sendCmd(hi2c, SSD1306_SET_SEGMENT_REMAP | 0x1);
    ssd1306_sendCmd(hi2c, SSD1306_COM_SCAN_DIR_DEC);
    ssd1306_sendCmd(hi2c, SSD1306_SET_COM_PINS);
    ssd1306_sendCmd(hi2c, 0x12);
    ssd1306_sendCmd(hi2c, SSD1306_SET_CONTRAST_CONTROL);
	ssd1306_sendCmd(hi2c, 0xCF);
    ssd1306_sendCmd(hi2c, SSD1306_SET_PRECHARGE_PERIOD);
	ssd1306_sendCmd(hi2c, 0xF1);
    ssd1306_sendCmd(hi2c, SSD1306_SET_VCOM_DESELECT);
    ssd1306_sendCmd(hi2c, 0x40);
    ssd1306_sendCmd(hi2c, SSD1306_DISPLAY_ALL_ON_RESUME);
    ssd1306_sendCmd(hi2c, SSD1306_NORMAL_DISPLAY);
	ssd1306_sendCmd(hi2c, SSD1306_DISPLAY_ON);

    ssd1306_clear();
    ssd1306_update(hi2c);
}
