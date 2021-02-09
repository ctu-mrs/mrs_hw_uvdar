/*
 * uv_led_driver.h
 *
 *  Created on: Mar 27, 2019
 *      Author: zaitldav
 */

#ifndef UV_LED_DRIVER_H_
#define UV_LED_DRIVER_H_

#include "main.h"

#define TIMER_FREQ				10000	// Hz

#define UV_LED_FREQ_MIN			1 		// HZ
#define UV_LED_FREQ_MAX			60		// Hz

uint32_t uv_led_periods_ms[8];
uint8_t uv_led_enabled[8];

GPIO_TypeDef* uv_led_ports[8];
uint16_t uv_led_pins [8];

uint16_t uv_led_counter [8];

uint8_t enable_global;


void uv_led_init( void );

uint8_t uv_led_set_frequency( uint8_t led_id, uint8_t frequency );

uint8_t uv_led_enable(uint8_t led_id, uint8_t enable );

void uv_led_toggle( uint8_t led_id );



#endif /* UV_LED_DRIVER_H_ */
