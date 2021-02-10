/*
 * uv_led_driver.c
 *
 *  Created on: Mar 27, 2019
 *      Author: zaitldav
 */


#include "uv_led_driver.h"

/** @brief Initializes the LEDs
 *
 * This function initializes counters and sets pins and ports of LEDs
 * (Initialization of pins and ports MUST be done before using HAL).
 * This function sets LED counters to 0, and sets frequencies for LEDs.
 * This function sets all LEDs to enable.
 *
 * @param Void
 * @ return Void
 */
void uv_led_init( void )
{
	// bind the ports and pins to the arrays
	uv_led_ports[0] = UV_LED_0_GPIO_Port;
	uv_led_ports[1] = UV_LED_1_GPIO_Port;
	uv_led_ports[2] = UV_LED_2_GPIO_Port;
	uv_led_ports[3] = UV_LED_3_GPIO_Port;
	uv_led_ports[4] = UV_LED_4_GPIO_Port;
	uv_led_ports[5] = UV_LED_5_GPIO_Port;
	uv_led_ports[6] = UV_LED_6_GPIO_Port;
	uv_led_ports[7] = UV_LED_7_GPIO_Port;

	uv_led_pins[0] = UV_LED_0_Pin;
	uv_led_pins[1] = UV_LED_1_Pin;
	uv_led_pins[2] = UV_LED_2_Pin;
	uv_led_pins[3] = UV_LED_3_Pin;
	uv_led_pins[4] = UV_LED_4_Pin;
	uv_led_pins[5] = UV_LED_5_Pin;
	uv_led_pins[6] = UV_LED_6_Pin;
	uv_led_pins[7] = UV_LED_7_Pin;

	// UVLEDs global enable flag is disabled by default
	enable_global = 0;

	// Read setup from FLASH
	// Load uv_led_enabled values from FLASH
	uint32_t flash_address_enabled = FLASH_UVLEDS_ENABLED_ADDR;

	for(uint8_t i = 0; i < 8; i++)
	{
		uv_led_enabled[i] = *(uint16_t *)flash_address_enabled;
		flash_address_enabled += 2;
	}

	// Load uv_led_periods_ms values from FLASH
	uint32_t flash_address_periods = FLASH_UVLEDS_PERIODS_ADDR;

	for(uint8_t i = 0; i < 8; i++)
	{
		uv_led_periods_ms[i] = *(uint16_t *)flash_address_periods;
		flash_address_periods += 4;
	}

	// Reset the counters
	for (uint8_t i = 0; i < 8; i++)
	{
		uv_led_counter[i] = 0;
	}
}

/** @brief Sets period of the selected LED
 *
 *
 * @param led_id ID of the LED (0-7)
 * @param frequency frequency of the UV LED
 * @return 1 if success, otherwise 0
 */
uint8_t uv_led_set_frequency( uint8_t led_id, uint8_t frequency)
{
	// Check if the UVLED ID is valid
	if ((0 <= led_id) && (led_id < 8))
	{
		// Check if the frequency to be set is from valid range
		if ((UV_LED_FREQ_MIN <= frequency) && (frequency <= UV_LED_FREQ_MAX)) {
			// calculate the number of ticks needed to toggle the LED
			uint32_t period = (TIMER_FREQ/frequency)/2;
			uv_led_periods_ms[(uint8_t)led_id] = (uint32_t)period;
			uv_led_counter [(uint8_t)led_id] = 0;

			// Write the setup to the FLASH
			// Erase the FLASH Page with the periods
			HAL_FLASH_Unlock();
			FLASH_EraseInitTypeDef EraseInitStruct;
			uint32_t PAGEError = 0;
			EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
			EraseInitStruct.PageAddress = FLASH_UVLEDS_PERIODS_ADDR;
			EraseInitStruct.NbPages     = 1;
			HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError);
			CLEAR_BIT (FLASH->CR, (FLASH_CR_PER));
			HAL_FLASH_Lock();

			// Write to the USER FLASH
			HAL_FLASH_Unlock();
			uint32_t flashAddress = FLASH_UVLEDS_PERIODS_ADDR;
			for(int i = 0; i <8; i++)
			{
				HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, flashAddress, uv_led_periods_ms[i]);
				flashAddress += 4;
			}
			CLEAR_BIT (FLASH->CR, (FLASH_CR_PG));
			HAL_FLASH_Lock();
			return 1;
		}
		return 0;
	}
	return 0;
}

/** @brief Enables selected LED
 *
 *
 * @param led_id ID of the LED (0-7)
 * @param enable 1 if the LED is enables, otherwise 0
 * @return 1 if success, otherwise 0
 */
uint8_t uv_led_enable(uint8_t led_id, uint8_t enable)
{
	// Check if the UVLED ID is valid
	if ((0 <= led_id) && (led_id < 8))
	{
		// Check if the value is valid
		if ((enable == 0) || (enable == 1))
		{
			uv_led_enabled[led_id] = enable;
			uv_led_counter [(uint8_t)led_id] = 0;
			HAL_GPIO_WritePin(uv_led_ports[led_id], uv_led_pins[led_id], enable);

			// Erase the USER FLASH
			HAL_FLASH_Unlock();
			FLASH_EraseInitTypeDef EraseInitStruct;
			uint32_t PAGEError = 0;
			EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
			EraseInitStruct.PageAddress = FLASH_UVLEDS_ENABLED_ADDR;
			EraseInitStruct.NbPages     = 1;
			HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError);
			CLEAR_BIT (FLASH->CR, (FLASH_CR_PER));
			HAL_FLASH_Lock();

			// Write to the USER FLASH
			HAL_FLASH_Unlock();
			uint32_t flashAddress = FLASH_UVLEDS_ENABLED_ADDR;
			for(int i = 0; i <8; i++)
			{
				HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, flashAddress, uv_led_enabled[i]);
				flashAddress += 2;
			}
			CLEAR_BIT (FLASH->CR, (FLASH_CR_PG));
			HAL_FLASH_Lock();
			return 1;
		}
		return 0;
	}
	return 0;
}

/** @brief Toggles the selected LED
 * 	This is a simple wrapper for HAL_GPIO_TogglePin. If the LED is enabled, toggles the LED with selected id.
 *
 * 	@param led_id ID of the LED to be toggled
 * 	@return Void
 */
void uv_led_toggle(uint8_t led_id)
{
		if ((uv_led_enabled[led_id]))
		{
			HAL_GPIO_WritePin(uv_led_ports[led_id], uv_led_pins[led_id], !HAL_GPIO_ReadPin(uv_led_ports[led_id], uv_led_pins[led_id]) && enable_global);
		}
}
