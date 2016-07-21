/*
 *******************************************************************************
 * Copyright © 2016 Sviatoslav Semchyshyn
 *******************************************************************************
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *******************************************************************************
 */
#ifndef __NEXUS_SSD1306_H__
#define __NEXUS_SSD1306_H__

#include <stdint.h>
#include <stdlib.h>

// Here you must include HAL I2C headers
#include "i2c.h"

#define SSD1306_WIDTH   128
#define SSD1306_HEIGHT  64

typedef enum Color_enum {COLOR_BLACK = 0, COLOR_WHITE = 1, COLOR_INV = 2} color_t;

void    ssd1306_init(I2C_HandleTypeDef* hi2c);
void    ssd1306_update(I2C_HandleTypeDef* hi2c);
void    ssd1306_clear();
void    ssd1306_setPixel(size_t x, size_t y, color_t color);
color_t ssd1306_getPixel(size_t x, size_t y);
//void    ssd1306_setContrast(); // TODO
//void    ssd1306_getContrast(); // TODO

#endif // __NEXUS_SSD1306_H__
