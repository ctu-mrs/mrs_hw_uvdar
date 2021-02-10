/*
 * baca_protocol.c
 *
 *  Created on: Mar 22, 2019
 *      Author: zaitldav
 */

#include "baca_protocol.h"

/** @brief General baca protocol encoder
 *	Encodes buffer of bytes using baca protocol
 *
 *	@param buffer Buffer to store the encoded message
 *	@param payload Buffer of payload bytes
 *	@param payload_size length of the payload buffer
 *	@param data_id ID of the device which sends the payload
 *	@return The lenght of the baca buffer. Generaly, it should be payload_size + 4
 */
uint8_t baca_protocol_encode(uint8_t* buffer, uint8_t* payload, uint8_t payload_size, uint8_t data_id)
{
  uint8_t checksum = (uint8_t) 'b';
  uint8_t buffer_length = 0;
  checksum += payload_size + 1;

  buffer[buffer_length++] = 'b';
  buffer[buffer_length++] = payload_size + 1; // Add one for useless byte
  buffer[buffer_length++] = data_id;
  for (int i = 0; i < payload_size; i++)
  {
	  buffer[buffer_length++] = payload[i];
	  checksum += payload[i];
  }
  buffer[buffer_length++] = checksum;

  return buffer_length;
}

/** @brief Baca protocol encoder for one data byte payload
 *	Encodes one byte using baca protocol
 *
 *	@param buffer Buffer to store the encoded message
 *	@param payload Payload byte
 *	@param data_id ID of the device which sends the payload
 *	@return The lenght of the baca buffer. Generaly, it should be payload_size + 3
 */
uint8_t baca_protocol_encode_uint8(uint8_t* buffer, uint8_t payload, uint8_t data_id)
{
	  uint8_t checksum = (uint8_t) 'b';
	  uint8_t buffer_length = 0;
	  checksum += 2 + payload;

	  buffer[buffer_length++] = 'b';
	  buffer[buffer_length++] = 2;
	  buffer[buffer_length++] = data_id;
	  buffer[buffer_length++] = payload;
	  buffer[buffer_length++] = checksum;

	  return buffer_length;
}

/** @brief Baca protocol encoder for two data bytes payload
 *	Encodes two bytes using baca protocol - e.g. data from Garmin Lidarlite or ADC
 *
 *	@param buffer Buffer to store the encoded message
 *	@param payload Payload bytes
 *	@param data_id ID of the device which sends the payload
 *	@return The lenght of the baca buffer. Generaly, it should be payload_size + 3
 */
uint8_t baca_protocol_encode_uint16(uint8_t* buffer, uint16_t payload, uint8_t data_id)
{
	  uint8_t checksum = (uint8_t) 'b';
	  uint8_t buffer_length = 0;
	  checksum += 3 + ((payload >> 8) & 0xFF) + ( payload & 0xFF);

	  buffer[buffer_length++] = 'b';
	  buffer[buffer_length++] = 3;
	  buffer[buffer_length++] = data_id;
	  buffer[buffer_length++] = ((payload >> 8));
	  buffer[buffer_length++] = (payload & 0xFF);
	  buffer[buffer_length++] = checksum;

	  return buffer_length;
}

/** @brief General baca protocol decode
 *	Decodes buffer of bytes using baca protocol
 *
 *	@param buffer Buffer with the encoded message
 *	@param payload Buffer to store payload bytes
 *	@return the struct with the payload and its properties
 */
struct baca_payload baca_protocol_decode(uint8_t* buffer, uint8_t buffer_size)
{
	struct baca_payload ret;
	ret.data_correct = 0;

	// is a baca message with at least one byte
	if ((buffer[0] == 'b') && (buffer_size >= 4))
	{
		// read the size of the payload (2nd byte)
		uint8_t payload_buffer_size = buffer[1];
		// if the buffer is large enough to accommodate the payload
		if (buffer_size >= payload_buffer_size + 3)
		{
			// write the size of the payload
			ret.payload_size = payload_buffer_size;

			// start calculating checksum
			uint8_t checksum = (uint8_t) 'b' + payload_buffer_size;
			// copy the payload and calculate the checksum
			for (uint8_t i = 0; i < payload_buffer_size; i ++)
			{
				ret.payload[i] = buffer[i+2];
				checksum += buffer[i+2];
			}
			// read the checksum sent (last byte)
			uint8_t checksum_sent = buffer[payload_buffer_size+2];
			 // If the sent checksum is correct set the data correct flag
			if (checksum == checksum_sent)
			{
				ret.data_correct = 1;
				return ret;
			} else
			{
				return ret;
			}
		}
	} else
	{
		return ret;
	}
}
