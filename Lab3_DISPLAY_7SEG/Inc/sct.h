/*
 * sct.h
 *
 *  Created on: Oct 22, 2021
 *      Author: Filip
 */

#ifndef SCT_H_
#define SCT_H_
#include "stdint.h"

void sct_led(uint32_t to_display);
void sct_display_digit(uint32_t digit_to_display);
void sct_init(void);

#endif /* SCT_H_ */
